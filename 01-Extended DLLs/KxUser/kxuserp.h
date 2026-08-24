///////////////////////////////////////////////////////////////////////////////
//
// Module Name:
//
//     kxuserp.h
//
// Abstract:
//
//     Private header file for KxUser.
//
// Author:
//
//     vxiiduu (10-Feb-2022)
//
// Revision History:
//
//     vxiiduu              10-Feb-2022  Initial creation.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <KexComm.h>
#include <KexDll.h>
#include <KxUser.h>

EXTERN PKEX_PROCESS_DATA KexData;

//
// Available ExStyle on Windows 7
//

#define WS_EX_AVAILABLE_STYLES (WS_EX_DLGMODALFRAME | \
		WS_EX_NOPARENTNOTIFY | \
		WS_EX_TOPMOST | \
		WS_EX_ACCEPTFILES | \
		WS_EX_TRANSPARENT |  \
		WS_EX_MDICHILD | \
		WS_EX_TOOLWINDOW | \
		WS_EX_WINDOWEDGE | \
		WS_EX_CLIENTEDGE | \
		WS_EX_CONTEXTHELP | \
		WS_EX_RIGHT | \
		WS_EX_LEFT | \
		WS_EX_RTLREADING | \
		WS_EX_LTRREADING | \
		WS_EX_LEFTSCROLLBAR | \
		WS_EX_RIGHTSCROLLBAR | \
		WS_EX_CONTROLPARENT | \
		WS_EX_STATICEDGE | \
		WS_EX_APPWINDOW | \
		WS_EX_LAYERED | \
		WS_EX_NOINHERITLAYOUT | \
		WS_EX_LAYOUTRTL | \
		WS_EX_COMPOSITED | \
		WS_EX_NOACTIVATE)

typedef struct _FNDWORDMSG
{
	PVOID		pwnd;
	UINT		msg;
	WPARAM		wParam;
	LPARAM		lParam;
	ULONG_PTR	xParam;
	PVOID		xpfnProc;
} TYPEDEF_TYPE_NAME(FNDWORDMSG);

typedef struct _TOUCH_POINT_ENTRY
{
	DWORD				PointerId;
	BOOL				Valid;
	BOOL				Active;
	POINTER_TOUCH_INFO	TouchInfo;
} TYPEDEF_TYPE_NAME(TOUCH_POINT_ENTRY);

typedef struct _PEN_POINT_ENTRY
{
	DWORD				PointerId;
	BOOL				Valid;
	BOOL				Active;
	POINTER_PEN_INFO	PenInfo;
} TYPEDEF_TYPE_NAME(PEN_POINT_ENTRY);

//
// loadsyslib.c
//

HMODULE LoadSystemLibrary(
	LPCWSTR	FileName);

//
// pointer.c
//

KXUSERAPI BOOL WINAPI IsMouseInPointerEnabled(
	VOID);

//
// winmsg.c
//

VOID InitializeTouchAndPenPoints(
	VOID);

VOID CleanupTouchAndPenPoints(
	VOID);

PTOUCH_POINT_ENTRY FindOrCreateTouchPoint(
	DWORD	PointerId);

VOID DeactivateTouchPoint(
	DWORD	PointerId);

VOID RemoveTouchPoint(
	DWORD	PointerId);

PPEN_POINT_ENTRY FindOrCreatePenPoint(
	DWORD	PointerId);

VOID DeactivatePenPoint(
	DWORD	PointerId);

VOID RemovePenPoint(
	DWORD	PointerId);

PVOID FindOrCreateTouchOrPenPoint(
	DWORD	PointerId,
	BOOL	IsPenMessage);

VOID DeactivateTouchOrPenPoint(
	DWORD	PointerId,
	BOOL	IsPenMessage);

VOID RemoveTouchOrPenPoint(
	DWORD	PointerId,
	BOOL	IsPenMessage);

NTSTATUS EnableWindowMessageInterception(
	VOID);

//
// syscal.c
//

#if defined(KEX_ARCH_X64)

#define KXUSER_DECLARE_SYSCALL(SyscallName, ...) \
KXUSERAPI NTSTATUS NTAPI KxUser##SyscallName##_Win7(__VA_ARGS__); \
KXUSERAPI NTSTATUS NTAPI KxUser##SyscallName(__VA_ARGS__);

#else

#define KXUSER_DECLARE_SYSCALL(SyscallName, ...) \
KXUSERAPI NTSTATUS NTAPI KxUser##SyscallName##_Win7_Native32(__VA_ARGS__); \
KXUSERAPI NTSTATUS NTAPI KxUser##SyscallName##_Win7_Wow64(__VA_ARGS__); \
KXUSERAPI NTSTATUS NTAPI KxUser##SyscallName(__VA_ARGS__);

#endif

KXUSER_DECLARE_SYSCALL(NtUserGetTouchInputInfo,
					   HTOUCHINPUT	TouchInput,
					   UINT		Inputs,
					   PTOUCHINPUT	InputsPtr,
					   int			Size);