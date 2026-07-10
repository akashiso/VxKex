#include "buildcfg.h"
#include "kxd2p.h"

HRESULT STDMETHODCALLTYPE IID2D1Device_QueryInterface(
	IID2D1Device0To6* This,
	REFIID riid,
	OUT void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	if (IsEqualIID(riid, &IID_IUnknown) ||
		IsEqualIID(riid, &IID_ID2D1Device) ||
		IsEqualIID(riid, &IID_ID2D1Device1) ||
		IsEqualIID(riid, &IID_ID2D1Device2) ||
		IsEqualIID(riid, &IID_ID2D1Device3) ||
		IsEqualIID(riid, &IID_ID2D1Device4) ||
		IsEqualIID(riid, &IID_ID2D1Device5) ||
		IsEqualIID(riid, &IID_ID2D1Device6))
	{
		*ppvObject = This->Thi;
		This->lpVtbl->AddRef(This->Thi);

		return S_OK;
	}

	return This->lpVtbl->QueryInterface(This->Thi, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IID2D1Device_Release(
	IID2D1Device0To6* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount;
	RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 0)
		KexVtblUnwrap(&lpVtbl);
	return RefCount;
}

HRESULT STDMETHODCALLTYPE IID2D1Device_CreateDeviceContext(
	IID2D1Device0To6* This,
	D2D1_DEVICE_CONTEXT_OPTIONS options,
	OUT ID2D1DeviceContext** deviceContext
)
{
	HRESULT Result;
	ID2D1DeviceContext* dCtx;
	Result = This->lpVtbl->CreateDeviceContext(This->Thi, options, &dCtx);

	if (FAILED(Result))
		return Result;

	*deviceContext = dCtx;
	Result = WrapDeviceContext(dCtx, This->factory, Result);
	if (FAILED(Result))
		IUnknown_Release((IUnknown*)dCtx);

	return Result;
}

D2D1_RENDERING_PRIORITY STDMETHODCALLTYPE IID2D1Device1_GetRenderingPriority(
	IID2D1Device0To6* This
)
{
	return This->renderPrior;
}

void STDMETHODCALLTYPE IID2D1Device1_SetRenderingPriority(
	IID2D1Device0To6* This,
	D2D1_RENDERING_PRIORITY renderingPriority
)
{
	This->renderPrior = renderingPriority;
}

void STDMETHODCALLTYPE IID2D1Device2_FlushDeviceContexts(
	IID2D1Device0To6* This,
	IN ID2D1Bitmap* bitmap
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

HRESULT STDMETHODCALLTYPE IID2D1Device2_GetDxgiDevice(
	IID2D1Device0To6* This,
	OUT IDXGIDevice** dxgiDevice
)
{
	if (This->dxgiDevice == NULL)
		return This->lpVtbl->QueryInterface(This->Thi, &IID_IDXGIDevice2, dxgiDevice);
	else
	{
		*dxgiDevice = This->dxgiDevice;
		IUnknown_AddRef((IUnknown*)This->dxgiDevice);
	}

	return S_OK;
}

void STDMETHODCALLTYPE IID2D1Device4_SetMaximumColorGlyphCacheMemory(
	IID2D1Device0To6* This,
	UINT64 maximumInBytes
)
{
	This->maxColorGlyphCache = maximumInBytes;
}

UINT64 STDMETHODCALLTYPE IID2D1Device4_GetMaximumColorGlyphCacheMemory(
	IID2D1Device0To6* This
)
{
	return This->maxColorGlyphCache;
}

void* IID2D1Device0To6Vtbl[] = {
	IID2D1Device_QueryInterface,
	0,//IID2D1Device_AddRef,
	IID2D1Device_Release,
	0,//IID2D1Device_GetFactory,
	IID2D1Device_CreateDeviceContext,
	0,//IID2D1Device_CreatePrintControl,
	0,//IID2D1Device_SetMaximumTextureMemory,
	0,//IID2D1Device_GetMaximumTextureMemory,
	0,//IID2D1Device_ClearResources,
	// 1
	IID2D1Device1_GetRenderingPriority,
	IID2D1Device1_SetRenderingPriority,
	IID2D1Device_CreateDeviceContext,
	// 2
	IID2D1Device_CreateDeviceContext,
	IID2D1Device2_FlushDeviceContexts,
	IID2D1Device2_GetDxgiDevice,
	// 3
	IID2D1Device_CreateDeviceContext,
	// 4
	IID2D1Device_CreateDeviceContext,
	IID2D1Device4_SetMaximumColorGlyphCacheMemory,
	IID2D1Device4_GetMaximumColorGlyphCacheMemory,
	// 5
	IID2D1Device_CreateDeviceContext,
	// 6
	IID2D1Device_CreateDeviceContext
};

HRESULT WrapDevice(
	ID2D1Device* device,
	IDXGIDevice* dxgiDevice,
	void* fact,
	HRESULT dispatchResult)
{
	IID2D1Device0To6* device2;

	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IID2D1Device_Release, KEX_VTBL_REPLACING_ALL}
	};
	if (!KexVtblWrap(device, rpl, IID2D1Device0To6Vtbl, 1, ARRAYSIZE(IID2D1Device0To6Vtbl),
						sizeof(IID2D1Device0To6) - sizeof(KEX_VTBL_WRAPPER), (PPKEX_VTBL_WRAPPER)&device2))
		return E_OUTOFMEMORY;

	device2->factory = (ID2D1Factory*)fact;
	device2->dxgiDevice = dxgiDevice;
	device2->maxColorGlyphCache = 1048576;
	device2->renderPrior = D2D1_RENDERING_PRIORITY_NORMAL;

	return dispatchResult;
}