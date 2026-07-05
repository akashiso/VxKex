#include "buildcfg.h"
#include "kxuserp.h"

KXUSERAPI BOOL WINAPI GetProcessUIContextInformation(
	IN	HANDLE							ProcessHandle,
	OUT	PPROCESS_UICONTEXT_INFORMATION	UIContextInformation)
{
	if (!UIContextInformation) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	UIContextInformation->UIContext = PROCESS_UICONTEXT_DESKTOP;
	UIContextInformation->Flags		= PROCESS_UIF_NONE;

	return TRUE;
}

KXUSERAPI HWND WINAPI CreateWindowInBand(
	IN	DWORD			dwExStyle,
	IN	PCWSTR			lpClassName,
	IN	PCWSTR			lpWindowName,
	IN	DWORD			dwStyle,
	IN	INT				X,
	IN	INT				Y,
	IN	INT				nWidth,
	IN	INT				nHeight,
	IN	HWND			hWndParent,
	IN	HMENU			hMenu,
	IN	HINSTANCE		hInstance,
	IN	PVOID			lpParam,
	IN	ZBID			zbid)
{
	return CreateWindowExW(
		dwExStyle,
		lpClassName,
		lpWindowName,
		dwStyle,
		X,
		Y,
		nWidth,
		nHeight,
		hWndParent,
		hMenu,
		hInstance,
		lpParam);
}

KXUSERAPI BOOL WINAPI GetWindowBand(
	IN	HWND	Window,
	OUT	PZBID	Band)
{
	if (!IsWindow(Window)) {
		SetLastError(ERROR_INVALID_WINDOW_HANDLE);
		return FALSE;
	}

	if (!Band) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (Window == GetDesktopWindow()) {
		*Band = ZBID_DESKTOP;
	} else {
		*Band = ZBID_DEFAULT;
	}

	return TRUE;
}

KXUSERAPI BOOL WINAPI GetCurrentInputMessageSource(
	OUT	PINPUT_MESSAGE_SOURCE	MessageSource)
{
	if (!MessageSource) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	MessageSource->DeviceType = IMDT_UNAVAILABLE;
	MessageSource->OriginId = IMO_UNAVAILABLE;
	return TRUE;
}

KXUSERAPI BOOL WINAPI IsImmersiveProcess(
	IN	HANDLE	ProcessHandle)
{
	SetLastError(ERROR_SUCCESS);
	return FALSE;
}

KXUSERAPI BOOL WINAPI SetWindowFeedbackSetting(
	IN   HWND          hwnd,
	IN   FEEDBACK_TYPE feedback,
	IN   DWORD         dwFlags,
	IN   UINT32        size,
	IN	 const VOID* configuration
)
{
	return TRUE;
}

KXUSERAPI BOOL WINAPI Ext_GetMessageA(
	OUT LPMSG lpMsg,
	IN  HWND hWnd,
	IN  UINT wMsgFilterMin,
	IN  UINT wMsgFilterMax)
{
	BOOL Return = GetMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);

	ProcessMouseMessage(lpMsg, TRUE);

	return Return;
}

KXUSERAPI BOOL WINAPI Ext_GetMessageW(
	OUT LPMSG lpMsg,
	IN  HWND hWnd,
	IN  UINT wMsgFilterMin,
	IN  UINT wMsgFilterMax)
{
	BOOL Return = GetMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);

	ProcessMouseMessage(lpMsg, FALSE);

	return Return;
}

KXUSERAPI BOOL WINAPI Ext_PeekMessageA(
	OUT LPMSG lpMsg,
	IN  HWND hWnd,
	IN  UINT wMsgFilterMin,
	IN  UINT wMsgFilterMax,
	IN  UINT wRemoveMsg)
{
	BOOL Return = PeekMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

	ProcessMouseMessage(lpMsg, TRUE);

	return Return;
}

KXUSERAPI BOOL WINAPI Ext_PeekMessageW(
	OUT LPMSG lpMsg,
	IN  HWND hWnd,
	IN  UINT wMsgFilterMin,
	IN  UINT wMsgFilterMax,
	IN  UINT wRemoveMsg)
{
	BOOL Return = PeekMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

	ProcessMouseMessage(lpMsg, FALSE);

	return Return;
}