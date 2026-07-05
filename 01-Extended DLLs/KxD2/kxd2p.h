#pragma once

#include <KexComm.h>
#include <KexDll.h>
#include <KxD2.h>
#include <math.h>

EXTERN PKEX_PROCESS_DATA KexData;

#if defined(KEX_TARGET_TYPE_EXE) || defined(KEX_TARGET_TYPE_DLL)
#  if defined(KEX_ARCH_X64)
#    pragma comment(lib, "dwrite_x64.lib")
#    pragma comment(lib, "dwrw10_x64.lib")
#  elif defined(KEX_ARCH_X86)
#    pragma comment(lib, "dwrite_x86.lib")
#    pragma comment(lib, "dwrw10_x86.lib")
#  endif
#endif


DEFINE_GUID(IID_PSContrast, 0xaea1b7b, 0x916d, 0x47b3, 0x95, 0xb9, 0xdd, 0xcf, 0x91, 0xe8, 0x1e, 0xa1);
DEFINE_GUID(IID_PSRGBToHue, 0xbdc3f4d7, 0xbc48, 0x4296, 0x82, 0xf4, 0x2e, 0x58, 0xf6, 0x60, 0xf0, 0xb);
DEFINE_GUID(IID_PSHueToRGB, 0xb6b94538, 0xa2ed, 0x4f6d, 0x95, 0x33, 0x71, 0x23, 0x8f, 0xdd, 0x87, 0xea);

typedef struct IID2D1EffectFactoryList IID2D1EffectFactoryList;

typedef struct IID2D1EffectFactoryList
{
	CLSID clsid;
	PD2D1_EFFECT_FACTORY factory;

	IID2D1EffectFactoryList* next;
} IID2D1EffectFactoryList;

// text.c

typedef struct IID2D1TextRdrContext
{
	ID2D1DeviceContext* dc;
	ID2D1Brush* brush;
	D2D1_DRAW_TEXT_OPTIONS option;
} IID2D1TextRdrContext;

typedef struct IID2D1TextRenderer
{
	PPVOID lpVtbl;
	ULONG RefCount;
	ID2D1Factory* factory;
} IID2D1TextRenderer;

// factory.c & device.c & context.c

typedef struct IID2D1Factory1To8
{
	ID2D1Factory1Vtbl* lpVtbl;
	ID2D1Factory1* Thi;

	IID2D1EffectFactoryList* effectFactoryList;
	PD2D1_EFFECT_FACTORY currentCreatingEffect;
} IID2D1Factory1To8;

typedef struct IID2D1Device0To6
{
	ID2D1DeviceVtbl* lpVtbl;
	ID2D1Device* Thi;
	IDXGIDevice* dxgiDevice;
	ID2D1Factory* factory;
	SIZE_T maxColorGlyphCache;
	D2D1_RENDERING_PRIORITY renderPrior;
} IID2D1Device0To6;

typedef struct IID2D1DeviceContext0To6
{
	ID2D1DeviceContextVtbl* lpVtbl;
	ID2D1DeviceContext* Thi;
	ID2D1Factory1* factory;
	IID2D1TextRenderer* textRenderer;
} IID2D1DeviceContext0To6;

typedef struct IID2D1AnyRenderTarget1
{
	IUnknownVtbl* lpVtbl;
	IUnknown* Thi;
	IUnknown* ctx;
} IID2D1AnyRenderTarget1;

// effect.c & effectimpl.c

typedef struct IID2D1EffectContext2
{
	ID2D1EffectContextVtbl* lpVtbl;
	ID2D1EffectContext* Thi;
	IID2D1Factory1To8* factory;
} IID2D1EffectContext2;

typedef struct IID2D1EffectImpl
{
	ID2D1EffectImplVtbl* lpVtbl;
	ID2D1EffectImpl* Thi;
	IID2D1Factory1To8* factory;
} IID2D1EffectImpl;

typedef struct IID2D1PixelShaderTransform
{
	PPVOID lpVtbl;
	ID2D1DrawInfo* info;
	const GUID* shader;
	UINT numInput;
	ULONG RefCount;
} IID2D1PixelShaderTransform;

typedef struct IID2D1PixelEffectImpl
{
	PPVOID lpVtbl;
	IID2D1EffectContext2* ectx;
	IID2D1PixelShaderTransform node;

	const GUID* shaderGuid;
	const BYTE* shaderCompiled;
	UINT32 szShaderCompiled;

	LPBYTE constantBuffer;
	UINT32 szConstantBuffer;

	ULONG RefCount;
} IID2D1PixelEffectImpl;

typedef struct IID2D1EffectWrapper IID2D1EffectWrapper;

typedef struct IID2D1EffectWrapper
{
	PPVOID lpVtbl;
	IID2D1EffectContext2* ectx;
	ID2D1Effect* effect;
	void (*callback)(IID2D1EffectWrapper* This);
	const GUID* effectGuid;
	ULONG RefCount;
	FLOAT Data[4];
} IID2D1EffectWrapper;

typedef struct IID2D1EmbossEffect
{
	PPVOID lpVtbl;
	IID2D1EffectContext2* ectx;
	ID2D1Effect* diffuse;
	ULONG RefCount;
	FLOAT height;
	FLOAT direction;
} IID2D1EmbossEffect;

// misc.c

typedef struct IID2D1ColorContext
{
	ID2D1ColorContextVtbl* lpVtbl;
	ID2D1ColorContext* Thi;
} IID2D1ColorContext;

typedef struct IID2D1GeometryRealization
{
	PPVOID lpVtbl;
	ID2D1Factory1* factory;

	ID2D1StrokeStyle* strokeStyle;
	ID2D1Geometry* geometry;
	FLOAT flatteningTolerance;
	FLOAT strokeWidth;

	ULONG RefCount;
	BOOL isFilled;
} IID2D1GeometryRealization;

// patch.c

typedef struct IID2D1ColorManagementPatch
{
	ID2D1EffectVtbl* lpVtbl;
	ID2D1Effect* Thi;
	BOOL isSetSrcColorSpaceCalled;
	BOOL isSetDstColorSpaceCalled;
} IID2D1ColorManagementPatch;


CRITICAL_SECTION effectCreatingCS;
IID2D1Factory1To8* effectCreatingFactory;


HRESULT STDMETHODCALLTYPE _IID2D1_CreateEffectDispatch(IUnknown** out);
IID2D1Factory1To8* CreateIID2D1Factory1To8(ID2D1Factory1* pFactory1);
void IID2D_RegisterBuiltinEffects(ID2D1Factory1* pFactory);
void IID2D_UnregisterBuiltinEffects(ID2D1Factory1* pFactory);
BOOL IID2D_IsBuiltinEffects(const IID* id);


IID2D1GeometryRealization* CreateFilledGeometryRealization(
	IN ID2D1Geometry* geometry,
	FLOAT flatteningTolerance,
	void* fact);
IID2D1GeometryRealization* CreateStrokedGeometryRealization(
	IN ID2D1Geometry* geometry,
	FLOAT flatteningTolerance,
	FLOAT strokeWidth,
	IN ID2D1StrokeStyle* strokeStyle,
	void* fact);

HRESULT WrapDeviceContext(
	ID2D1DeviceContext* dc,
	void* fact,
	HRESULT dispatchResult);
ID2D1DeviceContext* WrapRenderTarget(
	ID2D1RenderTarget* rTarget,
	void* fact);
HRESULT WrapAnyRenderTarget(
	ID2D1RenderTarget* rTarget,
	void* fact,
	HRESULT dispatchResult);

HRESULT WrapDevice(
	ID2D1Device* device,
	IDXGIDevice* dxgiDevice,
	void* fact,
	HRESULT dispatchResult);


ID2D1EffectImpl* WrapEffectImpl(
	ID2D1EffectImpl* effectCtx,
	void* fact);
ID2D1EffectContext* WrapEffectContext(
	ID2D1EffectContext* effectCtx,
	void* fact);


HRESULT WrapColorContext(
	ID2D1ColorContext* effectCtx,
	HRESULT dispatchResult);

IID2D1TextRenderer* CreateTextRenderer(
	void* fact);
HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_DrawGlyphRun(
	IID2D1TextRenderer* This,
	void* ctx,
	float baseline_origin_x,
	float baseline_origin_y,
	DWRITE_MEASURING_MODE measuring_mode,
	const DWRITE_GLYPH_RUN* glyph_run,
	const DWRITE_GLYPH_RUN_DESCRIPTION* glyph_run_desc,
	IUnknown* effect);

ID2D1Effect* PatchColorManagement(ID2D1Effect* effect);