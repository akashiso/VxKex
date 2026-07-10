#include "buildcfg.h"
#include "kxd2p.h"

//
// effect.c
//

CRITICAL_SECTION effectCreatingCS;
IID2D1Factory1To8* effectCreatingFactory = NULL;


HRESULT STDMETHODCALLTYPE _IID2D1_CreateEffectDispatch(IUnknown** out)
{
	HRESULT Result;
	ID2D1EffectImpl* effect;

	Result = effectCreatingFactory->currentCreatingEffect((IUnknown**)&effect);
	if (FAILED(Result))
		return Result;

	WrapEffectImpl(effect, effectCreatingFactory);
	*out = (IUnknown*)effect;

	return Result;
}


HRESULT STDMETHODCALLTYPE IID2D1EffectContext_QueryInterface(
	IID2D1EffectContext2* This,
	IN REFIID riid,
	OUT void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	if (IsEqualIID(riid, &IID_IUnknown) ||
		IsEqualIID(riid, &IID_ID2D1EffectContext) ||
		IsEqualIID(riid, &IID_ID2D1EffectContext1) ||
		IsEqualIID(riid, &IID_ID2D1EffectContext2))
	{
		*ppvObject = This->Thi;
		This->lpVtbl->AddRef(This->Thi);

		return S_OK;
	}

	return This->lpVtbl->QueryInterface(This->Thi, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE IID2D1EffectContext_Release(
	IID2D1EffectContext2* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 0)
		KexVtblUnwrap(&lpVtbl);

	return RefCount;
}

HRESULT STDMETHODCALLTYPE IID2D1EffectContext_CreateEffect(
	IID2D1EffectContext2* This,
	IN REFCLSID effectId,
	OUT ID2D1Effect** effectOut
)
{
	IID2D1Factory1To8* fact = (IID2D1Factory1To8*)This->factory;
	EnterCriticalSection(&effectCreatingCS);

	IID2D1EffectFactoryList* list = fact->effectFactoryList;

	while (list)
	{
		if (IsEqualCLSID(&list->clsid, effectId))
			break;
		list = list->next;
	}

	if (list)
		fact->currentCreatingEffect = list->factory;
	effectCreatingFactory = fact;

	HRESULT Result;
	ID2D1Effect* effect;

	Result = This->lpVtbl->CreateEffect(This->Thi, effectId, &effect);
	fact->currentCreatingEffect = NULL;
	if (FAILED(Result))
	{
		LPOLESTR IidAsString;
		StringFromIID(effectId, &IidAsString);

		KexLogWarningEvent(L"Failed to create effect %s\n"
						   L"HRESULT error code: 0x%08lx: %s",
						   IidAsString, Result, Win32ErrorAsString(Result));
		*effectOut = NULL;
		LeaveCriticalSection(&effectCreatingCS);
		CoTaskMemFree(IidAsString);
		return Result;
	}
	*effectOut = (ID2D1Effect*)effect;

	if (IsEqualCLSID(effectId, &CLSID_D2D1ColorManagement))
		PatchColorManagement(effect);

	LeaveCriticalSection(&effectCreatingCS);

	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1EffectContext_CreateColorContext(
	IID2D1EffectContext2* This,
	D2D1_COLOR_SPACE space,
	IN CONST BYTE* profile,
	UINT32 profileSize,
	OUT ID2D1ColorContext** colorContext
)
{
	HRESULT Result;
	ID2D1ColorContext* cc;

	Result = This->lpVtbl->CreateColorContext(This->Thi, space, profile, profileSize, &cc);
	if (FAILED(Result))
		return Result;

	*colorContext = cc;
	Result = WrapColorContext(cc, Result);
	if (FAILED(Result))
		IUnknown_Release((IUnknown*)cc);

	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1EffectContext_CreateColorContextFromFilename(
	IID2D1EffectContext2* This,
	IN PCWSTR filename,
	OUT ID2D1ColorContext** colorContext
)
{
	HRESULT Result;
	ID2D1ColorContext* cc;

	Result = This->lpVtbl->CreateColorContextFromFilename(This->Thi, filename, &cc);
	if (FAILED(Result))
		return Result;

	*colorContext = cc;
	Result = WrapColorContext(cc, Result);
	if (FAILED(Result))
		IUnknown_Release((IUnknown*)cc);

	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1EffectContext_CreateColorContextFromWicColorContext(
	IID2D1EffectContext2* This,
	IN IWICColorContext* wicColorContext,
	OUT ID2D1ColorContext** colorContext
)
{
	HRESULT Result;
	ID2D1ColorContext* cc;

	Result = This->lpVtbl->CreateColorContextFromWicColorContext(This->Thi, wicColorContext, &cc);
	if (FAILED(Result))
		return Result;

	*colorContext = cc;
	Result = WrapColorContext(cc, Result);
	if (FAILED(Result))
		IUnknown_Release((IUnknown*)cc);

	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1EffectContext1_CreateLookupTable3D(
	IID2D1EffectContext2* This,
	D2D1_BUFFER_PRECISION precision,
	IN CONST UINT32* extents,
	IN CONST BYTE* data,
	UINT32 dataCount,
	IN CONST UINT32* strides,
	OUT ID2D1LookupTable3D** lookupTable
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1EffectContext2_CreateColorContextFromDxgiColorSpace(
	IID2D1EffectContext2* This,
	DXGI_COLOR_SPACE_TYPE colorSpace,
	OUT ID2D1ColorContext1** colorContext
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1EffectContext2_CreateColorContextFromSimpleColorProfile(
	IID2D1EffectContext2* This,
	IN CONST D2D1_SIMPLE_COLOR_PROFILE* simpleProfile,
	OUT ID2D1ColorContext1** colorContext
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

void* IID2D1EffectContext2Vtbl[] = {
	IID2D1EffectContext_QueryInterface, // 0
	0,//IID2D1EffectContext_AddRef,
	IID2D1EffectContext_Release,
	0,//IID2D1EffectContext_GetDpi,
	IID2D1EffectContext_CreateEffect,
	0,//IID2D1EffectContext_GetMaximumSupportedFeatureLevel,
	0,//IID2D1EffectContext_CreateTransformNodeFromEffect,
	0,//IID2D1EffectContext_CreateBlendTransform,
	0,//IID2D1EffectContext_CreateBorderTransform,
	0,//IID2D1EffectContext_CreateOffsetTransform,
	0,//IID2D1EffectContext_CreateBoundsAdjustmentTransform, // 10
	0,//IID2D1EffectContext_LoadPixelShader,
	0,//IID2D1EffectContext_LoadVertexShader,
	0,//IID2D1EffectContext_LoadComputeShader,
	0,//IID2D1EffectContext_IsShaderLoaded,
	0,//IID2D1EffectContext_CreateResourceTexture,
	0,//IID2D1EffectContext_FindResourceTexture,
	0,//IID2D1EffectContext_CreateVertexBuffer,
	0,//IID2D1EffectContext_FindVertexBuffer,
	IID2D1EffectContext_CreateColorContext,
	IID2D1EffectContext_CreateColorContextFromFilename, // 20
	IID2D1EffectContext_CreateColorContextFromWicColorContext,
	0,//IID2D1EffectContext_CheckFeatureSupport,
	0,//IID2D1EffectContext_IsBufferPrecisionSupported, //23
	IID2D1EffectContext1_CreateLookupTable3D,
	IID2D1EffectContext2_CreateColorContextFromDxgiColorSpace,
	IID2D1EffectContext2_CreateColorContextFromSimpleColorProfile, //26
};

ID2D1EffectContext* WrapEffectContext(
	ID2D1EffectContext* effectCtx,
	void* fact)
{
	IID2D1EffectContext2* ctx2;

	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IID2D1EffectContext_Release, KEX_VTBL_REPLACING_ALL}
	};
	if (!KexVtblWrap(effectCtx, rpl, IID2D1EffectContext2Vtbl, 1, ARRAYSIZE(IID2D1EffectContext2Vtbl),
						sizeof(IID2D1EffectContext2) - sizeof(KEX_VTBL_WRAPPER), (PPKEX_VTBL_WRAPPER)&ctx2))
		return NULL;

	ctx2->factory = (IID2D1Factory1To8*)fact;
	return effectCtx;
}

HRESULT STDMETHODCALLTYPE IID2D1EffectImpl_QueryInterface(
	IID2D1EffectImpl* This,
	IN REFIID riid,
	OUT void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	if (IsEqualIID(riid, &IID_IUnknown) ||
		IsEqualIID(riid, &IID_ID2D1EffectImpl))
	{
		*ppvObject = This->Thi;
		This->lpVtbl->AddRef(This->Thi);

		return S_OK;
	}

	return This->lpVtbl->QueryInterface(This->Thi, riid, ppvObject);
}

HRESULT STDMETHODCALLTYPE IID2D1EffectImpl_Release(
	IID2D1EffectImpl* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount;
	RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 1)
	{
		This->lpVtbl->Release(This->Thi);
		KexVtblUnwrap(&lpVtbl);
	}

	return RefCount - 1;
}

HRESULT STDMETHODCALLTYPE IID2D1EffectImpl_Initialize(
	IID2D1EffectImpl* This,
	IN ID2D1EffectContext* effectContext,
	IN ID2D1TransformGraph* transformGraph
)
{
	HRESULT Result;
	WrapEffectContext(effectContext, This->factory);

	Result = This->lpVtbl->Initialize(This->Thi, effectContext, transformGraph);

	return Result;
}

ID2D1EffectImpl* WrapEffectImpl(
	ID2D1EffectImpl* effectCtx,
	void* fact)
{
	HRESULT Result;
	ID2D1EffectImpl* effectRef;

	Result = effectCtx->lpVtbl->QueryInterface(effectCtx, &IID_ID2D1EffectImpl, &effectRef);
	if (FAILED(Result))
		return NULL;

	IID2D1EffectImpl* ctx2;

	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, QueryInterface), IID2D1EffectImpl_QueryInterface, KEX_VTBL_REPLACING_EXTERNAL_ONLY},
		{offsetof(IUnknownVtbl, Release), IID2D1EffectImpl_Release, KEX_VTBL_REPLACING_ALL},
		{offsetof(ID2D1EffectImplVtbl, Initialize), IID2D1EffectImpl_Initialize, KEX_VTBL_REPLACING_EXTERNAL_ONLY}
	};
	if (!KexVtblWrap(effectRef, rpl, NULL, ARRAYSIZE(rpl), sizeof(ID2D1EffectImplVtbl) / sizeof(PVOID) + 10/*Reserved space*/,
						sizeof(IID2D1EffectImpl) - sizeof(KEX_VTBL_WRAPPER), (PPKEX_VTBL_WRAPPER)&ctx2))
		return NULL;

	ctx2->factory = (IID2D1Factory1To8*)fact;
	return effectRef;
}