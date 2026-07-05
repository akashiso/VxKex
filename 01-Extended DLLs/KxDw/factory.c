#include "buildcfg.h"
#include "kxdwp.h"

int _fltused = 0;

UINT STDMETHODCALLTYPE IUnknownObj_Release(
	IUnknown* This
)
{
	PPVOID lpVtbl = KexVtblFindOriginalVFTable(This->lpVtbl);
	PVOID pVtbl = This->lpVtbl;

	// Call to IUnknown?_Release
	UINT RefCount =  ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown* This
		))(lpVtbl[2]))(This);
	if (RefCount == 0)
		KexVtblReleaseRewriteData(pVtbl);

	return RefCount;
}

HRESULT STDMETHODCALLTYPE IDWriteTextLayout4_GetFontCollection(
	IUnknown* This,
	UINT32 pos,
	IUnknown** collection,
	void* range)
{
	HRESULT hr;
	PPVOID lpVtbl = KexVtblFindOriginalVFTable(This->lpVtbl);

	// Call to IDWriteTextLayout4_GetFontCollection
	hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown * This,
		UINT32 pos,
		IUnknown * *collection,
		void* range))(lpVtbl[44]))(This, pos, collection, range);

	if (collection)
	{
		// DWrite will return S_OK but the output collection is NULL
		// so we have to patch it.
		if (*collection == NULL)
		{
			IUnknown* dwfact;
			if (FAILED(hr = DWriteCoreCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
													&IID_IDWriteFactory3, &dwfact)))
				return hr;

			// Call to IDWriteFactory_GetSystemFontCollection
			lpVtbl = (PPVOID)dwfact->lpVtbl;
			hr = ((HRESULT(STDMETHODCALLTYPE*)(
				IUnknown * This,
				IUnknown * *fontCollection,
				BOOL       checkForUpdates))(lpVtbl[3]))(dwfact, collection, FALSE);
			IUnknown_Release(dwfact);
		}
}
	return hr;
}

HRESULT STDMETHODCALLTYPE IDWriteFactory7_CreateTextLayout(
	IUnknown* This, 
	const WCHAR* string, 
	UINT32 len, 
	IUnknown* format, 
	FLOAT max_width,
	FLOAT max_height, 
	IUnknown** layout)
{
	HRESULT hr;
	PPVOID lpVtbl = KexVtblFindOriginalVFTable(This->lpVtbl);

	// Call to IDWriteFactory7_CreateTextLayout
	hr = ((HRESULT(STDMETHODCALLTYPE*)(
		IUnknown * This,
		const WCHAR * string,
		UINT32 len,
		IUnknown * format,
		FLOAT max_width,
		FLOAT max_height,
		IUnknown * *layout))(lpVtbl[18]))(This, string, len, format, max_width, max_height, layout);

	if (SUCCEEDED(hr))
	{
		KEX_VFT_VTBL_MODIFICATION mod[] = {
			{2 * sizeof(PVOID), IUnknownObj_Release},
			{44 * sizeof(PVOID), IDWriteTextLayout4_GetFontCollection}
		};
		KexVtblRewriteInplace((*layout)->lpVtbl, mod, 2, FALSE, NULL);
	}

	return hr;
}

KXDWAPI HRESULT WINAPI Ext_DWriteCreateFactory(
	IN	DWRITE_FACTORY_TYPE	factoryType,
	IN	REFIID              iid,
	OUT	IUnknown            **factory)
{
	if (IsEqualIID(iid, &IID_IDWriteFactory)) {
		unless (KexData->IfeoParameters.DisableAppSpecific) {
			if ((KexData->Flags & KEXDATA_FLAG_CHROMIUM)) {
				return DWriteCoreCreateFactory(factoryType, iid, factory);
			}
		}
		return DWriteCreateFactory(factoryType, iid, factory);
	} else {
		unless (KexData->IfeoParameters.DisableAppSpecific) {
			if (AshExeBaseNameIs(L"Zps.exe")) {
				return DWriteCreateFactory(factoryType, iid, factory);
			}
		}

		IUnknown* obj;
		HRESULT Result = DWriteCoreCreateFactory(factoryType, iid, &obj);

		KEX_VFT_VTBL_MODIFICATION mod[] = {
			{2 * sizeof(PVOID), IUnknownObj_Release},
			{18 * sizeof(PVOID), IDWriteFactory7_CreateTextLayout}
		};
		KexVtblRewriteInplace(obj->lpVtbl, mod, 2, FALSE, NULL);
		*factory = obj;
		return Result;
	}
}