#include "buildcfg.h"
#include "kxuserp.h"

typedef struct POINTER_INFO_CHAIN POINTER_INFO_CHAIN;

typedef struct POINTER_INFO_CHAIN
{
	POINTER_INFO info;
	POINTER_INFO_CHAIN* pNext;
	DWORD threadId;
} POINTER_INFO_CHAIN;

STATIC INT isMouseInPointerEnabled = -1;
STATIC POINTER_INFO_CHAIN* pPointerInfos = NULL;
STATIC RTL_SRWLOCK infoSRWLock = { 0 };

BOOL WINAPI GetPointerDevices(
	IN OUT	UINT32* DeviceCount,
	OUT		POINTER_DEVICE_INFO* PointerDevices)
{
	*DeviceCount = 0;
	return TRUE;
}

BOOL WINAPI GetPointerType(
	IN	DWORD				PointerId,
	OUT	POINTER_INPUT_TYPE* PointerType)
{
	*PointerType = PT_MOUSE;
	return TRUE;
}

BOOL WINAPI GetPointerInfo(
	IN	DWORD			PointerId,
	OUT	POINTER_INFO* PointerInfo)
{
	if (PointerInfo == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	RtlAcquireSRWLockShared(&infoSRWLock);

	DWORD threadId = GetCurrentThreadId();

	POINTER_INFO_CHAIN* cur = pPointerInfos;
	while (cur != NULL)
	{
		if (cur->threadId == threadId)
		{
			if (cur->info.pointerId != PointerId)
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				RtlReleaseSRWLockShared(&infoSRWLock);
				return FALSE;
			}

			*PointerInfo = cur->info;
			RtlReleaseSRWLockShared(&infoSRWLock);
			return TRUE;
		}
		cur = cur->pNext;
	}

	SetLastError(ERROR_ACCESS_DENIED);
	RtlReleaseSRWLockShared(&infoSRWLock);
	return FALSE;
}

BOOL WINAPI GetPointerInfoHistory(
	IN		UINT32			PointerId,
	IN OUT  UINT32* entriesCount,
	OUT		POINTER_INFO* PointerInfo)
{
	if (*entriesCount == 0)
		return TRUE;
	*entriesCount = 1;

	GetPointerInfo(PointerId, PointerInfo);
	return TRUE;
}

BOOL WINAPI GetPointerTouchInfo(
	IN	DWORD	PointerId,
	OUT	LPVOID	TouchInfo)
{
	return FALSE;
}

BOOL WINAPI GetPointerTouchInfoHistory(
	IN		UINT32	PointerId,
	IN OUT  UINT32* entriesCount,
	OUT		LPVOID	TouchInfo)
{
	return FALSE;
}

BOOL WINAPI GetPointerFrameTouchInfo(
	IN		DWORD	PointerId,
	IN OUT	LPDWORD PointerCount,
	OUT		LPVOID	TouchInfo)
{
	return FALSE;
}

BOOL WINAPI GetPointerFrameTouchInfoHistory(
	IN		DWORD	PointerId,
	IN OUT	DWORD	EntriesCount,
	IN OUT	LPDWORD PointerCount,
	OUT		LPVOID	TouchInfo)
{
	return FALSE;
}

BOOL WINAPI GetPointerPenInfo(
	IN	DWORD	PointerId,
	OUT	LPVOID	PenInfo)
{
	return FALSE;
}

BOOL WINAPI GetPointerPenInfoHistory(
	IN		DWORD	PointerId,
	IN OUT	LPDWORD	EntriesCount,
	OUT		LPVOID	PenInfo)
{
	return FALSE;
}

BOOL WINAPI SkipPointerFrameMessages(
	IN	DWORD	PointerId)
{
	return TRUE;
}

BOOL WINAPI GetPointerDeviceRects(
	IN	HANDLE	Device,
	OUT	LPRECT	PointerDeviceRect,
	OUT	LPRECT	DisplayRect)
{
	PointerDeviceRect->top = 0;
	PointerDeviceRect->left = 0;
	PointerDeviceRect->bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	PointerDeviceRect->right = GetSystemMetrics(SM_CXVIRTUALSCREEN);

	DisplayRect->top = 0;
	DisplayRect->left = 0;
	DisplayRect->bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	DisplayRect->right = GetSystemMetrics(SM_CXVIRTUALSCREEN);

	return TRUE;
}

VOID FreeCurrentPointerInfo()
{
	RtlAcquireSRWLockExclusive(&infoSRWLock);

	DWORD threadId = GetCurrentThreadId();

	POINTER_INFO_CHAIN* cur = pPointerInfos;
	POINTER_INFO_CHAIN* prev = NULL;
	while (cur != NULL)
	{
		if (cur->threadId == threadId)
		{
			if (prev == NULL)
				pPointerInfos = cur->pNext;
			else
				prev->pNext = cur->pNext;

			SafeFree(cur);
			return;
		}
		prev = cur;
		cur = cur->pNext;
	}

	RtlReleaseSRWLockExclusive(&infoSRWLock);
}

VOID ProcessMouseMessage(MSG* lpMsg, BOOL isAscii)
{
	if (lpMsg == NULL || isMouseInPointerEnabled != 1)
		return;

	switch (lpMsg->message)
	{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			break;
		case WM_QUIT:
			FreeCurrentPointerInfo();
			return;
		default:
			return;
	}

	MSG cMsg = *(MSG*)lpMsg;
	MSG* msg = &cMsg;
	UINT dpi = GetDpiForWindow(msg->hwnd);

	msg->pt.x /= dpi;
	msg->pt.y /= dpi;

	SetThreadDpiAwarenessContext(GetWindowDpiAwarenessContext(msg->hwnd));
	POINTER_INFO cInfo;
	POINTER_INFO* info = &cInfo;
	LARGE_INTEGER counter;
	DWORD message = 0;

	QueryPerformanceCounter(&counter);

	info->pointerType = PT_MOUSE;
	info->pointerId = 1;
	info->sourceDevice = INVALID_HANDLE_VALUE;
	info->historyCount = 1;
	info->frameId;
	info->pointerFlags = 0x20000 | POINTER_MESSAGE_FLAG_PRIMARY;
	info->hwndTarget = msg->hwnd;
	info->ptPixelLocation.x = msg->pt.x;
	info->ptPixelLocation.y = msg->pt.y;
	info->ptHimetricLocation.x = msg->pt.x;
	info->ptHimetricLocation.y = msg->pt.y;
	info->ptPixelLocationRaw.x = msg->pt.x;
	info->ptPixelLocationRaw.y = msg->pt.y;
	info->ptHimetricLocationRaw.x = msg->pt.x;
	info->ptHimetricLocationRaw.y = msg->pt.y;
	info->dwTime = msg->time;
	info->PerformanceCount = counter.QuadPart;
	info->ButtonChangeType = 0;

	switch (msg->message)
	{
		case WM_MOUSEMOVE:
			message = WM_POINTERUPDATE;
			info->pointerFlags |= POINTER_MESSAGE_FLAG_INRANGE;
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
			message = WM_POINTERDOWN;
			info->pointerFlags |= POINTER_MESSAGE_FLAG_INRANGE | POINTER_MESSAGE_FLAG_INCONTACT;
			if (msg->message == WM_LBUTTONDOWN)
			{
				info->pointerFlags |= POINTER_MESSAGE_FLAG_FIRSTBUTTON;
				info->ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_DOWN;
			}
			if (msg->message == WM_RBUTTONDOWN)
			{
				info->pointerFlags |= POINTER_MESSAGE_FLAG_SECONDBUTTON;
				info->ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_DOWN;
			}
			if (msg->message == WM_MBUTTONDOWN)
			{
				info->pointerFlags |= POINTER_MESSAGE_FLAG_THIRDBUTTON;
				info->ButtonChangeType = POINTER_CHANGE_THIRDBUTTON_DOWN;
			}
			if (msg->message == WM_XBUTTONDOWN && LOWORD(msg->wParam) == MK_LBUTTON)
			{
				info->pointerFlags |= POINTER_MESSAGE_FLAG_FIRSTBUTTON;
				info->ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_DOWN;
			}
			if (msg->message == WM_XBUTTONDOWN && LOWORD(msg->wParam) == MK_RBUTTON)
			{
				info->pointerFlags |= POINTER_MESSAGE_FLAG_SECONDBUTTON;
				info->ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_DOWN;
			}
			if (msg->message == WM_XBUTTONDOWN && LOWORD(msg->wParam) == MK_MBUTTON)
			{
				info->pointerFlags |= POINTER_MESSAGE_FLAG_THIRDBUTTON;
				info->ButtonChangeType = POINTER_CHANGE_THIRDBUTTON_DOWN;
			}
			if (msg->message == WM_XBUTTONDOWN && LOWORD(msg->wParam) == MK_XBUTTON1)
			{
				info->pointerFlags |= POINTER_MESSAGE_FLAG_FOURTHBUTTON;
				info->ButtonChangeType = POINTER_CHANGE_FOURTHBUTTON_DOWN;
			}
			if (msg->message == WM_XBUTTONDOWN && LOWORD(msg->wParam) == MK_XBUTTON2)
			{
				info->pointerFlags |= POINTER_MESSAGE_FLAG_FIFTHBUTTON;
				info->ButtonChangeType = POINTER_CHANGE_FIFTHBUTTON_DOWN;
			}
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
			message = WM_POINTERUP;
			if (msg->message == WM_LBUTTONUP) info->ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_UP;
			if (msg->message == WM_RBUTTONUP) info->ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_UP;
			if (msg->message == WM_MBUTTONUP) info->ButtonChangeType = POINTER_CHANGE_THIRDBUTTON_UP;
			if (msg->message == WM_XBUTTONUP && LOWORD(msg->wParam) == MK_LBUTTON)
				info->ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_UP;
			if (msg->message == WM_XBUTTONUP && LOWORD(msg->wParam) == MK_RBUTTON)
				info->ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_UP;
			if (msg->message == WM_XBUTTONUP && LOWORD(msg->wParam) == MK_MBUTTON)
				info->ButtonChangeType = POINTER_CHANGE_THIRDBUTTON_UP;
			if (msg->message == WM_XBUTTONUP && LOWORD(msg->wParam) == MK_XBUTTON1)
				info->ButtonChangeType = POINTER_CHANGE_FOURTHBUTTON_UP;
			if (msg->message == WM_XBUTTONUP && LOWORD(msg->wParam) == MK_XBUTTON2)
				info->ButtonChangeType = POINTER_CHANGE_FIFTHBUTTON_UP;
			break;
		case WM_MOUSEWHEEL:
			message = WM_POINTERWHEEL;
			info->pointerFlags = HIWORD(msg->wParam);
			break;
		case WM_MOUSEHWHEEL:
			message = WM_POINTERHWHEEL;
			info->pointerFlags = HIWORD(msg->wParam);
			break;
	}

	if (message)
	{
		if (isAscii)
			PostMessageA(msg->hwnd, message, MAKELONG(info->pointerId, LOWORD(info->pointerFlags)),
						 MAKELONG(msg->pt.x, msg->pt.y));
		else
			PostMessageW(msg->hwnd, message, MAKELONG(info->pointerId, LOWORD(info->pointerFlags)),
						 MAKELONG(msg->pt.x, msg->pt.y));

		RtlAcquireSRWLockExclusive(&infoSRWLock);

		DWORD threadId = GetCurrentThreadId();

		POINTER_INFO_CHAIN* cur = pPointerInfos;
		while (cur != NULL)
		{
			if (cur->threadId == threadId)
				break;
			cur = cur->pNext;
		}

		if (cur)
		{
			cur->info = cInfo;
		}
		else
		{
			POINTER_INFO_CHAIN* info2 = SafeAlloc(POINTER_INFO_CHAIN, 1);
			if (info2 == NULL)
			{
				RtlReleaseSRWLockExclusive(&infoSRWLock);
				return;
			}

			info2->info = cInfo;
			info2->threadId = threadId;
			info2->pNext = pPointerInfos;

			pPointerInfos = info2;
		}

		RtlReleaseSRWLockExclusive(&infoSRWLock);
	}
}

BOOL WINAPI EnableMouseInPointer(
	IN	BOOL	Enable)
{
	if (InterlockedCompareExchange(&isMouseInPointerEnabled, Enable ? 1 : 0, -1) != -1)
	{
		RtlSetLastWin32Error(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	return TRUE;
}

BOOL WINAPI IsMouseInPointerEnabled()
{
	return isMouseInPointerEnabled == 1;
}

BOOL WINAPI RegisterPointerDeviceNotifications(
	IN	HWND	window,
	IN	BOOL	notifyRange)
{
	return TRUE;
}