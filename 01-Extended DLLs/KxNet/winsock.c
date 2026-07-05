#include "buildcfg.h"
#include "kxnetp.h"
#include <WS2tcpip.h>

//
// libuv (which is used by some random node.js package that a lot of electron
// apps, such as VSCode, Signal, etc.) depends on GetHostNameW and will shit
// itself if not found.
//
KXNETAPI INT WINAPI GetHostNameW(
	OUT	PWSTR	Name,
	IN	INT		NameCch)
{
	INT ReturnValue;
	INT ConvertedCch;
	PCHAR NameAnsi;

	if (!NameCch)
	{
		WSASetLastError(WSAEFAULT);
		return SOCKET_ERROR;
	}

	NameAnsi = StackAlloc(CHAR, NameCch);
	ReturnValue = gethostname(NameAnsi, NameCch);

	if (ReturnValue == SOCKET_ERROR)
	{
		return ReturnValue;
	}

	ConvertedCch = MultiByteToWideChar(
		CP_ACP,
		0,
		NameAnsi,
		-1,
		Name,
		NameCch);

	if (ConvertedCch == 0)
	{
		WSASetLastError(WSAEFAULT);
		return SOCKET_ERROR;
	}

	return ReturnValue;
}

//
// Used for app-specific hack.
//
KXNETAPI INT WINAPI Ext_getaddrinfo(
	IN	PCSTR					NodeName,
	IN	PCSTR					ServerName,
	IN	const struct addrinfo* Hints,
	OUT	struct addrinfo** Results)
{
	unless(KexData->IfeoParameters.DisableAppSpecific)
	{
		//
		// APPSPECIFICHACK: The game "Life is Strange: True Colors" attempts to connect
		// to a server and send some telemetry data. On Windows 7, the gathering of this
		// telemetry data fails. The error handling code in the game is faulty and will
		// cause a crash.
		//
		// As a workaround, we will simply fail "getaddrinfo" calls coming from the game, to
		// simulate the lack of an internet connection.
		//
		// For reference, the actual module that calls getaddrinfo with the game's
		// telemetry server is "osdk.dll", and the host name requested is
		// "see-siren.os.eidos.com".
		//

		if (AshExeBaseNameIs(L"Siren-Win64-Shipping.exe"))
		{
			return WSANO_DATA;
		}
	}

	return getaddrinfo(NodeName, ServerName, Hints, Results);
}

KXNETAPI INT WINAPI Ext_GetAddrInfoExW(
	IN OPTIONAL  PCWSTR                pName,
	IN OPTIONAL  PCWSTR                pServiceName,
	IN           DWORD                 dwNameSpace,
	IN OPTIONAL  LPGUID                lpNspId,
	IN OPTIONAL  const ADDRINFOEXW*    hints,
	OUT          PADDRINFOEXW*         ppResult,
	IN OPTIONAL  struct timeval*       timeout,
	IN OPTIONAL  LPOVERLAPPED                       lpOverlapped,
	IN OPTIONAL  LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine,
	OUT OPTIONAL LPHANDLE                           lpHandle)
{
	INT Result = GetAddrInfoExW(pName, pServiceName, dwNameSpace, lpNspId, hints, ppResult, timeout,
								lpOverlapped, lpCompletionRoutine, lpHandle);

	//
	// Windows 7 doesn't support asynchronous operations,
	// so these parameter must be set to NULL 
	//

	if (Result == WSAEINVAL)
	{
		if (lpOverlapped != NULL)
			if (lpOverlapped->hEvent && lpCompletionRoutine)
				return WSAEINVAL;

		if (lpOverlapped || lpCompletionRoutine || lpHandle)
		{
			KexLogWarningEvent(L"GetAddrInfoExW was called with : \n\n"
							   L"lpOverlapped : %p\n"
							   L"lpCompletionRoutine : %p\n"
							   L"lpHandle : %p",
							   lpOverlapped, lpCompletionRoutine, lpHandle);
		}

		Result = GetAddrInfoExW(pName, pServiceName, dwNameSpace, lpNspId, hints, ppResult, timeout,
								NULL, NULL, NULL);
	}
	return Result;
}

KXNETAPI INT WINAPI Ext_GetAddrInfoExA(
	IN OPTIONAL  PCSTR                pName,
	IN OPTIONAL  PCSTR                pServiceName,
	IN           DWORD                dwNameSpace,
	IN OPTIONAL  LPGUID               lpNspId,
	IN OPTIONAL  const ADDRINFOEXA*   hints,
	OUT          PADDRINFOEXA*		  ppResult,
	IN OPTIONAL  struct timeval*	  timeout,
	IN OPTIONAL  LPOVERLAPPED                       lpOverlapped,
	IN OPTIONAL  LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine,
	OUT OPTIONAL LPHANDLE                           lpHandle)
{
	INT Result = GetAddrInfoExA(pName, pServiceName, dwNameSpace, lpNspId, hints, ppResult, timeout,
								lpOverlapped, lpCompletionRoutine, lpHandle);

	if (Result == WSAEINVAL)
	{
		if (lpOverlapped != NULL)
			if (lpOverlapped->hEvent && lpCompletionRoutine)
				return WSAEINVAL;

		if (lpOverlapped || lpCompletionRoutine || lpHandle)
		{
			KexLogWarningEvent(L"GetAddrInfoExA was called with : \n\n"
							   L"lpOverlapped : %p\n"
							   L"lpCompletionRoutine : %p\n"
							   L"lpHandle : %p",
							   lpOverlapped, lpCompletionRoutine, lpHandle);
		}

		Result = GetAddrInfoExA(pName, pServiceName, dwNameSpace, lpNspId, hints, ppResult, timeout,
							    NULL, NULL, NULL);
	}
	return Result;
}

KXNETAPI INT WINAPI GetAddrInfoExOverlappedResult(
	LPOVERLAPPED lpOverlapped)
{
	return WSAEINVAL;
}

KXNETAPI INT WINAPI GetAddrInfoExCancel(
	IN	LPHANDLE	lpHandle)
{
	KexLogWarningEvent(L"GetAddrInfoExCancel called");

	return S_OK;
}