#include "buildcfg.h"
#include "kxd2p.h"


//
// misc.c
//


HRESULT STDMETHODCALLTYPE IID2D1GeometryRealization_QueryInterface(
	IID2D1GeometryRealization* This,
	IN REFIID riid,
	OUT  void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	if (IsEqualIID(riid, &IID_IUnknown) ||
		IsEqualIID(riid, &IID_ID2D1Resource) ||
		IsEqualIID(riid, &IID_ID2D1GeometryRealization))
	{
		*ppvObject = This;
		InterlockedIncrement(&This->RefCount);

		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE IID2D1GeometryRealization_AddRef(
	IID2D1GeometryRealization* This)
{
	return InterlockedIncrement(&This->RefCount);
}

ULONG STDMETHODCALLTYPE IID2D1GeometryRealization_Release(
	IID2D1GeometryRealization* This)
{
	ULONG RefCount;
	RefCount = InterlockedDecrement(&This->RefCount);

	if (RefCount == 0)
	{
		ID2D1Geometry_Release(This->geometry);
		if (!This->isFilled)
			ID2D1StrokeStyle_Release(This->strokeStyle);

		CoTaskMemFree(This);
	}
	return RefCount;
}

void STDMETHODCALLTYPE IID2D1GeometryRealization_GetFactory(
	IID2D1GeometryRealization* This,
	OUT ID2D1Factory** factory
)
{
	IUnknown_AddRef((IUnknown*)This->factory);
	*factory = (ID2D1Factory*)This->factory;
}

void* IID2D1GeometryRealizationVtbl[] = {
	IID2D1GeometryRealization_QueryInterface, // 0
	IID2D1GeometryRealization_AddRef,
	IID2D1GeometryRealization_Release,
	IID2D1GeometryRealization_GetFactory
};

IID2D1GeometryRealization* CreateFilledGeometryRealization(
	IN ID2D1Geometry* geometry,
	FLOAT flatteningTolerance,
	void* fact)
{
	IID2D1GeometryRealization* r;
	r = (IID2D1GeometryRealization*)CoTaskMemAlloc(sizeof(IID2D1GeometryRealization));
	if (r == NULL)
		return NULL;

	r->lpVtbl = IID2D1GeometryRealizationVtbl;
	r->RefCount = 1;
	r->factory = ((IID2D1Factory1To8*)fact)->Thi;
	r->geometry = geometry;
	r->flatteningTolerance = flatteningTolerance;
	r->isFilled = TRUE;

	ID2D1Geometry_AddRef(geometry);

	return r;
}

IID2D1GeometryRealization* CreateStrokedGeometryRealization(
	IN ID2D1Geometry* geometry,
	FLOAT flatteningTolerance,
	FLOAT strokeWidth,
	IN ID2D1StrokeStyle* strokeStyle,
	void* fact)
{
	IID2D1GeometryRealization* r;
	r = (IID2D1GeometryRealization*)CoTaskMemAlloc(sizeof(IID2D1GeometryRealization));
	if (r == NULL)
		return NULL;

	r->lpVtbl = IID2D1GeometryRealizationVtbl;
	r->RefCount = 1;
	r->factory = (ID2D1Factory1*)((IID2D1Factory1To8*)fact)->Thi;
	r->geometry = geometry;
	r->flatteningTolerance = flatteningTolerance;
	r->strokeWidth = strokeWidth;
	r->strokeStyle = strokeStyle;
	r->isFilled = FALSE;

	ID2D1Geometry_AddRef(geometry);
	ID2D1StrokeStyle_AddRef(strokeStyle);

	return r;
}


HRESULT STDMETHODCALLTYPE IID2D1ColorContext_QueryInterface(
	IID2D1ColorContext* This,
	IN REFIID riid,
	OUT  void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	if (IsEqualIID(riid, &IID_IUnknown) ||
		IsEqualIID(riid, &IID_ID2D1Resource) ||
		IsEqualIID(riid, &IID_ID2D1ColorContext) ||
		IsEqualIID(riid, &IID_ID2D1ColorContext1))
	{
		*ppvObject = This->Thi;
		This->lpVtbl->AddRef(This->Thi);

		return S_OK;
	}

	return This->lpVtbl->QueryInterface(This->Thi, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IID2D1ColorContext_Release(
	IID2D1ColorContext* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 0)
		KexVtblReleaseReplaceData(&lpVtbl);

	return RefCount;
}

D2D1_COLOR_CONTEXT_TYPE STDMETHODCALLTYPE IID2D1ColorContext1_GetColorContextType(
	IID2D1ColorContext* This
)
{
	return D2D1_COLOR_CONTEXT_TYPE_ICC;
}

DXGI_COLOR_SPACE_TYPE STDMETHODCALLTYPE IID2D1ColorContext1_GetDXGIColorSpace(
	IID2D1ColorContext* This
)
{
	return DXGI_COLOR_SPACE_CUSTOM;
}

HRESULT STDMETHODCALLTYPE IID2D1ColorContext1_GetSimpleColorProfile(
	ID2D1ColorContext* This,
	D2D1_SIMPLE_COLOR_PROFILE* simple_profile
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

void* IID2D1ColorContextVtbl[] = {
	IID2D1ColorContext_QueryInterface, // 0
	0,//IID2D1ColorContext_AddRef,
	IID2D1ColorContext_Release,
	0,//IID2D1ColorContext_GetFactory,
	0,//IID2D1ColorContext_GetColorSpace,
	0,//IID2D1ColorContext_GetProfileSize,
	0,//IID2D1ColorContext_GetProfile,
	IID2D1ColorContext1_GetColorContextType,
	IID2D1ColorContext1_GetDXGIColorSpace,
	IID2D1ColorContext1_GetSimpleColorProfile,
	0,//Reserved
	0,//Reserved
	0,//Reserved
	0,//Reserved
	0,//Reserved
	0,//Reserved
};

HRESULT WrapColorContext(
	ID2D1ColorContext* effectCtx,
	HRESULT dispatchResult)
{
	IID2D1ColorContext* ctx2;

	KEX_VFT_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IID2D1ColorContext_Release, KEX_VFT_REPLACING_ALL}
	};
	if (!KexVtblReplace(effectCtx, rpl, IID2D1ColorContextVtbl, 1, ARRAYSIZE(IID2D1ColorContextVtbl),
						sizeof(IID2D1ColorContext) - sizeof(KEX_VFT_WRAPPER), (PPKEX_VFT_WRAPPER)&ctx2))
		return E_OUTOFMEMORY;

	return dispatchResult;
}