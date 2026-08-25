#include "buildcfg.h"
#include "kxdwp.h"

IUnknown* IIDWriteFontFace_GetDefaultRenderingParams()
{
	IUnknown* dwfact;

	if (FAILED(DWriteCoreCreateFactory(DWRITE_FACTORY_TYPE_SHARED, &IID_IDWriteFactory,
									   (IUnknown**)&dwfact))) {
		return NULL;
	}

	IUnknown* params;
	PPVOID lpVtbl = (PPVOID)(((IUnknown*)dwfact)->lpVtbl);

	// Call to IDWriteFactory_CreateRenderingParams
	if (FAILED(((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown*,
		IUnknown**))(lpVtbl[10]))(dwfact, &params))) {
		IUnknown_Release(dwfact);
		return NULL;
	}

	IUnknown_Release(dwfact);
	return params;
}

HRESULT STDMETHODCALLTYPE IDWriteFontFace_GetRecommendedRenderingMode_15(
	IUnknown* This,
	FLOAT emSize,
	FLOAT pixels_per_dip,
	UINT32 mode,
	IUnknown* params,
	UINT32* rendering_mode)
{
	HRESULT hr;
	PPVOID lpVtbl = KexVtblLookupOriginalTable(This->lpVtbl);
	BOOL defaultParams = FALSE;

	if (params == NULL) {
		params = IIDWriteFontFace_GetDefaultRenderingParams();
		defaultParams = TRUE;
	}

	hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown * This,
		FLOAT emSize,
		FLOAT pixels_per_dip,
		UINT32 mode,
		IUnknown * params,
		UINT32 * rendering_mode))(lpVtbl[15]))(This, emSize, pixels_per_dip, mode, params, rendering_mode);

	if (defaultParams) IUnknown_Release(params);
	return hr;
}

HRESULT STDMETHODCALLTYPE IDWriteFontFace2_GetRecommendedRenderingMode(
	IUnknown* This,
	FLOAT fontEmSize,
	FLOAT dpiX,
	FLOAT dpiY,
	const void* transform,
	BOOL is_sideways,
	UINT32 threshold,
	UINT32 measuringmode,
	IUnknown* params,
	UINT32* renderingmode,
	UINT32* gridfitmode)
{
	HRESULT hr;
	PPVOID lpVtbl = KexVtblLookupOriginalTable(This->lpVtbl);
	BOOL defaultParams = FALSE;

	if (params == NULL) {
		params = IIDWriteFontFace_GetDefaultRenderingParams();
		defaultParams = TRUE;
	}

	hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown * This,
		FLOAT fontEmSize,
		FLOAT dpiX,
		FLOAT dpiY,
		const void* transform,
		BOOL is_sideways,
		UINT32 threshold,
		UINT32 measuringmode,
		IUnknown * params,
		UINT32 * renderingmode,
		UINT32 * gridfitmode))(lpVtbl[34]))(This, fontEmSize, dpiX, dpiY, transform, is_sideways, threshold, measuringmode, params, renderingmode, gridfitmode);

	if (defaultParams) IUnknown_Release(params);
	return hr;
}

HRESULT STDMETHODCALLTYPE IDWriteFontFace3_GetRecommendedRenderingMode(
	IUnknown* This,
	FLOAT emsize,
	FLOAT dpi_x,
	FLOAT dpi_y,
	const void* transform,
	BOOL is_sideways,
	UINT32 threshold,
	UINT32 measuring_mode,
	IUnknown* params,
	UINT32* rendering_mode,
	UINT32* gridfit_mode)
{
	HRESULT hr;
	PPVOID lpVtbl = KexVtblLookupOriginalTable(This->lpVtbl);
	BOOL defaultParams = FALSE;

	if (params == NULL) {
		params = IIDWriteFontFace_GetDefaultRenderingParams();
		defaultParams = TRUE;
	}

	hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown * This,
		FLOAT emsize,
		FLOAT dpi_x,
		FLOAT dpi_y,
		const void* transform,
		BOOL is_sideways,
		UINT32 threshold,
		UINT32 measuring_mode,
		IUnknown * params,
		UINT32 * rendering_mode,
		UINT32 * gridfit_mode))(lpVtbl[44]))(This, emsize, dpi_x, dpi_y, transform, is_sideways, threshold, measuring_mode, params, rendering_mode, gridfit_mode);

	if (defaultParams) IUnknown_Release(params);
	return hr;
}

//
// DWriteFontFace in dwrw10 only got two versions of VFTable:
// const DWriteCore::ApiImpl::DWriteFontFace::`vftable'{for `DWriteCore::IFontFaceInternal'}	dwrw10+00000000001D8468
// const DWriteCore::ApiImpl::DWriteFontFace::`vftable'{for `IDWriteFontFaceReference1'}		dwrw10+00000000001D83C8
// 
// Currently, we just simply patch the first one.
//

void PatchDWriteFontFaceByDWriteFactory(IUnknown* dwfact)
{
	KEX_VTBL_MODIFICATION mod[] = {
		{15 * sizeof(PVOID), IDWriteFontFace_GetRecommendedRenderingMode_15},
		{34 * sizeof(PVOID), IDWriteFontFace2_GetRecommendedRenderingMode},
		{44 * sizeof(PVOID), IDWriteFontFace3_GetRecommendedRenderingMode}
	};

	IUnknown* fontset;
	// Call to IDWriteFactory_GetSystemFontSet
	if (FAILED(((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown*,
		IUnknown**))(((PPVOID)dwfact->lpVtbl)[35]))(dwfact, &fontset))) {
		return;
	}

	// Call to IDWriteFontSet_GetFontCount
	if (((UINT32(STDMETHODCALLTYPE*)(
		IUnknown*))((PPVOID)fontset->lpVtbl)[3])(fontset) == 0) {
		return;
	}

	IUnknown* fontface;
	// Call to IDWriteFontSet1_CreateFontFace
	if (FAILED(((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown*,
		UINT32,
		IUnknown**))(((PPVOID)fontset->lpVtbl)[24]))(fontset, 0, &fontface))) {
		IUnknown_Release(fontset);
		return;
	}

	KexVtblPatchInplace(fontface->lpVtbl, mod, ARRAYSIZE(mod), FALSE, NULL);

	IUnknown_Release(fontset);
	IUnknown_Release(fontface);
}