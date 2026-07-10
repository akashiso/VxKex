#include "buildcfg.h"
#include "kxdxp.h"
#include <d3d11.h>

#if defined(KEX_TARGET_TYPE_EXE) || defined(KEX_TARGET_TYPE_DLL)
#  if defined(KEX_ARCH_X64)
#    pragma comment(lib, "d3d11_x64.lib")
#  elif defined(KEX_ARCH_X86)
#    pragma comment(lib, "d3d11_x86.lib")
#  endif
#endif


HRESULT STDMETHODCALLTYPE IID3D11DeviceContext_QueryInterface(
    IN	IIDXGIOutput* This,
    IN	REFIID			RefIID,
    OUT	PPVOID			Object)
{
    HRESULT Result;

    ASSERT(This != NULL);
    ASSERT(RefIID != NULL);
    ASSERT(Object != NULL);

    if (IsEqualIID(RefIID, &IID_ID3D11DeviceContext2) ||
        IsEqualIID(RefIID, &IID_ID3D11DeviceContext3) ||
        IsEqualIID(RefIID, &IID_ID3D11DeviceContext4))
    {

        *Object = This->Thi;
        This->lpVtbl->AddRef(This->Thi);
        return S_OK;
    }

    Result = This->lpVtbl->QueryInterface(This->Thi, RefIID, Object);
    return Result;
}

ULONG STDMETHODCALLTYPE IID3D11DeviceContext_Release(
    IIDXGIOutput* This)
{
    PVOID lpVtbl = This->Thi->lpVtbl;
    ULONG RefCount;
    RefCount = This->lpVtbl->Release(This->Thi);

    if (RefCount == 0)
        KexVtblUnwrap(&lpVtbl);
    return RefCount;
}

HRESULT STDMETHODCALLTYPE IID3D11DeviceContext_UpdateTileMappings(
    IID3D11DeviceContext* This,
    IN  ID3D11Resource* pTiledResource,
    IN  UINT NumTiledResourceRegions,
    IN  const D3D11_TILED_RESOURCE_COORDINATE* pTiledResourceRegionStartCoordinates,
    IN  const D3D11_TILE_REGION_SIZE* pTiledResourceRegionSizes,
    IN  ID3D11Buffer* pTilePool,
    IN  UINT NumRanges,
    IN  const UINT* pRangeFlags,
    IN  const UINT* pTilePoolStartOffsets,
    IN  const UINT* pRangeTileCounts,
    IN  UINT Flags)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11DeviceContext_CopyTileMappings(
    IID3D11DeviceContext* This,
    IN  ID3D11Resource* pDestTiledResource,
    IN  const D3D11_TILED_RESOURCE_COORDINATE* pDestRegionStartCoordinate,
    IN  ID3D11Resource* pSourceTiledResource,
    IN  const D3D11_TILED_RESOURCE_COORDINATE* pSourceRegionStartCoordinate,
    IN  const D3D11_TILE_REGION_SIZE* pTileRegionSize,
    IN  UINT Flags)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

void STDMETHODCALLTYPE IID3D11DeviceContext_CopyTiles(
    IID3D11DeviceContext* This,
    IN  ID3D11Resource* pTiledResource,
    IN  const D3D11_TILED_RESOURCE_COORDINATE* pTileRegionStartCoordinate,
    IN  const D3D11_TILE_REGION_SIZE* pTileRegionSize,
    IN  ID3D11Buffer* pBuffer,
    IN  UINT64 BufferStartOffsetInBytes,
    IN  UINT Flags)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

void STDMETHODCALLTYPE IID3D11DeviceContext_UpdateTiles(
    IID3D11DeviceContext* This,
    IN  ID3D11Resource* pDestTiledResource,
    IN  const D3D11_TILED_RESOURCE_COORDINATE* pDestTileRegionStartCoordinate,
    IN  const D3D11_TILE_REGION_SIZE* pDestTileRegionSize,
    IN  const void* pSourceTileData,
    IN  UINT Flags)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

HRESULT STDMETHODCALLTYPE IID3D11DeviceContext_ResizeTilePool(
    IID3D11DeviceContext* This,
    IN  ID3D11Buffer* pTilePool,
    IN  UINT64 NewSizeInBytes)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

void STDMETHODCALLTYPE IID3D11DeviceContext_TiledResourceBarrier(
    IID3D11DeviceContext* This,
    IN  ID3D11DeviceChild* pTiledResourceOrViewAccessBeforeBarrier,
    IN  ID3D11DeviceChild* pTiledResourceOrViewAccessAfterBarrier)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

BOOL STDMETHODCALLTYPE IID3D11DeviceContext_IsAnnotationEnabled(
    IID3D11DeviceContext* This)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return FALSE;
}

void STDMETHODCALLTYPE IID3D11DeviceContext_SetMarkerInt(
    IID3D11DeviceContext* This,
    IN  LPCWSTR pLabel,
    INT Data)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

void STDMETHODCALLTYPE IID3D11DeviceContext_BeginEventInt(
    IID3D11DeviceContext* This,
    IN  LPCWSTR pLabel,
    INT Data)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

void STDMETHODCALLTYPE IID3D11DeviceContext_EndEvent(
    IID3D11DeviceContext* This)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

void STDMETHODCALLTYPE IID3D11DeviceContext_Flush1(
    IID3D11DeviceContext* This,
    D3D11_CONTEXT_TYPE ContextType,
    IN  HANDLE hEvent)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

void STDMETHODCALLTYPE IID3D11DeviceContext_SetHardwareProtectionState(
    IID3D11DeviceContext* This,
    IN  BOOL HwProtectionEnable)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

void STDMETHODCALLTYPE IID3D11DeviceContext_GetHardwareProtectionState(
    IID3D11DeviceContext* This,
    OUT  BOOL* pHwProtectionEnable)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

HRESULT STDMETHODCALLTYPE IID3D11DeviceContext_Signal(
    IID3D11DeviceContext* This,
    IN  ID3D11Fence* pFence,
    IN  UINT64 Value)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11DeviceContext_Wait(
    IID3D11DeviceContext* This,
    IN  ID3D11Fence* pFence,
    IN  UINT64 Value)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

// I surrendered...
PVOID IID3D11DeviceContext4Vtbl[] = {
    IID3D11DeviceContext_QueryInterface,
    0,//AddRef
    IID3D11DeviceContext_Release,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //16 per row
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,//CSGetConstantBuffers1
    0,//SwapDeviceContextState
    0,//ClearView
    IID3D11DeviceContext_UpdateTileMappings,
    IID3D11DeviceContext_CopyTileMappings,
    IID3D11DeviceContext_CopyTiles,
    IID3D11DeviceContext_UpdateTiles,
    IID3D11DeviceContext_ResizeTilePool,
    IID3D11DeviceContext_TiledResourceBarrier,
    IID3D11DeviceContext_IsAnnotationEnabled,
    IID3D11DeviceContext_SetMarkerInt,
    IID3D11DeviceContext_BeginEventInt,
    IID3D11DeviceContext_EndEvent,
    IID3D11DeviceContext_Flush1, // lpVtbl[144]
    IID3D11DeviceContext_SetHardwareProtectionState,
    IID3D11DeviceContext_GetHardwareProtectionState,
    IID3D11DeviceContext_Signal,
    IID3D11DeviceContext_Wait
};

ID3D11DeviceContext* WrapID3D11DeviceContext(ID3D11DeviceContext* pAdapt)
{
    if (pAdapt == NULL)
        return NULL;

    PVOID obj;
    if (pAdapt->lpVtbl->QueryInterface(pAdapt, &IID_ID3D11DeviceContext4, &obj) == S_OK)
    {
        pAdapt->lpVtbl->Release(pAdapt);
        return pAdapt;
    }

    IID3D11Device* adapt;
    KEX_VTBL_REPLACING_ENTRY rpl[] = {
        {offsetof(IUnknownVtbl, Release), IID3D11DeviceContext_Release, KEX_VTBL_REPLACING_ALL}
    };
    if (!KexVtblWrap(pAdapt, rpl, IID3D11DeviceContext4Vtbl, 1, ARRAYSIZE(IID3D11DeviceContext4Vtbl),
                        0, (PPKEX_VTBL_WRAPPER)&adapt))
        return NULL;

    return pAdapt;
}


HRESULT STDMETHODCALLTYPE IID3D11Device_QueryInterface(
    IN	IIDXGIOutput* This,
    IN	REFIID			RefIID,
    OUT	PPVOID			Object)
{
    HRESULT Result;

    ASSERT(This != NULL);
    ASSERT(RefIID != NULL);
    ASSERT(Object != NULL);

    if (IsEqualIID(RefIID, &IID_ID3D11Device2) ||
        IsEqualIID(RefIID, &IID_ID3D11Device3) ||
        IsEqualIID(RefIID, &IID_ID3D11Device4) ||
        IsEqualIID(RefIID, &IID_ID3D11Device5))
    {

        *Object = This->Thi;
        This->lpVtbl->AddRef(This->Thi);
        return S_OK;
    }

    Result = This->lpVtbl->QueryInterface(This->Thi, RefIID, Object);
    return Result;
}

ULONG STDMETHODCALLTYPE IID3D11Device_Release(
    IIDXGIOutput* This)
{
    PVOID lpVtbl = This->Thi->lpVtbl;
    ULONG RefCount;
    RefCount = This->lpVtbl->Release(This->Thi);

    if (RefCount == 0)
        KexVtblUnwrap(&lpVtbl);
    return RefCount;
}

void STDMETHODCALLTYPE IID3D11Device_GetImmediateContext1(
    IID3D11Device* This,
    OUT  ID3D11DeviceContext1** ppImmediateContext)
{
    ID3D11DeviceContext* ctx;

    This->lpVtbl->GetImmediateContext1(This->Thi, &ctx);
    WrapID3D11DeviceContext(ctx);

    *ppImmediateContext = (ID3D11DeviceContext1*)ctx;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateDeferredContext1(
    IID3D11Device* This,
    UINT ContextFlags,
    OUT  ID3D11DeviceContext1** ppDeferredContext)
{
    ID3D11DeviceContext* ctx;
    HRESULT Result;

    Result = This->lpVtbl->CreateDeferredContext1(This->Thi, ContextFlags, &ctx);
    if (FAILED(Result))
        return Result;

    if (WrapID3D11DeviceContext(ctx) == NULL)
        return E_OUTOFMEMORY;

    *ppDeferredContext = (ID3D11DeviceContext1*)ctx;
    return S_OK;
}

void STDMETHODCALLTYPE IID3D11Device_GetImmediateContext2(
    IID3D11Device* This,
    OUT  ID3D11DeviceContext2** ppImmediateContext)
{
    ID3D11DeviceContext* ctx;

    This->lpVtbl->GetImmediateContext1(This->Thi, &ctx);
    WrapID3D11DeviceContext(ctx);

    *ppImmediateContext = (ID3D11DeviceContext2*)ctx;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateDeferredContext2(
    IID3D11Device* This,
    UINT ContextFlags,
    OUT  ID3D11DeviceContext2** ppDeferredContext)
{
    ID3D11DeviceContext* ctx;
    HRESULT Result;

    Result = This->lpVtbl->CreateDeferredContext1(This->Thi, ContextFlags, &ctx);
    if (FAILED(Result))
        return Result;

    if (WrapID3D11DeviceContext(ctx) == NULL)
        return E_OUTOFMEMORY;

    *ppDeferredContext = (ID3D11DeviceContext2*)ctx;
    return S_OK;
}

void STDMETHODCALLTYPE IID3D11Device_GetResourceTiling(
    IID3D11Device* This,
    IN  ID3D11Resource* pTiledResource,
    OUT  UINT* pNumTilesForEntireResource,
    OUT  D3D11_PACKED_MIP_DESC* pPackedMipDesc,
    OUT  D3D11_TILE_SHAPE* pStandardTileShapeForNonPackedMips,
     UINT* pNumSubresourceTilings,
    IN  UINT FirstSubresourceTilingToGet,
    OUT  D3D11_SUBRESOURCE_TILING* pSubresourceTilingsForNonPackedMips)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CheckMultisampleQualityLevels1(
    IID3D11Device* This,
    IN  DXGI_FORMAT Format,
    IN  UINT SampleCount,
    IN  UINT Flags,
    IN  UINT* pNumQualityLevels)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateTexture2D1(
    IID3D11Device* This,
    IN  const D3D11_TEXTURE2D_DESC1* pDesc1,
    IN  const D3D11_SUBRESOURCE_DATA* pInitialData,
    OUT  ID3D11Texture2D1** ppTexture2D)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateTexture3D1(
    IID3D11Device* This,
    IN  const D3D11_TEXTURE3D_DESC1* pDesc1,
    IN  const D3D11_SUBRESOURCE_DATA* pInitialData,
    OUT  ID3D11Texture3D1** ppTexture3D)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateRasterizerState2(
    IID3D11Device* This,
    IN  const D3D11_RASTERIZER_DESC2* pRasterizerDesc,
    OUT  ID3D11RasterizerState2** ppRasterizerState)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateShaderResourceView1(
    IID3D11Device* This,
    IN  ID3D11Resource* pResource,
    IN  const D3D11_SHADER_RESOURCE_VIEW_DESC1* pDesc1,
    OUT  ID3D11ShaderResourceView1** ppSRView1)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateUnorderedAccessView1(
    IID3D11Device* This,
    IN  ID3D11Resource* pResource,
    IN  const D3D11_UNORDERED_ACCESS_VIEW_DESC1* pDesc1,
    OUT  ID3D11UnorderedAccessView1** ppUAView1)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateRenderTargetView1(
    IID3D11Device* This,
    IN  ID3D11Resource* pResource,
    IN  const D3D11_RENDER_TARGET_VIEW_DESC1* pDesc1,
    OUT  ID3D11RenderTargetView1** ppRTView1)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateQuery1(
    IID3D11Device* This,
    IN  const D3D11_QUERY_DESC1* pQueryDesc1,
    OUT  ID3D11Query1** ppQuery1)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

void STDMETHODCALLTYPE IID3D11Device_GetImmediateContext3(
    IID3D11Device* This,
    OUT  ID3D11DeviceContext3** ppImmediateContext)
{
    ID3D11DeviceContext* ctx;

    This->lpVtbl->GetImmediateContext1(This->Thi, &ctx);
    WrapID3D11DeviceContext(ctx);

    *ppImmediateContext = (ID3D11DeviceContext3*)ctx;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateDeferredContext3(
    IID3D11Device* This,
    UINT ContextFlags,
    OUT  ID3D11DeviceContext3** ppDeferredContext)
{
    ID3D11DeviceContext* ctx;
    HRESULT Result;

    Result = This->lpVtbl->CreateDeferredContext1(This->Thi, ContextFlags, &ctx);
    if (FAILED(Result))
        return Result;

    if (WrapID3D11DeviceContext(ctx) == NULL)
        return E_OUTOFMEMORY;

    *ppDeferredContext = (ID3D11DeviceContext3*)ctx;
    return S_OK;
}

void STDMETHODCALLTYPE IID3D11Device_WriteToSubresource(
    IID3D11Device* This,
    IN  ID3D11Resource* pDstResource,
    IN  UINT DstSubresource,
    IN  const D3D11_BOX* pDstBox,
    IN  const void* pSrcData,
    IN  UINT SrcRowPitch,
    IN  UINT SrcDepthPitch)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

void STDMETHODCALLTYPE IID3D11Device_ReadFromSubresource(
    IID3D11Device* This,
    IN  void* pDstData,
    IN  UINT DstRowPitch,
    IN  UINT DstDepthPitch,
    IN  ID3D11Resource* pSrcResource,
    IN  UINT SrcSubresource,
    IN  const D3D11_BOX* pSrcBox)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

HRESULT STDMETHODCALLTYPE IID3D11Device_RegisterDeviceRemovedEvent(
    IID3D11Device* This,
    IN  HANDLE hEvent,
    IN  DWORD* pdwCookie)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

void STDMETHODCALLTYPE IID3D11Device_UnregisterDeviceRemoved(
    IID3D11Device* This,
    IN  DWORD dwCookie)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
}

HRESULT STDMETHODCALLTYPE IID3D11Device_OpenSharedFence(
    IID3D11Device* This,
    IN  HANDLE hFence,
    IN  REFIID ReturnedInterface,
    OUT  void** ppFence)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D11Device_CreateFence(
    IID3D11Device* This,
    IN  UINT64 InitialValue,
    IN  D3D11_FENCE_FLAG Flags,
    IN  REFIID ReturnedInterface,
    OUT  void** ppFence)
{
    KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
    return E_NOTIMPL;
}

PVOID IID3D11Device5Vtbl[] = {
    IID3D11Device_QueryInterface,
    0,//AddRef
    IID3D11Device_Release,
    0,//CreateBuffer
    0,//CreateTexture1D
    0,//CreateTexture2D
    0,//CreateTexture3D
    0,//CreateShaderResourceView
    0,//CreateUnorderedAccessView
    0,//CreateRenderTargetView
    0,//CreateDepthStencilView
    0,//CreateInputLayout
    0,//CreateVertexShader
    0,//CreateGeometryShader
    0,//CreateGeometryShaderWithStreamOutput
    0,//CreatePixelShader
    0,//CreateHullShader
    0,//CreateDomainShader
    0,//CreateComputeShader
    0,//CreateClassLinkage
    0,//CreateBlendState
    0,//CreateDepthStencilState
    0,//CreateRasterizerState
    0,//CreateSamplerState
    0,//CreateQuery
    0,//CreatePredicate
    0,//CreateCounter
    0,//CreateDeferredContext
    0,//OpenSharedResource
    0,//CheckFormatSupport
    0,//CheckMultisampleQualityLevels
    0,//CheckCounterInfo
    0,//CheckCounter
    0,//CheckFeatureSupport
    0,//GetPrivateData
    0,//SetPrivateData
    0,//SetPrivateDataInterface
    0,//GetFeatureLevel
    0,//GetCreationFlags
    0,//GetDeviceRemovedReason
    0,//GetImmediateContext
    0,//SetExceptionMode
    IID3D11Device_GetImmediateContext1,
    IID3D11Device_CreateDeferredContext1,
    0,//CreateBlendState1
    0,//CreateRasterizerState1
    0,//CreateDeviceContextState
    0,//OpenSharedResource1
    0,//OpenSharedResourceByName
    IID3D11Device_GetImmediateContext2,
    IID3D11Device_CreateDeferredContext2,
    IID3D11Device_GetResourceTiling,
    IID3D11Device_CheckMultisampleQualityLevels1,
    IID3D11Device_CreateTexture2D1,
    IID3D11Device_CreateTexture3D1,
    IID3D11Device_CreateRasterizerState2,
    IID3D11Device_CreateShaderResourceView1,
    IID3D11Device_CreateUnorderedAccessView1,
    IID3D11Device_CreateRenderTargetView1,
    IID3D11Device_CreateQuery1,
    IID3D11Device_GetImmediateContext3,
    IID3D11Device_CreateDeferredContext3,
    IID3D11Device_WriteToSubresource,
    IID3D11Device_ReadFromSubresource,
    IID3D11Device_RegisterDeviceRemovedEvent,
    IID3D11Device_UnregisterDeviceRemoved,
    IID3D11Device_OpenSharedFence,
    IID3D11Device_CreateFence
};

KXDXAPI IDXGIDevice* STDMETHODCALLTYPE KexWrapID3D11RawDevice(IDXGIDevice* pAdapt)
{
    if (pAdapt == NULL)
        return NULL;

    if (WrapIDXGIDevice(pAdapt) == NULL)
        return NULL;

    PVOID obj;
    if (pAdapt->lpVtbl->QueryInterface(pAdapt, &IID_ID3D11Device5, &obj) == S_OK)
    {
        pAdapt->lpVtbl->Release(pAdapt);
        return pAdapt;
    }

    IIDXGIDevice* adapt;
    KEX_VTBL_REPLACING_ENTRY rpl[] = {
        {offsetof(IUnknownVtbl, Release), IID3D11Device_Release, KEX_VTBL_REPLACING_ALL}
    };
    if (!KexVtblWrap(pAdapt, rpl, IID3D11Device5Vtbl, 1, ARRAYSIZE(IID3D11Device5Vtbl),
                        0, (PPKEX_VTBL_WRAPPER)&adapt))
        return NULL;

    return pAdapt;
}

KXDXAPI HRESULT WINAPI Ext_D3D11CreateDevice(
	IN							 IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE				 DriverType,
	HMODULE						 Software,
	UINT						 Flags,
	IN  const D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT						 FeatureLevels,
	UINT						 SDKVersion,
	OUT	ID3D11Device** ppDevice,
	OUT	D3D_FEATURE_LEVEL* pFeatureLevel,
	OUT	ID3D11DeviceContext** ppImmediateContext
)
{
	//
	// These flags are not supported by Windows 7
	//

	Flags &= ~0x40; // D3D11_CREATE_DEVICE_DEBUGGABLE
	Flags &= ~0x80; // D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY
	Flags &= ~0x100; // D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT
	Flags &= ~0x800; // D3D11_CREATE_DEVICE_VIDEO_SUPPORT

	HRESULT Result;
	ID3D11Device* device;
	Result = D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, &device, pFeatureLevel, ppImmediateContext);

	unless(KexData->IfeoParameters.DisableAppSpecific)
	{
        //
        // APPSPECIFICHACK : Paint.NET will try to create a D3D11 device
        // for GpuKeepAlive task and query its parent as DXGIFactory7.
        // so we have to wrap the factory.
        //
		if (AshExeBaseNameIs(L"paintdotnet.exe")
			&& device)
		{
			IDXGIDevice* dxgiDevice;
			IDXGIAdapter* dxgiAdapter;
			IDXGIFactory2* dxgiFactory;

			ID3D11Device_QueryInterface(device, &IID_IDXGIDevice, &dxgiDevice);
			IDXGIDevice_GetAdapter(dxgiDevice, &dxgiAdapter);
			IDXGIAdapter_GetParent(dxgiAdapter, &IID_IDXGIFactory2, &dxgiFactory);

			CreateIIDXGIFactory2To7(dxgiFactory, 0);

			IDXGIDevice_Release(dxgiDevice);
			IDXGIAdapter_Release(dxgiAdapter);
			IDXGIFactory_Release(dxgiFactory);
		}
	}
	*ppDevice = device;
	return Result;
}

KXDXAPI HRESULT WINAPI D3D11On12CreateDevice(
    IN	IUnknown* pDevice,
    UINT						Flags,
    IN	const D3D_FEATURE_LEVEL* pFeatureLevels	OPTIONAL,
    UINT						FeatureLevels,
    IN	IUnknown* const* ppCommandQueues	OPTIONAL,
    UINT						NumQueues,
    UINT						NodeMask,
    OUT	ID3D11Device** ppDevice OPTIONAL,
    OUT	ID3D11DeviceContext** ppImmediateContext	OPTIONAL,
    OUT	D3D_FEATURE_LEVEL* pChosenFeatureLevel	OPTIONAL)
{
    return Ext_D3D11CreateDevice((IDXGIAdapter*)pDevice, D3D_DRIVER_TYPE_HARDWARE, NULL, Flags, pChosenFeatureLevel, FeatureLevels, D3D11_SDK_VERSION, ppDevice, pChosenFeatureLevel, ppImmediateContext);
}