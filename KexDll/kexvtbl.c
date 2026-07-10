#include "buildcfg.h"
#include "kexdllp.h"

#define WRAPCALLTABLE_SIZE 192

// from wrapcall.asm
EXTERN CONST PVOID WrapCallFuncTable[WRAPCALLTABLE_SIZE];
EXTERN CONST PVOID ExternalOnlyWrapCallFuncTable[WRAPCALLTABLE_SIZE];

//
// Rewrite the given virtual function table inplace. Generally in the .data/.rdata segement.
// Once a VFT has been rewritten then all interface that used this table will also be affected. 
// 
// If you are trying to rewrite an rewritten table, nothing will happen except the 
// reference count of this table will increase. This is considered to be that a new interface 
// is referring this modified table. So you have to release the reference to the table 
// when IUnknown_Release is called and the interface's reference count is turned to zero.
//

STATIC PRTL_DYNAMIC_HASH_TABLE pRewriteRecordTable = NULL;
STATIC RTL_SRWLOCK pSRWLock = { 0 }; // That's what RtlInitializeSRWLock exactly did

KEXAPI NTSTATUS NTAPI KexVtblPatchInplace(
	IN	PVOID	lpVtbl,
	IN  PKEX_VTBL_MODIFICATION Entries,
	IN  UINT    NumberOfEntries,
	IN  BOOL    AllowOverlap,
	OUT PPVOID* pOriginalVtbl)
{
	if (lpVtbl == NULL)
		return STATUS_INVALID_PARAMETER_1;

	RtlAcquireSRWLockExclusive(&pSRWLock);
	if (pRewriteRecordTable == NULL)
		RtlCreateHashTable(&pRewriteRecordTable, 0, 0);

	NTSTATUS status = STATUS_SUCCESS;
	SIZE_T maxOffset = 0;

	//
	// Calculate the furthest function we have to cover.
	//

	for (UINT i = 0; i < NumberOfEntries; i++)
	{
		if (Entries[i].ByteOffset > maxOffset)
			maxOffset = Entries[i].ByteOffset;
	}
	maxOffset /= sizeof(PVOID);
	maxOffset++;

	//
	// To check if the table is already rewritten.
	// If AllowOverlap is not specified, then also check the overlaps.
	//

	PPVOID ppVtbl = (PPVOID)lpVtbl;
	RTL_DYNAMIC_HASH_TABLE_ENUMERATOR enumerator;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY pEntry;

	if (!AllowOverlap)
	{
		RtlInitWeakEnumerationHashTable(pRewriteRecordTable, &enumerator);
		pEntry = RtlWeaklyEnumerateEntryHashTable(pRewriteRecordTable, &enumerator);
		while (pEntry)
		{
			KEX_VTBL_REWRITE_DATA* currentRecord = CONTAINING_RECORD(pEntry, KEX_VTBL_REWRITE_DATA, HashTableEntry);

			PPVOID overlappedStart = NULL;
			PPVOID overlappedEnd = NULL;

			overlappedStart = max(currentRecord->RewrittenVtbl, ppVtbl);
			overlappedEnd = min(currentRecord->RewrittenVtbl + currentRecord->NumberOfFuncs, ppVtbl + maxOffset);

			if (currentRecord->RewrittenVtbl == ppVtbl || overlappedStart < overlappedEnd)
			{
				//
				// Increase the reference count of the collided rewritten table
				// because there is at least one interface referring this table.
				//

				currentRecord->RefCount++;
				RtlEndWeakEnumerationHashTable(pRewriteRecordTable, &enumerator);
				status = STATUS_ADDRESS_ALREADY_EXISTS;
				goto Exit;
			}
			pEntry = RtlWeaklyEnumerateEntryHashTable(pRewriteRecordTable, &enumerator);
		}
		RtlEndWeakEnumerationHashTable(pRewriteRecordTable, &enumerator);
	}
	else
	{
		pEntry = RtlLookupEntryHashTable(pRewriteRecordTable, (ULONG_PTR)ppVtbl, NULL);
		if (pEntry)
		{
			KEX_VTBL_REWRITE_DATA* currentRecord = CONTAINING_RECORD(pEntry, KEX_VTBL_REWRITE_DATA, HashTableEntry);
			currentRecord->RefCount++;
			status = STATUS_ADDRESS_ALREADY_EXISTS;
			goto Exit;
		}
	}

	//
	// Create a new record and store the information and the original functions we covered.
	//

	KEX_VTBL_REWRITE_DATA* newRecord;
	newRecord = HeapAlloc(GetProcessHeap(), 0, sizeof(KEX_VTBL_REWRITE_DATA) + sizeof(PVOID) * maxOffset);
	newRecord->RewrittenVtbl = lpVtbl;
	newRecord->OriginalVtbl = (PPVOID)(((LPBYTE)newRecord) + sizeof(KEX_VTBL_REWRITE_DATA));
	newRecord->RefCount = 1;
	newRecord->NumberOfFuncs = maxOffset;
	CopyMemory(newRecord->OriginalVtbl, lpVtbl, maxOffset * sizeof(PVOID));

	//
	// Modify the table inplace. We have to change the memory protection
	// because usually tables are stored in the .data/.rdata section which is readonly.
	//

	SIZE_T regionSize = NumberOfEntries * sizeof(PVOID);
	UINT oldProtect;

	status = NtProtectVirtualMemory(
		NtCurrentProcess(),
		&lpVtbl,
		&regionSize,
		PAGE_READWRITE,
		&oldProtect
	);
	if (!NT_SUCCESS(status))
	{
		KexLogErrorEvent(
			L"Failed to rewrite the virtual function table (%p, %d functions)\r\n\r\n"
			L"While attempting to change memory protections, encountered %s.",
			lpVtbl,
			maxOffset,
			KexRtlNtStatusToString(status));
	}

	try
	{

		for (UINT j = 0; j < NumberOfEntries; j++)
		{

			SIZE_T Offset = Entries[j].ByteOffset / sizeof(PVOID);
			ppVtbl[Offset] = Entries[j].Function;

		}
	} except(GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{
		//
		// This shouldn't happen unless lpVtbl is not an effective address. 
		//

		KexLogErrorEvent(
			L"Failed to rewrite the virtual function table (%p, %d functions)\r\n\r\n"
			L"Encountered STATUS_ACCESS_VIOLATION even after changing page protections.\r\n"
			L"Maybe %p is not an effective address.",
			lpVtbl,
			maxOffset,
			lpVtbl);

		status = STATUS_ACCESS_VIOLATION;
		goto Exit;
	}

	status = NtProtectVirtualMemory(
		NtCurrentProcess(),
		&lpVtbl,
		&regionSize,
		oldProtect,
		&oldProtect
	);
	ASSERT(NT_SUCCESS(status));

	//
	// Insert the record into the hash table.
	//

	if (pOriginalVtbl)
		*pOriginalVtbl = newRecord->OriginalVtbl;
	RtlInsertEntryHashTable(pRewriteRecordTable, &newRecord->HashTableEntry, (ULONG_PTR)newRecord->RewrittenVtbl, NULL);

Exit:
	RtlReleaseSRWLockExclusive(&pSRWLock);
	return status;
}

//
// Decrease the reference count of a modified table.
// You could call this function when an interface that is referring this table is truly released.
//

KEXAPI VOID NTAPI KexVtblUnpatchInplace(
	IN	PVOID	lpVtbl)
{
	RtlAcquireSRWLockExclusive(&pSRWLock);

	KEX_VTBL_REWRITE_DATA* currentRecord = NULL;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY pEntry;

	pEntry = RtlLookupEntryHashTable(pRewriteRecordTable, (ULONG_PTR)lpVtbl, NULL);
	if (pEntry)
	{
		currentRecord = CONTAINING_RECORD(pEntry, KEX_VTBL_REWRITE_DATA, HashTableEntry);
		currentRecord->RefCount--;

		//
		// Clear the record if there's no interface is referring this table.
		//

		if (currentRecord->RefCount == 0)
		{
			BOOL HaveModifiedPageProtection = FALSE;
			SIZE_T regionSize = currentRecord->NumberOfFuncs * sizeof(PVOID);
			PVOID baseAddress = lpVtbl;
			UINT oldProtect;
			NTSTATUS status;

			//
			// try to resume the rewritten virtual function table.
			// We have to change the memory protection because 
			// usually tables are stored in the .data/.rdata section which is readonly.
			//

			status = NtProtectVirtualMemory(
				NtCurrentProcess(),
				&baseAddress,
				&regionSize,
				PAGE_READWRITE,
				&oldProtect
			);
			if (!NT_SUCCESS(status))
			{
				KexLogErrorEvent(
					L"Failed to resume the rewritten virtual function table (%p <- %p, %d functions)\r\n\r\n"
					L"While attempting to change memory protections, encountered %s.",
					lpVtbl,
					currentRecord->OriginalVtbl,
					KexRtlNtStatusToString(status),
					currentRecord->NumberOfFuncs);
			}

			try
			{
				CopyMemory(lpVtbl, currentRecord->OriginalVtbl, currentRecord->NumberOfFuncs * sizeof(PVOID));
			} except(GetExceptionCode() == STATUS_ACCESS_VIOLATION)
			{
				//
				// This shouldn't happen unless lpVtbl is not an effective address (in theory). 
				// But it actually happened sometimes for some reasons.
				// For this we just do nothing because the rewritten functions will query
				// the original table and will crash if not found.
				//
				KexLogErrorEvent(
					L"Failed to resume the rewritten virtual function table (%p <- %p, %d functions)\r\n\r\n"
					L"Encountered STATUS_ACCESS_VIOLATION even after changing page protections.\r\n"
					L"Maybe %p is not an effective address. (the module is unloaded etc.)",
					lpVtbl,
					currentRecord->OriginalVtbl,
					currentRecord->NumberOfFuncs,
					lpVtbl);

				currentRecord->RefCount++;
				RtlReleaseSRWLockExclusive(&pSRWLock);
				return;

			}

			status = NtProtectVirtualMemory(
				NtCurrentProcess(),
				&baseAddress,
				&regionSize,
				oldProtect,
				&oldProtect
			);
			ASSERT(NT_SUCCESS(status));

			RtlRemoveEntryHashTable(pRewriteRecordTable, &currentRecord->HashTableEntry, NULL);
			HeapFree(GetProcessHeap(), 0, currentRecord);
		}
	}

	RtlReleaseSRWLockExclusive(&pSRWLock);
}

KEXAPI PPVOID NTAPI KexVtblLookupOriginalTable(
	IN	PVOID	PatchedVtbl)
{
	RtlAcquireSRWLockShared(&pSRWLock);

	KEX_VTBL_REWRITE_DATA* currentRecord = NULL;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY pEntry;
	PPVOID Result = NULL;

	pEntry = RtlLookupEntryHashTable(pRewriteRecordTable, (ULONG_PTR)PatchedVtbl, NULL);
	if (pEntry)
	{
		currentRecord = CONTAINING_RECORD(pEntry, KEX_VTBL_REWRITE_DATA, HashTableEntry);
		Result = currentRecord->OriginalVtbl;
	}

	RtlReleaseSRWLockShared(&pSRWLock);
	return Result;
}

//
// I don't know if some guys will use them.
// I just create it for fun.
//

KEXAPI PPVOID NTAPI KexVtblLookupPatchedTable(
	IN	PVOID	OriginalVtbl)
{
	RtlAcquireSRWLockShared(&pSRWLock);

	PPVOID ppVtbl = (PPVOID)OriginalVtbl;
	PPVOID Result = NULL;

	RTL_DYNAMIC_HASH_TABLE_ENUMERATOR enumerator;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY pEntry;

	RtlInitEnumerationHashTable(pRewriteRecordTable, &enumerator);
	pEntry = RtlEnumerateEntryHashTable(pRewriteRecordTable, &enumerator);
	while (pEntry)
	{
		KEX_VTBL_REWRITE_DATA* currentRecord = CONTAINING_RECORD(pEntry, KEX_VTBL_REWRITE_DATA, HashTableEntry);

		PVOID currentVtbl = (PVOID)currentRecord->OriginalVtbl;
		if (currentVtbl == ppVtbl)
		{
			Result = currentRecord->RewrittenVtbl;
			break;
		}
		pEntry = RtlEnumerateEntryHashTable(pRewriteRecordTable, &enumerator);
	}
	RtlEndEnumerationHashTable(pRewriteRecordTable, &enumerator);

	RtlReleaseSRWLockShared(&pSRWLock);
	return Result;
}

//
// Replace the pointer to the virtual function table of the given interface.
// Note that this kind of rewriting will slow down the program since it insert some 
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
	IN OUT	PVOID	Interface,
	IN	PKEX_VTBL_REPLACING_ENTRY	Entries,
	IN  PVOID   RefVtbl,
	IN  UINT    NumberOfEntries,
	IN  UINT    NumberOfFuncs,
	IN  SIZE_T  SizeOfContext,
	OUT PPKEX_VTBL_WRAPPER  pContext)
{
	PPVOID lpVtbl = *(PPVOID*)(Interface);
	PPVOID pRefVtbl = (PPVOID)(RefVtbl);
	PCHAR wrapperBuffer;
	PVOID moduleAddrStart = NULL;
	PVOID moduleAddrEnd = NULL;

	wrapperBuffer = HeapAlloc(GetProcessHeap(), 0, SizeOfContext
					 + sizeof(KEX_VTBL_WRAPPER)
					 + sizeof(KEX_VTBL_REPLACEMENT_WRAPPER)
					 + 2 * NumberOfFuncs * sizeof(PVOID));
	if (wrapperBuffer == NULL)
		return FALSE;

	KEX_VTBL_WRAPPER* wrapper = (KEX_VTBL_WRAPPER*)(wrapperBuffer);
	wrapper->lpVtbl = lpVtbl;
	wrapper->This = Interface;
	wrapperBuffer += SizeOfContext + sizeof(KEX_VTBL_WRAPPER);

	PPVOID wrapcallTable = (PPVOID)(wrapperBuffer + sizeof(KEX_VTBL_REPLACEMENT_WRAPPER));
	PPVOID modifiedTable = wrapcallTable + NumberOfFuncs;
	try
	{
		if (pRefVtbl)
		{
			for (UINT i = 0; i < NumberOfFuncs; i++)
			{
				if (pRefVtbl[i] == NULL)
				{
					wrapcallTable[i] = lpVtbl[i];
				}
				else
				{
					moduleAddrStart = (PVOID)1ULL;
					modifiedTable[i] = pRefVtbl[i];
					wrapcallTable[i] = ExternalOnlyWrapCallFuncTable[i];
				}
			}
		}
		else
		{
			for (UINT i = 0; i < NumberOfFuncs; i++)
			{
				wrapcallTable[i] = lpVtbl[i];
			}
		}
	} except(GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{

	}

	for (UINT j = 0; j < NumberOfEntries; j++)
	{
		SIZE_T offset = Entries[j].ByteOffset / sizeof(PVOID);
		if (Entries[j].Function)
		{
			if (Entries[j].Mode == KEX_VTBL_REPLACING_EXTERNAL_ONLY)
			{
				modifiedTable[offset] = Entries[j].Function;
				wrapcallTable[offset] = ExternalOnlyWrapCallFuncTable[offset];
				moduleAddrStart = (PVOID)1ULL;
			}
			else
			{
				modifiedTable[offset] = Entries[j].Function;
				wrapcallTable[offset] = WrapCallFuncTable[offset];
			}
		}
		else
		{
			wrapcallTable[j] = lpVtbl[j];
		}
	}
	if (moduleAddrStart)
	{
		PVOID func0 = lpVtbl[0];
		PLDR_DATA_TABLE_ENTRY dllEntry;
		LdrFindEntryForAddress(func0, &dllEntry);
		moduleAddrStart = dllEntry->DllBase;
		moduleAddrEnd = (PVOID)((PCHAR)dllEntry->DllBase + dllEntry->SizeOfImage);
	}

	KEX_VTBL_REPLACEMENT_WRAPPER* rWrapper;
	rWrapper = (KEX_VTBL_REPLACEMENT_WRAPPER*)(wrapperBuffer);
	rWrapper->pContext = wrapper;
	rWrapper->OriginalVtbl = lpVtbl;
	rWrapper->ModifiedVtbl = modifiedTable;
	rWrapper->ModuleAddrStart = moduleAddrStart;
	rWrapper->ModuleAddrEnd = moduleAddrEnd;

	*(PPVOID*)(Interface) = wrapcallTable;
	if (pContext)
		*pContext = wrapper;

	return TRUE;
}

KEXAPI VOID NTAPI KexVtblGetWrapperContext(
	IN	PVOID	Interface,
	OUT PPVOID  pContext,
	OUT PPVOID  pOriginalVtbl)
{
	PCHAR lpVtbl = *(PCHAR*)(Interface);
	KEX_VTBL_REPLACEMENT_WRAPPER* rWrapper;
	rWrapper = (KEX_VTBL_REPLACEMENT_WRAPPER*)(lpVtbl - sizeof(KEX_VTBL_REPLACEMENT_WRAPPER));

	if (pContext)
		*pContext = rWrapper->pContext;
	if (pOriginalVtbl)
		*pOriginalVtbl = rWrapper->OriginalVtbl;
}

KEXAPI VOID NTAPI KexVtblUnwrap(
	IN	PVOID	Interface)
{
	PCHAR lpVtbl = *(PCHAR*)(Interface);
	KEX_VTBL_REPLACEMENT_WRAPPER* rWrapper;
	rWrapper = (KEX_VTBL_REPLACEMENT_WRAPPER*)(lpVtbl - sizeof(KEX_VTBL_REPLACEMENT_WRAPPER));

	try
	{
		*(PPVOID*)(Interface) = rWrapper->OriginalVtbl;
	} except(GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{

	}
	HeapFree(GetProcessHeap(), 0, rWrapper->pContext);
}