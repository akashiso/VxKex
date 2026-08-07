#include "buildcfg.h"
#include "kxd2p.h"
#include <KexW32ML.h>

//
// The ID2D1Factory1 interface is actually supported in Windows 7 but only with
// Platform Update.
//
HRESULT WINAPI Ext_D2D1CreateFactory(
	D2D1_FACTORY_TYPE			factoryType,
	REFIID						riid_1,
	const D2D1_FACTORY_OPTIONS* pFactoryOptions,
	void** ppIFactory
)
{
	HRESULT Result;
	BOOL IsD2D1Factory1Used = FALSE;
	BOOL ForceWrapping = FALSE;

	LPCGUID riid = riid_1;

	// 
	// TODO: Figure out if this is a good idea or not.
	//

	unless(KexData->IfeoParameters.DisableAppSpecific)
		if (AshExeBaseNameIs(L"ImageGlass.exe"))
			ForceWrapping = TRUE;

	if (ForceWrapping)
		riid = &IID_ID2D1Factory7;

	if (IsEqualIID(riid, &IID_ID2D1Factory)
		|| IsEqualIID(riid, &IID_ID2D1Factory1))
	{
		Result = D2D1CreateFactory(
			factoryType,
			riid,
			pFactoryOptions,
			ppIFactory);
	}
	else 
	if (IsEqualIID(riid, &IID_ID2D1Factory2)
		|| IsEqualIID(riid, &IID_ID2D1Factory3)
		|| IsEqualIID(riid, &IID_ID2D1Factory4)
		|| IsEqualIID(riid, &IID_ID2D1Factory5)
		|| IsEqualIID(riid, &IID_ID2D1Factory6)
		|| IsEqualIID(riid, &IID_ID2D1Factory7))
	{
		//Sleep(10 * 1000);
		KexLogInformationEvent(
			L"Higher version of D2D1Factory was queried."
		);

		IsD2D1Factory1Used = TRUE;
		Result = D2D1CreateFactory(
			factoryType,
			&IID_ID2D1Factory1,
			pFactoryOptions,
			ppIFactory);

		if (SUCCEEDED(Result))
		{
			IID2D1Factory1To8* factory = CreateIID2D1Factory1To8(*ppIFactory);

			if (factory == NULL)
				Result = E_OUTOFMEMORY;
			else
			{
				PVOID lpVtbl;
				PVOID lpVtbl0 = factory->lpVtbl;
				KexVtblGetWrapperContext(factory, NULL, &lpVtbl);
				factory->lpVtbl = lpVtbl;

				//IID2D_RegisterBuiltinEffects(factory->Thi);
				IID2D_RegisterBuiltinEffects((ID2D1Factory1*)factory);
				factory->lpVtbl = lpVtbl0;

				*ppIFactory = factory;
			}
		}
	}
	else
		return E_NOINTERFACE;

	if (FAILED(Result))
	{
		if (IsD2D1Factory1Used || IsEqualIID(riid, &IID_ID2D1Factory1))
		{
			KexLogErrorEvent(
				L"Failed to create ID2D1Factory1\r\n\r\n"
				L"HRESULT error code: 0x%08lx: %s\r\n"
				L"This interface is only supported by Windows 7 with Platform Update. "
				L"In order to solve this error, install Platform Update.",
				Result, Win32ErrorAsString(Result));
		}
		else
		{
			HRESULT Result2;
			BSTR IidAsString;

			Result2 = StringFromIID(riid, &IidAsString);
			ASSERT(SUCCEEDED(Result2));

			if (FAILED(Result2))
			{
				IidAsString = L"{unknown}";
			}

			KexLogErrorEvent(
				L"Failed to create D2D1 factory: %s\r\n\r\n"
				L"HRESULT error code: 0x%08lx: %s",
				IidAsString,
				Result, Win32ErrorAsString(Result));

			KexDebugCheckpoint();
			SysFreeString(IidAsString);
		}
	}

	return Result;
}