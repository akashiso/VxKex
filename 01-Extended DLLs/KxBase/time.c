#include "buildcfg.h"
#include "kxbasep.h"

//
// If strong SharedUserData spoofing is enabled, this function
// supersedes KernelBase!GetSystemTimeAsFileTime because the original
// function reads system time from SharedUserData.
//
KXBASEAPI VOID WINAPI KxBasepGetSystemTimeAsFileTimeHook(
	OUT	PFILETIME	SystemTimeAsFileTime)
{
	ASSERT (KexData->IfeoParameters.StrongVersionSpoof & KEX_STRONGSPOOF_SHAREDUSERDATA);
	NtQuerySystemTime((PLONGLONG) SystemTimeAsFileTime);
}

//
// Same as above but this function supersedes GetSystemTime when doing
// SharedUserData-based version spoofing.
//
KXBASEAPI VOID WINAPI KxBasepGetSystemTimeHook(
	OUT	PSYSTEMTIME	SystemTime)
{
	LONGLONG Time;
	TIME_FIELDS TimeFields;

	ASSERT (KexData->IfeoParameters.StrongVersionSpoof & KEX_STRONGSPOOF_SHAREDUSERDATA);

	NtQuerySystemTime(&Time);
	RtlTimeToTimeFields(&Time, &TimeFields);

	//
	// Annoyingly, the TIME_FIELDS structure is not directly compatible with
	// the SYSTEMTIME structure...
	//

	SystemTime->wYear			= TimeFields.Year;
	SystemTime->wMonth			= TimeFields.Month;
	SystemTime->wDay			= TimeFields.Day;
	SystemTime->wDayOfWeek		= TimeFields.Weekday;
	SystemTime->wHour			= TimeFields.Hour;
	SystemTime->wMinute			= TimeFields.Minute;
	SystemTime->wSecond			= TimeFields.Second;
	SystemTime->wMilliseconds	= TimeFields.Milliseconds;
}

KXBASEAPI VOID WINAPI GetSystemTimePreciseAsFileTime(
	OUT	PFILETIME	SystemTimeAsFileTime)
{
	//
	// The real NtQuerySystemTime export from NTDLL is actually just a jump to
	// RtlQuerySystemTime, which reads from SharedUserData.
	//
	// However, if we are doing SharedUserData-based version spoofing, we will
	// overwrite that stub function with KexNtQuerySystemTime, so it is the best
	// of both worlds in terms of speed and actually working.
	//

	NtQuerySystemTime((PLONGLONG) SystemTimeAsFileTime);
}

KXBASEAPI VOID WINAPI QueryUnbiasedInterruptTimePrecise(
	OUT	PULONGLONG	UnbiasedInterruptTimePrecise)
{
	QueryUnbiasedInterruptTime(UnbiasedInterruptTimePrecise);
}

DWORD WINAPI EnumDynamicTimeZoneInformation(
	IN	const DWORD						Index,
	OUT	PDYNAMIC_TIME_ZONE_INFORMATION	TimeZoneInformation)
{
	LSTATUS LStatus;
	DYNAMIC_TIME_ZONE_INFORMATION DynamicTimeZoneInformation;
	HKEY TimeZoneRootKey = NULL;
	DWORD TZKeyNameLength = ARRAYSIZE(TimeZoneInformation->TimeZoneKeyName);

	if (!TimeZoneInformation) return ERROR_INVALID_PARAMETER;

	LStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones", 0, KEY_READ, &TimeZoneRootKey);
	if (LStatus != ERROR_SUCCESS) goto Exit;

	LStatus = RegEnumKeyEx(TimeZoneRootKey, Index, DynamicTimeZoneInformation.TimeZoneKeyName, &TZKeyNameLength, NULL, NULL, NULL, NULL);
	if (LStatus != ERROR_SUCCESS) goto Exit;

	DynamicTimeZoneInformation.DynamicDaylightTimeDisabled = TRUE;
	if (!GetTimeZoneInformationForYear(0, &DynamicTimeZoneInformation, (PTIME_ZONE_INFORMATION)TimeZoneInformation))
	{
		LStatus = GetLastError();
		goto Exit;
	}

	StringCchCopy(TimeZoneInformation->TimeZoneKeyName, TZKeyNameLength, DynamicTimeZoneInformation.TimeZoneKeyName);
	TimeZoneInformation->DynamicDaylightTimeDisabled = FALSE;

Exit:
	if (TimeZoneRootKey) RegCloseKey(TimeZoneRootKey);
	return LStatus;
}

DWORD WINAPI GetDynamicTimeZoneInformationEffectiveYears(
	IN	const PDYNAMIC_TIME_ZONE_INFORMATION	TimeZoneInformation,
	OUT	LPDWORD									FirstYear,
	OUT	LPDWORD									LastYear)
{
	LSTATUS LStatus;
	HKEY TimeZoneRootKey = NULL;
	HKEY TimeZoneKey = NULL;
	HKEY DynamicDSTKey = NULL;
	DWORD FirstEntry = 0, LastEntry = 0;
	DWORD cbData;

	if (!TimeZoneInformation) return ERROR_INVALID_PARAMETER;

	LStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones", 0, KEY_READ, &TimeZoneRootKey);
	if (LStatus != ERROR_SUCCESS) goto Exit;

	LStatus = RegOpenKeyEx(TimeZoneRootKey, TimeZoneInformation->TimeZoneKeyName, 0, KEY_READ, &TimeZoneKey);
	if (LStatus != ERROR_SUCCESS) goto Exit;

	LStatus = RegOpenKeyEx(TimeZoneKey, L"Dynamic DST", 0, KEY_READ, &DynamicDSTKey);
	if (LStatus != ERROR_SUCCESS) goto Exit;

	cbData = sizeof(FirstEntry);
	LStatus = RegQueryValueEx(DynamicDSTKey, L"FirstEntry", NULL, NULL, (LPBYTE)&FirstEntry, &cbData);
	if (LStatus != ERROR_SUCCESS) goto Exit;

	cbData = sizeof(LastEntry);
	LStatus = RegQueryValueEx(DynamicDSTKey, L"LastEntry", NULL, NULL, (LPBYTE)&LastEntry, &cbData);
	if (LStatus != ERROR_SUCCESS) goto Exit;

	*FirstYear = FirstEntry;
	*LastYear = LastEntry;

Exit:
	if (DynamicDSTKey) RegCloseKey(DynamicDSTKey);
	if (TimeZoneKey) RegCloseKey(TimeZoneKey);
	if (TimeZoneRootKey) RegCloseKey(TimeZoneRootKey);

	return LStatus;
}