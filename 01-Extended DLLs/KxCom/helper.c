#include "buildcfg.h"
#include "kxcomp.h"

//
// WinRT 
// COM and WinRT must be the masterpiece of the whole world.
//


// UI : Windows.UI.ViewManagement.IColorHelperStatics

KXCOMAPI HRESULT STDMETHODCALLTYPE ColorHelperStatics_QueryInterface(
	IN	IColorHelperStatics* This,
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
		IsEqualIID(RefIID, &IID_IColorHelperStatics))
	{
		*Object = &CColorHelperStatics;
	}
	else if (IsEqualIID(RefIID, &IID_IColorHelperStatics2))
	{
		*Object = &CColorHelperStatics2;
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

KXCOMAPI ULONG STDMETHODCALLTYPE ColorHelperStatics_AddRef(
	IN	IColorHelperStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE ColorHelperStatics_Release(
	IN	IColorHelperStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ColorHelperStatics_GetIids(
	IN	IColorHelperStatics* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 2;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array)
	{
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_IColorHelperStatics;
	Array[0] = IID_IColorHelperStatics2;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ColorHelperStatics_GetRuntimeClassName(
	IN	IColorHelperStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.UI.ViewManagement.ColorHelperStatics";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ColorHelperStatics_GetTrustLevel(
	IN	IColorHelperStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ColorHelperStatics_FromArgb(
	IN IColorHelperStatics* thiz,
	IN BYTE A,
	IN BYTE R,
	IN BYTE G, 
	IN BYTE B, 
	OUT UIColor* color)
{
	if (color == NULL)
		return E_POINTER;

	color->A = A;
	color->R = R;
	color->G = G;
	color->B = B;

	return S_OK;
}

IColorHelperStaticsVtbl CColorHelperStaticsVtbl = {
	ColorHelperStatics_QueryInterface,
	ColorHelperStatics_AddRef,
	ColorHelperStatics_Release,
	ColorHelperStatics_GetIids,
	ColorHelperStatics_GetRuntimeClassName,
	ColorHelperStatics_GetTrustLevel,
	ColorHelperStatics_FromArgb
};

IColorHelperStatics CColorHelperStatics = {
	&CColorHelperStaticsVtbl
};

// UI : Windows.UI.ViewManagement.IColorHelperStatics2

KXCOMAPI HRESULT STDMETHODCALLTYPE ColorHelperStatics2_ToDisplayName(
	IN IColorHelperStatics2* thiz,
	IN UIColor color, 
	OUT HSTRING* str)
{
	if (str == NULL)
		return E_POINTER;

	WCHAR HexColor[9];
	CONST WCHAR AlphaBet[17] = L"0123456789ABCDEF";

	HexColor[0] = L'#';
	HexColor[1] = AlphaBet[color.A >> 4];
	HexColor[2] = AlphaBet[color.A & 15];

	HexColor[3] = AlphaBet[color.R >> 4];
	HexColor[4] = AlphaBet[color.R & 15];

	HexColor[5] = AlphaBet[color.G >> 4];
	HexColor[6] = AlphaBet[color.G & 15];

	HexColor[7] = AlphaBet[color.B >> 4];
	HexColor[8] = AlphaBet[color.B & 15];

	return WindowsCreateString(HexColor, 9, str);
}

IColorHelperStatics2Vtbl CColorHelperStatics2Vtbl = {
	ColorHelperStatics_QueryInterface,
	ColorHelperStatics_AddRef,
	ColorHelperStatics_Release,
	ColorHelperStatics_GetIids,
	ColorHelperStatics_GetRuntimeClassName,
	ColorHelperStatics_GetTrustLevel,
	ColorHelperStatics2_ToDisplayName
};

IColorHelperStatics2 CColorHelperStatics2 = {
	&CColorHelperStatics2Vtbl
};