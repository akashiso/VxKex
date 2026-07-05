#include "buildcfg.h"
#include "kxdxp.h"
#include <KexW32ML.h>


HRESULT WINAPI Ext_CreateDXGIFactory1(
	IN	REFIID	RefIID,
	OUT	PPVOID	Factory)
{
	if (IsEqualIID(RefIID, &IID_IDXGIFactoryMedia))
	{
		return CreateIDXGIFactoryMedia(Factory);
	}

	return CreateDXGIFactory2(0, RefIID, Factory);
}

//
// Function introduced in Windows 8.1.
// The IDXGIFactory2 interface is actually supported in Windows 7 but only with
// Platform Update.
//
HRESULT WINAPI CreateDXGIFactory2(
	IN	ULONG	Flags,
	IN	REFIID	RefIID,
	OUT	PPVOID	Factory)
{
	HRESULT Result;
	BOOL dxgifact2Used = FALSE;

	if (IsEqualIID(RefIID, &IID_IDXGIFactory)
		|| IsEqualIID(RefIID, &IID_IDXGIFactory2))
	{
		Result = CreateDXGIFactory1(
			RefIID,
			Factory);
	}
	else 
	if (IsEqualIID(RefIID, &IID_IDXGIFactory3)
		|| IsEqualIID(RefIID, &IID_IDXGIFactory4)
		|| IsEqualIID(RefIID, &IID_IDXGIFactory5)
		|| IsEqualIID(RefIID, &IID_IDXGIFactory6)
		|| IsEqualIID(RefIID, &IID_IDXGIFactory7))
	{
		KexLogInformationEvent(
			L"Higher version of DXGIFactory was queried."
		);

		dxgifact2Used = TRUE;
		Result = CreateDXGIFactory1(
			&IID_IDXGIFactory2,
			Factory);

		if (SUCCEEDED(Result))
		{
			IIDXGIFactory2To7* factory = CreateIIDXGIFactory2To7(*Factory, Flags);

			if (factory == NULL)
				Result = E_OUTOFMEMORY;
			else
				*Factory = factory;
		}
	}
	else
		return E_NOINTERFACE;


	if (FAILED(Result))
	{
		if (dxgifact2Used || IsEqualIID(RefIID, &IID_IDXGIFactory2))
		{
			KexLogErrorEvent(
				L"Failed to create IDXGIFactory2\r\n\r\n"
				L"HRESULT error code: 0x%08lx: %s\r\n"
				L"This interface is only supported by Windows 7 with Platform Update. "
				L"In order to solve this error, install Platform Update.",
				Result, Win32ErrorAsString(Result));
		}
		else
		{
			HRESULT Result2;
			BSTR IidAsString;

			Result2 = StringFromIID(RefIID, &IidAsString);
			ASSERT(SUCCEEDED(Result2));

			if (FAILED(Result2))
			{
				IidAsString = L"{unknown}";
			}

			KexLogErrorEvent(
				L"Failed to create DXGI factory: %s\r\n\r\n"
				L"HRESULT error code: 0x%08lx: %s",
				IidAsString,
				Result, Win32ErrorAsString(Result));

			KexDebugCheckpoint();
			SysFreeString(IidAsString);
		}
	}

	return Result;
}