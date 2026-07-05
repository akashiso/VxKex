#include "buildcfg.h"
#include "kxcomp.h"

//
// WinRT 
//

// Foundation : Windows.Foundation.IAsyncAction

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncAction_QueryInterface(
	IN	IAsyncAction* This,
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
		IsEqualIID(RefIID, &IID_IAsyncAction))
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

KXCOMAPI ULONG STDMETHODCALLTYPE AsyncAction_AddRef(
	IN	IAsyncAction* This)
{
	return InterlockedIncrement(&This->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE AsyncAction_Release(
	IN	IAsyncAction* This)
{
	ULONG NewRefCount;

	NewRefCount = InterlockedDecrement(&This->RefCount);

	if (NewRefCount == 0)
	{
		CoTaskMemFree(This);
	}

	return NewRefCount;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncAction_GetIids(
	IN	IAsyncAction* This,
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
	Array[0] = IID_IAsyncAction;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncAction_GetRuntimeClassName(
	IN	IAsyncAction* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.Foundation.AsyncAction";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncAction_GetTrustLevel(
	IN	IAsyncAction* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncAction_put_Completed(
	IN IAsyncAction* thiz,
	OUT IAsyncActionCompletedHandler* out)
{
	if (out == NULL)
		return E_POINTER;

	thiz->callback = out;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncAction_get_Completed(
	IN IAsyncAction* thiz,
	OUT IAsyncActionCompletedHandler** out)
{
	if (out == NULL)
		return E_POINTER;

	*out = thiz->callback;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncAction_GetResults(
	IN IAsyncAction* thiz)
{
	return thiz->Result;
}

IAsyncActionVtbl CAsyncActionVtbl = {
	AsyncAction_QueryInterface,
	AsyncAction_AddRef,
	AsyncAction_Release,
	AsyncAction_GetIids,
	AsyncAction_GetRuntimeClassName,
	AsyncAction_GetTrustLevel,
	AsyncAction_put_Completed,
	AsyncAction_get_Completed,
	AsyncAction_GetResults
};

IAsyncAction* CreateAsyncAction()
{
	IAsyncAction* action = (IAsyncAction*)CoTaskMemAlloc(sizeof(IAsyncAction));
	if (action == NULL)
		return NULL;

	action->lpVtbl = &CAsyncActionVtbl;
	action->RefCount = 1;
	action->Result = S_ASYNCHRONOUS;
	action->callback = NULL;
	return action;
}

void AsyncActionCompleted(IAsyncAction* action, HRESULT result)
{
	IAsyncActionCompletedHandler* call = action->callback;

	action->Result = result;
	if (call)
	{
		AsyncStatus status = SUCCEEDED(result) ? AsyncStatus_Completed : AsyncStatus_Error;
		call->lpVtbl->Invoke(call, action, status);
	}
	AsyncAction_Release(action);
}



// Foundation : Windows.Foundation.IAsyncInfo

#define CLOSED 4

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncInfo_QueryInterface(
	IN	IAsyncInfo* This,
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
		IsEqualIID(RefIID, &IID_IAsyncInfo))
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

KXCOMAPI ULONG STDMETHODCALLTYPE AsyncInfo_AddRef(
	IN	IAsyncInfo* This)
{
	return InterlockedIncrement(&This->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE AsyncInfo_Release(
	IN	IAsyncInfo* This)
{
	ULONG NewRefCount;

	NewRefCount = InterlockedDecrement(&This->RefCount);

	if (NewRefCount == 0)
	{
		CoTaskMemFree(This);
	}

	return NewRefCount;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncInfo_GetIids(
	IN	IAsyncInfo* This,
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
	Array[0] = IID_IAsyncInfo;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncInfo_GetRuntimeClassName(
	IN	IAsyncInfo* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.Foundation.AsyncInfo";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncInfo_GetTrustLevel(
	IN	IAsyncInfo* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncInfo_get_Id(
	IN IAsyncInfo* thiz,
	OUT UINT* out)
{
	HRESULT hr = S_OK;

	EnterCriticalSection(&thiz->cs);

	if (thiz->status == CLOSED)
		hr = /*E_ILLEGAL_METHOD_CALL*/0x8000000E;

	*out = 1;
	LeaveCriticalSection(&thiz->cs);

	return hr;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncInfo_get_Status(
	IN IAsyncInfo* thiz,
	OUT UINT* out)
{
	HRESULT hr = S_OK;

	EnterCriticalSection(&thiz->cs);

	if (thiz->status == CLOSED)
		hr = /*E_ILLEGAL_METHOD_CALL*/0x8000000E;
	*out = thiz->status;
	LeaveCriticalSection(&thiz->cs);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncInfo_get_ErrorCode(
	IN IAsyncInfo* thiz,
	OUT HRESULT* out)
{
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncInfo_Cancel(
	IN IAsyncInfo* thiz)
{
	HRESULT hr = S_OK;

	EnterCriticalSection(&thiz->cs);

	if (thiz->status == CLOSED)
		hr = /*E_ILLEGAL_METHOD_CALL*/0x8000000E;
	else if (thiz->status == AsyncStatus_Started)
		thiz->status = AsyncStatus_Canceled;

	LeaveCriticalSection(&thiz->cs);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE AsyncInfo_Close(
	IN IAsyncInfo* thiz)
{
	return S_OK;
}

IAsyncInfoVtbl CAsyncInfoVtbl = {
	AsyncInfo_QueryInterface,
	AsyncInfo_AddRef,
	AsyncInfo_Release,

	AsyncInfo_GetIids,
	AsyncInfo_GetRuntimeClassName,
	AsyncInfo_GetTrustLevel,

	AsyncInfo_get_Id,
	AsyncInfo_get_Status,
	AsyncInfo_get_ErrorCode,
	AsyncInfo_Cancel,
	AsyncInfo_Close
};