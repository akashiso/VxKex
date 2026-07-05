#include "buildcfg.h"
#include "kxcomp.h"

//
// WinRT
// Gaming : Windows.Gaming.UI.GameBar
//

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_QueryInterface(
	IN	IGameBarStatics* This,
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
		IsEqualIID(RefIID, &IID_IGameBarStatics))
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

KXCOMAPI ULONG STDMETHODCALLTYPE GameBarStatics_AddRef(
	IN	IGameBarStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE GameBarStatics_Release(
	IN	IGameBarStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_GetIids(
	IN	IGameBarStatics* This,
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
	Array[0] = IID_IGameBarStatics;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_GetRuntimeClassName(
	IN	IGameBarStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.Gaming.UI.IGameBarStatics";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_GetTrustLevel(
	IN	IGameBarStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_add_VisibilityChanged(
	IN IGameBarStatics* thiz,
	IN ITypedEventHandler* in,
	OUT EventRegistrationToken* out)
{
	out->value = 0xdeadbeef;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_remove_VisibilityChanged(
	IN IGameBarStatics* thiz,
	IN EventRegistrationToken in)
{
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_add_IsInputRedirectedChanged(
	IN IGameBarStatics* thiz,
	IN ITypedEventHandler* in,
	OUT EventRegistrationToken* out)
{
	out->value = 0xdeadbeef;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_remove_IsInputRedirectedChanged(
	IN IGameBarStatics* thiz,
	IN EventRegistrationToken in)
{
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_get_Visible(
	IN IGameBarStatics* thiz,
	OUT BOOL* out)
{
	if (!out) return E_POINTER;

	*out = FALSE;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE GameBarStatics_get_IsInputRedirected(
	IN IGameBarStatics* thiz,
	OUT BOOL* out)
{
	if (!out) return E_POINTER;

	*out = FALSE;
	return S_OK;
}

IGameBarStaticsVtbl CGameBarStaticsVtbl = {
	GameBarStatics_QueryInterface,
	GameBarStatics_AddRef,
	GameBarStatics_Release,

	GameBarStatics_GetIids,
	GameBarStatics_GetRuntimeClassName,
	GameBarStatics_GetTrustLevel,

	GameBarStatics_add_VisibilityChanged,
	GameBarStatics_remove_VisibilityChanged,
	GameBarStatics_add_IsInputRedirectedChanged,
	GameBarStatics_remove_IsInputRedirectedChanged,
	GameBarStatics_get_Visible,
	GameBarStatics_get_IsInputRedirected
};

IGameBarStatics CGameBarStatics = {
	&CGameBarStaticsVtbl
};