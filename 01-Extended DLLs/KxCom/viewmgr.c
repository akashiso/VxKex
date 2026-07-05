#include "buildcfg.h"
#include "kxcomp.h"

//
// WinRT 
// COM and WinRT must be the masterpiece of the whole world.
//

// UI : Windows.UI.ViewManagement.IAccessibilitySettings

KXCOMAPI HRESULT STDMETHODCALLTYPE AccessibilitySettings_QueryInterface(
	IN	IAccessibilitySettings* This,
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
		IsEqualIID(RefIID, &IID_IAccessibilitySettings))
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

KXCOMAPI ULONG STDMETHODCALLTYPE AccessibilitySettings_AddRef(
	IN	IAccessibilitySettings* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE AccessibilitySettings_Release(
	IN	IAccessibilitySettings* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AccessibilitySettings_GetIids(
	IN	IAccessibilitySettings* This,
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
	Array[0] = IID_IAccessibilitySettings;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AccessibilitySettings_GetRuntimeClassName(
	IN	IAccessibilitySettings* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.UI.ViewManagement.AccessibilitySettings";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AccessibilitySettings_GetTrustLevel(
	IN	IAccessibilitySettings* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AccessibilitySettings_get_HighContrast(
	IN IAccessibilitySettings* thiz,
	OUT BOOL* out)
{
	HIGHCONTRASTW high_contrast = { .cbSize = sizeof(HIGHCONTRASTW) };

	if (!SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRASTW), &high_contrast, 0))
		return E_FAIL;

	*out = !!(high_contrast.dwFlags & HCF_HIGHCONTRASTON);
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AccessibilitySettings_get_HighContrastScheme(
	IN IAccessibilitySettings* thiz,
	OUT HSTRING* out)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AccessibilitySettings_add_HighContrastChanged(
	IN	IAccessibilitySettings* This,
	IN	ITypedEventHandler* in,
	OUT	EventRegistrationToken* token)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AccessibilitySettings_remove_HighContrastChanged(
	IN	IAccessibilitySettings* This,
	IN	EventRegistrationToken token)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

IAccessibilitySettingsVtbl CAccessibilitySettingsVtbl = {
	AccessibilitySettings_QueryInterface,
	AccessibilitySettings_AddRef,
	AccessibilitySettings_Release,
	AccessibilitySettings_GetIids,
	AccessibilitySettings_GetRuntimeClassName,
	AccessibilitySettings_GetTrustLevel,
	AccessibilitySettings_get_HighContrast,
	AccessibilitySettings_get_HighContrastScheme,
	AccessibilitySettings_add_HighContrastChanged,
	AccessibilitySettings_remove_HighContrastChanged
};

IAccessibilitySettings CAccessibilitySettings = {
	&CAccessibilitySettingsVtbl
};


// UI : Windows.UI.ViewManagement.IInputPane & IInputPane2

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane_QueryInterface(
	IN	IInputPane* This,
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
		IsEqualIID(RefIID, &IID_IInputPane))
	{
		*Object = This->Parent;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else if (IsEqualIID(RefIID, &IID_IInputPane2))
	{
		*Object = &This->Parent->cInputPane2;
		InterlockedIncrement(&This->Parent->RefCount);
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

KXCOMAPI ULONG STDMETHODCALLTYPE InputPane_AddRef(
	IN	IInputPane* This)
{
	return InterlockedIncrement(&This->Parent->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE InputPane_Release(
	IN	IInputPane* This)
{
	ULONG NewRefCount;

	NewRefCount = InterlockedDecrement(&This->Parent->RefCount);

	if (NewRefCount == 0)
	{
		CoTaskMemFree(This);
	}

	return NewRefCount;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane_GetIids(
	IN	IInputPane* This,
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
	Array[0] = IID_IInputPane;
	Array[1] = IID_IInputPane2;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane_GetRuntimeClassName(
	IN	IInputPane* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.UI.ViewManagement.InputPane";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane_GetTrustLevel(
	IN	IInputPane* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane_add_Showing(
	IN	IInputPane* This,
	IN	ITypedEventHandler* in,
	OUT	EventRegistrationToken* token)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane_remove_Showing(
	IN	IInputPane* This,
	IN	EventRegistrationToken token)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane_add_Hiding(
	IN	IInputPane* This,
	IN	ITypedEventHandler* in,
	OUT	EventRegistrationToken* token)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane_remove_Hiding(
	IN	IInputPane* This,
	IN	EventRegistrationToken token)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane_get_OccludedRect(
	IN IInputPane* thiz,
	OUT RECT* out)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane2_TryShow(
	IN IInputPane* thiz,
	OUT BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = TRUE;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPane2_TryHide(
	IN IInputPane* thiz,
	OUT BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = TRUE;
	return S_OK;
}

IInputPaneVtbl CInputPaneVtbl = {
	InputPane_QueryInterface,
	InputPane_AddRef,
	InputPane_Release,
	InputPane_GetIids,
	InputPane_GetRuntimeClassName,
	InputPane_GetTrustLevel,
	InputPane_add_Showing,
	InputPane_remove_Showing,
	InputPane_add_Hiding,
	InputPane_remove_Hiding,
	InputPane_get_OccludedRect
};

IInputPane2Vtbl CInputPane2Vtbl = {
	InputPane_QueryInterface,
	InputPane_AddRef,
	InputPane_Release,
	InputPane_GetIids,
	InputPane_GetRuntimeClassName,
	InputPane_GetTrustLevel,
	InputPane2_TryShow,
	InputPane2_TryHide
};

IInputPane* CreateInputPane()
{
	IInputPane* pane = (IInputPane*)CoTaskMemAlloc(sizeof(IInputPane));

	pane->lpVtbl = &CInputPaneVtbl;
	pane->Parent = pane;
	pane->RefCount = 1;

	pane->cInputPane2.lpVtbl = &CInputPane2Vtbl;
	pane->cInputPane2.Parent = pane;

	return pane;
}

// UI : Windows.UI.ViewManagement.IInputPaneStatics

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPaneStatics_QueryInterface(
	IN	IInputPaneStatics* This,
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
		IsEqualIID(RefIID, &IID_IInputPaneStatics))
	{
		*Object = &CInputPaneStatics;
	}
	else if (IsEqualIID(RefIID, &IID_IInputPaneStatics2))
	{
		*Object = &CInputPaneStatics2;
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

KXCOMAPI ULONG STDMETHODCALLTYPE InputPaneStatics_AddRef(
	IN	IInputPaneStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE InputPaneStatics_Release(
	IN	IInputPaneStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPaneStatics_GetIids(
	IN	IInputPaneStatics* This,
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
	Array[0] = IID_IInputPaneStatics;
	Array[1] = IID_IInputPaneStatics2;
	Array[2] = IID_IInputPaneInterop;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPaneStatics_GetRuntimeClassName(
	IN	IInputPaneStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.UI.ViewManagement.InputPaneStatics";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPaneStatics_GetTrustLevel(
	IN	IInputPaneStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPaneStatics_GetForCurrentView(
	IN IInputPaneStatics* thiz,
	OUT IInputPane** out)
{
	if (out == NULL)
		return E_POINTER;

	*out = CreateInputPane();
	if (*out == NULL)
		return E_OUTOFMEMORY;

	return S_OK;
}

IInputPaneStaticsVtbl CInputPaneStaticsVtbl = {
	InputPaneStatics_QueryInterface,
	InputPaneStatics_AddRef,
	InputPaneStatics_Release,
	InputPaneStatics_GetIids,
	InputPaneStatics_GetRuntimeClassName,
	InputPaneStatics_GetTrustLevel,
	InputPaneStatics_GetForCurrentView
};

IInputPaneStatics CInputPaneStatics = {
	&CInputPaneStaticsVtbl
};

// UI : Windows.UI.ViewManagement.IInputPaneStatics2

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPaneStatics2_GetForUIContext(
	IN IInputPaneStatics2* thiz,
	IN IUIContext* ctx,
	OUT IInputPane** out)
{
	if (out == NULL)
		return E_POINTER;

	*out = CreateInputPane();
	if (*out == NULL)
		return E_OUTOFMEMORY;

	return S_OK;
}

IInputPaneStatics2Vtbl CInputPaneStatics2Vtbl = {
	InputPaneStatics_QueryInterface,
	InputPaneStatics_AddRef,
	InputPaneStatics_Release,
	InputPaneStatics_GetIids,
	InputPaneStatics_GetRuntimeClassName,
	InputPaneStatics_GetTrustLevel,
	InputPaneStatics2_GetForUIContext
};

IInputPaneStatics2 CInputPaneStatics2 = {
	&CInputPaneStatics2Vtbl
};

// UI : Windows.UI.ViewManagement.IInputPaneInterop

KXCOMAPI HRESULT STDMETHODCALLTYPE InputPaneInterop_GetForUIContext(
	IN IInputPaneInterop* thiz,
	HWND Window, 
	REFIID RefIID,
	PPVOID out)
{
	if (out == NULL)
		return E_POINTER;

	if (!IsWindow(Window))
	{
		return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);
	}

	if (!(IsEqualIID(RefIID, &IID_IInputPane)
		  || IsEqualIID(RefIID, &IID_IInputPane2)))
	{
		return E_NOINTERFACE;
	}

	*out = CreateInputPane();
	if (*out == NULL)
		return E_OUTOFMEMORY;

	if (IsEqualIID(RefIID, &IID_IInputPane2))
		*out = &((IInputPane*)(*out))->cInputPane2;

	return S_OK;
}

IInputPaneInteropVtbl CInputPaneInteropVtbl = {
	InputPaneStatics_QueryInterface,
	InputPaneStatics_AddRef,
	InputPaneStatics_Release,
	InputPaneStatics_GetIids,
	InputPaneStatics_GetRuntimeClassName,
	InputPaneStatics_GetTrustLevel,
	InputPaneInterop_GetForUIContext
};

IInputPaneInterop CInputPaneInterop = {
	&CInputPaneInteropVtbl
};