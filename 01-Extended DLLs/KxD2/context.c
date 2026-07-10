#include "buildcfg.h"
#include "kxd2p.h"

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_QueryInterface(
	IID2D1DeviceContext0To6* This,
	REFIID riid,
	OUT void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	if (IsEqualIID(riid, &IID_IUnknown) ||
		IsEqualIID(riid, &IID_ID2D1Resource) ||
		IsEqualIID(riid, &IID_ID2D1RenderTarget) ||
		IsEqualIID(riid, &IID_ID2D1DeviceContext) ||
		IsEqualIID(riid, &IID_ID2D1DeviceContext1) ||
		IsEqualIID(riid, &IID_ID2D1DeviceContext2) ||
		IsEqualIID(riid, &IID_ID2D1DeviceContext3) ||
		IsEqualIID(riid, &IID_ID2D1DeviceContext4) ||
		IsEqualIID(riid, &IID_ID2D1DeviceContext5) ||
		IsEqualIID(riid, &IID_ID2D1DeviceContext6))
	{
		*ppvObject = This->Thi;
		This->lpVtbl->Base.Base.Base.AddRef((IUnknown*)This->Thi);

		return S_OK;
	}

	return This->lpVtbl->Base.Base.Base.QueryInterface((IUnknown*)This->Thi, riid, ppvObject);;
}

ULONG STDMETHODCALLTYPE IID2D1DeviceContext0To6_Release(
	IID2D1DeviceContext0To6* This)
{
	IID2D1TextRenderer* rdr = This->textRenderer;
	PVOID lpVtbl = This->Thi->lpVtbl;

	ULONG RefCount;
	RefCount = This->lpVtbl->Base.Base.Base.Release((IUnknown*)This->Thi);

	if (RefCount == 0)
	{
		IUnknown_Release((IUnknown*)rdr);
		KexVtblUnwrap(&lpVtbl);
	}
	return RefCount;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_CreateCompatibleRenderTarget(
	IID2D1DeviceContext0To6* This,
	IN CONST D2D1_SIZE_F* desiredSize,
	IN CONST D2D1_SIZE_U* desiredPixelSize,
	IN CONST D2D1_PIXEL_FORMAT* desiredFormat,
	D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS options,
	OUT ID2D1BitmapRenderTarget** bitmapRenderTarget
)
{
	HRESULT Result;
	ID2D1BitmapRenderTarget* rTarget;

	Result = This->lpVtbl->Base.CreateCompatibleRenderTarget((ID2D1RenderTarget*)This->Thi, desiredSize, desiredPixelSize, desiredFormat, options, &rTarget);
	if (FAILED(Result))
		return Result;

	*bitmapRenderTarget = rTarget;
	Result = WrapAnyRenderTarget((ID2D1RenderTarget*)rTarget, This->factory, Result);
	if (FAILED(Result))
		IUnknown_Release((IUnknown*)rTarget);

	return Result;
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_DrawText(
	IID2D1DeviceContext0To6* This,
	IN CONST WCHAR* string,
	UINT32 stringLength,
	IN IDWriteTextFormat* textFormat,
	IN CONST D2D1_RECT_F* layoutRect,
	IN ID2D1Brush* defaultFillBrush,
	D2D1_DRAW_TEXT_OPTIONS options,
	DWRITE_MEASURING_MODE measuringMode
)
{
	IUnknown* fmt;

	if (IUnknown_QueryInterface((IUnknown*)textFormat, &IID_IDWriteTextFormat3, &fmt) != S_OK)
		This->lpVtbl->Base.DrawText((ID2D1RenderTarget*)This->Thi, string, stringLength, textFormat, layoutRect, defaultFillBrush, options, measuringMode);
	else
	{
		IDWriteTextLayout* textLayout;
		IUnknown* dwfact;
		D2D1_POINT_2F origin;
		HRESULT hr;
		float width, height;

		IUnknown_Release((IUnknown*)fmt);
		if (FAILED(hr = DWriteCoreCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
												&IID_IDWriteFactory3, &dwfact)))
			return;

		width = max(0.0f, layoutRect->right - layoutRect->left);
		height = max(0.0f, layoutRect->bottom - layoutRect->top);

		PPVOID lpVtbl = (PPVOID)((IUnknown*)dwfact)->lpVtbl;
		if (measuringMode == DWRITE_MEASURING_MODE_NATURAL)
		{
			// Call to IDWriteFactory1_CreateTextLayout
			hr = ((HRESULT(STDMETHODCALLTYPE*)(
				IUnknown * This,
				const WCHAR * string,
				UINT32 len,
				IDWriteTextFormat * format,
				FLOAT max_width,
				FLOAT max_height,
				IDWriteTextLayout * *layout))(lpVtbl[18]))(dwfact, string, stringLength, textFormat,
														   width, height, &textLayout);
		}
		else
		{
			D2D1_MATRIX_3X2_F m;
			float dpiX, dpiY;

			This->lpVtbl->Base.GetDpi((ID2D1RenderTarget*)This->Thi, &dpiX, &dpiY);
			This->lpVtbl->Base.GetTransform((ID2D1RenderTarget*)This->Thi, &m);

			// Call to IDWriteFactory1_CreateGdiCompatibleTextLayout
			hr = ((HRESULT(STDMETHODCALLTYPE*)(
				IUnknown * This,
				const WCHAR * string,
				UINT32 len,
				IDWriteTextFormat * format,
				FLOAT layout_width,
				FLOAT layout_height,
				FLOAT pixels_per_dip,
				const D2D1_MATRIX_3X2_F * transform,
				BOOL use_gdi_natural,
				IDWriteTextLayout * *layout))(lpVtbl[19]))(dwfact, string, stringLength, textFormat,
														   width, height, dpiX / 96.0f, &m,
														   measuringMode == DWRITE_MEASURING_MODE_GDI_NATURAL, &textLayout);
		}

		IUnknown_Release(dwfact);
		if (FAILED(hr))
		{
			KexLogWarningEvent(L"Failed to create text layout.\r\n"
							   L"HRESULT error code: 0x%08lx: %s",
							   hr, Win32ErrorAsString(hr));
			return;
		}

		origin.x = min(layoutRect->left, layoutRect->right);
		origin.y = min(layoutRect->top, layoutRect->bottom);

		This->Thi->lpVtbl->Base.DrawTextLayout((ID2D1RenderTarget*)This->Thi, origin, textLayout, defaultFillBrush, options);
		IUnknown_Release((IUnknown*)textLayout);
	}
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_DrawTextLayout(
	IID2D1DeviceContext0To6* This,
	D2D1_POINT_2F origin,
	IN IDWriteTextLayout* textLayout,
	IN ID2D1Brush* defaultFillBrush,
	D2D1_DRAW_TEXT_OPTIONS options
)
{
	IUnknown* layout;

	if (IUnknown_QueryInterface((IUnknown*)textLayout, &IID_IDWriteTextLayout3, &layout) != S_OK)
		This->lpVtbl->Base.DrawTextLayout((ID2D1RenderTarget*)This->Thi, origin, textLayout, defaultFillBrush, options);
	else
	{
		IID2D1TextRdrContext ctx;
		ctx.brush = defaultFillBrush;
		ctx.dc = (ID2D1DeviceContext*)This->Thi;
		ctx.option = options;

		// Call to IDWriteTextLayout_Draw
		PPVOID lpVtbl = (PPVOID)((IUnknown*)textLayout)->lpVtbl;
		HRESULT hr;
		hr = ((HRESULT(STDMETHODCALLTYPE*)(
			IDWriteTextLayout * This,
			void* context,
			IID2D1TextRenderer * renderer,
			FLOAT originX,
			FLOAT originY))(lpVtbl[58]))(textLayout, &ctx, This->textRenderer, origin.x, origin.y);

		if (FAILED(hr))
			KexLogWarningEvent(L"IDWriteTextLayout_Draw failed.\r\n"
							   L"HRESULT error code: 0x%08lx: %s",
							   hr, Win32ErrorAsString(hr));

		IUnknown_Release(layout);
	}
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_DrawGlyphRun(
	IID2D1DeviceContext0To6* This,
	D2D1_POINT_2F baselineOrigin,
	IN CONST DWRITE_GLYPH_RUN* glyphRun,
	IN ID2D1Brush* foregroundBrush,
	DWRITE_MEASURING_MODE measuringMode
)
{
	IUnknown* face;

	if (IUnknown_QueryInterface((IUnknown*)glyphRun->fontFace, &IID_IDWriteFontFace3, &face) != S_OK)
		This->lpVtbl->Base.DrawGlyphRun((ID2D1RenderTarget*)This->Thi, baselineOrigin, glyphRun, foregroundBrush, measuringMode);
	else
	{
		IID2D1TextRdrContext ctx;
		ctx.brush = foregroundBrush;
		ctx.dc = (ID2D1DeviceContext*)This;
		ctx.option = 0;

		IID2D1TextRenderer_DrawGlyphRun(This->textRenderer, &ctx, baselineOrigin.x, baselineOrigin.y, measuringMode, glyphRun, NULL, NULL);
		IUnknown_Release(face);
	}
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_CreateColorContext(
	IID2D1DeviceContext0To6* This,
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

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_CreateColorContextFromFilename(
	IID2D1DeviceContext0To6* This,
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

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_CreateColorContextFromWicColorContext(
	IID2D1DeviceContext0To6* This,
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

//
// For ID2D1EffectContext2
// 

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_CreateEffect(
	IID2D1DeviceContext0To6* This,
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
	if (FAILED(Result))
	{
		LPOLESTR IidAsString;
		StringFromIID(effectId, &IidAsString);

		KexLogWarningEvent(L"Failed to create effect %s\r\n"
						   L"HRESULT error code: 0x%08lx: %s",
						   IidAsString, Result, Win32ErrorAsString(Result));
		*effectOut = NULL;
		CoTaskMemFree(IidAsString);
		LeaveCriticalSection(&effectCreatingCS);
		return Result;
	}
	*effectOut = (ID2D1Effect*)effect;

	if (IsEqualCLSID(effectId, &CLSID_D2D1ColorManagement))
		PatchColorManagement(effect);

	LeaveCriticalSection(&effectCreatingCS);

	return Result;
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_GetDevice(
	IID2D1DeviceContext0To6* This,
	OUT ID2D1Device** device
)
{
	This->lpVtbl->GetDevice(This->Thi, device);

	if (device != NULL && *device != NULL)
	{
		ID2D1Device* d = *device;

		if (FAILED(d->lpVtbl->QueryInterface(d, &IID_ID2D1Device6, device)))
		{
			*device = d;
			WrapDevice(d, NULL, This->factory, S_OK);
		}
		else d->lpVtbl->Release(d);
	}
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_DrawGlyphRun1(
	IID2D1DeviceContext0To6* This,
	D2D1_POINT_2F baselineOrigin,
	IN CONST DWRITE_GLYPH_RUN* glyphRun,
	IN CONST DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
	IN ID2D1Brush* foregroundBrush,
	DWRITE_MEASURING_MODE measuringMode
)
{
	IUnknown* face;

	if (IUnknown_QueryInterface((IUnknown*)glyphRun->fontFace, &IID_IDWriteFontFace3, &face) != S_OK)
		This->lpVtbl->DrawGlyphRun1(This->Thi, baselineOrigin, glyphRun, glyphRunDescription,
									foregroundBrush, measuringMode);
	else
	{
		IID2D1TextRdrContext ctx;
		ctx.brush = foregroundBrush;
		ctx.dc = (ID2D1DeviceContext*)This;
		ctx.option = 0;

		IID2D1TextRenderer_DrawGlyphRun(This->textRenderer, &ctx, baselineOrigin.x, baselineOrigin.y, measuringMode, glyphRun, glyphRunDescription, NULL);
		IUnknown_Release(face);
	}
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_1_CreateFilledGeometryRealization(
	IID2D1DeviceContext0To6* This,
	IN ID2D1Geometry* geometry,
	FLOAT flatteningTolerance,
	OUT ID2D1GeometryRealization** geometryRealization
)
{
	*geometryRealization = (ID2D1GeometryRealization*)
		CreateFilledGeometryRealization(geometry, flatteningTolerance, This->factory);
	if (*geometryRealization == NULL)
		return E_OUTOFMEMORY;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_1_CreateStrokedGeometryRealization(
	IID2D1DeviceContext0To6* This,
	IN ID2D1Geometry* geometry,
	FLOAT flatteningTolerance,
	FLOAT strokeWidth,
	IN ID2D1StrokeStyle* strokeStyle,
	OUT ID2D1GeometryRealization** geometryRealization
)
{
	*geometryRealization = (ID2D1GeometryRealization*)
		CreateStrokedGeometryRealization(geometry, flatteningTolerance, strokeWidth, strokeStyle, This->factory);
	if (*geometryRealization == NULL)
		return E_OUTOFMEMORY;

	return S_OK;
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_1_DrawGeometryRealization(
	IID2D1DeviceContext0To6* This,
	IN ID2D1GeometryRealization* geometryRealization,
	IN ID2D1Brush* brush
)
{
	IID2D1GeometryRealization* r = (IID2D1GeometryRealization*)geometryRealization;

	if (r->isFilled)
		This->lpVtbl->Base.FillGeometry((ID2D1RenderTarget*)This->Thi, r->geometry, brush, NULL);
	else
		This->lpVtbl->Base.DrawGeometry((ID2D1RenderTarget*)This->Thi, r->geometry, brush, r->strokeWidth, r->strokeStyle);
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_CreateInk(
	IID2D1DeviceContext0To6* This,
	IN CONST D2D1_INK_POINT* startPoint,
	OUT ID2D1Ink** ink
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_CreateInkStyle(
	IID2D1DeviceContext0To6* This,
	IN CONST D2D1_INK_STYLE_PROPERTIES* inkStyleProperties,
	OUT ID2D1InkStyle** inkStyle
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_CreateGradientMesh(
	IID2D1DeviceContext0To6* This,
	IN CONST D2D1_GRADIENT_MESH_PATCH* patches,
	UINT32 patchesCount,
	OUT ID2D1GradientMesh** gradientMesh
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_CreateImageSourceFromWic(
	IID2D1DeviceContext0To6* This,
	IN IWICBitmapSource* wicBitmapSource,
	/*D2D1_IMAGE_SOURCE_LOADING_OPTIONS*/INT loadingOptions,
	D2D1_ALPHA_MODE alphaMode,
	OUT ID2D1ImageSourceFromWic** imageSource
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_CreateLookupTable3D(
	IID2D1DeviceContext0To6* This,
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

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_CreateImageSourceFromDxgi(
	IID2D1DeviceContext0To6* This,
	IN IDXGISurface** surfaces,
	UINT32 surfaceCount,
	DXGI_COLOR_SPACE_TYPE colorSpace,
	/*D2D1_IMAGE_SOURCE_FROM_DXGI_OPTIONS*/INT options,
	OUT ID2D1ImageSource** imageSource
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_GetGradientMeshWorldBounds(
	IID2D1DeviceContext0To6* This,
	IN ID2D1GradientMesh* gradientMesh,
	OUT D2D1_RECT_F* pBounds
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_DrawInk(
	IID2D1DeviceContext0To6* This,
	IN ID2D1Ink* ink,
	IN ID2D1Brush* brush,
	IN ID2D1InkStyle* inkStyle
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_DrawGradientMesh(
	IID2D1DeviceContext0To6* This,
	IN ID2D1GradientMesh* gradientMesh
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_DrawGdiMetafile(
	IID2D1DeviceContext0To6* This,
	IN ID2D1GdiMetafile* gdiMetafile,
	IN CONST D2D1_RECT_F* destinationRectangle,
	IN CONST D2D1_RECT_F* sourceRectangle
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_2_CreateTransformedImageSource(
	IID2D1DeviceContext0To6* This,
	IN ID2D1ImageSource* imageSource,
	IN CONST D2D1_TRANSFORMED_IMAGE_SOURCE_PROPERTIES* properties,
	OUT ID2D1TransformedImageSource** transformedImageSource
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_3_CreateSpriteBatch(
	IID2D1DeviceContext0To6* This,
	OUT ID2D1SpriteBatch** spriteBatch
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_3_DrawSpriteBatch(
	IID2D1DeviceContext0To6* This,
	IN ID2D1SpriteBatch* spriteBatch,
	UINT32 startIndex,
	UINT32 spriteCount,
	IN ID2D1Bitmap* bitmap,
	D2D1_BITMAP_INTERPOLATION_MODE interpolationMode,
	/*D2D1_SPRITE_OPTIONS*/INT spriteOptions
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_4_CreateSvgGlyphStyle(
	IID2D1DeviceContext0To6* This,
	OUT ID2D1SvgGlyphStyle** svgGlyphStyle
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_4_DrawText(
	IID2D1DeviceContext0To6* This,
	IN CONST WCHAR* string,
	UINT32 stringLength,
	IN IDWriteTextFormat* textFormat,
	IN CONST D2D1_RECT_F* layoutRect,
	IN ID2D1Brush* defaultFillBrush,
	IN ID2D1SvgGlyphStyle* svgGlyphStyle,
	UINT32 colorPaletteIndex,
	D2D1_DRAW_TEXT_OPTIONS options,
	DWRITE_MEASURING_MODE measuringMode
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_4_DrawTextLayout(
	IID2D1DeviceContext0To6* This,
	D2D1_POINT_2F origin,
	IN IDWriteTextLayout* textLayout,
	IN ID2D1Brush* defaultFillBrush,
	IN ID2D1SvgGlyphStyle* svgGlyphStyle,
	UINT32 colorPaletteIndex,
	D2D1_DRAW_TEXT_OPTIONS options
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_4_DrawColorBitmapGlyphRun(
	IID2D1DeviceContext0To6* This,
	DWRITE_GLYPH_IMAGE_FORMATS glyphImageFormat,
	D2D1_POINT_2F baselineOrigin,
	IN CONST DWRITE_GLYPH_RUN* glyphRun,
	DWRITE_MEASURING_MODE measuringMode,
	/*D2D1_COLOR_BITMAP_GLYPH_SNAP_OPTION*/UINT bitmapSnapOption
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_4_DrawSvgGlyphRun(
	IID2D1DeviceContext0To6* This,
	D2D1_POINT_2F baselineOrigin,
	IN CONST DWRITE_GLYPH_RUN* glyphRun,
	IN ID2D1Brush* defaultFillBrush,
	IN ID2D1SvgGlyphStyle* svgGlyphStyle,
	UINT32 colorPaletteIndex,
	DWRITE_MEASURING_MODE measuringMode
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_4_GetColorBitmapGlyphImage(
	IID2D1DeviceContext0To6* This,
	DWRITE_GLYPH_IMAGE_FORMATS glyphImageFormat,
	D2D1_POINT_2F glyphOrigin,
	IN IDWriteFontFace* fontFace,
	FLOAT fontEmSize,
	UINT16 glyphIndex,
	BOOL isSideways,
	IN CONST D2D1_MATRIX_3X2_F* worldTransform,
	FLOAT dpiX,
	FLOAT dpiY,
	OUT D2D1_MATRIX_3X2_F* glyphTransform,
	OUT ID2D1Image** glyphImage
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_4_GetSvgGlyphImage(
	IID2D1DeviceContext0To6* This,
	D2D1_POINT_2F glyphOrigin,
	IN IDWriteFontFace* fontFace,
	FLOAT fontEmSize,
	UINT16 glyphIndex,
	BOOL isSideways,
	IN CONST D2D1_MATRIX_3X2_F* worldTransform,
	IN ID2D1Brush* defaultFillBrush,
	IN ID2D1SvgGlyphStyle* svgGlyphStyle,
	UINT32 colorPaletteIndex,
	OUT D2D1_MATRIX_3X2_F* glyphTransform,
	OUT ID2D1CommandList** glyphImage
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_5_CreateSvgDocument(
	IID2D1DeviceContext0To6* This,
	IN IStream* inputXmlStream,
	D2D1_SIZE_F viewportSize,
	OUT ID2D1SvgDocument** svgDocument
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_5_DrawSvgDocument(
	IID2D1DeviceContext0To6* This,
	IN ID2D1SvgDocument* svgDocument
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_5_CreateColorContextFromDxgiColorSpace(
	IID2D1DeviceContext0To6* This,
	DXGI_COLOR_SPACE_TYPE colorSpace,
	OUT ID2D1ColorContext1** colorContext
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID2D1DeviceContext0To6_5_CreateColorContextFromSimpleColorProfile(
	IID2D1DeviceContext0To6* This,
	IN CONST D2D1_SIMPLE_COLOR_PROFILE* simpleProfile,
	OUT ID2D1ColorContext1** colorContext
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE IID2D1DeviceContext0To6_6_BlendImage(
	IID2D1DeviceContext0To6* This,
	IN ID2D1Image* image,
	D2D1_BLEND_MODE blendMode,
	IN CONST D2D1_POINT_2F* targetOffset,
	IN CONST D2D1_RECT_F* imageRectangle,
	D2D1_INTERPOLATION_MODE interpolationMode
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	KexDebugCheckpoint();
}

void* IID2D1DeviceContext0To6Vtbl[] = {
	IID2D1DeviceContext0To6_QueryInterface, // 1
	0,//IID2D1DeviceContext0To6_AddRef,
	IID2D1DeviceContext0To6_Release,
	0,//IID2D1DeviceContext0To6_GetFactory,
	0,//IID2D1DeviceContext0To6_CreateBitmap,
	0,//IID2D1DeviceContext0To6_CreateBitmapFromWicBitmap,
	0,//IID2D1DeviceContext0To6_CreateSharedBitmap,
	0,//IID2D1DeviceContext0To6_CreateBitmapBrush,
	0,//IID2D1DeviceContext0To6_CreateSolidColorBrush,
	0,//IID2D1DeviceContext0To6_CreateGradientStopCollection,
	0,//IID2D1DeviceContext0To6_CreateLinearGradientBrush,
	0,//IID2D1DeviceContext0To6_CreateRadialGradientBrush,
	IID2D1DeviceContext0To6_CreateCompatibleRenderTarget,
	0,//IID2D1DeviceContext0To6_CreateLayer,
	0,//IID2D1DeviceContext0To6_CreateMesh,
	0,//IID2D1DeviceContext0To6_DrawLine,
	0,//IID2D1DeviceContext0To6_DrawRectangle,
	0,//IID2D1DeviceContext0To6_FillRectangle,
	0,//IID2D1DeviceContext0To6_DrawRoundedRectangle,
	0,//IID2D1DeviceContext0To6_FillRoundedRectangle, // 20
	0,//IID2D1DeviceContext0To6_DrawEllipse,
	0,//IID2D1DeviceContext0To6_FillEllipse,
	0,//IID2D1DeviceContext0To6_DrawGeometry,
	0,//IID2D1DeviceContext0To6_FillGeometry,
	0,//IID2D1DeviceContext0To6_FillMesh,
	0,//IID2D1DeviceContext0To6_FillOpacityMask,
	0,//IID2D1DeviceContext0To6_DrawBitmap,
	IID2D1DeviceContext0To6_DrawText,
	IID2D1DeviceContext0To6_DrawTextLayout,
	IID2D1DeviceContext0To6_DrawGlyphRun,
	0,//IID2D1DeviceContext0To6_SetTransform,
	0,//IID2D1DeviceContext0To6_GetTransform,
	0,//IID2D1DeviceContext0To6_SetAntialiasMode,
	0,//IID2D1DeviceContext0To6_GetAntialiasMode,
	0,//IID2D1DeviceContext0To6_SetTextAntialiasMode,
	0,//IID2D1DeviceContext0To6_GetTextAntialiasMode,
	0,//IID2D1DeviceContext0To6_SetTextRenderingParams,
	0,//IID2D1DeviceContext0To6_GetTextRenderingParams,
	0,//IID2D1DeviceContext0To6_SetTags,
	0,//IID2D1DeviceContext0To6_GetTags, // 40
	0,//IID2D1DeviceContext0To6_PushLayer,
	0,//IID2D1DeviceContext0To6_PopLayer,
	0,//IID2D1DeviceContext0To6_Flush,
	0,//IID2D1DeviceContext0To6_SaveDrawingState,
	0,//IID2D1DeviceContext0To6_RestoreDrawingState,
	0,//IID2D1DeviceContext0To6_PushAxisAlignedClip,
	0,//IID2D1DeviceContext0To6_PopAxisAlignedClip,
	0,//IID2D1DeviceContext0To6_Clear,
	0,//IID2D1DeviceContext0To6_BeginDraw,
	0,//IID2D1DeviceContext0To6_EndDraw,
	0,//IID2D1DeviceContext0To6_GetPixelFormat,
	0,//IID2D1DeviceContext0To6_SetDpi,
	0,//IID2D1DeviceContext0To6_GetDpi,
	0,//IID2D1DeviceContext0To6_GetSize,
	0,//IID2D1DeviceContext0To6_GetPixelSize,
	0,//IID2D1DeviceContext0To6_GetMaximumBitmapSize,
	0,//IID2D1DeviceContext0To6_IsSupported, // The last function of ID2D1RenderTarget
	0,//IID2D1DeviceContext0To6_CreateBitmap1,
	0,//IID2D1DeviceContext0To6_CreateBitmapFromWicBitmap1,
	IID2D1DeviceContext0To6_CreateColorContext, // 60
	IID2D1DeviceContext0To6_CreateColorContextFromFilename,
	IID2D1DeviceContext0To6_CreateColorContextFromWicColorContext,
	0,//IID2D1DeviceContext0To6_CreateBitmapFromDxgiSurface,
	IID2D1DeviceContext0To6_CreateEffect,
	0,//IID2D1DeviceContext0To6_CreateGradientStopCollection1,
	0,//IID2D1DeviceContext0To6_CreateImageBrush,
	0,//IID2D1DeviceContext0To6_CreateBitmapBrush1,
	0,//IID2D1DeviceContext0To6_CreateCommandList,
	0,//IID2D1DeviceContext0To6_IsDxgiFormatSupported,
	0,//IID2D1DeviceContext0To6_IsBufferPrecisionSupported,
	0,//IID2D1DeviceContext0To6_GetImageLocalBounds,
	0,//IID2D1DeviceContext0To6_GetImageWorldBounds,
	0,//IID2D1DeviceContext0To6_GetGlyphRunWorldBounds,
	IID2D1DeviceContext0To6_GetDevice,
	0,//IID2D1DeviceContext0To6_SetTarget,
	0,//IID2D1DeviceContext0To6_GetTarget,
	0,//IID2D1DeviceContext0To6_SetRenderingControls,
	0,//IID2D1DeviceContext0To6_GetRenderingControls,
	0,//IID2D1DeviceContext0To6_SetPrimitiveBlend,
	0,//IID2D1DeviceContext0To6_GetPrimitiveBlend, // 80
	0,//IID2D1DeviceContext0To6_SetUnitMode,
	0,//IID2D1DeviceContext0To6_GetUnitMode,
	IID2D1DeviceContext0To6_DrawGlyphRun1,
	0,//IID2D1DeviceContext0To6_DrawImage,
	0,//IID2D1DeviceContext0To6_DrawGdiMetafile,
	0,//IID2D1DeviceContext0To6_DrawBitmap1,
	0,//IID2D1DeviceContext0To6_PushLayer1,
	0,//IID2D1DeviceContext0To6_InvalidateEffectInputRectangle,
	0,//IID2D1DeviceContext0To6_GetEffectInvalidRectangleCount,
	0,//IID2D1DeviceContext0To6_GetEffectInvalidRectangles,
	0,//IID2D1DeviceContext0To6_GetEffectRequiredInputRectangles,
	0,//IID2D1DeviceContext0To6_FillOpacityMask1,
	IID2D1DeviceContext0To6_1_CreateFilledGeometryRealization,
	IID2D1DeviceContext0To6_1_CreateStrokedGeometryRealization,
	IID2D1DeviceContext0To6_1_DrawGeometryRealization,
	IID2D1DeviceContext0To6_2_CreateInk,
	IID2D1DeviceContext0To6_2_CreateInkStyle,
	IID2D1DeviceContext0To6_2_CreateGradientMesh,
	IID2D1DeviceContext0To6_2_CreateImageSourceFromWic,
	IID2D1DeviceContext0To6_2_CreateLookupTable3D, // 100
	IID2D1DeviceContext0To6_2_CreateImageSourceFromDxgi,
	IID2D1DeviceContext0To6_2_GetGradientMeshWorldBounds,
	IID2D1DeviceContext0To6_2_DrawInk,
	IID2D1DeviceContext0To6_2_DrawGradientMesh,
	IID2D1DeviceContext0To6_2_DrawGdiMetafile,
	IID2D1DeviceContext0To6_2_CreateTransformedImageSource,
	IID2D1DeviceContext0To6_3_CreateSpriteBatch,
	IID2D1DeviceContext0To6_3_DrawSpriteBatch,
	IID2D1DeviceContext0To6_4_CreateSvgGlyphStyle,
	IID2D1DeviceContext0To6_4_DrawText,
	IID2D1DeviceContext0To6_4_DrawTextLayout,
	IID2D1DeviceContext0To6_4_DrawColorBitmapGlyphRun,
	IID2D1DeviceContext0To6_4_DrawSvgGlyphRun,
	IID2D1DeviceContext0To6_4_GetColorBitmapGlyphImage,
	IID2D1DeviceContext0To6_4_GetSvgGlyphImage,
	IID2D1DeviceContext0To6_5_CreateSvgDocument,
	IID2D1DeviceContext0To6_5_DrawSvgDocument,
	IID2D1DeviceContext0To6_5_CreateColorContextFromDxgiColorSpace,
	IID2D1DeviceContext0To6_5_CreateColorContextFromSimpleColorProfile,
	IID2D1DeviceContext0To6_6_BlendImage //120
};

HRESULT WrapDeviceContext(
	ID2D1DeviceContext* dc,
	void* fact,
	HRESULT dispatchResult)
{
	IID2D1DeviceContext0To6* rct2;
	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, Release), IID2D1DeviceContext0To6_Release, KEX_VTBL_REPLACING_ALL}
	};
	if (!KexVtblWrap(dc, rpl, IID2D1DeviceContext0To6Vtbl, 1, ARRAYSIZE(IID2D1DeviceContext0To6Vtbl),
						sizeof(IID2D1DeviceContext0To6) - sizeof(KEX_VTBL_WRAPPER), (PPKEX_VTBL_WRAPPER)&rct2))
		return E_OUTOFMEMORY;

	rct2->textRenderer = CreateTextRenderer(((IID2D1Factory1To8*)fact)->Thi);
	rct2->factory = (ID2D1Factory1*)fact;

	return dispatchResult;
}

ID2D1DeviceContext* WrapRenderTarget(
	ID2D1RenderTarget* rTarget,
	void* fact)
{
	HRESULT Result;
	ID2D1DeviceContext* dCtx;

	Result = ID2D1RenderTarget_QueryInterface(rTarget, &IID_ID2D1DeviceContext, &dCtx);
	if (FAILED(Result))
		return NULL;

	Result = WrapDeviceContext(dCtx, fact, Result);
	IUnknown_Release((IUnknown*)dCtx);

	return dCtx;
}

HRESULT STDMETHODCALLTYPE IID2D1AnyRenderTarget_QueryInterface(
	IID2D1AnyRenderTarget1* This,
	REFIID riid,
	OUT void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	HRESULT Result;

	if (FAILED(Result = This->lpVtbl->QueryInterface(This->Thi, riid, ppvObject)))
	{
		return IUnknown_QueryInterface(This->ctx, riid, ppvObject);
	}

	return Result;
}

ULONG STDMETHODCALLTYPE IID2D1AnyRenderTarget_Release(
	IID2D1AnyRenderTarget1* This)
{
	PVOID lpVtbl = This->Thi->lpVtbl;
	ULONG RefCount;
	RefCount = This->lpVtbl->Release(This->Thi);

	if (RefCount == 0)
		KexVtblUnwrap(&lpVtbl);
	return RefCount;
}

HRESULT WrapAnyRenderTarget(
	ID2D1RenderTarget* rTarget,
	void* fact,
	HRESULT dispatchResult)
{
	IID2D1AnyRenderTarget1* rct2;

	ID2D1DeviceContext* dc = WrapRenderTarget((ID2D1RenderTarget*)rTarget, fact);
	if (dc == NULL)
		return E_OUTOFMEMORY;

	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(IUnknownVtbl, QueryInterface), IID2D1AnyRenderTarget_QueryInterface, KEX_VTBL_REPLACING_EXTERNAL_ONLY},
		{offsetof(IUnknownVtbl, Release), IID2D1AnyRenderTarget_Release, KEX_VTBL_REPLACING_ALL},
		{offsetof(ID2D1RenderTargetVtbl, CreateCompatibleRenderTarget), IID2D1DeviceContext0To6_CreateCompatibleRenderTarget, KEX_VTBL_REPLACING_EXTERNAL_ONLY},
		{offsetof(ID2D1RenderTargetVtbl, DrawText), IID2D1DeviceContext0To6_DrawText, KEX_VTBL_REPLACING_EXTERNAL_ONLY},
		{offsetof(ID2D1RenderTargetVtbl, DrawTextLayout), IID2D1DeviceContext0To6_DrawTextLayout, KEX_VTBL_REPLACING_EXTERNAL_ONLY},
		{offsetof(ID2D1RenderTargetVtbl, DrawGlyphRun), IID2D1DeviceContext0To6_DrawGlyphRun, KEX_VTBL_REPLACING_EXTERNAL_ONLY}
	};
	if (!KexVtblWrap(rTarget, rpl, NULL, ARRAYSIZE(rpl), sizeof(ID2D1DCRenderTargetVtbl) / sizeof(PVOID) + 6/*Reserved space*/,
						sizeof(IID2D1AnyRenderTarget1) - sizeof(KEX_VTBL_WRAPPER), (PPKEX_VTBL_WRAPPER)&rct2))
		return E_OUTOFMEMORY;

	rct2->ctx = (IUnknown*)dc;

	return dispatchResult;
}