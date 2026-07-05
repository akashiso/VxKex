#include "buildcfg.h"
#include "kexdllp.h"

#define WRAPCALLTABLE_SIZE 192

// from wrapcall.asm
EXTERN CONST PVOID VWrapCallFuncTable[WRAPCALLTABLE_SIZE];
EXTERN CONST PVOID ExternalOnlyVWrapCallFuncTable[WRAPCALLTABLE_SIZE];

//
// Rewrite the given virtual function table inplace. Generally in the .data/.rdata segement.
// Once a VFT has been rewritten then all interface that used this table will also be affected. 
// 
// If you are trying to rewrite an rewritten table, nothing will happen except the 
// reference count of this table will increase. This is considered to be that a new interface 
// is refering this modified table. So you have to release the reference to the table 
// when IUnknown_Release is called and the interface's reference count is turned to zero.
//

STATIC PRTL_DYNAMIC_HASH_TABLE RewriteDataTable = NULL;
STATIC RTL_SRWLOCK pSRWLock = { 0 }; // That's what RtlInitializeSRWLock exactly did

KEXAPI NTSTATUS NTAPI KexVtblRewriteInplace(
	IN	PVOID	lpVtbl,
	IN  PKEX_VFT_VTBL_MODIFICATION Entry,
	IN  UINT    NumberOfEntry,
	IN  BOOL    CanOverlap,
	OUT PPVOID* OriginalVFTable)
{
	if (lpVtbl == NULL)
		return STATUS_INVALID_PARAMETER_1;

	RtlAcquireSRWLockExclusive(&pSRWLock);
	if (RewriteDataTable == NULL)
		RtlCreateHashTable(&RewriteDataTable, 0, 0);

	NTSTATUS status = STATUS_SUCCESS;
	SIZE_T maxOffset = 0;

	//
	// Calculate the furthest function we have to cover.
	//

	for (UINT i = 0; i < NumberOfEntry; i++)
	{
		if (Entry[i].ByteOffset > maxOffset)
			maxOffset = Entry[i].ByteOffset;
	}
	maxOffset /= sizeof(PVOID);
	maxOffset++;

	//
	// To check if the table is already rewritten.
	// If CannotOverlap is specified, then also check the overlaps.
	//

	PPVOID pVtbl = (PPVOID)lpVtbl;
	RTL_DYNAMIC_HASH_TABLE_ENUMERATOR enumerator;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY pEntry;

	if (!CanOverlap)
	{
		RtlInitWeakEnumerationHashTable(RewriteDataTable, &enumerator);
		pEntry = RtlWeaklyEnumerateEntryHashTable(RewriteDataTable, &enumerator);
		while (pEntry)
		{
			KEX_VFT_REWRITE_DATA* cur = CONTAINING_RECORD(pEntry, KEX_VFT_REWRITE_DATA, HashTableEntry);

			PPVOID overlappedStart = NULL;
			PPVOID overlappedEnd = NULL;

			overlappedStart = max(cur->RewrittenVFTable, pVtbl);
			overlappedEnd = min(cur->RewrittenVFTable + cur->NumberOfFuncs, pVtbl + maxOffset);

			if (cur->RewrittenVFTable == pVtbl || overlappedStart < overlappedEnd)
			{
				//
				// Increase the reference count of the colided rewritten table
				// because an interface is actually refering this table.
				//

				cur->RefCount++;
				RtlEndWeakEnumerationHashTable(RewriteDataTable, &enumerator);
				status = STATUS_ADDRESS_ALREADY_EXISTS;
				goto Exit;
			}
			pEntry = RtlWeaklyEnumerateEntryHashTable(RewriteDataTable, &enumerator);
		}
		RtlEndWeakEnumerationHashTable(RewriteDataTable, &enumerator);
	}
	else
	{
		pEntry = RtlLookupEntryHashTable(RewriteDataTable, (ULONG_PTR)pVtbl, NULL);
		if (pEntry)
		{
			KEX_VFT_REWRITE_DATA* cur = CONTAINING_RECORD(pEntry, KEX_VFT_REWRITE_DATA, HashTableEntry);
			cur->RefCount++;
			status = STATUS_ADDRESS_ALREADY_EXISTS;
			goto Exit;
		}
	}

	//
	// Create a new record and store the information and the original functions we covered.
	//

	KEX_VFT_REWRITE_DATA* newObj;
	newObj = HeapAlloc(GetProcessHeap(), 0, sizeof(KEX_VFT_REWRITE_DATA) + sizeof(PVOID) * maxOffset);
	newObj->RewrittenVFTable = lpVtbl;
	newObj->OriginalVFTable = (PPVOID)(((LPBYTE)newObj) + sizeof(KEX_VFT_REWRITE_DATA));
	newObj->RefCount = 1;
	newObj->NumberOfFuncs = maxOffset;
	CopyMemory(newObj->OriginalVFTable, lpVtbl, maxOffset * sizeof(PVOID));

	//
	// Modify the table inplace. We have to change the memory protection
	// because usually tables are stored in the .data/.rdata section which is readonly.
	//

	SIZE_T regionSize = NumberOfEntry * sizeof(PVOID);
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

		for (UINT j = 0; j < NumberOfEntry; j++)
		{

			SIZE_T Offset = Entry[j].ByteOffset / sizeof(PVOID);
			pVtbl[Offset] = Entry[j].Function;

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

	if (OriginalVFTable)
		*OriginalVFTable = newObj->OriginalVFTable;
	RtlInsertEntryHashTable(RewriteDataTable, &newObj->HashTableEntry, (ULONG_PTR)newObj->RewrittenVFTable, NULL);

Exit:
	RtlReleaseSRWLockExclusive(&pSRWLock);
	return status;
}

//
// Decrease the reference count of a modified table.
// You could call this function when an interface that is refering this table is truly released.
//

KEXAPI VOID NTAPI KexVtblReleaseRewriteData(
	IN	PVOID	lpVtbl)
{
	RtlAcquireSRWLockExclusive(&pSRWLock);

	KEX_VFT_REWRITE_DATA* cur = NULL;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY pEntry;

	pEntry = RtlLookupEntryHashTable(RewriteDataTable, (ULONG_PTR)lpVtbl, NULL);
	if (pEntry)
	{
		cur = CONTAINING_RECORD(pEntry, KEX_VFT_REWRITE_DATA, HashTableEntry);
		cur->RefCount--;

		//
		// Clear the record if there's no interface is refering this table.
		//

		if (cur->RefCount == 0)
		{
			BOOL HaveModifiedPageProtection = FALSE;
			SIZE_T regionSize = cur->NumberOfFuncs * sizeof(PVOID);
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
					cur->OriginalVFTable,
					KexRtlNtStatusToString(status),
					cur->NumberOfFuncs);
			}

			try
			{
				CopyMemory(lpVtbl, cur->OriginalVFTable, cur->NumberOfFuncs * sizeof(PVOID));
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
					cur->OriginalVFTable,
					cur->NumberOfFuncs,
					lpVtbl);

				cur->RefCount++;
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

			RtlRemoveEntryHashTable(RewriteDataTable, &cur->HashTableEntry, NULL);
			HeapFree(GetProcessHeap(), 0, cur);
		}
	}

	RtlReleaseSRWLockExclusive(&pSRWLock);
}

KEXAPI PPVOID NTAPI KexVtblFindOriginalVFTable(
	IN	PVOID	RewrittenVFTable)
{
	RtlAcquireSRWLockShared(&pSRWLock);

	KEX_VFT_REWRITE_DATA* cur = NULL;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY pEntry;
	PPVOID Result = NULL;

	pEntry = RtlLookupEntryHashTable(RewriteDataTable, (ULONG_PTR)RewrittenVFTable, NULL);
	if (pEntry)
	{
		cur = CONTAINING_RECORD(pEntry, KEX_VFT_REWRITE_DATA, HashTableEntry);
		Result = cur->OriginalVFTable;
	}

	RtlReleaseSRWLockShared(&pSRWLock);
	return Result;
}

//
// I don't know if some guys will use them.
// I just create it for fun.
//

KEXAPI PPVOID NTAPI KexVtblFindRewrittenVFTable(
	IN	PVOID	OriginalVFTable)
{
	RtlAcquireSRWLockShared(&pSRWLock);

	PPVOID pVtbl = (PPVOID)OriginalVFTable;
	PPVOID Result = NULL;

	RTL_DYNAMIC_HASH_TABLE_ENUMERATOR enumerator;
	PRTL_DYNAMIC_HASH_TABLE_ENTRY pEntry;

	RtlInitEnumerationHashTable(RewriteDataTable, &enumerator);
	pEntry = RtlEnumerateEntryHashTable(RewriteDataTable, &enumerator);
	while (pEntry)
	{
		KEX_VFT_REWRITE_DATA* cur = CONTAINING_RECORD(pEntry, KEX_VFT_REWRITE_DATA, HashTableEntry);

		PVOID p = (PVOID)cur->OriginalVFTable;
		if (p == pVtbl)
		{
			Result = cur->RewrittenVFTable;
			break;
		}
		pEntry = RtlEnumerateEntryHashTable(RewriteDataTable, &enumerator);
	}
	RtlEndEnumerationHashTable(RewriteDataTable, &enumerator);

	RtlReleaseSRWLockShared(&pSRWLock);
	return Result;
}

//
// Replace the pointer to the virtual function table of the given interface.
// Note that this kind of rewriting will slow down the program since it insert some 
// code between the caller and callee.
// 
// The structure of the replaced table (x64):
// Bundles
// 	 pOriginalVtbl(void*)
// 	 This(void*)
//	 Misc(Unknown)
// pBundles(void*)				 (p - 40)
// ModuleAddrStart(void*)		 (p - 32)
// ModuleAddrEnd(void*)			 (p - 24)
// pOriginalVtbl(void*)			 (p - 16)
// pModifiedVtbl(void*)			 (p - 8)
// VWrapVtblTable(void*)*numFunc (p - 0)
// ModifiedVtbl(void*)*numFunc
//

KEXAPI BOOLEAN NTAPI KexVtblReplace(
	IN OUT	PVOID	Interface,
	IN	PKEX_VFT_REPLACING_ENTRY	Entries,
	IN  PVOID   RefVtbl,
	IN  UINT    NumberOfEntries,
	IN  UINT    NumberOfFuncs,
	IN  SIZE_T  SizeOfBundles,
	OUT PPKEX_VFT_WRAPPER  pBundles)
{
	PPVOID lpVtbl = *(PPVOID*)(Interface);
	PPVOID pRefVtbl = (PPVOID)(RefVtbl);
	PCHAR data;
	PVOID moduleAddrStart = NULL;
	PVOID moduleAddrEnd = NULL;

	data = HeapAlloc(GetProcessHeap(), 0, SizeOfBundles
					 + sizeof(KEX_VFT_WRAPPER)
					 + sizeof(KEX_VFT_REPLACEMENT_WRAPPER)
					 + 2 * NumberOfFuncs * sizeof(PVOID));
	if (data == NULL)
		return FALSE;

	KEX_VFT_WRAPPER* wrapper = (KEX_VFT_WRAPPER*)(data);
	wrapper->lpVtbl = lpVtbl;
	wrapper->This = Interface;
	data += SizeOfBundles + sizeof(KEX_VFT_WRAPPER);

	PPVOID wrapcallTable = (PPVOID)(data + sizeof(KEX_VFT_REPLACEMENT_WRAPPER));
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
					wrapcallTable[i] = ExternalOnlyVWrapCallFuncTable[i];
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
			if (Entries[j].Mode == KEX_VFT_REPLACING_EXTERNAL_ONLY)
			{
				modifiedTable[offset] = Entries[j].Function;
				wrapcallTable[offset] = ExternalOnlyVWrapCallFuncTable[offset];
				moduleAddrStart = (PVOID)1ULL;
			}
			else
			{
				modifiedTable[offset] = Entries[j].Function;
				wrapcallTable[offset] = VWrapCallFuncTable[offset];
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

	KEX_VFT_REPLACEMENT_WRAPPER* rWrapper;
	rWrapper = (KEX_VFT_REPLACEMENT_WRAPPER*)(data);
	rWrapper->pBundles = wrapper;
	rWrapper->OriginalVtbl = lpVtbl;
	rWrapper->ModifiedVtbl = modifiedTable;
	rWrapper->ModuleAddrStart = moduleAddrStart;
	rWrapper->ModuleAddrEnd = moduleAddrEnd;

	*(PPVOID*)(Interface) = wrapcallTable;
	if (pBundles)
		*pBundles = wrapper;

	return TRUE;
}

KEXAPI VOID NTAPI KexVtblGetReplaceBundles(
	IN	PVOID	Interface,
	OUT PPVOID  pBundles,
	OUT PPVOID  pOriginalVtbl)
{
	PCHAR lpVtbl = *(PCHAR*)(Interface);
	KEX_VFT_REPLACEMENT_WRAPPER* rWrapper;
	rWrapper = (KEX_VFT_REPLACEMENT_WRAPPER*)(lpVtbl - sizeof(KEX_VFT_REPLACEMENT_WRAPPER));

	if (pBundles)
		*pBundles = rWrapper->pBundles;
	if (pOriginalVtbl)
		*pOriginalVtbl = rWrapper->OriginalVtbl;
}

KEXAPI VOID NTAPI KexVtblReleaseReplaceData(
	IN	PVOID	Interface)
{
	PCHAR lpVtbl = *(PCHAR*)(Interface);
	KEX_VFT_REPLACEMENT_WRAPPER* rWrapper;
	rWrapper = (KEX_VFT_REPLACEMENT_WRAPPER*)(lpVtbl - sizeof(KEX_VFT_REPLACEMENT_WRAPPER));

	try
	{
		*(PPVOID*)(Interface) = rWrapper->OriginalVtbl;
	} except(GetExceptionCode() == STATUS_ACCESS_VIOLATION)
	{

	}
	HeapFree(GetProcessHeap(), 0, rWrapper->pBundles);
}