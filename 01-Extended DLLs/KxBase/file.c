#include "buildcfg.h"
#include "kxbasep.h"

KXBASEAPI HANDLE WINAPI CreateFile2(
	IN	PCWSTR								FileName,
	IN	ULONG								DesiredAccess,
	IN	ULONG								ShareMode,
	IN	ULONG								CreationDisposition,
	IN	PCREATEFILE2_EXTENDED_PARAMETERS	ExtendedParameters OPTIONAL)
{
	if (ExtendedParameters) {
		ULONG FlagsAndAttributes;

		if (ExtendedParameters->dwSize < sizeof(CREATEFILE2_EXTENDED_PARAMETERS)) {
			BaseSetLastNTError(STATUS_INVALID_PARAMETER);
			return INVALID_HANDLE_VALUE;
		}

		FlagsAndAttributes = ExtendedParameters->dwFileFlags |
							 ExtendedParameters->dwSecurityQosFlags |
							 ExtendedParameters->dwFileAttributes;

		return CreateFile(
			FileName,
			DesiredAccess,
			ShareMode,
			ExtendedParameters->lpSecurityAttributes,
			CreationDisposition,
			FlagsAndAttributes,
			ExtendedParameters->hTemplateFile);
	} else {
		return CreateFile(
			FileName,
			DesiredAccess,
			ShareMode,
			NULL,
			CreationDisposition,
			0,
			NULL);
	}
}

//
// Technically, these functions are supposed to behave differently when the
// current thread is running under the local SYSTEM account. However, we'll
// cross that bridge when we get there, since very very few programs actually
// run under those conditions.
//
// See Windows 11 kernelbase.dll for more information.
//
// For future reference:
// https://devblogs.microsoft.com/oldnewthing/20210106-00/?p=104669
//

KXBASEAPI ULONG WINAPI GetTempPath2A(
	IN	ULONG	BufferCch,
	OUT	PSTR	Buffer)
{
	return GetTempPathA(BufferCch, Buffer);
}

KXBASEAPI ULONG WINAPI GetTempPath2W(
	IN	ULONG	BufferCch,
	OUT	PWSTR	Buffer)
{
	return GetTempPathW(BufferCch, Buffer);
}

STATIC DWORD CALLBACK KxBasepCopyFile2ProgressRoutine(
	IN	LARGE_INTEGER	TotalFileSize,
	IN	LARGE_INTEGER	TotalBytesTransferred,
	IN	LARGE_INTEGER	StreamSize,
	IN	LARGE_INTEGER	StreamBytesTransferred,
	IN	DWORD			StreamNumber,
	IN	DWORD			CallbackReason,
	IN	HANDLE			SourceFile,
	IN	HANDLE			DestinationFile,
	IN	PVOID			Context OPTIONAL)
{
	COPYFILE2_MESSAGE Message;
	PCOPYFILE2_EXTENDED_PARAMETERS Copyfile2Parameters;

	ASSERT (Context != NULL);

	RtlZeroMemory(&Message, sizeof(Message));

	switch (CallbackReason) {
	case CALLBACK_CHUNK_FINISHED:
		Message.Type = COPYFILE2_CALLBACK_CHUNK_FINISHED;
		Message.Info.ChunkFinished.dwStreamNumber = StreamNumber;
		Message.Info.ChunkFinished.uliTotalFileSize.QuadPart = TotalFileSize.QuadPart;
		Message.Info.ChunkFinished.uliTotalBytesTransferred.QuadPart = TotalBytesTransferred.QuadPart;
		Message.Info.ChunkFinished.uliStreamSize.QuadPart = StreamSize.QuadPart;
		Message.Info.ChunkFinished.uliStreamBytesTransferred.QuadPart = StreamBytesTransferred.QuadPart;
		break;
	case CALLBACK_STREAM_SWITCH:
		Message.Type = COPYFILE2_CALLBACK_STREAM_STARTED;
		Message.Info.StreamStarted.dwStreamNumber = StreamNumber;
		Message.Info.StreamStarted.hDestinationFile = DestinationFile;
		Message.Info.StreamStarted.hSourceFile = SourceFile;
		Message.Info.StreamStarted.uliStreamSize.QuadPart = StreamSize.QuadPart;
		Message.Info.StreamStarted.uliTotalFileSize.QuadPart = TotalFileSize.QuadPart;
		break;
	default:
		ASSUME (FALSE);
	}

	Copyfile2Parameters = (PCOPYFILE2_EXTENDED_PARAMETERS) Context;

	return Copyfile2Parameters->pProgressRoutine(
		&Message,
		Copyfile2Parameters->pvCallbackContext);
}

KXBASEAPI HRESULT WINAPI CopyFile2(
	IN	PCWSTR							ExistingFileName,
	IN	PCWSTR							NewFileName,
	IN	PCOPYFILE2_EXTENDED_PARAMETERS	ExtendedParameters OPTIONAL)
{
	BOOL Success;
	ULONG EffectiveCopyFlags;

	if (ExtendedParameters == NULL) {
		Success = CopyFileW(
			ExistingFileName,
			NewFileName,
			FALSE);

		if (Success) {
			return S_OK;
		} else {
			return HRESULT_FROM_WIN32(GetLastError());
		}
	}

	if (ExtendedParameters->dwSize != sizeof(COPYFILE2_EXTENDED_PARAMETERS)) {
		//
		// Windows 11 defines a COPYFILE2_EXTENDED_PARAMETERS_V2 struture.
		// When apps start using it, we will support it too.
		//

		KexLogWarningEvent(
			L"Unrecognized dwSize member of ExtendedParameters: %lu",
			ExtendedParameters->dwSize);

		return E_INVALIDARG;
	}

	if (ExtendedParameters->dwCopyFlags & ~COPY_FILE_ALL_VALID_FLAGS) {
		return E_INVALIDARG;
	}

	EffectiveCopyFlags = ExtendedParameters->dwCopyFlags & COPY_FILE_WIN7_VALID_FLAGS;

	Success = CopyFileExW(
		ExistingFileName,
		NewFileName,
		ExtendedParameters->pProgressRoutine ? KxBasepCopyFile2ProgressRoutine : NULL,
		ExtendedParameters,
		ExtendedParameters->pfCancel,
		EffectiveCopyFlags);

	if (Success) {
		return S_OK;
	} else {
		return HRESULT_FROM_WIN32(GetLastError());
	}
}

KXBASEAPI BOOL WINAPI GetOverlappedResultEx(
	IN	HANDLE					FileHandle,
	IN	VOLATILE LPOVERLAPPED	Overlapped,
	OUT	PULONG					NumberOfBytesTransferred,
	IN	ULONG					TimeoutMs,
	IN	BOOL					Alertable)
{
	if (Overlapped->Internal == STATUS_PENDING) {
		if (TimeoutMs != 0) {
			HANDLE HandleToWaitOn;
			ULONG WaitResult;

			//
			// We will wait on the hEvent member of the OVERLAPPED structure.
			// If hEvent is NULL, we will wait on the file handle instead.
			//

			if (Overlapped->hEvent) {
				HandleToWaitOn = Overlapped->hEvent;
			} else {
				HandleToWaitOn = FileHandle;
			}

			WaitResult = WaitForSingleObjectEx(
				HandleToWaitOn,
				TimeoutMs,
				Alertable);

			if (WaitResult != WAIT_OBJECT_0) {
				if (WaitResult == WAIT_TIMEOUT || WaitResult == WAIT_IO_COMPLETION) {
					RtlSetLastWin32Error(WaitResult);
				}

				return FALSE;
			}
		} else {
			//
			// If the timeout is zero and the operation is still pending, the
			// function returns FALSE immediately with the last-error code being
			// ERROR_IO_INCOMPLETE.
			//

			RtlSetLastWin32Error(ERROR_IO_INCOMPLETE);
			return FALSE;
		}
	}

	*NumberOfBytesTransferred = (ULONG) Overlapped->InternalHigh;

	if (NT_SUCCESS((NTSTATUS) Overlapped->Internal)) {
		return TRUE;
	} else {
		BaseSetLastNTError((NTSTATUS) Overlapped->Internal);
		return FALSE;
	}
}

KXBASEAPI BOOL WINAPI GetFileInformationByName(
	IN	PCWSTR					FileName,
	IN	FILE_INFO_BY_NAME_CLASS	FileInformationClass,
	OUT	PVOID					FileInformationBuffer,
	IN	ULONG					BufferCb)
{
	HANDLE hFile;
	hFile = CreateFileW(FileName,
						FILE_READ_ATTRIBUTES,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						NULL,
						OPEN_EXISTING,
						0,
						NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	ZeroMemory(FileInformationBuffer, BufferCb);
	BY_HANDLE_FILE_INFORMATION baseInfo;
	FILE_ATTRIBUTE_TAG_INFO attrTagInfo;
	FILE_STANDARD_INFO stdInfo;
	FILE_BASIC_INFO basicInfo;
	BOOL Succeed = TRUE;

	Succeed &= GetFileInformationByHandleEx(hFile, FileAttributeTagInfo, &attrTagInfo, sizeof(FILE_ATTRIBUTE_TAG_INFO));
	Succeed &= GetFileInformationByHandleEx(hFile, FileStandardInfo, &stdInfo, sizeof(FILE_STANDARD_INFO));
	Succeed &= GetFileInformationByHandleEx(hFile, FileBasicInfo, &basicInfo, sizeof(FILE_BASIC_INFO));
	Succeed &= GetFileInformationByHandle(hFile, &baseInfo);

	if (!Succeed)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	switch (FileInformationClass)
	{
		case FileCaseSensitiveByNameInfo:
		{
			*((PULONG)FileInformationBuffer) = 0;
			break;
		}
		case FileStatLxByNameInfo:
		case FileStatByNameInfo:
		{
			FILE_STAT_INFORMATION* info = (FILE_STAT_INFORMATION*)FileInformationBuffer;

			info->FileAttributes = attrTagInfo.FileAttributes;
			info->ReparseTag = attrTagInfo.ReparseTag;

			info->AllocationSize = stdInfo.AllocationSize;
			info->NumberOfLinks = stdInfo.NumberOfLinks;
			info->EndOfFile = stdInfo.EndOfFile;

			info->ChangeTime = basicInfo.ChangeTime;
			info->CreationTime = basicInfo.CreationTime;
			info->FileAttributes = basicInfo.FileAttributes;
			info->LastAccessTime = basicInfo.LastAccessTime;
			info->LastWriteTime = basicInfo.LastWriteTime;

			info->FileId.HighPart = baseInfo.nFileIndexHigh;
			info->FileId.LowPart = baseInfo.nFileIndexLow;

			ACCESS_MASK mask = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE;
			if (attrTagInfo.FileAttributes & FILE_ATTRIBUTE_READONLY)
				mask &= ~GENERIC_WRITE;

			info->EffectiveAccess = mask;
			break;
		}
		case FileStatBasicByNameInfo:
		{
			FILE_STAT_BASIC_INFORMATION* info = (FILE_STAT_BASIC_INFORMATION*)FileInformationBuffer;

			info->FileAttributes = attrTagInfo.FileAttributes;
			info->ReparseTag = attrTagInfo.ReparseTag;

			info->AllocationSize = stdInfo.AllocationSize;
			info->NumberOfLinks = stdInfo.NumberOfLinks;
			info->EndOfFile = stdInfo.EndOfFile;

			info->ChangeTime = basicInfo.ChangeTime;
			info->CreationTime = basicInfo.CreationTime;
			info->FileAttributes = basicInfo.FileAttributes;
			info->LastAccessTime = basicInfo.LastAccessTime;
			info->LastWriteTime = basicInfo.LastWriteTime;

			info->VolumeSerialNumber.HighPart = 0;
			info->VolumeSerialNumber.LowPart = baseInfo.dwVolumeSerialNumber;
			info->FileId.HighPart = baseInfo.nFileIndexHigh;
			info->FileId.LowPart = baseInfo.nFileIndexLow;

			info->DeviceCharacteristics = 0;
			info->DeviceType = 0;
			info->Reserved = 0;
			ZeroMemory(&info->FileId128, sizeof(FILE_ID_128));
			break;
		}
	}

	CloseHandle(hFile);
	return TRUE;
}

KXBASEAPI HRESULT WINAPI GetDiskSpaceInformationW(
	IN  LPCWSTR                  rootPath,
	OUT DISK_SPACE_INFORMATION*  diskSpaceInfo)
{
	UNICODE_STRING ntPathName;
	LPCWSTR actualRootPath = L"\\";
	if (rootPath)
		actualRootPath = rootPath;

	if (!diskSpaceInfo)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return E_INVALIDARG;
	}

	if (!RtlDosPathNameToNtPathName_U(actualRootPath, &ntPathName, 0, 0))
	{
		SetLastError(ERROR_PATH_NOT_FOUND);
		return HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
	}
	
	OBJECT_ATTRIBUTES attributes;
	IO_STATUS_BLOCK ioStatusBlock;
	NTSTATUS status;
	HANDLE handle;
	InitializeObjectAttributes(&attributes, &ntPathName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = NtOpenFile(&handle, SYNCHRONIZE, &attributes, &ioStatusBlock, 0, FILE_OPEN_FOR_FREE_SPACE_QUERY | FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE);
	if (!NT_SUCCESS(status))
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			SetLastError(ERROR_PATH_NOT_FOUND);
		RtlFreeUnicodeString(&ntPathName);

		return HRESULT_FROM_NT(status);
	}

	RtlFreeUnicodeString(&ntPathName);
	
	FILE_FS_FULL_SIZE_INFORMATION info;
	status = NtQueryVolumeInformationFile(handle, &ioStatusBlock, &info, sizeof(FILE_FS_FULL_SIZE_INFORMATION), FileFsFullSizeInformation);
	NtClose(handle);

	if (!NT_SUCCESS(status))
		return HRESULT_FROM_NT(status);

	diskSpaceInfo->ActualAvailableAllocationUnits = info.ActualAvailableAllocationUnits.QuadPart;
	diskSpaceInfo->ActualTotalAllocationUnits = info.TotalAllocationUnits.QuadPart;
	diskSpaceInfo->AvailableCommittedAllocationUnits = 0;
	diskSpaceInfo->ActualPoolUnavailableAllocationUnits = 0;

	diskSpaceInfo->CallerAvailableAllocationUnits = info.CallerAvailableAllocationUnits.QuadPart;
	diskSpaceInfo->CallerTotalAllocationUnits = info.TotalAllocationUnits.QuadPart;
	diskSpaceInfo->CallerPoolUnavailableAllocationUnits = 0;

	diskSpaceInfo->UsedAllocationUnits = (ULONGLONG)info.TotalAllocationUnits.QuadPart - (ULONGLONG)info.ActualAvailableAllocationUnits.QuadPart;
	diskSpaceInfo->VolumeStorageReserveAllocationUnits = 0;
	diskSpaceInfo->TotalReservedAllocationUnits = 0;
	diskSpaceInfo->PoolAvailableAllocationUnits = 0;
	diskSpaceInfo->SectorsPerAllocationUnit = info.SectorsPerAllocationUnit;
	diskSpaceInfo->BytesPerSector = info.BytesPerSector;

	return S_OK;
}

KXBASEAPI HRESULT WINAPI GetDiskSpaceInformationA(
	IN  LPCSTR                  rootPath,
	OUT DISK_SPACE_INFORMATION* diskSpaceInfo)
{
	LPCSTR path;
	HRESULT DiskSpaceInformationW;
	UNICODE_STRING UnicodeString;
	ANSI_STRING AnsiString;

	path = "\\";
	if (rootPath)
		path = rootPath;

	RtlInitAnsiString(&AnsiString, path);
	if (NT_SUCCESS(RtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, TRUE)))
	{
		DiskSpaceInformationW = GetDiskSpaceInformationW(UnicodeString.Buffer, diskSpaceInfo);
		RtlFreeUnicodeString(&UnicodeString);
		return DiskSpaceInformationW;
	}
	else
	{
		HRESULT LastErrorValue = GetLastError();
		if (LastErrorValue > 0)
			return HRESULT_FROM_WIN32(LastErrorValue);
	}
	return S_OK;
}

KXBASEAPI BOOL WINAPI Ext_DeviceIoControl(
	IN		HANDLE			DeviceHandle,
	IN		ULONG			IoControlCode,
	IN		PVOID			InBuffer OPTIONAL,
	IN		ULONG			InBufferSize,
	OUT		PVOID			OutBuffer OPTIONAL,
	IN		ULONG			OutBufferSize,
	OUT		PULONG			BytesReturned OPTIONAL,
	IN OUT	LPOVERLAPPED	Overlapped OPTIONAL)
{
	ULONG DummyBytesReturned;

	//
	// Microsoft's documentation lists the BytesReturned (lpBytesReturned) parameter
	// as being optional. However, on Windows 7, the BytesReturned parameter is only
	// really optional when the Overlapped parameter is non-null.
	//
	// To summarize:
	//
	// Parameter combination                       | Result (Win7)    | Result (Win8)
	// --------------------------------------------+------------------+---------------
	// Overlapped == NULL && BytesReturned != NULL | OK               | OK
	// Overlapped == NULL && BytesReturned == NULL | Access violation | OK
	// Overlapped != NULL && BytesReturned == NULL | OK               | OK
	// Overlapped != NULL && BytesReturned != NULL | OK               | OK
	//
	// This is a discrepancy in behavior which was silently changed in Win8. An
	// additional check was added to the branch of code which is executed when
	// Overlapped == NULL.
	//
	// The fix is very simple, as you can see:
	//

	if (BytesReturned == NULL)
	{
		BytesReturned = &DummyBytesReturned;
	}

	return DeviceIoControl(
		DeviceHandle,
		IoControlCode,
		InBuffer,
		InBufferSize,
		OutBuffer,
		OutBufferSize,
		BytesReturned,
		Overlapped);
}

KXBASEAPI BOOL WINAPI Ext_ReadFile(
	IN		HANDLE			FileHandle,
	OUT		PVOID			Buffer,
	IN		ULONG			NumberOfBytesToRead,
	OUT		PULONG			NumberOfBytesRead OPTIONAL,
	IN OUT	LPOVERLAPPED	Overlapped OPTIONAL)
{
	ULONG DummyNumberOfBytesRead;

	// See comment in Ext_DeviceIoControl for why this is necessary.
	// They changed the behavior of ReadFile in Windows 8 as well.
	if (NumberOfBytesRead == NULL)
	{
		NumberOfBytesRead = &DummyNumberOfBytesRead;
	}

	return ReadFile(
		FileHandle,
		Buffer,
		NumberOfBytesToRead,
		NumberOfBytesRead,
		Overlapped);
}

KXBASEAPI BOOL WINAPI Ext_WriteFile(
	IN		HANDLE			FileHandle,
	IN		PCVOID			Buffer,
	IN		ULONG			NumberOfBytesToWrite,
	OUT		PULONG			NumberOfBytesWritten OPTIONAL,
	IN OUT	LPOVERLAPPED	Overlapped OPTIONAL)
{
	ULONG DummyNumberOfBytesWritten;

	// See comment in Ext_DeviceIoControl for why this is necessary.
	// They changed the behavior of WriteFile in Windows 8 as well.
	if (NumberOfBytesWritten == NULL)
	{
		NumberOfBytesWritten = &DummyNumberOfBytesWritten;
	}

	return WriteFile(
		FileHandle,
		Buffer,
		NumberOfBytesToWrite,
		NumberOfBytesWritten,
		Overlapped);
}