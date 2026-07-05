#include "buildcfg.h"
#include "kxgamingp.h"
#include "kxgaming.h"

KXGAMINGAPI HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void** out)
{
	HRESULT Result2;
	LPOLESTR IidAsString;
	LPOLESTR ClsidAsString;

	Result2 = StringFromIID(riid, &IidAsString);
	ASSERT(SUCCEEDED(Result2));
	if (FAILED(Result2))
	{
		IidAsString = L"{unknown}";
	}

	Result2 = StringFromIID(clsid, &ClsidAsString);
	ASSERT(SUCCEEDED(Result2));
	if (FAILED(Result2))
	{
		ClsidAsString = L"{unknown}";
	}

	KexLogWarningEvent(
		L"DllGetClassObject called : CLSID = %s, IID = %s",
		ClsidAsString, IidAsString);

	CoTaskMemFree(IidAsString);
	CoTaskMemFree(ClsidAsString);

    return CLASS_E_CLASSNOTAVAILABLE;
}