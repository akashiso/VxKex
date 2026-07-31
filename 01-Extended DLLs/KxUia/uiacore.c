#include "buildcfg.h"
#include "kxuiap.h"

KXUIAAPI HRESULT WINAPI UiaRaiseNotificationEvent(
	IN	DWORD					*provider,
	NotificationKind			notificationKind,
	NotificationProcessing		notificationProcessing,
	IN	BSTR					displayString	OPTIONAL,
	IN	BSTR					activityId)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

KXUIAAPI HRESULT WINAPI UiaDisconnectProvider(
	IN		IUnknown* Provider)
{
	return E_NOTIMPL;
}

//
// Some Unity games require this.
//
KXUIAAPI HRESULT WINAPI UiaDisconnectAllProviders(
	VOID)
{
	return E_NOTIMPL;
}

KXUIAAPI HRESULT WINAPI UiaRaiseChangesEvent(
	IN IUnknown* pProvider, /*IRawElementProviderSimple*/
	IN  int                       eventIdCount,
	IN UiaChangeInfo* pUiaChanges
)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}