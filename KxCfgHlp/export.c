#include "buildcfg.h"
#include <KexComm.h>
#include <KxCfgHlp.h>
#include <KexW32ML.h>


typedef struct _INI_PARSER
{
	PCWSTR BufferStart;      // pointer to the beginning of the entire file content
	PCWSTR BufferEnd;        // pointer to the terminating null character
	PCWSTR CurrentPos;       // current scanning position (used by NextSection)
	PCWSTR SectionStart;     // start of the current section content (after '[')
	PCWSTR SectionEnd;       // end of the current section (before next '[' or end-of-file)
} INI_PARSER;

STATIC PCWSTR SkipSpacesW(
	IN PCWSTR Str)
{
	ASSERT(Str != NULL);

	while (*Str == L' ' || *Str == L'\t') {
		++Str;
	}

	return Str;
}

STATIC PCWSTR SkipLineW(
	IN PCWSTR Str)
{
	ASSERT(Str != NULL);

	while (*Str != L'\0' && *Str != L'\r' && *Str != L'\n') {
		++Str;
	}
	if (*Str == L'\r') {
		++Str;
	}
	if (*Str == L'\n') {
		++Str;
	}

	return Str;
}

STATIC PCWSTR FindCharInRangeW(
	IN PCWSTR Start,
	IN PCWSTR End,
	IN WCHAR  Ch)
{
	PCWSTR Ptr;

	ASSERT(Start != NULL);
	ASSERT(End >= Start);

	Ptr = Start;

	while (Ptr < End && *Ptr != Ch) {
		++Ptr;
	}
	if (Ptr == End || *Ptr != Ch) {
		return NULL;
	}

	return Ptr;
}

STATIC PCWSTR SkipToNextSignificantLineW(
	IN PCWSTR Start,
	IN PCWSTR End)
{
	PCWSTR LineStart;

	ASSERT(Start != NULL);
	ASSERT(End >= Start);

	LineStart = Start;

	while (LineStart < End) {
		PCWSTR Ptr;
		PCWSTR LineEnd;

		Ptr = SkipSpacesW(LineStart);

		if (*Ptr == L'\0' || *Ptr == L'\r' || *Ptr == L'\n' || *Ptr == L';' || *Ptr == L'#') {
			LineEnd = SkipLineW(LineStart);
			LineStart = LineEnd;
			continue;
		}

		return LineStart;
	}

	return End;
}

VOID IniParserInitialize(
	IN  PCWSTR      FileContent,
	OUT INI_PARSER* Context)
{
	SIZE_T Length;

	ASSERT(FileContent != NULL);
	ASSERT(Context != NULL);

	if (FileContent == NULL || Context == NULL) {
		return;
	}

	StringCchLengthW(FileContent, STRSAFE_MAX_CCH, &Length);

	Context->BufferStart = FileContent;
	Context->BufferEnd = FileContent + Length;
	Context->CurrentPos = FileContent;
	Context->SectionStart = NULL;
	Context->SectionEnd = NULL;
}

BOOLEAN IniParserNextSection(
	IN OUT  INI_PARSER* Context,
	OUT     PWSTR       SectionName,
	IN      SIZE_T      SectionNameCch)
{
	PCWSTR SectionStartPos;
	PCWSTR SectionEndPos;
	PCWSTR NameStart;
	PCWSTR NameEnd;
	SIZE_T NameLength;
	HRESULT Result;

	ASSERT(Context != NULL);
	ASSERT(SectionName != NULL);
	ASSERT(SectionNameCch > 0);

	if (SectionName == NULL || SectionNameCch == 0) {
		return FALSE;
	}

	if (Context->CurrentPos >= Context->BufferEnd) {
		return FALSE;
	}

	SectionStartPos = Context->CurrentPos;

	while (SectionStartPos < Context->BufferEnd) {
		PCWSTR AfterSpaces;

		AfterSpaces = SkipSpacesW(SectionStartPos);

		if (AfterSpaces < Context->BufferEnd && *AfterSpaces == L'[') {
			SectionStartPos = AfterSpaces + 1;
			break;
		}

		SectionStartPos = SkipLineW(SectionStartPos);
	}

	if (SectionStartPos >= Context->BufferEnd) {
		Context->CurrentPos = Context->BufferEnd;
		Context->SectionStart = NULL;
		Context->SectionEnd = NULL;
		return FALSE;
	}

	NameStart = SectionStartPos;
	NameEnd = FindCharInRangeW(NameStart, Context->BufferEnd, L']');

	if (NameEnd == NULL) {
		// malformed section header (missing ']'), skip it and try again
		Context->CurrentPos = SkipLineW(SectionStartPos - 1);
		return IniParserNextSection(Context, SectionName, SectionNameCch);
	}

	NameLength = NameEnd - NameStart;
	if (NameLength >= SectionNameCch) {
		// buffer too small – fail
		ASSERT(FALSE);
		return FALSE;
	}

	Result = StringCchCopyNW(SectionName, SectionNameCch, NameStart, NameLength);
	ASSERT(SUCCEEDED(Result));

	if (FAILED(Result)) {
		return FALSE;
	}

	SectionEndPos = NameEnd + 1;

	while (SectionEndPos < Context->BufferEnd) {
		PCWSTR AfterSpaces;
		PCWSTR LineEnd;

		AfterSpaces = SkipSpacesW(SectionEndPos);

		if (AfterSpaces < Context->BufferEnd && *AfterSpaces == L'[') {
			break;
		}

		LineEnd = SkipLineW(SectionEndPos);
		if (LineEnd == SectionEndPos) {
			break;
		}
		SectionEndPos = LineEnd;
	}

	Context->CurrentPos = SectionEndPos;           // next search starts here
	Context->SectionStart = NameEnd + 1;             // after ']', i.e. start of section body
	Context->SectionEnd = SectionEndPos;           // before next '[' or end of file

	return TRUE;
}

BOOLEAN IniParserGetValue(
	IN  INI_PARSER* Context,
	IN  PCWSTR      Key,
	OUT PWSTR       Value,
	IN  SIZE_T      ValueCch,
	IN  PCWSTR      DefaultValue)
{
	PCWSTR ScanPtr;
	PCWSTR SectionStart;
	PCWSTR SectionEnd;
	SIZE_T KeyLen;

	ASSERT(Context != NULL);
	ASSERT(Key != NULL);
	ASSERT(Value != NULL);
	ASSERT(ValueCch > 0);
	ASSERT(DefaultValue != NULL);

	SectionStart = Context->SectionStart;
	SectionEnd = Context->SectionEnd;

	if (SectionStart == NULL || SectionEnd == NULL || SectionStart >= SectionEnd) {
		StringCchCopyW(Value, ValueCch, DefaultValue);
		return FALSE;
	}

	StringCchLengthW(Key, STRSAFE_MAX_CCH, &KeyLen);

	ScanPtr = SectionStart;

	while (ScanPtr < SectionEnd) {
		PCWSTR LineStart;
		PCWSTR LineEnd;
		PCWSTR EqPos;
		PCWSTR KeyStart, KeyEnd;
		PCWSTR ValueStart, ValueEnd;
		SIZE_T ValueLen;

		LineStart = SkipToNextSignificantLineW(ScanPtr, SectionEnd);
		if (LineStart >= SectionEnd) {
			break;
		}

		LineEnd = LineStart;
		while (LineEnd < SectionEnd && *LineEnd != L'\r' && *LineEnd != L'\n') {
			++LineEnd;
		}

		EqPos = FindCharInRangeW(LineStart, LineEnd, L'=');
		if (EqPos == NULL) {
			ScanPtr = SkipLineW(LineStart);
			continue;
		}

		KeyStart = SkipSpacesW(LineStart);
		KeyEnd = EqPos;
		while (KeyEnd > KeyStart && (*(KeyEnd - 1) == L' ' || *(KeyEnd - 1) == L'\t')) {
			--KeyEnd;
		}

		if ((SIZE_T)(KeyEnd - KeyStart) != KeyLen ||
			RtlCompareMemory(KeyStart, Key, KeyLen * sizeof(WCHAR)) != KeyLen * sizeof(WCHAR)) {
			ScanPtr = SkipLineW(LineStart);
			continue;
		}

		ValueStart = SkipSpacesW(EqPos + 1);
		ValueEnd = LineEnd;
		while (ValueEnd > ValueStart && (*(ValueEnd - 1) == L' ' || *(ValueEnd - 1) == L'\t')) {
			--ValueEnd;
		}

		ValueLen = ValueEnd - ValueStart;

		if (ValueLen >= ValueCch) {
			// buffer too small – truncate
			StringCchCopyNW(Value, ValueCch, ValueStart, ValueCch - 1);
			Value[ValueCch - 1] = L'\0';
		}
		else {
			StringCchCopyNW(Value, ValueCch, ValueStart, ValueLen);
		}

		return TRUE;
	}

	StringCchCopyW(Value, ValueCch, DefaultValue);
	return FALSE;
}



typedef struct _INI_WRITER
{
	PWSTR   Buffer;          // heap-allocated buffer
	SIZE_T  Capacity;        // total characters allocated (including null)
	SIZE_T  Length;          // current length in characters (excluding null)
	BOOLEAN HasContent;      // TRUE if anything has been written
	BOOLEAN SectionOpen;     // TRUE if a section is currently active
} INI_WRITER;

STATIC BOOLEAN EnsureCapacity(
	IN  INI_WRITER* Writer,
	IN  SIZE_T     AddLen)
{
	SIZE_T Required;
	SIZE_T NewCapacity;
	PWSTR NewBuffer;

	ASSERT(Writer != NULL);

	Required = Writer->Length + AddLen + 1;   // +1 for null terminator

	if (Required <= Writer->Capacity) {
		return TRUE;
	}

	// Double the capacity
	NewCapacity = Writer->Capacity * 2;

	while (NewCapacity < Required) {
		NewCapacity *= 2;
	}

	NewBuffer = SafeReAlloc( Writer->Buffer, WCHAR, NewCapacity);

	if (NewBuffer == NULL) {
		return FALSE;
	}

	Writer->Buffer = NewBuffer;
	Writer->Capacity = NewCapacity;
	return TRUE;
}

STATIC BOOLEAN AppendString(
	IN  INI_WRITER* Writer,
	IN  PCWSTR     String,
	IN  SIZE_T     StringLen)
{
	SIZE_T Len;

	ASSERT(Writer != NULL);
	ASSERT(String != NULL);

	if (StringLen == (SIZE_T)-1) {
		StringCchLengthW(String, STRSAFE_MAX_CCH, &Len);
	}
	else {
		Len = StringLen;
	}

	if (Len == 0) {
		return TRUE;
	}
	if (!EnsureCapacity(Writer, Len)) {
		return FALSE;
	}

	StringCchCopyNW(
		Writer->Buffer + Writer->Length,
		Writer->Capacity - Writer->Length,
		String,
		Len);

	Writer->Length += Len;
	Writer->Buffer[Writer->Length] = L'\0';

	return TRUE;
}

BOOL IniWriterInitialize(
	OUT INI_WRITER* Writer)
{
	if (Writer == NULL) {
		return FALSE;
	}

	Writer->Buffer = NULL;
	Writer->Capacity = 256;
	Writer->Length = 0;
	Writer->HasContent = FALSE;
	Writer->SectionOpen = FALSE;

	Writer->Buffer = SafeAlloc(WCHAR, Writer->Capacity);

	if (!Writer->Buffer) {
		return FALSE;
	}

	Writer->Buffer[0] = L'\0';
	return TRUE;
}

VOID IniWriterDestroy(
	IN  INI_WRITER* Writer)
{
	if (Writer == NULL) {
		return;
	}
	if (Writer->Buffer != NULL) {
		SafeFree(Writer->Buffer);
	}
}

BOOLEAN IniWriterBeginSection(
	IN  INI_WRITER* Writer,
	IN  PCWSTR     SectionName)
{
	BOOLEAN Result;

	ASSERT(Writer != NULL);
	ASSERT(SectionName != NULL);

	if (SectionName == NULL) {
		return FALSE;
	}

	if (Writer->SectionOpen) {
		Result = AppendString(Writer, L"\r\n", 2);
		if (!Result) {
			return FALSE;
		}
	}

	Result = AppendString(Writer, L"[", 1);
	if (!Result) goto Error;

	Result = AppendString(Writer, SectionName, (SIZE_T)-1);
	if (!Result) goto Error;

	Result = AppendString(Writer, L"]\r\n", 3);
	if (!Result) goto Error;

	Writer->SectionOpen = TRUE;
	Writer->HasContent = TRUE;
	return TRUE;

Error:
	return FALSE;
}

BOOLEAN IniWriterAddKeyValue(
	IN  INI_WRITER* Writer,
	IN  PCWSTR     Key,
	IN  PCWSTR     Value)
{
	BOOLEAN Result;

	ASSERT(Writer != NULL);
	ASSERT(Key != NULL);
	ASSERT(Value != NULL);

	if (Key == NULL || Value == NULL) {
		return FALSE;
	}

	if (!Writer->SectionOpen) {
		ASSERT(FALSE);
		SetLastError(ERROR_INVALID_STATE);
		return FALSE;
	}

	Result = AppendString(Writer, Key, (SIZE_T)-1);
	if (!Result) goto Error;

	Result = AppendString(Writer, L"=", 1);
	if (!Result) goto Error;

	Result = AppendString(Writer, Value, (SIZE_T)-1);
	if (!Result) goto Error;

	Result = AppendString(Writer, L"\r\n", 2);
	if (!Result) goto Error;

	Writer->HasContent = TRUE;
	return TRUE;

Error:
	return FALSE;
}

PCWSTR IniWriterGetContent(
	IN  INI_WRITER* Writer)
{
	ASSERT(Writer != NULL);

	if (Writer->Buffer == NULL) {
		return L"";
	}

	return Writer->Buffer;
}



BOOLEAN CALLBACK KxCfgExportConfigurationEnumerationCallback(
	IN	PCWSTR							ExeFullPathOrBaseName,
	IN	BOOLEAN							IsLegacyConfiguration,
	IN	PVOID							ExtraParameter)
{
	KXCFG_PROGRAM_CONFIGURATION Configuration;
	INI_WRITER* Writer;
	PWSTR ExeBaseName;
	PWSTR WinVerName;
	WCHAR Buffer[32];
	BOOLEAN Success;

	if (IsLegacyConfiguration) {
		ASSERT(!IsLegacyConfiguration);
		return TRUE;
	}

	Writer = (INI_WRITER*)ExtraParameter;
	ExeBaseName = PathFindFileNameW(ExeFullPathOrBaseName);

	Success = KxCfgGetConfiguration(ExeFullPathOrBaseName, &Configuration);

	ASSERT(Success);
	if (!Success) {
		return FALSE;
	}

	Success = IniWriterBeginSection(Writer, ExeBaseName);

	Success &= IniWriterAddKeyValue(Writer, L"ExeFullPath", ExeFullPathOrBaseName);

	Success &= IniWriterAddKeyValue(
		Writer, 
		L"KEX_DisableForChild", 
		Configuration.DisableForChild ? L"1" : L"0");

	Success &= IniWriterAddKeyValue(
		Writer, 
		L"KEX_DisableAppSpecific", 
		Configuration.DisableAppSpecificHacks ? L"1" : L"0");

	Success &= IniWriterAddKeyValue(
		Writer, 
		L"KEX_StrongVersionSpoof",
		Configuration.StrongSpoofOptions ? L"1" : L"0");

	StringCchPrintfW(Buffer, ARRAYSIZE(Buffer), L"%08X", Configuration.TlsForceDisabledProtocols);
	Success &= IniWriterAddKeyValue(Writer, L"KEX_TlsForceDisabledProtocols", Buffer);

	StringCchPrintfW(Buffer, ARRAYSIZE(Buffer), L"%08X", Configuration.TlsForceEnabledProtocols);
	Success &= IniWriterAddKeyValue(Writer, L"KEX_TlsForceEnabledProtocols", Buffer);

	switch (Configuration.WinVerSpoof) {
		case WinVerSpoofWin7:
			WinVerName = L"Win7";
			break;
		case WinVerSpoofWin8:
			WinVerName = L"Win8";
			break;
		case WinVerSpoofWin8Point1:
			WinVerName = L"Win8.1";
			break;
		case WinVerSpoofWin10:
			WinVerName = L"Win10";
			break;
		case WinVerSpoofWin11:
			WinVerName = L"Win11";
			break;
		default:
			WinVerName = L"None";
	}

	Success &= IniWriterAddKeyValue(Writer, L"KEX_WinVerSpoof", WinVerName);
	Success &= IniWriterAddKeyValue(Writer, L"KEX_DllRewriteEntries", Configuration.DllRewriteEntries);
	Success &= IniWriterAddKeyValue(Writer, L"KEX_DllRewriteExemptions", Configuration.DllRewriteExemptions);

	return Success;
}

KXCFGDECLSPEC BOOLEAN KXCFGAPI KxCfgExportConfigurationToIni(
	OUT	PWSTR	FilePath)
{
	INI_WRITER Writer;
	HANDLE FileHandle = NULL;
	BOOLEAN Success = TRUE;
	DWORD NumBytesWritten;

	if (!IniWriterInitialize(&Writer)) {
		return FALSE;
	}

	Success = KxCfgEnumerateConfiguration(KxCfgExportConfigurationEnumerationCallback, &Writer);

	if (!Success) {
		goto Error;
	}

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
		IniWriterGetContent(&Writer),
		(DWORD)(Writer.Length * sizeof(WCHAR)),
		&NumBytesWritten,
		NULL);

Error:
	IniWriterDestroy(&Writer);

	if (FileHandle) {
		CloseHandle(FileHandle);
	}

	return Success;
}

BOOLEAN KxCfgImportConfiguration(
	IN  INI_PARSER* Parser,
	IN  HANDLE		TransactionHandle)
{
	KXCFG_PROGRAM_CONFIGURATION Configuration;
	WCHAR PathBuffer[MAX_PATH];
	WCHAR Buffer[MAX_PATH];

	UNICODE_STRING String;
	ULONG WinVerSpoof;

	IniParserGetValue(Parser, L"ExeFullPath", PathBuffer, ARRAYSIZE(PathBuffer), L"");
	if (PathBuffer[0] == L'\0') {
		return FALSE;
	}

	IniParserGetValue(Parser, L"KEX_DisableForChild", Buffer, ARRAYSIZE(Buffer), L"0");
	Configuration.DisableForChild = (Buffer[0] != L'0');

	IniParserGetValue(Parser, L"KEX_DisableAppSpecific", Buffer, ARRAYSIZE(Buffer), L"0");
	Configuration.DisableAppSpecificHacks = (Buffer[0] != L'0');

	IniParserGetValue(Parser, L"KEX_StrongVersionSpoof", Buffer, ARRAYSIZE(Buffer), L"0");
	Configuration.StrongSpoofOptions = (Buffer[0] != L'0');

	IniParserGetValue(Parser, L"KEX_TlsForceDisabledProtocols", Buffer, ARRAYSIZE(Buffer), L"0");
	RtlInitUnicodeString(&String, Buffer);
	RtlUnicodeStringToInteger(&String, 16, &Configuration.TlsForceDisabledProtocols);

	IniParserGetValue(Parser, L"KEX_TlsForceEnabledProtocols", Buffer, ARRAYSIZE(Buffer), L"0");
	RtlInitUnicodeString(&String, Buffer);
	RtlUnicodeStringToInteger(&String, 16, &Configuration.TlsForceEnabledProtocols);

	IniParserGetValue(Parser, L"KEX_WinVerSpoof", Buffer, ARRAYSIZE(Buffer), L"None");

	if (StringEqualW(Buffer, L"Win7")) {
		WinVerSpoof = WinVerSpoofWin7;
	}
	else if (StringEqualW(Buffer, L"Win8")) {
		WinVerSpoof = WinVerSpoofWin8;
	}
	else if (StringEqualW(Buffer, L"Win8.1")) {
		WinVerSpoof = WinVerSpoofWin8Point1;
	}
	else if (StringEqualW(Buffer, L"Win10")) {
		WinVerSpoof = WinVerSpoofWin10;
	}
	else if (StringEqualW(Buffer, L"Win11")) {
		WinVerSpoof = WinVerSpoofWin11;
	}
	else {
		WinVerSpoof = WinVerSpoofNone;
	}

	Configuration.WinVerSpoof = WinVerSpoof;

	IniParserGetValue(
		Parser,
		L"KEX_DllRewriteEntries",
		Configuration.DllRewriteEntries,
		ARRAYSIZE(Configuration.DllRewriteEntries),
		L"");

	IniParserGetValue(
		Parser, 
		L"KEX_DllRewriteExemptions",
		Configuration.DllRewriteExemptions, 
		ARRAYSIZE(Configuration.DllRewriteExemptions), 
		L"");

	Configuration.Enabled = TRUE;

	return KxCfgSetConfiguration(
		PathBuffer,
		&Configuration,
		TransactionHandle);
}

KXCFGDECLSPEC BOOLEAN KXCFGAPI KxCfgImportConfigurationFromIni(
	IN	PWSTR	FilePath,
	IN  HANDLE  TransactionHandle OPTIONAL)
{
	INI_PARSER Parser;
	HANDLE FileHandle = NULL;
	PWSTR IniData = NULL;
	BOOLEAN Success = TRUE;
	DWORD FileSize;
	WCHAR NameBuffer[MAX_PATH];

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

	IniData = SafeAlloc(WCHAR, FileSize);
	
	Success = ReadFile(
		FileHandle,
		IniData,
		FileSize,
		&FileSize,
		NULL);

	if (!Success) {
		Success = FALSE;
		goto Error;
	}

	IniParserInitialize(IniData, &Parser);

	while (IniParserNextSection(&Parser, NameBuffer, ARRAYSIZE(NameBuffer))) {
		Success = KxCfgImportConfiguration(&Parser, TransactionHandle);

		if (!Success) {
			goto Error;
		}
	}

Error:
	if (!FileHandle) {
		CloseHandle(FileHandle);
	}
	if (IniData) {
		SafeFree(IniData);
	}

	return Success;
}