#include "buildcfg.h"
#include "kxcomp.h"
#include "wbemcli.h"

#include <InitGuid.h>

//
// WinRT 
// COM and WinRT are masterpieces in the world.
//

// SysInfo : Windows.System.Profile.SystemManufacturers.ISmbiosInformationStatics

DEFINE_GUID(IID_IWbemLocator, 0xdc12a687, 0x737f, 0x11cf, 0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24);
DEFINE_GUID(CLSID_WbemLocator, 0x4590f811, 0x1d3a, 0x11d0, 0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24);

KXCOMAPI HRESULT STDMETHODCALLTYPE SmbiosInformationStatics_QueryInterface(
	IN	ISmbiosInformationStatics* This,
	IN	REFIID		RefIID,
	OUT	PPVOID		Object)
{
	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	*Object = NULL;

	if (IsEqualIID(RefIID, &IID_IUnknown) ||
		IsEqualIID(RefIID, &IID_IInspectable) ||
		IsEqualIID(RefIID, &IID_IAgileObject) ||
		IsEqualIID(RefIID, &IID_ISmbiosInformationStatics))
	{
		*Object = This;
	}
	else
	{
		LPOLESTR IidAsString;

		StringFromIID(RefIID, &IidAsString);

		KexLogWarningEvent(
			L"QueryInterface called with unsupported IID: %s",
			IidAsString);

		CoTaskMemFree(IidAsString);
		return E_NOINTERFACE;
	}

	return S_OK;
}

KXCOMAPI ULONG STDMETHODCALLTYPE SmbiosInformationStatics_AddRef(
	IN	ISmbiosInformationStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE SmbiosInformationStatics_Release(
	IN	ISmbiosInformationStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE SmbiosInformationStatics_GetIids(
	IN	ISmbiosInformationStatics* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 1;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array)
	{
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_ISmbiosInformationStatics;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE SmbiosInformationStatics_GetRuntimeClassName(
	IN	ISmbiosInformationStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.System.Profile.SystemManufacturers.ISmbiosInformationStatics";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE SmbiosInformationStatics_GetTrustLevel(
	IN	ISmbiosInformationStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE SmbiosInformationStatics_get_SerialNumber(
	IN ISmbiosInformationStatics* thiz,
	OUT HSTRING* out)
{
	IWbemLocator* wbemLocator;
	HRESULT hr;
	BSTR bstr;

	hr = CoCreateInstance(&CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, &IID_IWbemLocator, (void**)&wbemLocator);
	if (FAILED(hr)) return hr;

	bstr = SysAllocString(L"ROOT\\CIMV2");
	if (!bstr)
	{
		IWbemLocator_Release(wbemLocator);
		return E_OUTOFMEMORY;
	}

	IWbemServices* wbemService;
	hr = IWbemLocator_ConnectServer(wbemLocator, bstr, NULL, NULL, NULL, 0, NULL, NULL, &wbemService);
	IWbemLocator_Release(wbemLocator);
	SysFreeString(bstr);
	if (FAILED(hr)) 
		return hr;

	CONST WCHAR* class = L"Win32_BIOS";
	bstr = SysAllocString(class);
	if (bstr == NULL)
	{
		IWbemServices_Release(wbemService);
		return E_OUTOFMEMORY;
	}

	IEnumWbemClassObject* wbemEnum;
	hr = IWbemServices_CreateInstanceEnum(wbemService, bstr, WBEM_FLAG_SYSTEM_ONLY, NULL, &wbemEnum);
	IWbemServices_Release(wbemService);
	SysFreeString(bstr);
	if (FAILED(hr)) 
		return hr;

	IWbemClassObject* wbemClass;
	ULONG count;
	hr = IEnumWbemClassObject_Next(wbemEnum, 1000, 1, &wbemClass, &count);
	IEnumWbemClassObject_Release(wbemEnum);
	if (FAILED(hr)) 
		return hr;

	VARIANT serial;
	hr = IWbemClassObject_Get(wbemClass, L"SerialNumber", 0, &serial, NULL, NULL);
	IWbemClassObject_Release(wbemClass);
	if (FAILED(hr)) 
		return hr;

	BSTR sSerial = V_BSTR(&serial);
	VariantClear(&serial);
	return  WindowsCreateString(sSerial, (ULONG)wcslen(sSerial), out);
}

ISmbiosInformationStaticsVtbl CSmbiosInformationStaticsVtbl = {
	SmbiosInformationStatics_QueryInterface,
	SmbiosInformationStatics_AddRef,
	SmbiosInformationStatics_Release,
	SmbiosInformationStatics_GetIids,
	SmbiosInformationStatics_GetRuntimeClassName,
	SmbiosInformationStatics_GetTrustLevel,
	SmbiosInformationStatics_get_SerialNumber
};

ISmbiosInformationStatics CSmbiosInformationStatics = {
	&CSmbiosInformationStaticsVtbl
};