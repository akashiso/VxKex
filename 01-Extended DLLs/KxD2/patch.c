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