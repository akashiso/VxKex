#include "buildcfg.h"
#include "kxd2p.h"
#include <math.h>

ID2D1Brush* IID2D1TextRenderer_GetTextBrush(
	IID2D1TextRenderer* This,
	IID2D1TextRdrContext* ctx,
	IUnknown* effect)
{
	ID2D1Brush* brush = NULL;

	if (effect && SUCCEEDED(IUnknown_QueryInterface(effect, &IID_ID2D1Brush, (void**)&brush)))
		return brush;

	ID2D1Brush_AddRef(ctx->brush);
	return ctx->brush;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_QueryInterface(
	IID2D1TextRenderer* This,
	REFIID iid,
	void** out)
{
	if (IsEqualGUID(iid, &IID_IDWriteTextRenderer)
		|| IsEqualGUID(iid, &IID_IDWritePixelSnapping)
		|| IsEqualGUID(iid, &IID_IUnknown))
	{
		InterlockedIncrement(&This->RefCount);
		*out = This;
		return S_OK;
	}

	*out = NULL;
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE IID2D1TextRenderer_AddRef(
	IID2D1TextRenderer* This)
{
	return InterlockedIncrement(&This->RefCount);
}

ULONG STDMETHODCALLTYPE IID2D1TextRenderer_Release(
	IID2D1TextRenderer* This)
{
	ULONG RefCount = InterlockedDecrement(&This->RefCount);

	if (RefCount == 0)
		CoTaskMemFree(This);
	return RefCount;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_IsPixelSnappingDisabled(
	IID2D1TextRenderer* This,
	void* ctx,
	BOOL* disabled)
{
	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;

	*disabled = rctx->option & D2D1_DRAW_TEXT_OPTIONS_NO_SNAP;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_GetCurrentTransform(
	IID2D1TextRenderer* This,
	void* ctx,
	D2D1_MATRIX_3X2_F* transform)
{
	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;
	ID2D1RenderTarget* dc = (ID2D1RenderTarget*)rctx->dc;

	ID2D1RenderTarget_GetTransform(dc, (D2D1_MATRIX_3X2_F*)transform);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_GetPixelsPerDip(
	IID2D1TextRenderer* This,
	void* ctx,
	float* ppd)
{
	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;
	ID2D1RenderTarget* dc = (ID2D1RenderTarget*)rctx->dc;
	FLOAT dpiX;
	FLOAT dpiY;

	ID2D1RenderTarget_GetDpi(dc, &dpiX, &dpiY);
	*ppd = dpiY / 96.0f;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_DrawGlyphRun_Outline(
	IID2D1TextRenderer* This,
	void* ctx,
	float baseline_origin_x,
	float baseline_origin_y,
	DWRITE_MEASURING_MODE measuring_mode,
	const DWRITE_GLYPH_RUN* glyph_run,
	const DWRITE_GLYPH_RUN_DESCRIPTION* glyph_run_desc,
	IUnknown* effect,
	ID2D1Brush* brush)
{
	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;
	D2D1_POINT_2F baseline_origin = { baseline_origin_x, baseline_origin_y };

	// DrawGlyphRun 
	// it will fail with E_INVAILARG when using the original implement for some reason.

	D2D1_MATRIX_3X2_F transform, prev_transform;
	D2D1_ANTIALIAS_MODE prev_antialias_mode;
	ID2D1PathGeometry* geometry;
	ID2D1GeometrySink* sink;
	HRESULT hr;

	if (FAILED(hr = ID2D1Factory_CreatePathGeometry(This->factory, &geometry)))
		return hr;

	if (FAILED(hr = ID2D1PathGeometry_Open(geometry, &sink)))
	{
		ID2D1PathGeometry_Release(geometry);
		return hr;
	}

	// Call to IDWriteFontFace_GetGlyphRunOutline
	PPVOID lpVtbl = (PPVOID)(((IUnknown*)glyph_run->fontFace)->lpVtbl);
	if (FAILED(hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IDWriteFontFace * This,
		FLOAT emSize,
		const UINT16 * glyph_indices,
		const FLOAT * glyph_advances,
		const DWRITE_GLYPH_OFFSET * glyph_offsets,
		UINT32 glyph_count,
		BOOL is_sideways,
		BOOL is_rtl,
		ID2D1GeometrySink * geometrysink))(lpVtbl[14]))(glyph_run->fontFace, glyph_run->fontEmSize,
														glyph_run->glyphIndices, glyph_run->glyphAdvances, glyph_run->glyphOffsets, glyph_run->glyphCount,
														glyph_run->isSideways, glyph_run->bidiLevel & 1, sink)))
	{
		ID2D1GeometrySink_Release(sink);
		ID2D1PathGeometry_Release(geometry);
		return hr;
	}

	ID2D1GeometrySink_Close(sink);
	ID2D1GeometrySink_Release(sink);

	ID2D1RenderTarget* dc = (ID2D1RenderTarget*)rctx->dc;

	ID2D1RenderTarget_GetTransform(dc, &transform);
	prev_transform = transform;
	transform._31 += baseline_origin.x * transform._11 + baseline_origin.y * transform._21;
	transform._32 += baseline_origin.x * transform._12 + baseline_origin.y * transform._22;
	ID2D1RenderTarget_SetTransform(dc, &transform);

	prev_antialias_mode = ID2D1RenderTarget_GetAntialiasMode(dc);
	ID2D1RenderTarget_SetAntialiasMode(dc,
									   ID2D1RenderTarget_GetTextAntialiasMode(dc) == D2D1_TEXT_ANTIALIAS_MODE_ALIASED ?
									   D2D1_ANTIALIAS_MODE_ALIASED : D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	ID2D1RenderTarget_FillGeometry(dc, (ID2D1Geometry*)geometry, brush, NULL);
	ID2D1RenderTarget_SetAntialiasMode(dc, prev_antialias_mode);
	ID2D1RenderTarget_SetTransform(dc, &prev_transform);

	ID2D1PathGeometry_Release(geometry);

	return hr;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_DrawGlyphRun_Bitmap(
	IID2D1TextRenderer* This,
	void* ctx,
	float baseline_origin_x,
	float baseline_origin_y,
	DWRITE_MEASURING_MODE measuring_mode,
	const DWRITE_GLYPH_RUN* glyph_run,
	const DWRITE_GLYPH_RUN_DESCRIPTION* glyph_run_desc,
	IUnknown* effect,
	ID2D1Brush* brush,
	DWRITE_RENDERING_MODE rendering_mode,
	DWRITE_TEXT_ANTIALIAS_MODE antialias_mode)
{
	HRESULT hr;
	FLOAT dpiX;
	FLOAT dpiY;
	D2D1_MATRIX_3X2_F transform;

	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;
	D2D1_POINT_2F baseline_origin = { baseline_origin_x, baseline_origin_y };

	IUnknown* dwfact;
	if (FAILED(hr = DWriteCoreCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
											&IID_IDWriteFactory3, &dwfact)))
		return hr;

	ID2D1RenderTarget* dc = (ID2D1RenderTarget*)rctx->dc;

	ID2D1RenderTarget_GetTransform(dc, &transform);
	ID2D1RenderTarget_GetDpi(dc, &dpiX, &dpiY);

	FLOAT scale_x = dpiX / 96.0f;
	FLOAT scale_y = dpiY / 96.0f;

	D2D1_MATRIX_3X2_F m;
	m._11 = transform._11 * scale_x;
	m._21 = transform._21 * scale_x;
	m._31 = transform._31 * scale_x;

	m._12 = transform._12 * scale_y;
	m._22 = transform._22 * scale_y;
	m._32 = transform._32 * scale_y;

	// Call to IDWriteFactory2_CreateGlyphRunAnalysis
	IUnknown* analysis;

	PPVOID lpVtbl = (PPVOID)(((IUnknown*)dwfact)->lpVtbl);
	if (FAILED(hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown*,
		const DWRITE_GLYPH_RUN*,
		D2D1_MATRIX_3X2_F*,
		DWRITE_RENDERING_MODE,
		DWRITE_MEASURING_MODE,
		UINT, DWRITE_TEXT_ANTIALIAS_MODE,
		float, float,
		IUnknown**))(lpVtbl[30]))(dwfact, glyph_run, &m,
								  rendering_mode, measuring_mode,
								  /*DWRITE_GRID_FIT_MODE_DEFAULT*/0, antialias_mode,
								  baseline_origin.x, baseline_origin.y, &analysis)))
	{
		IUnknown_Release(dwfact);
		return hr;
	}

	IUnknown_Release(dwfact);

	DWRITE_TEXTURE_TYPE texture_type;
	if (rendering_mode == DWRITE_RENDERING_MODE_ALIASED || antialias_mode == DWRITE_TEXT_ANTIALIAS_MODE_GRAYSCALE)
		texture_type = DWRITE_TEXTURE_ALIASED_1x1;
	else
		texture_type = DWRITE_TEXTURE_CLEARTYPE_3x1;

	// Call to IDWriteGlyphRunAnalysis_GetAlphaTextureBounds
	RECT bounds;

	lpVtbl = (PPVOID)(((IUnknown*)analysis)->lpVtbl);
	if (FAILED(hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown*,
		DWRITE_TEXTURE_TYPE,
		RECT*))(lpVtbl[3]))(analysis, texture_type, &bounds)))
	{
		return hr;
	}

	D2D1_SIZE_U bitmap_size;
	bitmap_size.width = bounds.right - bounds.left;
	bitmap_size.height = bounds.bottom - bounds.top;

	if (!bitmap_size.width || !bitmap_size.height)
	{
		IUnknown_Release(analysis);
		return S_OK;
	}
	if (texture_type == DWRITE_TEXTURE_CLEARTYPE_3x1)
		bitmap_size.width *= 3;

	LPBYTE opacity_values = SafeAlloc(BYTE, bitmap_size.height * bitmap_size.width);
	if (opacity_values == NULL)
	{
		IUnknown_Release(analysis);
		return E_OUTOFMEMORY;
	}

	// Call to IDWriteGlyphRunAnalysis_CreateAlphaTexture
	if (FAILED(hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown*,
		DWRITE_TEXTURE_TYPE,
		RECT*,
		LPBYTE, UINT))(lpVtbl[4]))(analysis, texture_type, &bounds, opacity_values, bitmap_size.height * bitmap_size.width)))
	{
		SafeFree(opacity_values);
		IUnknown_Release(analysis);
		return hr;
	}

	D2D1_BITMAP_PROPERTIES bitmap_desc;
	bitmap_desc.pixelFormat.format = DXGI_FORMAT_A8_UNORM;
	bitmap_desc.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmap_desc.dpiX = texture_type == DWRITE_TEXTURE_CLEARTYPE_3x1 ? dpiX * 3.f : dpiX;
	bitmap_desc.dpiY = dpiY;

	ID2D1Bitmap* opacity_bitmap = NULL;
	if (FAILED(hr = ID2D1RenderTarget_CreateBitmap(dc, bitmap_size, opacity_values, bitmap_size.width, &bitmap_desc, &opacity_bitmap)))
	{
		SafeFree(opacity_values);
		IUnknown_Release(analysis);
		return hr;
	}

	D2D1_RECT_F run_rect;
	run_rect.left = bounds.left / scale_x;
	run_rect.top = bounds.top / scale_y;
	run_rect.right = bounds.right / scale_x;
	run_rect.bottom = bounds.bottom / scale_y;

	D2D1_RECT_F cut_rect;
	cut_rect.left = 0;
	cut_rect.top = 0;
	cut_rect.right = bitmap_size.width / scale_x;
	cut_rect.top = bitmap_size.height / scale_y;

	D2D1_MATRIX_3X2_F identity;
	identity._11 = 1.f;
	identity._21 = 0.f;
	identity._31 = 0.f;

	identity._12 = 0.f;
	identity._22 = 1.f;
	identity._32 = 0.f;

	D2D1_ANTIALIAS_MODE prev_antialias_mode = ID2D1RenderTarget_GetAntialiasMode(dc);
	ID2D1RenderTarget_SetAntialiasMode(dc, D2D1_ANTIALIAS_MODE_ALIASED);
	ID2D1RenderTarget_SetTransform(dc, &identity);

	ID2D1RenderTarget_FillOpacityMask(dc, opacity_bitmap, brush, 0, &run_rect, &cut_rect);
	ID2D1RenderTarget_SetAntialiasMode(dc, prev_antialias_mode);
	ID2D1RenderTarget_SetTransform(dc, &transform);

	ID2D1Bitmap_Release(opacity_bitmap);
	IUnknown_Release(analysis);
	SafeFree(opacity_values);

	return hr;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_DrawGlyphRunImpl(
	IID2D1TextRenderer* This,
	void* ctx,
	float baseline_origin_x,
	float baseline_origin_y,
	DWRITE_MEASURING_MODE measuring_mode,
	const DWRITE_GLYPH_RUN* glyph_run,
	const DWRITE_GLYPH_RUN_DESCRIPTION* glyph_run_desc,
	IUnknown* effect,
	ID2D1Brush* brush)
{
	DWRITE_TEXT_ANTIALIAS_MODE antialias_mode = DWRITE_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	IDWriteRenderingParams* rendering_params;
	DWRITE_RENDERING_MODE rendering_mode;
	HRESULT hr;

	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;
	ID2D1RenderTarget* dc = (ID2D1RenderTarget*)rctx->dc;

	ID2D1RenderTarget_GetTextRenderingParams(dc, &rendering_params);

	// Call to IDWriteRenderingParams_GetRenderingMode
	PPVOID lpVtbl = (PPVOID)((rendering_params)->lpVtbl);
	rendering_mode = ((DWRITE_RENDERING_MODE(STDMETHODCALLTYPE*)(IUnknown*))(lpVtbl[7]))(rendering_params);

	D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode = ID2D1RenderTarget_GetTextAntialiasMode(dc);

	switch (textAntialiasMode)
	{
		case D2D1_TEXT_ANTIALIAS_MODE_ALIASED:
			if (rendering_mode == DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL
				|| rendering_mode == DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC
				|| rendering_mode == DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL
				|| rendering_mode == DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC)
				return E_INVALIDARG;
			break;

		case D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE:
			if (rendering_mode == DWRITE_RENDERING_MODE_ALIASED
				|| rendering_mode == DWRITE_RENDERING_MODE_OUTLINE)
				return E_INVALIDARG;
			break;

		case D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE:
			if (rendering_mode == DWRITE_RENDERING_MODE_ALIASED)
				return E_INVALIDARG;
			break;

		default:
			break;
	}

	rendering_mode = DWRITE_RENDERING_MODE_DEFAULT;
	switch (textAntialiasMode)
	{
		case D2D1_TEXT_ANTIALIAS_MODE_DEFAULT:
			// Call to IDWriteRenderingParams_GetClearTypeLevel
			if (((FLOAT(STDMETHODCALLTYPE*)(IUnknown*))(lpVtbl[5]))(rendering_params) > 0.0f)
				antialias_mode = DWRITE_TEXT_ANTIALIAS_MODE_CLEARTYPE;
			break;

		case D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE:
			antialias_mode = DWRITE_TEXT_ANTIALIAS_MODE_CLEARTYPE;
			break;

		case D2D1_TEXT_ANTIALIAS_MODE_ALIASED:
			rendering_mode = DWRITE_RENDERING_MODE_ALIASED;
			break;

		default:
			break;
	}

	if (rendering_mode == DWRITE_RENDERING_MODE_DEFAULT)
	{
		FLOAT dpiX;
		FLOAT dpiY;
		ID2D1RenderTarget_GetDpi(dc, &dpiX, &dpiY);

		PPVOID lpVtbl = (PPVOID)(((IUnknown*)glyph_run->fontFace)->lpVtbl);
		if (FAILED(hr = FAILED(hr = ((HRESULT(STDMETHODCALLTYPE*)(
			IDWriteFontFace*,
			float, float,
			DWRITE_MEASURING_MODE,
			IDWriteRenderingParams*,
			DWRITE_RENDERING_MODE*))(lpVtbl[15]))(glyph_run->fontFace, glyph_run->fontEmSize,
												  max(dpiX, dpiY) / 96.0f,
												  measuring_mode, rendering_params, &rendering_mode))))
		{
			rendering_mode = DWRITE_RENDERING_MODE_OUTLINE;
		}
	}

	if (rendering_mode == DWRITE_RENDERING_MODE_OUTLINE)
		return IID2D1TextRenderer_DrawGlyphRun_Outline(This, ctx, baseline_origin_x, baseline_origin_y,
													   measuring_mode, glyph_run, glyph_run_desc, effect, brush);
	else
		return IID2D1TextRenderer_DrawGlyphRun_Bitmap(This, ctx, baseline_origin_x, baseline_origin_y,
													  measuring_mode, glyph_run, glyph_run_desc, effect,
													  brush, rendering_mode, antialias_mode);
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_DrawGlyphRun(
	IID2D1TextRenderer* This,
	void* ctx,
	float baseline_origin_x,
	float baseline_origin_y,
	DWRITE_MEASURING_MODE measuring_mode,
	const DWRITE_GLYPH_RUN* glyph_run,
	const DWRITE_GLYPH_RUN_DESCRIPTION* glyph_run_desc,
	IUnknown* effect)
{
	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;
	ID2D1RenderTarget* dc = (ID2D1RenderTarget*)rctx->dc;
	ID2D1Brush* brush = IID2D1TextRenderer_GetTextBrush(This, rctx, effect);

	BOOL isColorFont = FALSE;
	HRESULT hr;

	if (rctx->option & D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT)
	{
		IUnknown* fontface = (IUnknown*)glyph_run->fontFace;
		if (SUCCEEDED(hr = IUnknown_QueryInterface(fontface, &IID_IDWriteFontFace2, &fontface)))
		{
			// Call to IDWriteFontFace2_IsColorFont
			PPVOID lpVtbl = (PPVOID)(fontface->lpVtbl);
			isColorFont = ((BOOL(STDMETHODCALLTYPE*)(IUnknown*))(lpVtbl[30]))(fontface);

			IUnknown_Release(fontface);
		}
	}

	if (!isColorFont)
	{
		hr = IID2D1TextRenderer_DrawGlyphRunImpl(This, ctx, baseline_origin_x, baseline_origin_y,
												 measuring_mode, glyph_run, glyph_run_desc, effect, brush);
		ID2D1Brush_Release(brush);
		return hr;
	}

	IUnknown* layers;
	IUnknown* dwfact;

	if (FAILED(hr = DWriteCoreCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory3,
											(IUnknown**)&dwfact)))
	{
		ID2D1Brush_Release(brush);
		return hr;
	}

	D2D1_MATRIX_3X2_F transform;
	ID2D1RenderTarget_GetTransform(dc, &transform);

	// Call to IDWriteFactory2_TranslateColorGlyphRun
	PPVOID lpVtbl = (PPVOID)(dwfact->lpVtbl);
	if (FAILED(hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown*,
		float, float,
		const DWRITE_GLYPH_RUN*,
		const DWRITE_GLYPH_RUN_DESCRIPTION*,
		DWRITE_MEASURING_MODE,
		D2D1_MATRIX_3X2_F*,
		UINT,
		IUnknown**))(lpVtbl[28]))(dwfact, baseline_origin_x, baseline_origin_y,
								  glyph_run, glyph_run_desc, measuring_mode,
								  &transform, 0, &layers)))
	{
		IUnknown_Release(dwfact);
		ID2D1Brush_Release(brush);
		return hr;

	}
	IUnknown_Release(dwfact);
	lpVtbl = (PPVOID)(layers->lpVtbl);

	while (1)
	{
		DWRITE_COLOR_GLYPH_RUN* color_run;
		ID2D1Brush* color_brush;
		BOOL has_run = FALSE;

		// Call to IDWriteColorGlyphRunEnumerator_MoveNext
		if (FAILED(hr = ((HRESULT(STDMETHODCALLTYPE*)(IUnknown*, BOOL*))(lpVtbl[3]))(layers, &has_run)))
			break;

		if (!has_run) break;

		// Call to IDWriteColorGlyphRunEnumerator_GetCurrentRun
		if (FAILED(hr = ((HRESULT(STDMETHODCALLTYPE*)(IUnknown*, DWRITE_COLOR_GLYPH_RUN**))(lpVtbl[4]))(layers, &color_run)))
			break;

		if (color_run->paletteIndex == 0xffff)
			color_brush = brush;
		else if (FAILED(hr = ID2D1RenderTarget_CreateSolidColorBrush(dc, &color_run->runColor, NULL, (ID2D1SolidColorBrush**)&color_brush)))
			break;

		hr = IID2D1TextRenderer_DrawGlyphRunImpl(This, ctx, color_run->baselineOriginX, color_run->baselineOriginY,
												 measuring_mode, &color_run->glyphRun, color_run->glyphRunDescription, effect, color_brush);
		if (color_brush != brush)
			ID2D1Brush_Release(color_brush);
	}

	IUnknown_Release(layers);
	ID2D1Brush_Release(brush);
	return hr;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_DrawUnderline(
	IID2D1TextRenderer* This,
	void* ctx,
	float baseline_origin_x,
	float baseline_origin_y,
	const DWRITE_UNDERLINE* underline,
	IUnknown* effect)
{
	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;
	D2D1_MATRIX_3X2_F m;
	D2D1_ANTIALIAS_MODE prev_antialias_mode;
	D2D1_POINT_2F start, end;
	ID2D1Brush* brush;
	float thickness;
	float dpiX;
	float dpiY;

	ID2D1RenderTarget* dc = (ID2D1RenderTarget*)rctx->dc;

	ID2D1RenderTarget_GetDpi(dc, &dpiX, &dpiY);
	ID2D1RenderTarget_GetTransform(dc, &m);

	/* minimal thickness in DIPs that will result in at least 1 pixel thick line */
	thickness = max(96.0f / (dpiY * sqrtf(m._21 * m._21 + m._22 * m._22)),
					underline->thickness);

	brush = IID2D1TextRenderer_GetTextBrush(This, rctx, effect);

	start.x = baseline_origin_x;
	start.y = baseline_origin_y + underline->offset + thickness / 2.0f;
	end.x = start.x + underline->width;
	end.y = start.y;

	prev_antialias_mode = ID2D1RenderTarget_GetAntialiasMode(dc);
	ID2D1RenderTarget_SetAntialiasMode(dc,
									   ID2D1RenderTarget_GetTextAntialiasMode(dc) == D2D1_TEXT_ANTIALIAS_MODE_ALIASED ?
									   D2D1_ANTIALIAS_MODE_ALIASED : D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	ID2D1RenderTarget_DrawLine(dc, start, end, brush, thickness, NULL);
	ID2D1RenderTarget_SetAntialiasMode(dc, prev_antialias_mode);

	ID2D1Brush_Release(brush);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_DrawStrikethrough(
	IID2D1TextRenderer* This,
	void* ctx,
	float baseline_origin_x,
	float baseline_origin_y,
	const DWRITE_STRIKETHROUGH* strikethrough,
	IUnknown* effect)
{
	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;
	D2D1_MATRIX_3X2_F m;
	D2D1_ANTIALIAS_MODE prev_antialias_mode;
	D2D1_POINT_2F start, end;
	ID2D1Brush* brush;
	float thickness;
	float dpiX;
	float dpiY;

	ID2D1RenderTarget* dc = (ID2D1RenderTarget*)rctx->dc;

	ID2D1RenderTarget_GetDpi(dc, &dpiX, &dpiY);
	ID2D1RenderTarget_GetTransform(dc, &m);

	/* minimal thickness in DIPs that will result in at least 1 pixel thick line */
	thickness = max(96.0f / (dpiY * sqrtf(m._21 * m._21 + m._22 * m._22)),
					strikethrough->thickness);

	brush = IID2D1TextRenderer_GetTextBrush(This, rctx, effect);

	start.x = baseline_origin_x;
	start.y = baseline_origin_y + strikethrough->offset + thickness / 2.0f;
	end.x = start.x + strikethrough->width;
	end.y = start.y;
	prev_antialias_mode = ID2D1RenderTarget_GetAntialiasMode(dc);
	ID2D1RenderTarget_SetAntialiasMode(dc,
									   ID2D1RenderTarget_GetTextAntialiasMode(dc) == D2D1_TEXT_ANTIALIAS_MODE_ALIASED ?
									   D2D1_ANTIALIAS_MODE_ALIASED : D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	ID2D1RenderTarget_DrawLine(dc, start, end, brush, thickness, NULL);
	ID2D1RenderTarget_SetAntialiasMode(dc, prev_antialias_mode);

	ID2D1Brush_Release(brush);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID2D1TextRenderer_DrawInlineObject(
	IID2D1TextRenderer* This,
	void* ctx,
	float origin_x,
	float origin_y,
	IUnknown* object,
	BOOL is_sideways,
	BOOL is_rtl,
	IUnknown* effect)
{
	IID2D1TextRdrContext* rctx = (IID2D1TextRdrContext*)ctx;
	ID2D1Brush* brush;
	HRESULT hr;

	/* Inline objects may not pass effects all the way down, when using layout object internally for example.
	   This is how default trimming sign object in DirectWrite works - it does not use effect passed to Draw(),
	   and resulting DrawGlyphRun() is always called with NULL effect, however original effect is used and correct
	   brush is selected at Direct2D level. */
	brush = rctx->brush;
	rctx->brush = IID2D1TextRenderer_GetTextBrush(This, rctx, effect);

	// Call to IDWriteInlineObject_Draw
	PPVOID lpVtbl = (PPVOID)object->lpVtbl;
	hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown * This,
		void* client_drawingontext,
		IID2D1TextRenderer * renderer,
		FLOAT originX,
		FLOAT originY,
		BOOL is_sideways,
		BOOL is_rtl,
		IUnknown * drawing_effect))(lpVtbl[3]))(object, ctx, This, origin_x, origin_y, is_sideways, is_rtl, effect);

	ID2D1Brush_Release(rctx->brush);
	rctx->brush = brush;

	return hr;
}

void* IID2D1TextRendererVtbl[] =
{
	IID2D1TextRenderer_QueryInterface,
	IID2D1TextRenderer_AddRef,
	IID2D1TextRenderer_Release,
	IID2D1TextRenderer_IsPixelSnappingDisabled,
	IID2D1TextRenderer_GetCurrentTransform,
	IID2D1TextRenderer_GetPixelsPerDip,
	IID2D1TextRenderer_DrawGlyphRun,
	IID2D1TextRenderer_DrawUnderline,
	IID2D1TextRenderer_DrawStrikethrough,
	IID2D1TextRenderer_DrawInlineObject,
};

IID2D1TextRenderer* CreateTextRenderer(void* fact)
{
	IID2D1TextRenderer* r = (IID2D1TextRenderer*)CoTaskMemAlloc(sizeof(IID2D1TextRenderer));
	if (r == NULL)
		return NULL;

	r->lpVtbl = IID2D1TextRendererVtbl;
	r->RefCount = 1;
	r->factory = (ID2D1Factory*)fact;

	return r;
}