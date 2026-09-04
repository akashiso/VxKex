///////////////////////////////////////////////////////////////////////////////
//
// Module Name:
//
//     util.c
//
// Abstract:
//
//     Miscellaneous utility functions for KexCfg
//
// Author:
//
//     vxiiduu (09-Feb-2024)
//
// Environment:
//
//     Win32 mode. Sometimes this program is run as Administrator, sometimes
//     as a standard account, sometimes as the local SYSTEM account.
//
// Revision History:
//
//     vxiiduu              09-Feb-2024  Initial creation.
//
///////////////////////////////////////////////////////////////////////////////

#include "buildcfg.h"
#include <KexComm.h>
#include <WtsApi32.h>

EXTERN PWSTR FRIENDLYAPPNAME;

BOOLEAN Interactive;
ULONG SessionId;

BOOLEAN RunningInInteractiveWindowStation(
	VOID)
{
	BOOLEAN Success;
	WCHAR WindowStationName[16];
	HWINSTA WindowStation;

	WindowStation = GetProcessWindowStation();
	if (WindowStation == NULL) {
		return FALSE;
	}

	Success = GetUserObjectInformation(
		WindowStation,
		UOI_NAME,
		WindowStationName,
		sizeof(WindowStationName),
		NULL);

	if (!Success) {
		return FALSE;
	}

	return StringEqualI(WindowStationName, L"WinSta0");
}

INT KexCfgMessageBox(
	IN	HWND	ParentWindow OPTIONAL,
	IN	PWSTR	Message,
	IN	PWSTR	Title,
	IN	ULONG	Flags)
{
	BOOLEAN Success;
	ULONG Response;

	if (Interactive) {
		//
		// Use the normal MessageBox function when running interactively.
		// This allows the message box to have comctl v6 themes.
		//

		return MessageBox(
			ParentWindow,
			Message,
			Title,
			Flags);
	} else {
		if (Title == NULL) {
			// WTSSendMessage will error out upon receiving a NULL pointer
			// for Title.
			Title = L"Error";
		}

		Success = WTSSendMessage(
			WTS_CURRENT_SERVER_HANDLE,
			SessionId,
			Title,
			(ULONG) wcslen(Title) * sizeof(WCHAR),
			Message,
			(ULONG) wcslen(Message) * sizeof(WCHAR),
			Flags,
			0,
			&Response,
			TRUE);
	}

	if (!Success) {
		return 0;
	}

	return Response;
}

BOOLEAN KexCfgParseBooleanParameter(
	IN	PCWSTR	Parameter)
{
	WCHAR ParameterValue[32];
	ULONG Index;

	Index = 0;

	until (Parameter[Index] == ' ' || Parameter[Index] == '\0' || Index >= ARRAYSIZE(ParameterValue - 2)) {
		ParameterValue[Index] = Parameter[Index];
		++Index;
	}

	ParameterValue[Index] = '\0';

	if (StringEqualI(ParameterValue, L"TRUE") ||
		StringEqualI(ParameterValue, L"YES") ||
		StringEqualI(ParameterValue, L"1")) {

		return TRUE;
	} else if (StringEqualI(ParameterValue, L"FALSE") ||
			   StringEqualI(ParameterValue, L"NO") ||
			   StringEqualI(ParameterValue, L"0")) {

		return FALSE;
	} else {
		KexCfgMessageBox(
			NULL,
			L"A boolean argument was invalid. Pass the /? argument for more information.",
			FRIENDLYAPPNAME,
			MB_ICONERROR | MB_OK);

		ExitProcess(STATUS_INVALID_PARAMETER);
	}
}

BOOLEAN KexCfgExportConfigurationToIni(
	OUT	PWSTR	FilePath)
{
	HANDLE FileHandle = NULL;
	BOOLEAN Success = TRUE;
	DWORD NumBytesWritten;
	PWSTR IniData = NULL;
	PSTR IniDataUTF8 = NULL;
	
	Success = KxCfgExportConfigurationToIni(&IniData);
	if (!Success) {
		goto Error;
	}

	NumBytesWritten = WideCharToMultiByte(CP_UTF8, 0, IniData, -1, NULL, 0, NULL, NULL);
	IniDataUTF8 = SafeAlloc(CHAR, NumBytesWritten);

	WideCharToMultiByte(CP_UTF8, 0, IniData, -1, IniDataUTF8, NumBytesWritten, NULL, NULL);

	FileHandle = CreateFileW(
		FilePath,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (!FileHandle) {
		Success = FALSE;
		goto Error;
	}

	Success = WriteFile(
		FileHandle,
		IniDataUTF8,
		NumBytesWritten - 1,
		&NumBytesWritten,
		NULL);

Error:
	if (FileHandle) {
		CloseHandle(FileHandle);
	}
	if (IniData) {
		SafeFree(IniData);
	}
	if (IniDataUTF8) {
		SafeFree(IniDataUTF8);
	}

	return Success;
}

BOOLEAN KexCfgImportConfigurationFromIni(
	IN	PWSTR	FilePath,
	IN  HANDLE  TransactionHandle OPTIONAL)
{
	HANDLE FileHandle = NULL;
	PWSTR IniData = NULL;
	PSTR IniDataUTF8 = NULL;
	BOOLEAN Success = TRUE;
	DWORD FileSize;

	FileHandle = CreateFileW(
		FilePath,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (!FileHandle) {
		Success = FALSE;
		goto Error;
	}

	FileSize = GetFileSize(FileHandle, NULL);

	if (FileSize == 0) {
		Success = TRUE;
		goto Error;
	}

	IniDataUTF8 = SafeAlloc(CHAR, FileSize + 1);

	Success = ReadFile(
		FileHandle,
		IniDataUTF8,
		FileSize,
		&FileSize,
		NULL);

	if (!Success) {
		Success = FALSE;
		goto Error;
	}

	IniDataUTF8[FileSize] = '\0';
	FileSize = MultiByteToWideChar(CP_UTF8, 0, IniDataUTF8, -1, NULL, 0);
	IniData = SafeAlloc(WCHAR, FileSize);

	MultiByteToWideChar(CP_UTF8, 0, IniDataUTF8, -1, IniData, FileSize);

	KxCfgImportConfigurationFromIni(IniData, TransactionHandle);

Error:
	if (FileHandle) {
		CloseHandle(FileHandle);
	}
	if (IniData) {
		SafeFree(IniData);
	}
	if (IniDataUTF8) {
		SafeFree(IniDataUTF8);
	}

	return Success;
}