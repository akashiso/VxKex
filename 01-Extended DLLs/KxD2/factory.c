#include "buildcfg.h"
#include "kxd2p.h"

//
// factory.c
//

int _fltused = 0;
UINT NumFactoryAlive = 0;


HRESULT STDMETHODCALLTYPE IID2D1Factory1_QueryInterface(
	IID2D1Factory1To8* This,
	REFIID riid,
	OUT void** ppvObject)
{

	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	if (IsEqualIID(riid, &IID_IUnknown) ||
		IsEqualIID(riid, &IID_ID2D1Factory) ||
		IsEqualIID(riid, &IID_ID2D1Factory1) ||
		IsEqualIID(riid, &IID_ID2D1Factory2) ||
		IsEqualIID(riid, &IID_ID2D1Factory3) ||
		IsEqualIID(riid, &IID_ID2D1Factory4) ||
		IsEqualIID(riid, &IID_ID2D1Factory5) ||
		IsEqualIID(riid, &IID_ID2D1Factory6) ||
		IsEqualIID(riid, &IID_ID2D1Factory7))
	{
		*ppvObject = This->Thi;
		This->lpVtbl->Base.Base.AddRef((IUnknown*)This->Thi);

		return S_OK;
	}

	return This->lpVtbl->Base.Base.QueryInterface((IUnknown*)This->Thi, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IID2D1Factory1_Release(
	IID2D1Factory1To8* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount;

	This->lpVtbl->Base.Base.AddRef((IUnknown*)This->Thi);
	RefCount = This->lpVtbl->Base.Base.Release((IUnknown*)This->Thi);
	if (RefCount == 1)
		IID2D_UnregisterBuiltinEffects(This->Thi);

	RefCount = This->lpVtbl->Base.Base.Release((IUnknown*)This->Thi);
	if (RefCount == 0)
	{
		KexVtblUnwrap(&lpVtbl);

		UINT numAlive = InterlockedDecrement(&NumFactoryAlive);
		if (numAlive == 0)
			DeleteCriticalSection(&effectCreatingCS);
	}
	return RefCount;
}

HRESULT STDMETHODCALLTYPE IID2D1Factory1_CreateWicBitmapRenderTarget(
	IID2D1Factory1To8* This,
	IN IWICBitmap* target,
	IN CONST D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
	OUT ID2D1RenderTarget** renderTarget
)
{
	HRESULT Result;
	ID2D1RenderTarget* rTarget;

	Result = This->lpVtbl->Base.CreateWicBitmapRenderTarget((ID2D1Factory*)This->Thi, target, renderTargetProperties, &rTarget);
	if (FAILED(Result))
		return Result;

	*renderTarget = (ID2D1RenderTarget*)WrapRenderTarget(rTarget, This);
	if (*renderTarget == NULL)
		return E_OUTOFMEMORY;

	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1Factory1_CreateHwndRenderTarget(
	IID2D1Factory1To8* This,
	IN CONST D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
	IN CONST D2D1_HWND_RENDER_TARGET_PROPERTIES* hwndRenderTargetProperties,
	OUT ID2D1HwndRenderTarget** hwndRenderTarget
)
{
	HRESULT Result;
	ID2D1HwndRenderTarget* rTarget;

	Result = This->lpVtbl->Base.CreateHwndRenderTarget((ID2D1Factory*)This->Thi, renderTargetProperties, hwndRenderTargetProperties, &rTarget);
	if (FAILED(Result))
		return Result;

	*hwndRenderTarget = rTarget;
	Result = WrapAnyRenderTarget((ID2D1RenderTarget*)rTarget, This, Result);
	if (FAILED(Result))
		IUnknown_Release((IUnknown*)rTarget);

	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1Factory1_CreateDxgiSurfaceRenderTarget(
	IID2D1Factory1To8* This,
	IN IDXGISurface* dxgiSurface,
	IN CONST D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
	OUT ID2D1RenderTarget** renderTarget
)
{
	HRESULT Result;
	ID2D1RenderTarget* rTarget;

	Result = This->lpVtbl->Base.CreateDxgiSurfaceRenderTarget((ID2D1Factory*)This->Thi, dxgiSurface, renderTargetProperties, &rTarget);
	if (FAILED(Result))
		return Result;

	*renderTarget = (ID2D1RenderTarget*)WrapRenderTarget(rTarget, This);
	if (*renderTarget == NULL)
	{
		IUnknown_Release((IUnknown*)rTarget);
		return E_OUTOFMEMORY;
	}

	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1Factory1_CreateDCRenderTarget(
	IID2D1Factory1To8* This,
	IN CONST D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
	OUT ID2D1DCRenderTarget** dcRenderTarget
)
{
	HRESULT Result;
	ID2D1DCRenderTarget* rTarget;

	Result = This->lpVtbl->Base.CreateDCRenderTarget((ID2D1Factory*)This->Thi, renderTargetProperties, &rTarget);
	if (FAILED(Result))
		return Result;

	*dcRenderTarget = rTarget;
	Result = WrapAnyRenderTarget((ID2D1RenderTarget*)rTarget, This, Result);
	if (FAILED(Result))
		IUnknown_Release((IUnknown*)rTarget);

	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1Factory1_CreateDevice(
	IID2D1Factory1To8* This,
	IN IDXGIDevice* dxgiDevice,
	OUT ID2D1Device** d2dDevice
)
{
	HRESULT Result;
	ID2D1Device* device;
	Result = This->lpVtbl->CreateDevice(This->Thi, dxgiDevice, &device);

	if (FAILED(Result))
		return Result;

	*d2dDevice = device;
	Result = WrapDevice(device, dxgiDevice, This, Result);
	if (FAILED(Result))
		IUnknown_Release((IUnknown*)device);

	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1Factory1_RegisterEffectFromStream(
	IID2D1Factory1To8* This,
	IN REFCLSID classId,
	IN IStream* propertyXml,
	IN CONST D2D1_PROPERTY_BINDING* bindings,
	UINT32 bindingsCount,
	IN CONST PD2D1_EFFECT_FACTORY effectFactory
)
{
	IID2D1EffectFactoryList* list = This->effectFactoryList;
	IID2D1EffectFactoryList** ppNext = &This->effectFactoryList;

	while (list)
	{
		if (IsEqualCLSID(classId, &list->clsid))
			goto RET;

		ppNext = &list->next;
		list = list->next;
	}

	IID2D1EffectFactoryList* factory;
	factory = (IID2D1EffectFactoryList*)CoTaskMemAlloc(sizeof(IID2D1EffectFactoryList));
	if (factory == NULL)
		return E_OUTOFMEMORY;

	factory->factory = effectFactory;
	factory->next = NULL;
	CopyMemory(&factory->clsid, classId, sizeof(CLSID));

	*ppNext = factory;
RET:
	return This->lpVtbl->RegisterEffectFromStream(This->Thi, classId, propertyXml, bindings, bindingsCount, _IID2D1_CreateEffectDispatch);
}

HRESULT STDMETHODCALLTYPE IID2D1Factory1_RegisterEffectFromString(
	IID2D1Factory1To8* This,
	IN REFCLSID classId,
	IN PCWSTR propertyXml,
	IN CONST D2D1_PROPERTY_BINDING* bindings,
	UINT32 bindingsCount,
	IN CONST PD2D1_EFFECT_FACTORY effectFactory
)
{
	if (IID2D_IsBuiltinEffects(classId))
		return 0x88990028L; // D2DERR_EFFECT_IS_NOT_REGISTERED

	IID2D1EffectFactoryList* list = This->effectFactoryList;
	IID2D1EffectFactoryList** ppNext = &This->effectFactoryList;

	while (list)
	{
		if (IsEqualCLSID(classId, &list->clsid))
			goto RET;

		ppNext = &list->next;
		list = list->next;
	}

	IID2D1EffectFactoryList* factory;
	factory = (IID2D1EffectFactoryList*)CoTaskMemAlloc(sizeof(IID2D1EffectFactoryList));
	if (factory == NULL)
		return E_OUTOFMEMORY;

	factory->factory = effectFactory;
	factory->next = NULL;
	CopyMemory(&factory->clsid, classId, sizeof(CLSID));

	*ppNext = factory;
RET:
	return This->lpVtbl->RegisterEffectFromString(This->Thi, classId, propertyXml, bindings, bindingsCount, _IID2D1_CreateEffectDispatch);
}

HRESULT STDMETHODCALLTYPE IID2D1Factory1_UnregisterEffect(
	IID2D1Factory1To8* This,
	IN REFCLSID classId
)
{
	if (IID2D_IsBuiltinEffects(classId))
		return 0x88990028L; // D2DERR_EFFECT_IS_NOT_REGISTERED

	IID2D1EffectFactoryList* list = This->effectFactoryList;
	IID2D1EffectFactoryList* prev = NULL;

	while (list)
	{
		if (IsEqualCLSID(classId, &list->clsid))
			break;

		prev = list;
		list = list->next;
	}

	if (list != NULL && prev != NULL)
		prev->next = list->next;
	else if (prev == NULL)
		This->effectFactoryList = NULL;

	CoTaskMemFree(list);

	return This->lpVtbl->UnregisterEffect(This->Thi, classId);
}


void* IID2D1Factory1To8Vtbl[] = {
	IID2D1Factory1_QueryInterface, // 0
	0,//IID2D1Factory1_AddRef,
	IID2D1Factory1_Release,
	0,//IID2D1Factory1_ReloadSystemMetrics,
	0,//IID2D1Factory1_GetDesktopDpi,
	0,//IID2D1Factory1_CreateRectangleGeometry,
	0,//IID2D1Factory1_CreateRoundedRectangleGeometry,
	0,//IID2D1Factory1_CreateEllipseGeometry,
	0,//IID2D1Factory1_CreateGeometryGroup,
	0,//IID2D1Factory1_CreateTransformedGeometry,
	0,//IID2D1Factory1_CreatePathGeometry,
	0,//IID2D1Factory1_CreateStrokeStyle,
	0,//IID2D1Factory1_CreateDrawingStateBlock,
	IID2D1Factory1_CreateWicBitmapRenderTarget,
	IID2D1Factory1_CreateHwndRenderTarget,
	IID2D1Factory1_CreateDxgiSurfaceRenderTarget,
	IID2D1Factory1_CreateDCRenderTarget,
	IID2D1Factory1_CreateDevice,
	0,//IID2D1Factory1_CreateStrokeStyle1,
	0,//IID2D1Factory1_CreatePathGeometry1,
	0,//IID2D1Factory1_CreateDrawingStateBlock1, // 20
	0,//IID2D1Factory1_CreateGdiMetafile,
	IID2D1Factory1_RegisterEffectFromStream,
	IID2D1Factory1_RegisterEffectFromString,
	IID2D1Factory1_UnregisterEffect,
	0,//IID2D1Factory1_GetRegisteredEffects,
	0,//IID2D1Factory1_GetEffectProperties,
	IID2D1Factory1_CreateDevice,
	IID2D1Factory1_CreateDevice,
	IID2D1Factory1_CreateDevice,
	IID2D1Factory1_CreateDevice,
	IID2D1Factory1_CreateDevice,
	IID2D1Factory1_CreateDevice //32
};

IID2D1Factory1To8* CreateIID2D1Factory1To8(ID2D1Factory1* pFactory1)
{
	IID2D1Factory1To8* factory;

	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IID2D1Factory1_Release, KEX_VTBL_REPLACING_ALL}
	};
	if (!KexVtblWrap(pFactory1, rpl, IID2D1Factory1To8Vtbl, 1, ARRAYSIZE(IID2D1Factory1To8Vtbl),
						sizeof(PVOID) * 2, (PPKEX_VTBL_WRAPPER)&factory))
		return NULL;

	factory->effectFactoryList = NULL;

	if (NumFactoryAlive == 0)
		InitializeCriticalSection(&effectCreatingCS);
	InterlockedIncrement(&NumFactoryAlive);

	return (IID2D1Factory1To8*)pFactory1;
}