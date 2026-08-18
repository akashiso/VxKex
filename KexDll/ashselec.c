///////////////////////////////////////////////////////////////////////////////
//
// Module Name:
//
//     ashselec.c
//
// Abstract:
//
//     This file contains routines which dynamically select between different
//     implementations of DLLs by changing the DLL rewrite entries.
//
// Author:
//
//     vxiiduu (16-Mar-2024)
//
// Environment:
//
//     Native mode
//
// Revision History:
//
//     vxiiduu              16-Mar-2024  Initial creation.
//
///////////////////////////////////////////////////////////////////////////////

#include "buildcfg.h"
#include "kexdllp.h"

NTSTATUS AshSelectDWriteImplementation(
	IN	KEX_DWRITE_IMPLEMENTATION	Implementation)
{
	UNICODE_STRING DllName;
	UNICODE_STRING RewrittenDllName;

	RtlInitConstantUnicodeString(&DllName, L"DWrite");

	switch (Implementation) {
	case DWriteNoImplementation:
		RtlInitEmptyUnicodeString(&RewrittenDllName, NULL, 0);
		break;
	case DWriteWindows10Implementation:
		RtlInitConstantUnicodeString(&RewrittenDllName, L"kxdw");
		break;
	default:
		NOT_REACHED;
	}

	return KexAddUpdateRemoveDllRewriteEntry(&DllName, &RewrittenDllName);
}

NTSTATUS AshSelectD3D12Implementation()
{
	UNICODE_STRING DllName;
	UNICODE_STRING RewrittenDllName;

	if (KexIs64BitBuild)
		return STATUS_SUCCESS;

	RtlInitConstantUnicodeString(&DllName, L"d3d12");
	RtlInitConstantUnicodeString(&RewrittenDllName, L"kxdx");

	return KexAddUpdateRemoveDllRewriteEntry(&DllName, &RewrittenDllName);
}