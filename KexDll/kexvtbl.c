#include "buildcfg.h"
#include "kexdllp.h"

#define WRAPCALLTABLE_SIZE 192

// from wrapcall.asm
EXTERN CONST PVOID WrapCallFuncTable[WRAPCALLTABLE_SIZE];
EXTERN CONST PVOID ExternalOnlyWrapCallFuncTable[WRAPCALLTABLE_SIZE];

//
// Rewrite the given virtual function table inplace. Generally in the .data/.rdata segment.
// Once a VFT has been rewritten then all interface that used this table will also be affected.
//
// If you are trying to rewrite an rewritten table, nothing will happen and 
// STATUS_ADDRESS_ALREADY_EXISTS will be returned.
//

STATIC PRTL_DYNAMIC_HASH_TABLE RewriteRecordTable = NULL;
STATIC RTL_SRWLOCK SRWLock = { 0 }; // That's what RtlInitializeSRWLock exactly did

KEXAPI NTSTATUS NTAPI KexVtblPatchInplace(
	IN	PVOID					Vtbl,
	IN	PKEX_VTBL_MODIFICATION	Entries,
	IN	UINT					NumberOfEntries,
	IN	BOOL					AllowOverlap,
	OUT	PPVOID* OriginalVtbl)
{
	if (Vtbl == NULL) {
		return STATUS_INVALID_PARAMETER_1;
	}

	RtlAcquireSRWLockExclusive(&SRWLock);
	if (RewriteRecordTable == NULL) {
		RtlCreateHashTable(&RewriteRecordTable, 0, 0);
	}

	NTSTATUS Status = STATUS_SUCCESS;
	SIZE_T MaxOffset = 0;

	//
	// Calculate the furthest function we have to cover.
	//

	for (UINT i = 0; i < NumberOfEntries; i++) {
		if (Entries[i].ByteOffset > MaxOffset) {
			MaxOffset = Entries[i].ByteOffset;
		}
	}
	MaxOffset /= sizeof(PVOID);
	MaxOffset++;

	//
	// To check if the table is already rewritten.
	// If AllowOverlap is not specified, then also check the overlaps.
	//

	PPVOID VtblPtr = (PPVOID)Vtbl;
	RTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY Entry;

	if (!AllowOverlap) {
		RtlInitWeakEnumerationHashTable(RewriteRecordTable, &Enumerator);
		Entry = RtlWeaklyEnumerateEntryHashTable(RewriteRecordTable, &Enumerator);
		while (Entry) {
			KEX_VTBL_REWRITE_DATA* Record = CONTAINING_RECORD(Entry, KEX_VTBL_REWRITE_DATA, HashTableEntry);

			PPVOID OverlappedStart = NULL;
			PPVOID OverlappedEnd = NULL;

			OverlappedStart = max(Record->RewrittenVtbl, VtblPtr);
			OverlappedEnd = min(Record->RewrittenVtbl + Record->NumberOfFuncs, VtblPtr + MaxOffset);

			if (Record->RewrittenVtbl == VtblPtr || OverlappedStart < OverlappedEnd) {
				RtlEndWeakEnumerationHashTable(RewriteRecordTable, &Enumerator);
				Status = STATUS_ADDRESS_ALREADY_EXISTS;
				goto Exit;
			}
			Entry = RtlWeaklyEnumerateEntryHashTable(RewriteRecordTable, &Enumerator);
		}
		RtlEndWeakEnumerationHashTable(RewriteRecordTable, &Enumerator);
	}
	else {
		Entry = RtlLookupEntryHashTable(RewriteRecordTable, (ULONG_PTR)VtblPtr, NULL);
		if (Entry) {
			Status = STATUS_ADDRESS_ALREADY_EXISTS;
			goto Exit;
		}
	}

	//
	// Create a new record and store the information and the original functions we covered.
	//

	KEX_VTBL_REWRITE_DATA* NewRecord;
	NewRecord = HeapAlloc(GetProcessHeap(), 0, sizeof(KEX_VTBL_REWRITE_DATA) + sizeof(PVOID) * MaxOffset);
	NewRecord->RewrittenVtbl = VtblPtr;
	NewRecord->OriginalVtbl = (PPVOID)(((LPBYTE)NewRecord) + sizeof(KEX_VTBL_REWRITE_DATA));
	NewRecord->NumberOfFuncs = MaxOffset;
	NewRecord->PrevPendingDelete = NULL;
	CopyMemory(NewRecord->OriginalVtbl, VtblPtr, MaxOffset * sizeof(PVOID));

	//
	// Modify the table inplace. We have to change the memory protection
	// because usually tables are stored in the .data/.rdata section which is readonly.
	//

	SIZE_T RegionSize = NumberOfEntries * sizeof(PVOID);
	ULONG OldProtect;

	Status = NtProtectVirtualMemory(
		NtCurrentProcess(),
		&Vtbl,
		&RegionSize,
		PAGE_READWRITE,
		&OldProtect);
	if (!NT_SUCCESS(Status)) {
		KexLogErrorEvent(
			L"Failed to rewrite the virtual function table (%p, %d functions)\r\n\r\n"
			L"While attempting to change memory protections, encountered %s.",
			Vtbl,
			MaxOffset,
			KexRtlNtStatusToString(Status));
	}

	try {
		for (UINT j = 0; j < NumberOfEntries; j++) {
			SIZE_T Offset = Entries[j].ByteOffset / sizeof(PVOID);
			VtblPtr[Offset] = Entries[j].Function;
		}
	} except(GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{
		//
		// This shouldn't happen unless Vtbl is not an effective address.
		//

		KexLogErrorEvent(
			L"Failed to rewrite the virtual function table (%p, %d functions)\r\n\r\n"
			L"Encountered STATUS_ACCESS_VIOLATION even after changing page protections.\r\n"
			L"Maybe %p is not an effective address.",
			Vtbl,
			MaxOffset,
			Vtbl);

		Status = STATUS_ACCESS_VIOLATION;
		goto Exit;
	}

	Status = NtProtectVirtualMemory(
		NtCurrentProcess(),
		&Vtbl,
		&RegionSize,
		OldProtect,
		&OldProtect);
	ASSERT(NT_SUCCESS(Status));

	//
	// Insert the record into the hash table.
	//

	if (OriginalVtbl) {
		*OriginalVtbl = NewRecord->OriginalVtbl;
	}
	RtlInsertEntryHashTable(RewriteRecordTable, &NewRecord->HashTableEntry, (ULONG_PTR)NewRecord->RewrittenVtbl, NULL);

Exit:
	RtlReleaseSRWLockExclusive(&SRWLock);
	return Status;
}

//
// Decrease the reference count of a modified table.
// You could call this function when an interface that is referring this table is truly released.
//

KEXAPI VOID NTAPI KexVtblUnpatchInplace(
	IN	PVOID	Vtbl)
{
	RtlAcquireSRWLockExclusive(&SRWLock);

	KEX_VTBL_REWRITE_DATA* Record = NULL;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY Entry;

	Entry = RtlLookupEntryHashTable(RewriteRecordTable, (ULONG_PTR)Vtbl, NULL);
	if (!Entry) {
		RtlReleaseSRWLockExclusive(&SRWLock);
		return;
	}

	Record = CONTAINING_RECORD(Entry, KEX_VTBL_REWRITE_DATA, HashTableEntry);

	BOOL HaveModifiedPageProtection = FALSE;
	SIZE_T RegionSize = Record->NumberOfFuncs * sizeof(PVOID);
	PVOID BaseAddress = Vtbl;
	ULONG OldProtect;
	NTSTATUS Status;

	//
	// try to resume the rewritten virtual function table.
	// We have to change the memory protection because
	// usually tables are stored in the .data/.rdata section which is readonly.
	//

	Status = NtProtectVirtualMemory(
		NtCurrentProcess(),
		&BaseAddress,
		&RegionSize,
		PAGE_READWRITE,
		&OldProtect);
	if (!NT_SUCCESS(Status)) {
		KexLogErrorEvent(
			L"Failed to resume the rewritten virtual function table (%p <- %p, %d functions)\r\n\r\n"
			L"While attempting to change memory protections, encountered %s.",
			Vtbl,
			Record->OriginalVtbl,
			Record->NumberOfFuncs,
			KexRtlNtStatusToString(Status));
	}

	try {
		CopyMemory(Vtbl, Record->OriginalVtbl, Record->NumberOfFuncs * sizeof(PVOID));
	} except(GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{
		//
		// This shouldn't happen unless Vtbl is not an effective address (in theory).
		// But it actually happened sometimes for some reasons.
		// For this we just do nothing because the rewritten functions will query
		// the original table and will crash if not found.
		//
		KexLogErrorEvent(
			L"Failed to resume the rewritten virtual function table (%p <- %p, %d functions)\r\n\r\n"
			L"Encountered STATUS_ACCESS_VIOLATION even after changing page protections.\r\n"
			L"Maybe %p is not an effective address. (the module is unloaded etc.)",
			Vtbl,
			Record->OriginalVtbl,
			Record->NumberOfFuncs,
			Vtbl);

		RtlReleaseSRWLockExclusive(&SRWLock);
		return;
	}

	Status = NtProtectVirtualMemory(
		NtCurrentProcess(),
		&BaseAddress,
		&RegionSize,
		OldProtect,
		&OldProtect);
	ASSERT(NT_SUCCESS(Status));

	RtlRemoveEntryHashTable(RewriteRecordTable, &Record->HashTableEntry, NULL);
	HeapFree(GetProcessHeap(), 0, Record);

	RtlReleaseSRWLockExclusive(&SRWLock);
}

KEXAPI PPVOID NTAPI KexVtblLookupOriginalTable(
	IN	PCVOID	PatchedVtbl)
{
	RtlAcquireSRWLockShared(&SRWLock);

	KEX_VTBL_REWRITE_DATA* Record = NULL;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY Entry;
	PPVOID Result = NULL;

	Entry = RtlLookupEntryHashTable(RewriteRecordTable, (ULONG_PTR)PatchedVtbl, NULL);
	if (Entry) {
		Record = CONTAINING_RECORD(Entry, KEX_VTBL_REWRITE_DATA, HashTableEntry);
		Result = Record->OriginalVtbl;
	}

	RtlReleaseSRWLockShared(&SRWLock);
	return Result;
}

//
// I don't know if some guys will use them.
// I just create it for fun.
//

KEXAPI PPVOID NTAPI KexVtblLookupPatchedTable(
	IN	PCVOID	OriginalVtbl)
{
	RtlAcquireSRWLockShared(&SRWLock);

	PPVOID VtblPtr = (PPVOID)OriginalVtbl;
	PPVOID Result = NULL;

	RTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY Entry;

	RtlInitEnumerationHashTable(RewriteRecordTable, &Enumerator);
	Entry = RtlEnumerateEntryHashTable(RewriteRecordTable, &Enumerator);
	while (Entry) {
		KEX_VTBL_REWRITE_DATA* Record = CONTAINING_RECORD(Entry, KEX_VTBL_REWRITE_DATA, HashTableEntry);

		PVOID CurrentVtbl = (PVOID)Record->OriginalVtbl;
		if (CurrentVtbl == VtblPtr) {
			Result = Record->RewrittenVtbl;
			break;
		}
		Entry = RtlEnumerateEntryHashTable(RewriteRecordTable, &Enumerator);
	}
	RtlEndEnumerationHashTable(RewriteRecordTable, &Enumerator);

	RtlReleaseSRWLockShared(&SRWLock);
	return Result;
}

//
// Clear the records when the patched DLL was already unloaded.
//

VOID KexVtblDllUnloadNotification(
	IN	PCLDR_DLL_NOTIFICATION_DATA	NotificationData)
{
	if (RewriteRecordTable == NULL)
		return;

	RtlAcquireSRWLockExclusive(&SRWLock);

	PPVOID ModuleAddressStart;
	PPVOID ModuleAddressEnd;

	ModuleAddressStart = NotificationData->DllBase;
	ModuleAddressEnd = (PPVOID)((PCHAR)NotificationData->DllBase + NotificationData->SizeOfImage);

	RTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY Entry;
	KEX_VTBL_REWRITE_DATA* PrevPendingDelete = NULL;

	RtlInitWeakEnumerationHashTable(RewriteRecordTable, &Enumerator);
	Entry = RtlWeaklyEnumerateEntryHashTable(RewriteRecordTable, &Enumerator);
	while (Entry) {
		KEX_VTBL_REWRITE_DATA* Record = CONTAINING_RECORD(Entry, KEX_VTBL_REWRITE_DATA, HashTableEntry);
		PPVOID VtblPtr = Record->RewrittenVtbl;

		if (VtblPtr >= ModuleAddressStart && VtblPtr < ModuleAddressEnd) {
			Record->PrevPendingDelete = PrevPendingDelete;
			PrevPendingDelete = Record;
		}

		Entry = RtlWeaklyEnumerateEntryHashTable(RewriteRecordTable, &Enumerator);
	}

	RtlEndWeakEnumerationHashTable(RewriteRecordTable, &Enumerator);

	while (PrevPendingDelete) {
		KEX_VTBL_REWRITE_DATA* Next = PrevPendingDelete->PrevPendingDelete;

		RtlRemoveEntryHashTable(RewriteRecordTable, &PrevPendingDelete->HashTableEntry, NULL);
		HeapFree(GetProcessHeap(), 0, PrevPendingDelete);
		PrevPendingDelete = Next;
	}

	RtlReleaseSRWLockExclusive(&SRWLock);
}

//
// Replace the pointer to the virtual function table of the given interface.
// Note that this kind of rewriting will slow down the program since it inserts some
// code between the caller and callee.
//
// The structure of the replaced table (x64):
// Context
// 	 pOriginalVtbl(void*)
// 	 This(void*)
//	 Misc(Unknown)
// pContext(void*)				 (p - 40)
// ModuleAddrStart(void*)		 (p - 32)
// ModuleAddrEnd(void*)			 (p - 24)
// pOriginalVtbl(void*)			 (p - 16)
// pModifiedVtbl(void*)			 (p - 8)
// VWrapVtblTable(void*)*numFunc (p - 0)
// ModifiedVtbl(void*)*numFunc
//

KEXAPI BOOLEAN NTAPI KexVtblWrap(
	IN OUT	PVOID						Interface,
	IN		PKEX_VTBL_REPLACING_ENTRY	Entries,
	IN		PVOID						ReferenceVtbl,
	IN		UINT						NumberOfEntries,
	IN		UINT						NumberOfFuncs,
	IN		SIZE_T						SizeOfContext,
	OUT		PPKEX_VTBL_WRAPPER			Context)
{
	PPVOID VtblPtr = *(PPVOID*)(Interface);
	PPVOID RefVtblPtr = (PPVOID)ReferenceVtbl;
	PCHAR WrapperBuffer;
	PVOID ModuleAddrStart = NULL;
	PVOID ModuleAddrEnd = NULL;

	WrapperBuffer = HeapAlloc(GetProcessHeap(), 0, SizeOfContext
							  + sizeof(KEX_VTBL_WRAPPER)
							  + sizeof(KEX_VTBL_REPLACEMENT_WRAPPER)
							  + 2 * NumberOfFuncs * sizeof(PVOID));
	if (WrapperBuffer == NULL) {
		return FALSE;
	}

	KEX_VTBL_WRAPPER* Wrapper = (KEX_VTBL_WRAPPER*)(WrapperBuffer);
	Wrapper->lpVtbl = VtblPtr;
	Wrapper->This = Interface;
	WrapperBuffer += SizeOfContext + sizeof(KEX_VTBL_WRAPPER);

	PPVOID WrapCallTable = (PPVOID)(WrapperBuffer + sizeof(KEX_VTBL_REPLACEMENT_WRAPPER));
	PPVOID ModifiedTable = WrapCallTable + NumberOfFuncs;

	try {
		if (RefVtblPtr) {
			for (UINT i = 0; i < NumberOfFuncs; i++) {
				if (RefVtblPtr[i] == NULL) {
					WrapCallTable[i] = VtblPtr[i];
				}
				else {
					ModuleAddrStart = (PVOID)1ULL;
					ModifiedTable[i] = RefVtblPtr[i];
					WrapCallTable[i] = ExternalOnlyWrapCallFuncTable[i];
				}
			}
		}
		else {
			for (UINT i = 0; i < NumberOfFuncs; i++) {
				WrapCallTable[i] = VtblPtr[i];
			}
		}
	} except(GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{
		// ignore
	}

	for (UINT j = 0; j < NumberOfEntries; j++) {
		SIZE_T Offset = Entries[j].ByteOffset / sizeof(PVOID);
		if (Entries[j].Function) {
			if (Entries[j].Mode == KEX_VTBL_REPLACING_EXTERNAL_ONLY) {
				ModifiedTable[Offset] = Entries[j].Function;
				WrapCallTable[Offset] = ExternalOnlyWrapCallFuncTable[Offset];
				ModuleAddrStart = (PVOID)1ULL;
			}
			else {
				ModifiedTable[Offset] = Entries[j].Function;
				WrapCallTable[Offset] = WrapCallFuncTable[Offset];
			}
		}
		else {
			WrapCallTable[j] = VtblPtr[j];
		}
	}

	if (ModuleAddrStart) {
		PVOID Func0 = VtblPtr[0];
		PLDR_DATA_TABLE_ENTRY DllEntry;
		LdrFindEntryForAddress(Func0, &DllEntry);
		ModuleAddrStart = DllEntry->DllBase;
		ModuleAddrEnd = (PVOID)((PCHAR)DllEntry->DllBase + DllEntry->SizeOfImage);
	}

	KEX_VTBL_REPLACEMENT_WRAPPER* ReplacementWrapper;
	ReplacementWrapper = (KEX_VTBL_REPLACEMENT_WRAPPER*)(WrapperBuffer);
	ReplacementWrapper->pContext = Wrapper;
	ReplacementWrapper->OriginalVtbl = VtblPtr;
	ReplacementWrapper->ModifiedVtbl = ModifiedTable;
	ReplacementWrapper->ModuleAddrStart = ModuleAddrStart;
	ReplacementWrapper->ModuleAddrEnd = ModuleAddrEnd;

	*(PPVOID*)(Interface) = WrapCallTable;
	if (Context) {
		*Context = Wrapper;
	}

	return TRUE;
}

KEXAPI VOID NTAPI KexVtblGetWrapperContext(
	IN	PVOID	Interface,
	OUT	PPVOID	Context,
	OUT	PPVOID	OriginalVtbl)
{
	PCHAR VtblPtr = *(PCHAR*)(Interface);
	KEX_VTBL_REPLACEMENT_WRAPPER* ReplacementWrapper;
	ReplacementWrapper = (KEX_VTBL_REPLACEMENT_WRAPPER*)(VtblPtr - sizeof(KEX_VTBL_REPLACEMENT_WRAPPER));

	if (Context) {
		*Context = ReplacementWrapper->pContext;
	}
	if (OriginalVtbl) {
		*OriginalVtbl = ReplacementWrapper->OriginalVtbl;
	}
}

KEXAPI VOID NTAPI KexVtblUnwrap(
	IN	PVOID	Interface)
{
	PCHAR VtblPtr = *(PCHAR*)(Interface);
	KEX_VTBL_REPLACEMENT_WRAPPER* ReplacementWrapper;
	ReplacementWrapper = (KEX_VTBL_REPLACEMENT_WRAPPER*)(VtblPtr - sizeof(KEX_VTBL_REPLACEMENT_WRAPPER));

	try {
		*(PPVOID*)(Interface) = ReplacementWrapper->OriginalVtbl;
	} except(GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{
		// ignore
	}
	HeapFree(GetProcessHeap(), 0, ReplacementWrapper->pContext);
}