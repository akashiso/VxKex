#include "buildcfg.h"
#include "kxcomp.h"

//
// WinRT 
//

// System : Windows.System.Threading.ThreadpoolStatics

typedef struct ThreadPool
{
	INIT_ONCE init_once;
	TP_CALLBACK_ENVIRON environment;
} ThreadPool;

typedef struct ThreadPoolWorkItem
{
	IWorkItemHandler* handler;
	IAsyncAction* action;
} ThreadPoolWorkItem;

STATIC ThreadPool pools[3];


STATIC BOOL CALLBACK ThreadPoolInitOnce(
	IN OUT INIT_ONCE* init_once,
	IN OUT PVOID param,
	IN OUT PPVOID context)
{
	ThreadPool* pool = param;

	ZeroMemory(&pool->environment, sizeof(TP_CALLBACK_ENVIRON));
	pool->environment.Version = 1;

	if (!(pool->environment.Pool = CreateThreadpool(NULL))) 
		return FALSE;

	SetThreadpoolThreadMaximum(pool->environment.Pool, 10);

	return TRUE;
}

STATIC VOID CALLBACK ThreadPoolWorkCallback(
	IN OUT TP_CALLBACK_INSTANCE* instance, 
	IN OUT PVOID context,
	IN OUT TP_WORK* work)
{
	ThreadPoolWorkItem* item = context;
	HRESULT result;

	result = item->handler->lpVtbl->Invoke(item->handler, item->action);
	item->handler->lpVtbl->Release(item->handler);
	AsyncActionCompleted(item->action, result);
}

STATIC HRESULT ThreadPoolSubmitWork(
	IN ThreadPoolWorkItem* item, 
	IN WorkItemPriority priority, 
	OUT IAsyncAction** action)
{
	ThreadPool* pool;
	TP_WORK* work;

	ASSERT(priority == WorkItemPriority_Low
		   || priority == WorkItemPriority_Normal
		   || priority == WorkItemPriority_High);

	pool = &pools[priority + 1];

	if (!InitOnceExecuteOnce(&pool->init_once, ThreadPoolInitOnce, pool, NULL))
		return E_FAIL;

	if (!(work = CreateThreadpoolWork(ThreadPoolWorkCallback, item, &pool->environment)))
		return E_FAIL;

	*action = item->action;
	item->action->lpVtbl->AddRef(item->action);
	SubmitThreadpoolWork(work);

	return S_OK;
}

STATIC DWORD WINAPI SlicedThreadProc(
	IN PVOID arg)
{
	ThreadPoolWorkItem* item = arg;
	HRESULT result;

	result = item->handler->lpVtbl->Invoke(item->handler, item->action);
	item->handler->lpVtbl->Release(item->handler);
	AsyncActionCompleted(item->action, result);
	return 0;
}

STATIC HRESULT StandaloneThreadSubmitWork(
	IN ThreadPoolWorkItem* item, 
	IN WorkItemPriority priority, 
	OUT IAsyncAction** action)
{
	HANDLE thread;
	thread = CreateThread(NULL, 0, SlicedThreadProc, item, priority == WorkItemPriority_Normal ?
						  0 : CREATE_SUSPENDED, NULL);

	if (thread == NULL)
	{
		KexLogWarningEvent(
			L"Failed to create a thread, error %ld %s.\n", 
			GetLastError(), Win32ErrorAsString(GetLastError()));

		return HRESULT_FROM_WIN32(GetLastError());
	}

	*action = item->action;
	item->action->lpVtbl->AddRef(item->action);

	if (priority != WorkItemPriority_Normal)
	{
		SetThreadPriority(thread, priority == WorkItemPriority_High ? THREAD_PRIORITY_HIGHEST : THREAD_PRIORITY_LOWEST);
		ResumeThread(thread);
	}
	CloseHandle(thread);

	return S_OK;
}

STATIC HRESULT ThreadPoolRunAync(
	IN IWorkItemHandler* handler, 
	IN WorkItemPriority priority, 
	IN WorkItemOptions options,
	OUT IAsyncAction** action)
{
	if (!handler)
		return E_INVALIDARG;

	if (priority < WorkItemPriority_Low || priority > WorkItemPriority_High)
		return E_INVALIDARG;

	ThreadPoolWorkItem* item;
	HRESULT hr;

	item = (ThreadPoolWorkItem*)CoTaskMemAlloc(sizeof(ThreadPoolWorkItem));
	if (item == NULL)
		return E_OUTOFMEMORY;

	item->handler = handler;
	item->action = CreateAsyncAction();
	if (item->action == NULL)
	{
		CoTaskMemFree(item);
		return E_OUTOFMEMORY;
	}

	handler->lpVtbl->AddRef(handler);
	*action = item->action;

	if (options == WorkItemOptions_TimeSliced)
		hr = StandaloneThreadSubmitWork(item, priority, action);
	else
		hr = ThreadPoolSubmitWork(item, priority, action);

	if (FAILED(hr))
	{
		item->handler->lpVtbl->Release(item->handler);
		AsyncActionCompleted(item->action, S_OK);
	}

	return hr;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolStatics_QueryInterface(
	IN	IThreadpoolStatics* This,
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
		IsEqualIID(RefIID, &IID_IThreadPoolStatics))
	{
		*Object = This;
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

KXCOMAPI ULONG STDMETHODCALLTYPE ThreadpoolStatics_AddRef(
	IN	IThreadpoolStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE ThreadpoolStatics_Release(
	IN	IThreadpoolStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolStatics_GetIids(
	IN	IThreadpoolStatics* This,
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
	Array[0] = IID_IThreadPoolStatics;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolStatics_GetRuntimeClassName(
	IN	IThreadpoolStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.System.Threading.ThreadPoolStatics";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolStatics_GetTrustLevel(
	IN	IThreadpoolStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolStatics_RunAsync(
	IN IThreadpoolStatics* thiz,
	IN IWorkItemHandler* handler, 
	OUT IAsyncAction** action)
{
	return ThreadPoolRunAync(handler, WorkItemPriority_Normal, WorkItemOptions_None, action);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolStatics_RunWithPriorityAsync(
	IN IThreadpoolStatics* thiz,
	IN IWorkItemHandler* handler,
	WorkItemPriority priority,
	OUT IAsyncAction** action)
{
	return ThreadPoolRunAync(handler, priority, WorkItemOptions_None, action);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolStatics_RunWithPriorityAndOptionsAsync(
	IN IThreadpoolStatics* thiz,
	IN IWorkItemHandler* handler,
	WorkItemPriority priority,
	WorkItemOptions options,
	OUT IAsyncAction** action)
{
	return ThreadPoolRunAync(handler, priority, options, action);
}

IThreadpoolStaticsVtbl CThreadpoolStaticsVtbl = {
	ThreadpoolStatics_QueryInterface,
	ThreadpoolStatics_AddRef,
	ThreadpoolStatics_Release,

	ThreadpoolStatics_GetIids,
	ThreadpoolStatics_GetRuntimeClassName,
	ThreadpoolStatics_GetTrustLevel,

	ThreadpoolStatics_RunAsync,
	ThreadpoolStatics_RunWithPriorityAsync,
	ThreadpoolStatics_RunWithPriorityAndOptionsAsync
};

IThreadpoolStatics CThreadpoolStatics = {
	&CThreadpoolStaticsVtbl
};
