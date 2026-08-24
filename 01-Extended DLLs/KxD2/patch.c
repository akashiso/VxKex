#include "buildcfg.h"
#include "kxd2p.h"

// 
// The source/destination color space of the ColorManagement effect are NULL by default.
// In Windows 7, CColorManagement::GetSourceColorSpace/GetDestinationColorSpace
// will always try to call QueryInterface for the output color space,
// if there's actually no color space specified (regard as NULL), these functions
// will lead to an access violation.
// 
// However, this bug is fixed in Windows 10.
//

HRESULT STDMETHODCALLTYPE IID2D1ColorManagementPatch_SetValue(
	IID2D1ColorManagementPatch* This,
	UINT32 index,
	D2D1_PROPERTY_TYPE type,
	IN CONST BYTE* data,
	UINT32 dataSize
	)
{
	HRESULT Result;
	Result = This->lpVtbl->SetValue(This->Thi, index, type, data, dataSize);

	if (SUCCEEDED(Result))
	{
		if (index == 0) // D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT
			This->isSetSrcColorSpaceCalled = TRUE;
		else if (index == 2) // D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT
			This->isSetDstColorSpaceCalled = TRUE;
	}
	return Result;
}

HRESULT STDMETHODCALLTYPE IID2D1ColorManagementPatch_GetValue(
	IID2D1ColorManagementPatch* This,
	UINT32 index,
	D2D1_PROPERTY_TYPE type,
	OUT BYTE* data,
	UINT32 dataSize
	)
{
	if ((index == 0 && !This->isSetSrcColorSpaceCalled)
		|| (index == 2 && !This->isSetDstColorSpaceCalled))
	{
		if (dataSize < sizeof(ID2D1ColorContext*))
			return E_NOT_SUFFICIENT_BUFFER;

		if (data && (type == D2D1_PROPERTY_TYPE_COLOR_CONTEXT || type == D2D1_PROPERTY_TYPE_UNKNOWN))
		{
			*((PPVOID)data) = NULL;
			return TRUE;
		}
		else
			return E_INVALIDARG;
	}

	return This->lpVtbl->GetValue(This->Thi, index, type, data, dataSize);
}

ID2D1Effect* PatchColorManagement(ID2D1Effect* effect)
{
	IID2D1ColorManagementPatch* patch;

	KEX_VTBL_REPLACING_ENTRY rpl[] = {
		{offsetof(ID2D1EffectVtbl, GetValue), IID2D1ColorManagementPatch_GetValue, KEX_VTBL_REPLACING_EXTERNAL_ONLY},
		{offsetof(ID2D1EffectVtbl, SetValue), IID2D1ColorManagementPatch_SetValue, KEX_VTBL_REPLACING_EXTERNAL_ONLY}
	};
	if (!KexVtblWrap(effect, rpl, NULL, 2, sizeof(ID2D1EffectVtbl) / sizeof(PVOID) + 2,
						sizeof(IID2D1ColorManagementPatch) - sizeof(KEX_VTBL_WRAPPER), (PPKEX_VTBL_WRAPPER)&patch))
		return NULL;

	patch->isSetDstColorSpaceCalled = FALSE;
	patch->isSetSrcColorSpaceCalled = FALSE;

	return effect;
}

//
// This is used to distinguish between DWrite.dll and Dwrw10.dll for some D2D1 interfaces.
// TODO : This isn't a sufficient way. Maybe I need some time to think for another way to do this.
//

void PatchD2D1RenderTarget(ID2D1RenderTarget* rt);
void PatchD2D1DeviceContext(ID2D1DeviceContext* ctx);
void PatchD2D1Device(ID2D1Device* device);


HRESULT STDMETHODCALLTYPE ID2D1Factory0_CreateWicBitmapRenderTarget(
	ID2D1Factory* This,
	IN IWICBitmap* target,
	IN CONST D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
	OUT ID2D1RenderTarget** renderTarget
)
{
	HRESULT Result;

	ID2D1FactoryVtbl* lpVtbl = (ID2D1FactoryVtbl*)KexVtblLookupOriginalTable(This->lpVtbl);
	Result = lpVtbl->CreateWicBitmapRenderTarget(This, target, renderTargetProperties, renderTarget);
	if (FAILED(Result))
		return Result;

	PatchD2D1RenderTarget(*renderTarget);
	return Result;
}

HRESULT STDMETHODCALLTYPE ID2D1Factory0_CreateHwndRenderTarget(
	ID2D1Factory* This,
	IN CONST D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
	IN CONST D2D1_HWND_RENDER_TARGET_PROPERTIES* hwndRenderTargetProperties,
	OUT ID2D1HwndRenderTarget** hwndRenderTarget
)
{
	HRESULT Result;

	ID2D1FactoryVtbl* lpVtbl = (ID2D1FactoryVtbl*)KexVtblLookupOriginalTable(This->lpVtbl);
	Result = lpVtbl->CreateHwndRenderTarget(This, renderTargetProperties, hwndRenderTargetProperties, hwndRenderTarget);
	if (FAILED(Result))
		return Result;

	PatchD2D1RenderTarget((ID2D1RenderTarget*)*hwndRenderTarget);
	return Result;
}

HRESULT STDMETHODCALLTYPE ID2D1Factory0_CreateDxgiSurfaceRenderTarget(
	ID2D1Factory* This,
	IN IDXGISurface* dxgiSurface,
	IN CONST D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
	OUT ID2D1RenderTarget** renderTarget
)
{
	HRESULT Result;

	ID2D1FactoryVtbl* lpVtbl = (ID2D1FactoryVtbl*)KexVtblLookupOriginalTable(This->lpVtbl);
	Result = lpVtbl->CreateDxgiSurfaceRenderTarget(This, dxgiSurface, renderTargetProperties, renderTarget);
	if (FAILED(Result))
		return Result;

	PatchD2D1RenderTarget(*renderTarget);
	return Result;
}

HRESULT STDMETHODCALLTYPE ID2D1Factory0_CreateDCRenderTarget(
	ID2D1Factory* This,
	IN CONST D2D1_RENDER_TARGET_PROPERTIES* renderTargetProperties,
	OUT ID2D1DCRenderTarget** dcRenderTarget
)
{
	HRESULT Result;

	ID2D1FactoryVtbl* lpVtbl = (ID2D1FactoryVtbl*)KexVtblLookupOriginalTable(This->lpVtbl);
	Result = lpVtbl->CreateDCRenderTarget(This, renderTargetProperties, dcRenderTarget);
	if (FAILED(Result))
		return Result;

	PatchD2D1RenderTarget((ID2D1RenderTarget*)*dcRenderTarget);
	return Result;
}

HRESULT STDMETHODCALLTYPE ID2D1Factory1_CreateDevice(
	ID2D1Factory1* This,
	IN IDXGIDevice* dxgiDevice,
	OUT ID2D1Device** d2dDevice
)
{
	HRESULT Result;

	ID2D1Factory1Vtbl* lpVtbl = (ID2D1Factory1Vtbl*)KexVtblLookupOriginalTable(This->lpVtbl);
	Result = lpVtbl->CreateDevice(This, dxgiDevice, d2dDevice);
	if (FAILED(Result))
		return Result;

	PatchD2D1Device(*d2dDevice);
	return Result;
}

HRESULT STDMETHODCALLTYPE ID2D1Device_CreateDeviceContext(
	ID2D1Device* This,
	D2D1_DEVICE_CONTEXT_OPTIONS options,
	OUT ID2D1DeviceContext** deviceContext
)
{
	HRESULT Result;
	ID2D1DeviceVtbl* lpVtbl = (ID2D1DeviceVtbl*)KexVtblLookupOriginalTable(This->lpVtbl);
	Result = lpVtbl->CreateDeviceContext(This, options, deviceContext);

	if (FAILED(Result))
		return Result;

	PatchD2D1DeviceContext(*deviceContext);
	return Result;
}

HRESULT STDMETHODCALLTYPE ID2D1DeviceContext_CreateCompatibleRenderTarget(
	ID2D1DeviceContext* This,
	IN CONST D2D1_SIZE_F* desiredSize,
	IN CONST D2D1_SIZE_U* desiredPixelSize,
	IN CONST D2D1_PIXEL_FORMAT* desiredFormat,
	D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS options,
	OUT ID2D1BitmapRenderTarget** bitmapRenderTarget
)
{
	HRESULT Result;

	ID2D1DeviceContextVtbl* lpVtbl = (ID2D1DeviceContextVtbl*)KexVtblLookupOriginalTable(This->lpVtbl); 
	Result = lpVtbl->Base.CreateCompatibleRenderTarget((ID2D1RenderTarget*)This, desiredSize, desiredPixelSize, desiredFormat, options, bitmapRenderTarget);

	if (FAILED(Result))
		return Result;

	PatchD2D1RenderTarget((ID2D1RenderTarget*)(*bitmapRenderTarget));
	return Result;
}

void STDMETHODCALLTYPE ID2D1DeviceContext_DrawTextLayout(
	ID2D1DeviceContext* This,
	D2D1_POINT_2F origin,
	IN IDWriteTextLayout* textLayout,
	IN ID2D1Brush* defaultFillBrush,
	D2D1_DRAW_TEXT_OPTIONS options
)
{
	IUnknown* layout;

	if (IUnknown_QueryInterface((IUnknown*)textLayout, &IID_IDWriteTextLayout3, &layout) != S_OK) {
		ID2D1DeviceContextVtbl* lpVtbl = (ID2D1DeviceContextVtbl*)KexVtblLookupOriginalTable(This->lpVtbl);
		lpVtbl->Base.DrawTextLayout((ID2D1RenderTarget*)This, origin, textLayout, defaultFillBrush, options);
	}
	else {
		IID2D1TextRdrContext ctx;
		ctx.brush = defaultFillBrush;
		ctx.dc = This;
		ctx.option = options;

		ID2D1Factory* fact;
		This->lpVtbl->Base.Base.GetFactory((ID2D1Resource*)This, &fact);

		IID2D1TextRenderer* textRenderer = CreateTextRenderer(fact);

		// Call to IDWriteTextLayout_Draw
		PPVOID lpVtbl = (PPVOID)((IUnknown*)textLayout)->lpVtbl;
		HRESULT hr;
		hr = ((HRESULT(STDMETHODCALLTYPE*)(
			IDWriteTextLayout * This,
			void* context,
			IID2D1TextRenderer * renderer,
			FLOAT originX,
			FLOAT originY))(lpVtbl[58]))(textLayout, &ctx, textRenderer, origin.x, origin.y);

		if (FAILED(hr))
			KexLogWarningEvent(L"IDWriteTextLayout_Draw failed.\r\n"
							   L"HRESULT error code: 0x%08lx: %s",
							   hr, Win32ErrorAsString(hr));

		IUnknown_Release(layout);
		IUnknown_Release((IUnknown*)textRenderer);
	}
}

void STDMETHODCALLTYPE ID2D1DeviceContext_DrawText(
	ID2D1DeviceContext* This,
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
	{
		ID2D1DeviceContextVtbl* lpVtbl = (ID2D1DeviceContextVtbl*)KexVtblLookupOriginalTable(This->lpVtbl);
		lpVtbl->Base.DrawText((ID2D1RenderTarget*)This, string, stringLength, textFormat, layoutRect, defaultFillBrush, options, measuringMode);
	}
	else {
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
		if (measuringMode == DWRITE_MEASURING_MODE_NATURAL) {
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
		else {
			D2D1_MATRIX_3X2_F m;
			float dpiX, dpiY;

			This->lpVtbl->Base.GetDpi((ID2D1RenderTarget*)This, &dpiX, &dpiY);
			This->lpVtbl->Base.GetTransform((ID2D1RenderTarget*)This, &m);

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
		if (FAILED(hr)) {
			KexLogWarningEvent(L"Failed to create text layout.\r\n"
							   L"HRESULT error code: 0x%08lx: %s",
							   hr, Win32ErrorAsString(hr));
			return;
		}

		origin.x = min(layoutRect->left, layoutRect->right);
		origin.y = min(layoutRect->top, layoutRect->bottom);

		ID2D1DeviceContext_DrawTextLayout(This, origin, textLayout, defaultFillBrush, options);
		IUnknown_Release((IUnknown*)textLayout);
	}
}

void STDMETHODCALLTYPE ID2D1DeviceContext_DrawGlyphRun1(
	ID2D1DeviceContext* This,
	D2D1_POINT_2F baselineOrigin,
	IN CONST DWRITE_GLYPH_RUN* glyphRun,
	IN CONST DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
	IN ID2D1Brush* foregroundBrush,
	DWRITE_MEASURING_MODE measuringMode
)
{
	IUnknown* face;

	if (IUnknown_QueryInterface((IUnknown*)glyphRun->fontFace, &IID_IDWriteFontFace3, &face) != S_OK) 
	{
		ID2D1DeviceContextVtbl* lpVtbl = (ID2D1DeviceContextVtbl*)KexVtblLookupOriginalTable(This->lpVtbl);
		lpVtbl->DrawGlyphRun1(This, baselineOrigin, glyphRun, glyphRunDescription,
									foregroundBrush, measuringMode);
	}
	else {
		IID2D1TextRdrContext ctx;
		ctx.brush = foregroundBrush;
		ctx.dc = This;
		ctx.option = 0;

		ID2D1Factory* fact;
		This->lpVtbl->Base.Base.GetFactory((ID2D1Resource*)This, &fact);

		IID2D1TextRenderer* textRenderer = CreateTextRenderer(fact);
		IID2D1TextRenderer_DrawGlyphRun(textRenderer, &ctx, baselineOrigin.x, baselineOrigin.y, measuringMode, glyphRun, glyphRunDescription, NULL);

		IUnknown_Release(face);
		IUnknown_Release((IUnknown*)textRenderer);
	}
}

void STDMETHODCALLTYPE ID2D1DeviceContext_DrawGlyphRun(
	ID2D1DeviceContext* This,
	D2D1_POINT_2F baselineOrigin,
	IN CONST DWRITE_GLYPH_RUN* glyphRun,
	IN ID2D1Brush* foregroundBrush,
	DWRITE_MEASURING_MODE measuringMode
)
{
	ID2D1DeviceContext_DrawGlyphRun1(This, baselineOrigin, glyphRun, NULL, foregroundBrush, measuringMode);
}

void PatchD2D1DeviceContext(ID2D1DeviceContext* ctx)
{
	KEX_VTBL_MODIFICATION mod[] = {
		{offsetof(ID2D1RenderTargetVtbl, CreateCompatibleRenderTarget), ID2D1DeviceContext_CreateCompatibleRenderTarget},
		{offsetof(ID2D1RenderTargetVtbl, DrawText), ID2D1DeviceContext_DrawText},
		{offsetof(ID2D1RenderTargetVtbl, DrawTextLayout), ID2D1DeviceContext_DrawTextLayout},
		{offsetof(ID2D1RenderTargetVtbl, DrawGlyphRun), ID2D1DeviceContext_DrawGlyphRun},
		{offsetof(ID2D1DeviceContextVtbl, DrawGlyphRun1), ID2D1DeviceContext_DrawGlyphRun1}
	};
	KexVtblPatchInplace(ctx->lpVtbl, mod, ARRAYSIZE(mod), FALSE, NULL);
}

void PatchD2D1RenderTarget(ID2D1RenderTarget* rt)
{
	ID2D1DeviceContext* ctx;
	if (FAILED(ID2D1RenderTarget_QueryInterface(rt, &IID_ID2D1DeviceContext, &ctx)))
		return;

	PatchD2D1DeviceContext(ctx);
	IUnknown_Release((IUnknown*)ctx);

	if ((ULONG_PTR)ctx->lpVtbl == (ULONG_PTR)rt->lpVtbl)
		return;

	KEX_VTBL_MODIFICATION mod[] = {
		{offsetof(ID2D1RenderTargetVtbl, CreateCompatibleRenderTarget), ID2D1DeviceContext_CreateCompatibleRenderTarget},
		{offsetof(ID2D1RenderTargetVtbl, DrawText), ID2D1DeviceContext_DrawText},
		{offsetof(ID2D1RenderTargetVtbl, DrawTextLayout), ID2D1DeviceContext_DrawTextLayout},
		{offsetof(ID2D1RenderTargetVtbl, DrawGlyphRun), ID2D1DeviceContext_DrawGlyphRun}
	};
	KexVtblPatchInplace((PVOID)rt->lpVtbl, mod, ARRAYSIZE(mod), FALSE, NULL);
}

void PatchD2D1Device(ID2D1Device* device)
{
	KEX_VTBL_MODIFICATION mod[] = {
		{offsetof(ID2D1DeviceVtbl, CreateDeviceContext), ID2D1Device_CreateDeviceContext}
	};
	KexVtblPatchInplace(device->lpVtbl, mod, ARRAYSIZE(mod), FALSE, NULL);
}

void PatchD2D1Factory(ID2D1Factory* fact, BOOL isVersion1)
{
	KEX_VTBL_MODIFICATION mod[] = {
		{offsetof(ID2D1FactoryVtbl, CreateWicBitmapRenderTarget), ID2D1Factory0_CreateWicBitmapRenderTarget},
		{offsetof(ID2D1FactoryVtbl, CreateHwndRenderTarget), ID2D1Factory0_CreateHwndRenderTarget},
		{offsetof(ID2D1FactoryVtbl, CreateDxgiSurfaceRenderTarget), ID2D1Factory0_CreateDxgiSurfaceRenderTarget},
		{offsetof(ID2D1FactoryVtbl, CreateDCRenderTarget), ID2D1Factory0_CreateDCRenderTarget},
		{offsetof(ID2D1Factory1Vtbl, CreateDevice), ID2D1Factory1_CreateDevice}
	};
	KexVtblPatchInplace((PVOID)fact->lpVtbl, mod, ARRAYSIZE(mod) - (isVersion1 ? 0 : 1), FALSE, NULL);
}