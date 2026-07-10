#include "buildcfg.h"
#include "kxdxp.h"

int _fltused = 0;

HRESULT STDMETHODCALLTYPE IIDXGIFactory2_QueryInterface(
	IIDXGIFactory2To7* This,
	REFIID riid,
	OUT void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	if (IsEqualIID(riid, &IID_IDXGIFactory3) ||
		IsEqualIID(riid, &IID_IDXGIFactory4) ||
		IsEqualIID(riid, &IID_IDXGIFactory5) ||
		IsEqualIID(riid, &IID_IDXGIFactory6) ||
		IsEqualIID(riid, &IID_IDXGIFactory7))
	{
		*ppvObject = This->Thi;
		This->lpVtbl->AddRef(This->Thi); 

		return S_OK;
	}

	return This->lpVtbl->QueryInterface(This->Thi, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IIDXGIFactory2_Release(
	IIDXGIFactory2To7* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount;
	RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 0)
		KexVtblUnwrap(&lpVtbl);
	return RefCount;
}

HRESULT STDMETHODCALLTYPE IIDXGIFactory2_CreateSwapChain(
	IIDXGIFactory2To7* This,
	IN  IUnknown* pDevice,
	IN  DXGI_SWAP_CHAIN_DESC* pDesc,
	OUT IDXGISwapChain** ppSwapChain)
{
	if (pDesc->SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD  // win10
		|| pDesc->SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)  // win8
	{
		DXGI_SWAP_CHAIN_DESC descCopy;
		CopyMemory(&descCopy, pDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		descCopy.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		return This->lpVtbl->CreateSwapChain(This->Thi, pDevice, &descCopy, ppSwapChain);
	}

	return This->lpVtbl->CreateSwapChain(This->Thi, pDevice, pDesc, ppSwapChain);
}

HRESULT STDMETHODCALLTYPE IIDXGIFactory2_EnumAdapters1(
	IIDXGIFactory2To7* This,
	UINT Adapter,
	OUT IDXGIAdapter1** ppAdapter)
{
	IDXGIAdapter1* adapter;

	HRESULT hr = This->lpVtbl->EnumAdapters1(This->Thi, Adapter, &adapter);

	if (FAILED(hr))
		return hr;

	WrapIDXGIAdapter(adapter);
	*ppAdapter = (IDXGIAdapter1*)adapter;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE IIDXGIFactory2_CreateSwapChainForHwnd(
	IIDXGIFactory2To7* This,
	IN  IUnknown* pDevice,
	IN  HWND hWnd,
	IN  const DXGI_SWAP_CHAIN_DESC1* pDesc,
	IN  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
	IN  IDXGIOutput* pRestrictToOutput,
	OUT IDXGISwapChain** ppSwapChain)
{
	DXGI_SWAP_CHAIN_DESC1 descCopy;
	HRESULT Result;

	if (pDesc->Scaling == DXGI_SCALING_NONE
		|| pDesc->SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD  // win10
		|| pDesc->SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)  // win8
	{
		CopyMemory(&descCopy, pDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));

		if (pDesc->SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD)
			descCopy.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		else if (pDesc->SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
			descCopy.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

		if (pDesc->Scaling == DXGI_SCALING_NONE)
			descCopy.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;

		pDesc = &descCopy;
	}

	Result = This->lpVtbl->CreateSwapChainForHwnd(This->Thi, pDevice, hWnd, pDesc,
													 pFullscreenDesc, pRestrictToOutput, ppSwapChain);
	if (ppSwapChain)
		*ppSwapChain = WrapIDXGISwapchain(*ppSwapChain);
	return Result;
}

UINT STDMETHODCALLTYPE IIDXGIFactory3_GetCreationFlags(
	IIDXGIFactory2To7* This)
{
	return This->CreationFlags;
}

HRESULT STDMETHODCALLTYPE IIDXGIFactory4_EnumAdapterByLuid(
	IIDXGIFactory2To7* This,
	IN  LUID AdapterLuid,
	IN  REFIID riid,
	OUT void** ppvAdapter)
{
	UINT32 adapterId = 0;

	while (1)
	{
		IDXGIAdapter* adapter;
		HRESULT hr = This->lpVtbl->EnumAdapters(This->Thi, adapterId++, &adapter);

		if (FAILED(hr))
			return hr;

		DXGI_ADAPTER_DESC desc;
		IDXGIAdapter_GetDesc(adapter, &desc);

		if (!memcmp(&AdapterLuid, &desc.AdapterLuid, sizeof(LUID)))
		{
			WrapIDXGIAdapter((IDXGIAdapter1*)adapter);
			*ppvAdapter = adapter;

			return S_OK;
		}
	}

	// This should be unreachable
	return DXGI_ERROR_NOT_FOUND;
}

HRESULT STDMETHODCALLTYPE IIDXGIFactory4_EnumWarpAdapter(
	IIDXGIFactory2To7* This,
	IN  REFIID riid,
	OUT void** ppvAdapter)
{
	IDXGIAdapter* adapter;
	HRESULT hr = This->lpVtbl->EnumAdapters(This->Thi, 0, &adapter);

	if (FAILED(hr))
		return hr;

	WrapIDXGIAdapter((IDXGIAdapter1*)adapter);
	*ppvAdapter = (IDXGIAdapter*)adapter;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IIDXGIFactory5_CheckFeatureSupport(
	IIDXGIFactory2To7* This,
	DXGI_FEATURE Feature,
	void* pFeatureSupportData,
	UINT FeatureSupportDataSize)
{
	if (Feature == DXGI_FEATURE_PRESENT_ALLOW_TEARING)
	{
		if (FeatureSupportDataSize != sizeof(BOOL))
			return E_INVALIDARG;

		*((PBOOL)pFeatureSupportData) = TRUE;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE IIDXGIFactory6_EnumAdapterByGpuPreference(
	IIDXGIFactory2To7* This,
	IN  UINT Adapter,
	IN  DXGI_GPU_PREFERENCE GpuPreference,
	IN  REFIID riid,
	OUT void** ppvAdapter)
{
	IDXGIAdapter* adapter;
	HRESULT hr = This->lpVtbl->EnumAdapters(This->Thi, Adapter, &adapter);

	if (FAILED(hr))
		return hr;

	WrapIDXGIAdapter((IDXGIAdapter1*)adapter);
	*ppvAdapter = adapter;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE IIDXGIFactory7_RegisterAdaptersChangedEvent(
	IIDXGIFactory2To7* This,
	IN  HANDLE hEvent,
	OUT DWORD* pdwCookie)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIFactory7_UnregisterAdaptersChangedEvent(
	IIDXGIFactory2To7* This,
	IN  DWORD dwCookie)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

void* IIDXGIFactory2To7Vtbl[] = {
	 IIDXGIFactory2_QueryInterface,
	 0,//IIDXGIFactory2_AddRef,
	 IIDXGIFactory2_Release,
	 0,//IIDXGIFactory2_SetPrivateData,
	 0,//IIDXGIFactory2_SetPrivateDataInterface,
	 0,//IIDXGIFactory2_GetPrivateData,
	 0,//IIDXGIFactory2_GetParent,
	 0,//IIDXGIFactory2_EnumAdapters,
	 0,//IIDXGIFactory2_MakeWindowAssociation,
	 0,//IIDXGIFactory2_GetWindowAssociation,
	 IIDXGIFactory2_CreateSwapChain,
	 0,//IIDXGIFactory2_CreateSoftwareAdapter,
	 IIDXGIFactory2_EnumAdapters1,
	 0,//IIDXGIFactory2_IsCurrent,
	 0,//IIDXGIFactory2_IsWindowedStereoEnabled,
	 IIDXGIFactory2_CreateSwapChainForHwnd,
	 0,//IIDXGIFactory2_CreateSwapChainForCoreWindow,
	 0,//IIDXGIFactory2_GetSharedResourceAdapterLuid,
	 0,//IIDXGIFactory2_RegisterStereoStatusWindow,
	 0,//IIDXGIFactory2_RegisterStereoStatusEvent,
	 0,//IIDXGIFactory2_UnregisterStereoStatus,
	 0,//IIDXGIFactory2_RegisterOcclusionStatusWindow,
	 0,//IIDXGIFactory2_RegisterOcclusionStatusEvent,
	 0,//IIDXGIFactory2_UnregisterOcclusionStatus,
	 0,//IIDXGIFactory2_CreateSwapChainForComposition,
	 IIDXGIFactory3_GetCreationFlags,
	 IIDXGIFactory4_EnumAdapterByLuid,
	 IIDXGIFactory4_EnumWarpAdapter,
	 IIDXGIFactory5_CheckFeatureSupport,
	 IIDXGIFactory6_EnumAdapterByGpuPreference,
	 IIDXGIFactory7_RegisterAdaptersChangedEvent,
	 IIDXGIFactory7_UnregisterAdaptersChangedEvent
};

IIDXGIFactory2To7* CreateIIDXGIFactory2To7(IDXGIFactory2* pFactory2, UINT CreationFlags)
{
	PVOID obj;
	if (pFactory2->lpVtbl->QueryInterface(pFactory2, &IID_IDXGIFactory7, &obj) == S_OK)
	{
		pFactory2->lpVtbl->Release(pFactory2);
		return (IIDXGIFactory2To7*)pFactory2;
	}

	IIDXGIFactory2To7* factory;
	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IIDXGIFactory2_Release, KEX_VTBL_REPLACING_ALL}
	};
	if (!KexVtblWrap(pFactory2, rpl, IIDXGIFactory2To7Vtbl, 1, ARRAYSIZE(IIDXGIFactory2To7Vtbl),
						sizeof(IIDXGIFactory2To7) - sizeof(KEX_VTBL_WRAPPER), (PPKEX_VTBL_WRAPPER)&factory))
		return NULL;

	factory->CreationFlags = CreationFlags;

	return (IIDXGIFactory2To7*)pFactory2;
}


HRESULT STDMETHODCALLTYPE IIDXGIAdapter_QueryInterface(
	IN	IIDXGIAdapter* This,
	IN	REFIID			RefIID,
	OUT	PPVOID			Object)
{
	HRESULT Result;

	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	if (IsEqualIID(RefIID, &IID_IDXGIAdapter2) ||
		IsEqualIID(RefIID, &IID_IDXGIAdapter3) ||
		IsEqualIID(RefIID, &IID_IDXGIAdapter4))
	{

		*Object = This->Thi;
		This->lpVtbl->AddRef((IDXGIAdapter1*)This->Thi);
		return S_OK;
	}

	Result = This->lpVtbl->QueryInterface((IDXGIAdapter1*)This->Thi, RefIID, Object);
	return Result;
}

ULONG STDMETHODCALLTYPE IIDXGIAdapter_Release(
	IIDXGIAdapter* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount;
	RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 0)
		KexVtblUnwrap(&lpVtbl);
	return RefCount;
}

HRESULT STDMETHODCALLTYPE IIDXGIAdapter_GetDesc2(
	IN	IIDXGIAdapter* This,
	OUT	PDXGI_ADAPTER_DESC2	Desc)
{
	ASSERT(This != NULL);
	ASSERT(Desc != NULL);

	RtlZeroMemory(Desc, sizeof(*Desc));

	return This->lpVtbl->GetDesc1(
		(IDXGIAdapter1*)This->Thi,
		(DXGI_ADAPTER_DESC1*)Desc);
}

HRESULT STDMETHODCALLTYPE IIDXGIAdapter_EnumOutputs(
	IIDXGIAdapter* This,
	UINT Output,
	OUT  IDXGIOutput** ppOutput)
{
	HRESULT Result = This->lpVtbl->EnumOutputs(This->Thi, Output, ppOutput);

	if (SUCCEEDED(Result))
		WrapIDXGIOutput(*ppOutput);
	return Result;
}

HRESULT STDMETHODCALLTYPE IIDXGIAdapter_RegisterHardwareContentProtectionTeardownStatusEvent(
	IN	IIDXGIAdapter* This,
	IN	HANDLE			Event,
	OUT	PULONG			Cookie)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIAdapter_UnregisterHardwareContentProtectionTeardownStatus(
	IN	IIDXGIAdapter* This,
	IN	ULONG			Cookie)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIAdapter_QueryVideoMemoryInfo(
	IN	IIDXGIAdapter* This,
	IN	ULONG							NodeIndex,
	IN	DXGI_MEMORY_SEGMENT_GROUP		MemorySegmentGroup,
	OUT	PDXGI_QUERY_VIDEO_MEMORY_INFO	VideoMemoryInfo)
{
	ASSERT(VideoMemoryInfo != NULL);

	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);

	RtlZeroMemory(VideoMemoryInfo, sizeof(*VideoMemoryInfo));
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIAdapter_SetVideoMemoryReservation(
	IN	IIDXGIAdapter* This,
	IN	ULONG							NodeIndex,
	IN	DXGI_MEMORY_SEGMENT_GROUP		MemorySegmentGroup,
	IN	ULONGLONG						Reservation)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIAdapter_RegisterVideoMemoryBudgetChangeNotificationEvent(
	IN	IIDXGIAdapter* This,
	IN	HANDLE			Event,
	OUT	PULONG			Cookie)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIAdapter_UnregisterVideoMemoryBudgetChangeNotification(
	IN	IIDXGIAdapter* This,
	IN	ULONG			Cookie)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIAdapter_GetDesc3(
	IN	IIDXGIAdapter* This,
	OUT	PDXGI_ADAPTER_DESC3	Desc)
{
	// DXGI_ADAPTER_DESC2 and DXGI_ADAPTER_DESC3 are basically the same shit
	// It only differs for "feature level 9 graphics hardware", which the Desc2
	// version of the structure pretends is a "Software Adapter".
	return IIDXGIAdapter_GetDesc2(
		This,
		(PDXGI_ADAPTER_DESC2)Desc);
}

PVOID IIDXGIAdapter4Vtbl[] = {
	IIDXGIAdapter_QueryInterface,
	0,//AddRef
	IIDXGIAdapter_Release,
	0,//SetPrivateData
	0,//SetPrivateDataInterface
	0,//GetPrivateData
	0,//GetParent,
	IIDXGIAdapter_EnumOutputs,
	0,//GetDesc
	0,//CheckInterfaceSupport
	0,//GetDesc1
	IIDXGIAdapter_GetDesc2,
	IIDXGIAdapter_RegisterHardwareContentProtectionTeardownStatusEvent,
	IIDXGIAdapter_UnregisterHardwareContentProtectionTeardownStatus,
	IIDXGIAdapter_QueryVideoMemoryInfo,
	IIDXGIAdapter_SetVideoMemoryReservation,
	IIDXGIAdapter_RegisterVideoMemoryBudgetChangeNotificationEvent,
	IIDXGIAdapter_UnregisterVideoMemoryBudgetChangeNotification,
	IIDXGIAdapter_GetDesc3
};

IDXGIAdapter1* WrapIDXGIAdapter(IDXGIAdapter1* pAdapt)
{
	if (pAdapt == NULL)
		return NULL;

	PVOID obj;
	if (pAdapt->lpVtbl->QueryInterface(pAdapt, &IID_IDXGIAdapter4, &obj) == S_OK)
	{
		pAdapt->lpVtbl->Release(pAdapt);
		return pAdapt;
	}

	IIDXGIAdapter* adapt;
	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IIDXGIAdapter_Release, KEX_VTBL_REPLACING_ALL}
	};
	if (!KexVtblWrap(pAdapt, rpl, IIDXGIAdapter4Vtbl, 1, ARRAYSIZE(IIDXGIAdapter4Vtbl),
						0, (PPKEX_VTBL_WRAPPER)&adapt))
		return NULL;

	return pAdapt;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_QueryInterface(
	IN	IIDXGISwapchain* This,
	IN	REFIID			RefIID,
	OUT	PPVOID			Object)
{
	HRESULT Result;

	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	if (IsEqualIID(RefIID, &IID_IDXGISwapChain2) ||
		IsEqualIID(RefIID, &IID_IDXGISwapChain3) ||
		IsEqualIID(RefIID, &IID_IDXGISwapChain4))
	{

		*Object = This->Thi;
		This->lpVtbl->AddRef(This->Thi);
		return S_OK;
	}

	Result = This->lpVtbl->QueryInterface(This->Thi, RefIID, Object);
	return Result;
}

ULONG STDMETHODCALLTYPE IIDXGISwapchain_Release(
	IIDXGISwapchain* This)
{
	This->lpVtbl->AddRef(This->Thi);
	ULONG RefCount;
	RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 1)
	{
		//
		// DXGI Objects used in D3D will always let their real release operation happen after 
		// D3D device actually flush the allocation pool.
		// So we have to restore the lpVtbl
		// because the object was not actually freed even its RefCount was down to zero,
		// its functions will be called later when the D3D device flush the allocation pool.
		//

		IUnknownVtbl* lpVtbl = (IUnknownVtbl*)This->lpVtbl;
		IUnknown* Thi = (IUnknown*)This->Thi;

		KexVtblUnwrap(&This->Thi->lpVtbl);
		RefCount = lpVtbl->Release(Thi);

		return RefCount;
	}
	
	return This->lpVtbl->Release(This->Thi);
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_SetSourceSize(
	IIDXGISwapchain* This,
	UINT Width,
	UINT Height)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_GetSourceSize(
	IIDXGISwapchain* This,
	OUT  UINT* pWidth,
	OUT  UINT* pHeight)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_SetMaximumFrameLatency(
	IIDXGISwapchain* This,
	UINT MaxLatency)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_GetMaximumFrameLatency(
	IIDXGISwapchain* This,
	OUT  UINT* pMaxLatency)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HANDLE STDMETHODCALLTYPE IIDXGISwapchain_GetFrameLatencyWaitableObject(
	IIDXGISwapchain* This)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return NULL;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_SetMatrixTransform(
	IIDXGISwapchain* This,
	const DXGI_MATRIX_3X2_F* pMatrix)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_GetMatrixTransform(
	IIDXGISwapchain* This,
	OUT  DXGI_MATRIX_3X2_F* pMatrix)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

UINT STDMETHODCALLTYPE IIDXGISwapchain_GetCurrentBackBufferIndex(
	IIDXGISwapchain* This)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_CheckColorSpaceSupport(
	IIDXGISwapchain* This,
	IN  DXGI_COLOR_SPACE_TYPE ColorSpace,
	OUT  UINT* pColorSpaceSupport)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_SetColorSpace1(
	IIDXGISwapchain* This,
	IN  DXGI_COLOR_SPACE_TYPE ColorSpace)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_ResizeBuffers1(
	IIDXGISwapchain* This,
	IN  UINT BufferCount,
	IN  UINT Width,
	IN  UINT Height,
	IN  DXGI_FORMAT Format,
	IN  UINT SwapChainFlags,
	IN  const UINT* pCreationNodeMask,
	IN  IUnknown* const* ppPresentQueue)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGISwapchain_SetHDRMetaData(
	IIDXGISwapchain* This,
	IN  UINT Type,
	IN  UINT Size,
	IN  void* pMetaData)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

PVOID IIDXGISwapchain4Vtbl[] = {
	IIDXGISwapchain_QueryInterface,
	0,//AddRef
	IIDXGISwapchain_Release,
	0,//SetPrivateData
	0,//SetPrivateDataInterface
	0,//GetPrivateData
	0,//GetParent
	0,//GetDevice
	0,//Present
	0,//GetBuffer
	0,//SetFullscreenState
	0,//GetFullscreenState
	0,//GetDesc
	0,//ResizeBuffers
	0,//ResizeTarget
	0,//GetContainingOutput
	0,//GetFrameStatistics
	0,//GetLastPresentCount
	0,//GetDesc1
	0,//GetFullscreenDesc
	0,//GetHwnd
	0,//GetCoreWindow
	0,//Present1
	0,//IsTemporaryMonoSupported
	0,//GetRestrictToOutput
	0,//SetBackgroundColor
	0,//GetBackgroundColor
	0,//SetRotation
	0,//GetRotation
	IIDXGISwapchain_SetSourceSize,
	IIDXGISwapchain_GetSourceSize,
	IIDXGISwapchain_SetMaximumFrameLatency,
	IIDXGISwapchain_GetMaximumFrameLatency,
	IIDXGISwapchain_GetFrameLatencyWaitableObject,
	IIDXGISwapchain_SetMatrixTransform,
	IIDXGISwapchain_GetMatrixTransform,
	IIDXGISwapchain_GetCurrentBackBufferIndex,
	IIDXGISwapchain_CheckColorSpaceSupport,
	IIDXGISwapchain_SetColorSpace1,
	IIDXGISwapchain_ResizeBuffers1,
	IIDXGISwapchain_SetHDRMetaData
};

IDXGISwapChain* WrapIDXGISwapchain(IDXGISwapChain* pAdapt)
{
	if (pAdapt == NULL)
		return NULL;

	PVOID obj;
	if (pAdapt->lpVtbl->QueryInterface(pAdapt, &IID_IDXGISwapChain4, &obj) == S_OK)
	{
		pAdapt->lpVtbl->Release(pAdapt);
		return pAdapt;
	}

	IIDXGISwapchain* adapt;
	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IIDXGISwapchain_Release, KEX_VTBL_REPLACING_ALL}
	};
	if (!KexVtblWrap(pAdapt, rpl, IIDXGISwapchain4Vtbl, 1, ARRAYSIZE(IIDXGISwapchain4Vtbl),
						0, (PPKEX_VTBL_WRAPPER)&adapt))
		return NULL;
	//pAdapt->lpVtbl->AddRef(pAdapt);

	return pAdapt;
}

HRESULT STDMETHODCALLTYPE IIDXGIOutput_QueryInterface(
	IN	IIDXGIOutput* This,
	IN	REFIID			RefIID,
	OUT	PPVOID			Object)
{
	HRESULT Result;

	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	if (IsEqualIID(RefIID, &IID_IDXGIOutput2) ||
		IsEqualIID(RefIID, &IID_IDXGIOutput3) ||
		IsEqualIID(RefIID, &IID_IDXGIOutput4) ||
		IsEqualIID(RefIID, &IID_IDXGIOutput5) ||
		IsEqualIID(RefIID, &IID_IDXGIOutput6))
	{

		*Object = This->Thi;
		This->lpVtbl->AddRef(This->Thi);
		return S_OK;
	}

	Result = This->lpVtbl->QueryInterface(This->Thi, RefIID, Object);
	return Result;
}

ULONG STDMETHODCALLTYPE IIDXGIOutput_Release(
	IIDXGIOutput* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount;
	RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 0)
		KexVtblUnwrap(&lpVtbl);
	return RefCount;
}

BOOL STDMETHODCALLTYPE IIDXGIOutput_SupportsOverlays(
	IIDXGIOutput* This)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return FALSE;
}

HRESULT STDMETHODCALLTYPE IIDXGIOutput_CheckOverlaySupport(
	IIDXGIOutput* This,
	IN  DXGI_FORMAT EnumFormat,
	IN  IUnknown* pConcernedDevice,
	OUT  UINT* pFlags)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIOutput_CheckOverlayColorSpaceSupport(
	IIDXGIOutput* This,
	IN  DXGI_FORMAT Format,
	IN  DXGI_COLOR_SPACE_TYPE ColorSpace,
	IN  IUnknown* pConcernedDevice,
	OUT  UINT* pFlags)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIOutput_DuplicateOutput1(
	IIDXGIOutput* This,
	IN  IUnknown* pDevice,
	IN UINT Flags,
	IN  UINT SupportedFormatsCount,
	IN  const DXGI_FORMAT* pSupportedFormats,
	OUT  IDXGIOutputDuplication** ppOutputDuplication)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIOutput_GetDesc1(
	IIDXGIOutput* This,
	OUT  DXGI_OUTPUT_DESC1* pDesc)
{
	HRESULT Result = This->lpVtbl->GetDesc(This->Thi, (DXGI_OUTPUT_DESC*)pDesc);

	if (SUCCEEDED(Result))
	{
		pDesc->RedPrimary[0] = 0.f;
		pDesc->RedPrimary[1] = 0.f;

		pDesc->GreenPrimary[0] = 0.f;
		pDesc->GreenPrimary[1] = 0.f;

		pDesc->BluePrimary[0] = 0.f;
		pDesc->BluePrimary[1] = 0.f;

		pDesc->WhitePoint[0] = 0.f;
		pDesc->WhitePoint[1] = 0.f;

		pDesc->MinLuminance = 0.f;
		pDesc->MaxLuminance = 200.f;
		pDesc->MaxFullFrameLuminance = 200.f;

		pDesc->BitsPerColor = 8;
		pDesc->ColorSpace = DXGI_COLOR_SPACE_RESERVED;
	}

	return Result;
}

HRESULT STDMETHODCALLTYPE IIDXGIOutput_CheckHardwareCompositionSupport(
	IIDXGIOutput* This,
	OUT  UINT* pFlags)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

PVOID IIDXGIOutput6Vtbl[] = {
	IIDXGIOutput_QueryInterface,
	0,//AddRef
	IIDXGIOutput_Release,
	0,//SetPrivateData
	0,//SetPrivateDataInterface
	0,//GetPrivateData
	0,//GetParent
	0,//GetDesc
	0,//GetDisplayModeList
	0,//FindClosestMatchingMode
	0,//WaitForVBlank
	0,//TakeOwnership
	0,//ReleaseOwnership
	0,//GetGammaControlCapabilities
	0,//SetGammaControl
	0,//GetGammaControl
	0,//SetDisplaySurface
	0,//GetDisplaySurfaceData
	0,//GetFrameStatistics
	0,//GetDisplayModeList1
	0,//FindClosestMatchingMode1
	0,//GetDisplaySurfaceData1
	0,//DuplicateOutput
	IIDXGIOutput_SupportsOverlays,
	IIDXGIOutput_CheckOverlaySupport,
	IIDXGIOutput_CheckOverlayColorSpaceSupport,
	IIDXGIOutput_DuplicateOutput1,
	IIDXGIOutput_GetDesc1,
	IIDXGIOutput_CheckHardwareCompositionSupport
};

IDXGIOutput* WrapIDXGIOutput(IDXGIOutput* pAdapt)
{
	if (pAdapt == NULL)
		return NULL;

	PVOID obj;
	if (pAdapt->lpVtbl->QueryInterface(pAdapt, &IID_IDXGIOutput6, &obj) == S_OK)
	{
		pAdapt->lpVtbl->Release(pAdapt);
		return pAdapt;
	}

	IIDXGIOutput* adapt;
	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IIDXGIOutput_Release, KEX_VTBL_REPLACING_ALL}
	};
	if (!KexVtblWrap(pAdapt, rpl, IIDXGIOutput6Vtbl, 1, ARRAYSIZE(IIDXGIOutput6Vtbl),
						0, (PPKEX_VTBL_WRAPPER)&adapt))
		return NULL;

	return pAdapt;
}

HRESULT STDMETHODCALLTYPE IIDXGIDevice_QueryInterface(
	IN	IIDXGIOutput* This,
	IN	REFIID			RefIID,
	OUT	PPVOID			Object)
{
	HRESULT Result;

	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	if (IsEqualIID(RefIID, &IID_IDXGIDevice3) ||
		IsEqualIID(RefIID, &IID_IDXGIDevice4))
	{

		*Object = This->Thi;
		This->lpVtbl->AddRef(This->Thi);
		return S_OK;
	}

	if (IsEqualIID(RefIID, &IID_ID3D11Device2) ||
		IsEqualIID(RefIID, &IID_ID3D11Device3) ||
		IsEqualIID(RefIID, &IID_ID3D11Device4) ||
		IsEqualIID(RefIID, &IID_ID3D11Device5))
	{

		Result = This->lpVtbl->QueryInterface(This->Thi, &IID_ID3D11Device, Object);
		return Result;
	}

	Result = This->lpVtbl->QueryInterface(This->Thi, RefIID, Object);
	return Result;
}

ULONG STDMETHODCALLTYPE IIDXGIDevice_Release(
	IIDXGIOutput* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount;
	RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 0)
		KexVtblUnwrap(&lpVtbl);
	return RefCount;
}

void STDMETHODCALLTYPE IIDXGIDevice_Trim(
	IIDXGIDevice* This)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

HRESULT STDMETHODCALLTYPE IIDXGIDevice_OfferResources1(
	IIDXGIDevice* This,
	IN  UINT NumResources,
	IN  IDXGIResource* const* ppResources,
	IN  DXGI_OFFER_RESOURCE_PRIORITY Priority,
	IN  UINT Flags)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IIDXGIDevice_ReclaimResources1(
	IIDXGIDevice* This,
	IN  UINT NumResources,
	IN  IDXGIResource* const* ppResources,
	OUT  DXGI_RECLAIM_RESOURCE_RESULTS* pResults)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

PVOID IIDXGIDevice4Vtbl[] = {
	IIDXGIDevice_QueryInterface,
	0,//AddRef
	IIDXGIDevice_Release,
	0,//SetPrivateData
	0,//SetPrivateDataInterface
	0,//GetPrivateData
	0,//GetParent
	0,//GetAdapter
	0,//CreateSurface
	0,//QueryResourceResidency
	0,//SetGPUThreadPriority
	0,//GetGPUThreadPriority
	0,//SetMaximumFrameLatency
	0,//GetMaximumFrameLatency
	0,//OfferResources
	0,//ReclaimResources
	0,//EnqueueSetEvent
	IIDXGIDevice_Trim,
	IIDXGIDevice_OfferResources1,
	IIDXGIDevice_ReclaimResources1
};

IDXGIDevice* WrapIDXGIDevice(IDXGIDevice* pAdapt)
{
	if (pAdapt == NULL)
		return NULL;

	PVOID obj;
	if (pAdapt->lpVtbl->QueryInterface(pAdapt, &IID_IDXGIDevice4, &obj) == S_OK)
	{
		pAdapt->lpVtbl->Release(pAdapt);
		return pAdapt;
	}

	IIDXGIDevice* adapt;
	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IIDXGIDevice_Release, KEX_VTBL_REPLACING_ALL}
	};
	if (!KexVtblWrap(pAdapt, rpl, IIDXGIDevice4Vtbl, 1, ARRAYSIZE(IIDXGIDevice4Vtbl),
						0, (PPKEX_VTBL_WRAPPER)&adapt))
		return NULL;

	return pAdapt;
}

KXDXAPI HRESULT WINAPI DXGIGetDebugInterface1(
	UINT		Flags,
	REFIID		riid,
	OUT	PPVOID	pDebug)
{
	return E_NOINTERFACE;
}

KXDXAPI HRESULT WINAPI DXGIDeclareAdapterRemovalSupport()
{
	return S_OK;
}