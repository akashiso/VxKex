#include "buildcfg.h"
#include "kxcomp.h"
#include <netlistmgr.h>
#include <ws2def.h>
#include <ws2ipdef.h>

//
// This is a good boilerplate for the fucking WinRT interface with many version.
// God knows why they seperated the function table.
//

//
// WinRT
// Network : Windows.Networking.Connectivity.ConnectionCost
//


KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionCost_QueryInterface(
	IN	IConnectionCost* This,
	IN	REFIID		RefIID,
	OUT	PPVOID		Object)
{
	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	*Object = NULL;

	if (IsEqualIID(RefIID, &IID_IUnknown) ||
		IsEqualIID(RefIID, &IID_IInspectable) ||
		IsEqualIID(RefIID, &IID_IAgileObject) ||
		IsEqualIID(RefIID, &IID_IConnectionCost))
	{
		*Object = This->Parent;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else if (IsEqualIID(RefIID, &IID_IConnectionCost2))
	{
		*Object = &This->Parent->cConnectionCost2;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else
	{
		LPOLESTR IidAsString;

		StringFromIID(RefIID, &IidAsString);

		KexLogWarningEvent(
			L"QueryInterface called with unsupported IID: %s",
			IidAsString);

		CoTaskMemFree(IidAsString);
		return E_NOINTERFACE;
	}

	return S_OK;
}

KXCOMAPI ULONG STDMETHODCALLTYPE ConnectionCost_AddRef(
	IN	IConnectionCost* This)
{
	return InterlockedIncrement(&This->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE ConnectionCost_Release(
	IN	IConnectionCost* This)
{
	ULONG NewRefCount;

	NewRefCount = InterlockedDecrement(&This->RefCount);

	if (NewRefCount == 0)
	{
		CoTaskMemFree(This);
	}

	return NewRefCount;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionCost_GetIids(
	IN	IConnectionCost* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 2;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array)
	{
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_IConnectionCost;
	Array[1] = IID_IConnectionCost2;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionCost_GetRuntimeClassName(
	IN	IConnectionCost* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.Networking.Connectivity.ConnectionCost";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionCost_GetTrustLevel(
	IN	IConnectionCost* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionCost_get_NetworkCostType(
	IN IConnectionCost* thiz,
	OUT NetworkCostType* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = NetworkCostType_Unrestricted;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionCost_get_Roaming(
	IN IConnectionCost* thiz,
	OUT BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = FALSE;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionCost_get_OverDataLimit(
	IN IConnectionCost* thiz,
	OUT BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = FALSE;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionCost_get_ApproachingDataLimit(
	IN IConnectionCost* thiz,
	OUT BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = FALSE;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionCost2_get_BackgroundDataUsageRestricted(
	IN IConnectionCost* thiz,
	OUT BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = FALSE;
	return S_OK;
}

IConnectionCostVtbl CConnectionCostVtbl = {
	ConnectionCost_QueryInterface,
	ConnectionCost_AddRef,
	ConnectionCost_Release,

	ConnectionCost_GetIids,
	ConnectionCost_GetRuntimeClassName,
	ConnectionCost_GetTrustLevel,

	ConnectionCost_get_NetworkCostType,
	ConnectionCost_get_Roaming,
	ConnectionCost_get_OverDataLimit,
	ConnectionCost_get_ApproachingDataLimit
};

IConnectionCost2Vtbl CConnectionCost2Vtbl = {
	ConnectionCost_QueryInterface,
	ConnectionCost_AddRef,
	ConnectionCost_Release,

	ConnectionCost_GetIids,
	ConnectionCost_GetRuntimeClassName,
	ConnectionCost_GetTrustLevel,

	ConnectionCost2_get_BackgroundDataUsageRestricted
};

//
// Network : Windows.Networking.Connectivity.ConnectionProfile
// Most of them are just a stub because no application will use.
//

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_QueryInterface(
	IN	IConnectionProfile* This,
	IN	REFIID		RefIID,
	OUT	PPVOID		Object)
{
	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	*Object = NULL;

	if (IsEqualIID(RefIID, &IID_IUnknown) ||
		IsEqualIID(RefIID, &IID_IInspectable) ||
		IsEqualIID(RefIID, &IID_IAgileObject) ||
		IsEqualIID(RefIID, &IID_IConnectionProfile))
	{
		*Object = This->Parent;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else if (IsEqualIID(RefIID, &IID_IConnectionProfile2))
	{
		*Object = &This->Parent->cConnectionProfile2;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else if (IsEqualIID(RefIID, &IID_IConnectionProfile3))
	{
		*Object = &This->Parent->cConnectionProfile3;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else if (IsEqualIID(RefIID, &IID_IConnectionProfile4))
	{
		*Object = &This->Parent->cConnectionProfile4;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else if (IsEqualIID(RefIID, &IID_IConnectionProfile5))
	{
		*Object = &This->Parent->cConnectionProfile5;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else if (IsEqualIID(RefIID, &IID_IConnectionProfile6))
	{
		*Object = &This->Parent->cConnectionProfile6;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else
	{
		LPOLESTR IidAsString;

		StringFromIID(RefIID, &IidAsString);

		KexLogWarningEvent(
			L"QueryInterface called with unsupported IID: %s",
			IidAsString);

		CoTaskMemFree(IidAsString);
		return E_NOINTERFACE;
	}

	return S_OK;
}

KXCOMAPI ULONG STDMETHODCALLTYPE ConnectionProfile_AddRef(
	IN	IConnectionProfile* This)
{
	return InterlockedIncrement(&This->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE ConnectionProfile_Release(
	IN	IConnectionProfile* This)
{
	ULONG NewRefCount;

	NewRefCount = InterlockedDecrement(&This->RefCount);

	if (NewRefCount == 0)
	{
		IUnknown_Release(This->manager);
		CoTaskMemFree(This);
	}

	return NewRefCount;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_GetIids(
	IN	IConnectionProfile* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 6;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array)
	{
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_IConnectionProfile;
	Array[1] = IID_IConnectionProfile2;
	Array[2] = IID_IConnectionProfile3;
	Array[3] = IID_IConnectionProfile4;
	Array[4] = IID_IConnectionProfile5;
	Array[5] = IID_IConnectionProfile6;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_GetRuntimeClassName(
	IN	IConnectionProfile* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.Networking.Connectivity.ConnectionProfile";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_GetTrustLevel(
	IN	IConnectionProfile* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_get_ProfileName(
	IN IConnectionProfile* thiz,
	OUT void** cb)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_GetNetworkConnectivityLevel(
	IN IConnectionProfile* thiz,
	OUT NetworkConnectivityLevel* value)
{
	if (value == NULL)
		return E_POINTER;

	HRESULT hr;
	NetworkConnectivityLevel level;
	NLM_CONNECTIVITY connectivity;
	INetworkListManager* mgr = (INetworkListManager*)thiz->manager;
	hr = INetworkListManager_GetConnectivity(mgr, &connectivity);
	if (FAILED(hr))
		return hr;

	if (connectivity == NLM_CONNECTIVITY_DISCONNECTED)
		level = NetworkConnectivityLevel_None;
	else if (connectivity & (NLM_CONNECTIVITY_IPV4_INTERNET | NLM_CONNECTIVITY_IPV6_INTERNET))
		level = NetworkConnectivityLevel_InternetAccess;
	else if (connectivity & (NLM_CONNECTIVITY_IPV4_LOCALNETWORK | NLM_CONNECTIVITY_IPV6_LOCALNETWORK | NLM_CONNECTIVITY_IPV4_NOTRAFFIC | NLM_CONNECTIVITY_IPV6_NOTRAFFIC))
		level = NetworkConnectivityLevel_LocalAccess;
	else
		level = NetworkConnectivityLevel_ConstrainedInternetAccess;

	*value = level;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_GetNetworkNames(
	IN IConnectionProfile* thiz,
	OUT void** cb)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_GetConnectionCost(
	IN IConnectionProfile* thiz,
	OUT void** out)
{
	if (out == NULL)
		return E_POINTER;

	IConnectionCost* pf = (IConnectionCost*)CoTaskMemAlloc(sizeof(IConnectionCost));
	if (!pf)
		return E_OUTOFMEMORY;

	pf->lpVtbl = &CConnectionCostVtbl;
	pf->Parent = pf;
	pf->RefCount = 1;

	pf->cConnectionCost2.lpVtbl = &CConnectionCost2Vtbl;
	pf->cConnectionCost2.Parent = pf;
	*out = pf;
	KexLogWarningEvent(L"Stub function " __FUNCTIONW__ L" called");

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_GetDataPlanStatus(
	IN IConnectionProfile* thiz,
	OUT void** cb)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_GetLocalUsage(
	IN IConnectionProfile* thiz,
	IN UINT64 a,
	IN UINT64 b,
	OUT void** c)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_GetLocalUsagePerRoamingStates(
	IN IConnectionProfile* thiz,
	IN UINT64 a,
	IN UINT64 b,
	IN UINT32 c,
	OUT void** d)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile_get_NetworkSecuritySettings(
	IN IConnectionProfile* thiz,
	OUT void** cb)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile2_get_IsWwanConnectionProfile(
	IN IConnectionProfile* thiz,
	OUT BOOL* cb)
{
	if (cb == NULL)
		return E_POINTER;

	*cb = TRUE;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile2_get_IsWlanConnectionProfile(
	IN IConnectionProfile* thiz,
	OUT BOOL* cb)
{
	if (cb == NULL)
		return E_POINTER;

	*cb = FALSE;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile2_get_WwanConnectionProfileDetails(
	IN IConnectionProfile* thiz,
	OUT void** cb)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile2_get_WlanConnectionProfileDetails(
	IN IConnectionProfile* thiz,
	OUT void** cb)
{
	if (cb == NULL)
		return E_POINTER;

	*cb = NULL;
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile2_get_ServiceProviderGuid(
	IN IConnectionProfile* thiz,
	OUT void** cb)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile2_GetSignalBars(
	IN IConnectionProfile* thiz,
	OUT void** cb)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile2_GetDomainConnectivityLevel(
	IN IConnectionProfile* thiz,
	OUT UINT32* cb)
{
	if (cb == NULL)
		return E_POINTER;

	*cb = 5;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile2_GetNetworkUsageAsync(
	IN IConnectionProfile* thiz,
	IN UINT64 a,
	IN UINT64 b,
	IN UINT32 c,
	OUT void** d)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile2_GetConnectivityIntervalsAsync(
	IN IConnectionProfile* thiz,
	IN UINT64 a,
	IN UINT64 b,
	OUT void** c)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile3_GetAttributedNetworkUsageAsync(
	IN IConnectionProfile* thiz,
	IN UINT64 a,
	IN UINT64 b,
	OUT void** c)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile4_GetProviderNetworkUsageAsync(
	IN IConnectionProfile* thiz,
	IN UINT64 a,
	IN UINT64 b,
	OUT void** c)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile5_get_CanDelete(
	IN IConnectionProfile* thiz,
	OUT BOOL* cb)
{
	if (cb == NULL)
		return E_POINTER;

	*cb = FALSE;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile5_TryDeleteAsync(
	IN IConnectionProfile* thiz,
	OUT void** cb)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_FAIL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ConnectionProfile6_IsDomainAuthenticatedBy(
	IN IConnectionProfile* thiz,
	OUT BOOL* cb)
{
	if (cb == NULL)
		return E_POINTER;

	*cb = TRUE;
	KexLogWarningEvent(L"Stub function " __FUNCTIONW__ L" called");
	return S_OK;
}

IConnectionProfileVtbl CConnectionProfileVtbl = {
	ConnectionProfile_QueryInterface,
	ConnectionProfile_AddRef,
	ConnectionProfile_Release,

	ConnectionProfile_GetIids,
	ConnectionProfile_GetRuntimeClassName,
	ConnectionProfile_GetTrustLevel,

	ConnectionProfile_get_ProfileName,
	ConnectionProfile_GetNetworkConnectivityLevel,
	ConnectionProfile_GetNetworkNames,
	ConnectionProfile_GetConnectionCost,
	ConnectionProfile_GetDataPlanStatus,
	ConnectionProfile_GetLocalUsage,
	ConnectionProfile_GetLocalUsagePerRoamingStates,
	ConnectionProfile_get_NetworkSecuritySettings
};

IConnectionProfile2Vtbl CConnectionProfile2Vtbl = {
	ConnectionProfile_QueryInterface,
	ConnectionProfile_AddRef,
	ConnectionProfile_Release,

	ConnectionProfile_GetIids,
	ConnectionProfile_GetRuntimeClassName,
	ConnectionProfile_GetTrustLevel,

	ConnectionProfile2_get_IsWwanConnectionProfile,
	ConnectionProfile2_get_IsWlanConnectionProfile,
	ConnectionProfile2_get_WwanConnectionProfileDetails,
	ConnectionProfile2_get_WlanConnectionProfileDetails,
	ConnectionProfile2_get_ServiceProviderGuid,
	ConnectionProfile2_GetSignalBars,
	ConnectionProfile2_GetDomainConnectivityLevel,
	ConnectionProfile2_GetNetworkUsageAsync,
	ConnectionProfile2_GetConnectivityIntervalsAsync,
};

IConnectionProfile3Vtbl CConnectionProfile3Vtbl = {
	ConnectionProfile_QueryInterface,
	ConnectionProfile_AddRef,
	ConnectionProfile_Release,

	ConnectionProfile_GetIids,
	ConnectionProfile_GetRuntimeClassName,
	ConnectionProfile_GetTrustLevel,

	ConnectionProfile3_GetAttributedNetworkUsageAsync
};

IConnectionProfile4Vtbl CConnectionProfile4Vtbl = {
	ConnectionProfile_QueryInterface,
	ConnectionProfile_AddRef,
	ConnectionProfile_Release,

	ConnectionProfile_GetIids,
	ConnectionProfile_GetRuntimeClassName,
	ConnectionProfile_GetTrustLevel,

	ConnectionProfile4_GetProviderNetworkUsageAsync
};

IConnectionProfile5Vtbl CConnectionProfile5Vtbl = {
	ConnectionProfile_QueryInterface,
	ConnectionProfile_AddRef,
	ConnectionProfile_Release,

	ConnectionProfile_GetIids,
	ConnectionProfile_GetRuntimeClassName,
	ConnectionProfile_GetTrustLevel,

	ConnectionProfile5_get_CanDelete,
	ConnectionProfile5_TryDeleteAsync
};

IConnectionProfile6Vtbl CConnectionProfile6Vtbl = {
	ConnectionProfile_QueryInterface,
	ConnectionProfile_AddRef,
	ConnectionProfile_Release,

	ConnectionProfile_GetIids,
	ConnectionProfile_GetRuntimeClassName,
	ConnectionProfile_GetTrustLevel,

	ConnectionProfile6_IsDomainAuthenticatedBy
};


// 
// Network : Windows.Networking.Connectivity.NetworkInformation
//

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_QueryInterface(
	IN	INetworkInformationStatics* This,
	IN	REFIID		RefIID,
	OUT	PPVOID		Object)
{
	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	*Object = NULL;

	if (IsEqualIID(RefIID, &IID_IUnknown) ||
		IsEqualIID(RefIID, &IID_IInspectable) ||
		IsEqualIID(RefIID, &IID_IAgileObject) ||
		IsEqualIID(RefIID, &IID_INetworkInformationStatics))
	{
		*Object = This;
	}
	else
	{
		LPOLESTR IidAsString;

		StringFromIID(RefIID, &IidAsString);

		KexLogWarningEvent(
			L"QueryInterface called with unsupported IID: %s",
			IidAsString);

		CoTaskMemFree(IidAsString);
		return E_NOINTERFACE;
	}

	return S_OK;
}

KXCOMAPI ULONG STDMETHODCALLTYPE NetworkInformation_AddRef(
	IN	INetworkInformationStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE NetworkInformation_Release(
	IN	INetworkInformationStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_GetIids(
	IN	INetworkInformationStatics* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 1;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array)
	{
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_INetworkInformationStatics;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_GetRuntimeClassName(
	IN	INetworkInformationStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.Networking.Connectivity.NetworkInformation";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_GetTrustLevel(
	IN	INetworkInformationStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_GetConnectionProfiles(
	IN INetworkInformationStatics* thiz,
	OUT void** out)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_GetInternetConnectionProfile(
	IN INetworkInformationStatics* thiz,
	OUT void** out)
{
	if (out == NULL)
		return E_POINTER;

	IConnectionProfile* pf = (IConnectionProfile*)CoTaskMemAlloc(sizeof(IConnectionProfile));
	if (!pf)
		return E_OUTOFMEMORY;

	HRESULT hr;
	INetworkListManager* mgr;
	hr = Ext_CoCreateInstance(&CLSID_NetworkListManager,
							  NULL,
							  CLSCTX_INPROC_SERVER,
							  &IID_INetworkListManager,
							  (void**)&mgr);
	if (FAILED(hr))
	{
		CoTaskMemFree(pf);
		return hr;
	}

	pf->lpVtbl = &CConnectionProfileVtbl;
	pf->Parent = pf;
	pf->manager = (IUnknown*)mgr;
	pf->RefCount = 1;

	NetworkConnectivityLevel level;
	hr = ConnectionProfile_GetNetworkConnectivityLevel(pf, &level);
	if (FAILED(hr) || level == NetworkConnectivityLevel_None)
	{
		CoTaskMemFree(pf);
		INetworkListManager_Release(mgr);
		return hr;
	}

	pf->cConnectionProfile2.lpVtbl = &CConnectionProfile2Vtbl;
	pf->cConnectionProfile2.Parent = pf;

	pf->cConnectionProfile3.lpVtbl = &CConnectionProfile3Vtbl;
	pf->cConnectionProfile3.Parent = pf;

	pf->cConnectionProfile4.lpVtbl = &CConnectionProfile4Vtbl;
	pf->cConnectionProfile4.Parent = pf;

	pf->cConnectionProfile5.lpVtbl = &CConnectionProfile5Vtbl;
	pf->cConnectionProfile5.Parent = pf;

	pf->cConnectionProfile6.lpVtbl = &CConnectionProfile6Vtbl;
	pf->cConnectionProfile6.Parent = pf;

	*out = pf;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_GetLanIdentifiers(
	IN INetworkInformationStatics* thiz,
	OUT void** out)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_GetHostNames(
	IN INetworkInformationStatics* thiz,
	OUT void** out)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_GetProxyConfigurationAsync(
	IN INetworkInformationStatics* thiz,
	void* out0,
	OUT IUnknown** out1)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_GetSortedEndpointPairs(
	INetworkInformationStatics* thiz,
	void* a,
	UINT32 b,
	void** c)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_add_NetworkStatusChanged(
	IN INetworkInformationStatics* thiz,
	IN ITypedEventHandler* hdr,
	OUT EventRegistrationToken* out1)
{
	out1->value = 0xDEADBEEF;
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE NetworkInformation_remove_NetworkStatusChanged(
	IN INetworkInformationStatics* thiz,
	IN EventRegistrationToken cb)
{
	if (cb.value != 0xDEADBEEF)
		return E_INVALIDARG;

	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return S_OK;
}

INetworkInformationStaticsVtbl CNetworkInformationStaticsVtbl = {
	NetworkInformation_QueryInterface,
	NetworkInformation_AddRef,
	NetworkInformation_Release,

	NetworkInformation_GetIids,
	NetworkInformation_GetRuntimeClassName,
	NetworkInformation_GetTrustLevel,

	NetworkInformation_GetConnectionProfiles,
	NetworkInformation_GetInternetConnectionProfile,
	NetworkInformation_GetLanIdentifiers,
	NetworkInformation_GetHostNames,
	NetworkInformation_GetProxyConfigurationAsync,
	NetworkInformation_GetSortedEndpointPairs,
	NetworkInformation_add_NetworkStatusChanged,
	NetworkInformation_remove_NetworkStatusChanged
};

INetworkInformationStatics CNetworkInformationStatics = {
	&CNetworkInformationStaticsVtbl
};

// Networking : Windows.Networking.IHostName

typedef struct INETADDR
{
	USHORT Family;
	USHORT Port;
	IN_ADDR ipv4Address;
	IN6_ADDR ipv6Address;
	ULONG ipv6ScopeId;
	CHAR Padding[4];
} INETADDR;

STATIC HRESULT HostNameAddressToString(
	IN INETADDR* lpsaAddress,
	OUT HSTRING* str)
{
	ULONG Length;
	WCHAR Buffer[72];

	Length = 65;
	if (lpsaAddress->Family == AF_INET)
	{
		RtlIpv4AddressToStringW(&lpsaAddress->ipv4Address, Buffer);
		return WindowsCreateString(Buffer, (ULONG)wcslen(Buffer), str);
	}
	if (lpsaAddress->Family == AF_INET6)
	{
		if (!IN6_IS_ADDR_V4MAPPED(&lpsaAddress->ipv6Address))
		{
			RtlIpv6AddressToStringExW(&lpsaAddress->ipv6Address, 0, 0, Buffer, &Length);
			return WindowsCreateString(Buffer, Length, str);
		}

		RtlIpv4AddressToStringW(&lpsaAddress->ipv4Address, Buffer);
		return WindowsCreateString(Buffer, (ULONG)wcslen(Buffer), str);
	}

	lpsaAddress->Family = 0;
	// TODO : Deal with Bluetooth Address
	return E_NOTIMPL;
}

STATIC HRESULT HostNameStringToAddress(
	IN  PCWSTR str,
	OUT INETADDR* lpsaAddress)
{
	NTSTATUS status;

	ZeroMemory(lpsaAddress, sizeof(INETADDR));
	status = RtlIpv6StringToAddressExW(str,
									   &lpsaAddress->ipv6Address,
									   &lpsaAddress->ipv6ScopeId,
									   &lpsaAddress->Port);
	if (NT_SUCCESS(status))
	{
		lpsaAddress->Family = AF_INET6;
		return S_OK;
	}

	status = RtlIpv4StringToAddressExW(str, TRUE, &lpsaAddress->ipv4Address, &lpsaAddress->Port);
	if (NT_SUCCESS(status))
	{
		lpsaAddress->Family = AF_INET;
		return S_OK;
	}

	lpsaAddress->Family = 0;
	// TODO : Deal with Bluetooth Address
	return E_NOTIMPL;
}

STATIC HRESULT HostNameCanonicalizeName(IHostName* name)
{
	PCWSTR RawName = WindowsGetStringRawBuffer(name->RawName, 0);
	ULONG Length = WindowsGetStringLen(name->RawName);

	HRESULT Result = S_OK;
	INETADDR Address;
	if (SUCCEEDED(HostNameStringToAddress(RawName, &Address)))
	{
		name->Type = Address.Family == AF_INET ? HostNameType_Ipv4 : HostNameType_Ipv6;

		Result = HostNameAddressToString(&Address, &name->DisplayName);
		if (SUCCEEDED(Result))
			return WindowsDuplicateString(name->DisplayName, &name->CanonicalName);

		return Result;
	}

	// Convert Unicode to punycoded name

	LPWSTR Punycoded = NULL;
	LPWSTR Unicode = NULL;

	INT PunycodedLength = IdnToAscii(0, RawName, Length, 0, 0);
	if (PunycodedLength == 0) goto Exit;

	Punycoded = (LPWSTR)CoTaskMemAlloc(PunycodedLength * sizeof(WCHAR));
	if (Punycoded)
	{
		PunycodedLength = IdnToAscii(0, RawName, Length, Punycoded, PunycodedLength);
		if (PunycodedLength == 0) goto Exit;
	}
	else return E_OUTOFMEMORY;

	// Convert punycoded name to Unicode

	INT UnicodeLength = IdnToUnicode(0, Punycoded, PunycodedLength, 0, 0);
	if (UnicodeLength == 0) goto Exit;

	Unicode = (LPWSTR)CoTaskMemAlloc(UnicodeLength * sizeof(WCHAR));
	if (Unicode)
	{
		UnicodeLength = IdnToUnicode(0, Punycoded, PunycodedLength, Unicode, UnicodeLength);
		if (UnicodeLength == 0) goto Exit;
	}
	else
	{
		CoTaskMemFree(Punycoded);
		return E_OUTOFMEMORY;
	}

	Result = WindowsCreateString(Unicode, UnicodeLength, &name->DisplayName);
	if (FAILED(Result)) goto Exit;

	// Convert punycoded name (lowercase) to Unicode

	for (INT i = 0; i < PunycodedLength; i++)
		Punycoded[i] = towlower(Punycoded[i]);

	CoTaskMemFree(Unicode);
	UnicodeLength = IdnToUnicode(0, Punycoded, PunycodedLength, 0, 0);
	if (UnicodeLength == 0) goto Exit;

	Unicode = (LPWSTR)CoTaskMemAlloc(UnicodeLength * sizeof(WCHAR));
	if (Unicode)
	{
		UnicodeLength = IdnToUnicode(0, Punycoded, PunycodedLength, Unicode, UnicodeLength);
		if (UnicodeLength == 0) goto Exit;
	}
	else
	{
		CoTaskMemFree(Punycoded);
		return E_OUTOFMEMORY;
	}

	// TODO : Use DnsVaildateName
	name->Type = HostNameType_DomainName;
	Result = WindowsCreateString(Unicode, UnicodeLength, &name->CanonicalName);

Exit:
	if (Punycoded)
		CoTaskMemFree(Punycoded);
	if (Unicode)
		CoTaskMemFree(Unicode);
	return SUCCEEDED(Result) ? HRESULT_FROM_WIN32(GetLastError()) : Result;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_QueryInterface(
	IN	IHostName* This,
	IN	REFIID		RefIID,
	OUT	PPVOID		Object)
{
	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	*Object = NULL;

	if (IsEqualIID(RefIID, &IID_IUnknown) ||
		IsEqualIID(RefIID, &IID_IInspectable) ||
		IsEqualIID(RefIID, &IID_IAgileObject) ||
		IsEqualIID(RefIID, &IID_IHostName))
	{
		*Object = This;
		InterlockedIncrement(&This->RefCount);
	}
	else
	{
		LPOLESTR IidAsString;

		StringFromIID(RefIID, &IidAsString);

		KexLogWarningEvent(
			L"QueryInterface called with unsupported IID: %s",
			IidAsString);

		CoTaskMemFree(IidAsString);
		return E_NOINTERFACE;
	}

	return S_OK;
}

KXCOMAPI ULONG STDMETHODCALLTYPE HostName_AddRef(
	IN	IHostName* This)
{
	return InterlockedIncrement(&This->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE HostName_Release(
	IN	IHostName* This)
{
	ULONG NewRefCount;

	NewRefCount = InterlockedDecrement(&This->RefCount);

	if (NewRefCount == 0)
	{
		WindowsDeleteString(This->RawName);
		WindowsDeleteString(This->CanonicalName);
		WindowsDeleteString(This->DisplayName);
		CoTaskMemFree(This);
	}

	return NewRefCount;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_GetIids(
	IN	IHostName* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 1;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array)
	{
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_IHostName;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_GetRuntimeClassName(
	IN	IHostName* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.Networking.HostName";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_GetTrustLevel(
	IN	IHostName* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_get_IPInformation(
	IN IHostName* thiz,
	OUT IUnknown** out)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_get_RawName(
	IN IHostName* thiz,
	OUT HSTRING* out)
{
	if (out == NULL)
		return E_POINTER;

	return WindowsDuplicateString(thiz->RawName, out);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_get_DisplayName(
	IN IHostName* thiz,
	OUT HSTRING* out)
{
	if (out == NULL)
		return E_POINTER;

	return WindowsDuplicateString(thiz->DisplayName, out);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_get_CanonicalName(
	IN IHostName* thiz,
	OUT HSTRING* out)
{
	if (out == NULL)
		return E_POINTER;

	return WindowsDuplicateString(thiz->CanonicalName, out);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_get_Type(
	IN IHostName* thiz,
	OUT HostNameType* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = thiz->Type;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostName_IsEqual(
	IN IHostName* thiz,
	IN IHostName* that,
	OUT BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	HRESULT Result;
	INT Cmp;
	Result = WindowsCompareStringOrdinal(thiz->CanonicalName, that->CanonicalName, &Cmp);

	*out = Cmp == 0 ? TRUE : FALSE;
	return Result;
}

IHostNameVtbl CHostNameVtbl = {
	HostName_QueryInterface,
	HostName_AddRef,
	HostName_Release,

	HostName_GetIids,
	HostName_GetRuntimeClassName,
	HostName_GetTrustLevel,

	HostName_get_IPInformation,
	HostName_get_RawName,
	HostName_get_DisplayName,
	HostName_get_CanonicalName,
	HostName_get_Type,
	HostName_IsEqual
};


// Networking : Windows.Networking.IHostNameFactory

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameFactory_QueryInterface(
	IN	IHostNameFactory* This,
	IN	REFIID		RefIID,
	OUT	PPVOID		Object)
{
	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	*Object = NULL;

	if (IsEqualIID(RefIID, &IID_IUnknown) ||
		IsEqualIID(RefIID, &IID_IInspectable) ||
		IsEqualIID(RefIID, &IID_IAgileObject) ||
		IsEqualIID(RefIID, &IID_IHostNameFactory))
	{
		*Object = This;
	}
	else
	{
		LPOLESTR IidAsString;

		StringFromIID(RefIID, &IidAsString);

		KexLogWarningEvent(
			L"QueryInterface called with unsupported IID: %s",
			IidAsString);

		CoTaskMemFree(IidAsString);
		return E_NOINTERFACE;
	}

	return S_OK;
}

KXCOMAPI ULONG STDMETHODCALLTYPE HostNameFactory_AddRef(
	IN	IHostNameFactory* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE HostNameFactory_Release(
	IN	IHostNameFactory* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameFactory_GetIids(
	IN	IHostNameFactory* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 1;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array)
	{
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_IHostNameFactory;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameFactory_GetRuntimeClassName(
	IN	IHostNameFactory* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.Networking.IHostNameFactory";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameFactory_GetTrustLevel(
	IN	IHostNameFactory* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameFactory_CreateHostName(
	IN IHostNameFactory* thiz,
	IN HSTRING hostName,
	OUT IHostName** out)
{
	if (out == NULL)
		return E_POINTER;

	IHostName* Name = (IHostName*)CoTaskMemAlloc(sizeof(IHostName));
	HRESULT hr;
	if (Name == NULL)
		return E_OUTOFMEMORY;

	Name->lpVtbl = &CHostNameVtbl;

	hr = WindowsDuplicateString(hostName, &Name->RawName);
	if (FAILED(hr))
	{
		CoTaskMemFree(Name);
		return hr;
	}
	HostNameCanonicalizeName(Name);

	*out = Name;
	return S_OK;
}

IHostNameFactoryVtbl CHostNameFactoryVtbl = {
	HostNameFactory_QueryInterface,
	HostNameFactory_AddRef,
	HostNameFactory_Release,
	HostNameFactory_GetIids,
	HostNameFactory_GetRuntimeClassName,
	HostNameFactory_GetTrustLevel,
	HostNameFactory_CreateHostName
};

IHostNameFactory CHostNameFactory = {
	&CHostNameFactoryVtbl
};

// Networking : Windows.Networking.IHostNameStatics

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameStatics_QueryInterface(
	IN	IHostNameStatics* This,
	IN	REFIID		RefIID,
	OUT	PPVOID		Object)
{
	ASSERT(This != NULL);
	ASSERT(RefIID != NULL);
	ASSERT(Object != NULL);

	*Object = NULL;

	if (IsEqualIID(RefIID, &IID_IUnknown) ||
		IsEqualIID(RefIID, &IID_IInspectable) ||
		IsEqualIID(RefIID, &IID_IAgileObject) ||
		IsEqualIID(RefIID, &IID_IHostNameStatics))
	{
		*Object = This;
	}
	else
	{
		LPOLESTR IidAsString;

		StringFromIID(RefIID, &IidAsString);

		KexLogWarningEvent(
			L"QueryInterface called with unsupported IID: %s",
			IidAsString);

		CoTaskMemFree(IidAsString);
		return E_NOINTERFACE;
	}

	return S_OK;
}

KXCOMAPI ULONG STDMETHODCALLTYPE HostNameStatics_AddRef(
	IN	IHostNameStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE HostNameStatics_Release(
	IN	IHostNameStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameStatics_GetIids(
	IN	IHostNameStatics* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 1;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array)
	{
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_IHostNameStatics;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameStatics_GetRuntimeClassName(
	IN	IHostNameStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.Networking.IHostNameStatics";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameStatics_GetTrustLevel(
	IN	IHostNameStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE HostNameStatics_Compare(
	IN IHostNameStatics* thiz,
	IN HSTRING str0,
	IN HSTRING str1,
	OUT INT* out)
{
	if (out == NULL)
		return E_POINTER;

	HRESULT Result;
	IHostName* Name0;
	Result = HostNameFactory_CreateHostName(&CHostNameFactory, str0, &Name0);
	if (FAILED(Result))
		return Result;

	IHostName* Name1;
	Result = HostNameFactory_CreateHostName(&CHostNameFactory, str0, &Name1);
	if (FAILED(Result))
	{
		HostName_Release(Name0);
		return Result;
	}

	Result = WindowsCompareStringOrdinal(Name0->CanonicalName, Name1->CanonicalName, out);
	HostName_Release(Name0);
	HostName_Release(Name1);
	return Result;
}

IHostNameStaticsVtbl CHostNameStaticsVtbl = {
	HostNameStatics_QueryInterface,
	HostNameStatics_AddRef,
	HostNameStatics_Release,
	HostNameStatics_GetIids,
	HostNameStatics_GetRuntimeClassName,
	HostNameStatics_GetTrustLevel,
	HostNameStatics_Compare
};

IHostNameStatics CHostNameStatics = {
	&CHostNameStaticsVtbl
};