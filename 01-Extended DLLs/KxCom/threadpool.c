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
	PTP_TIMER timer;
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

	InitializeThreadpoolEnvironment(&pool->environment);
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

	CloseThreadpoolWork(work);
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

	if (thread == NULL) {
		KexLogWarningEvent(
			L"Failed to create a thread, error %ld %s.\n",
			GetLastError(), Win32ErrorAsString(GetLastError()));

		return HRESULT_FROM_WIN32(GetLastError());
	}

	*action = item->action;
	item->action->lpVtbl->AddRef(item->action);

	if (priority != WorkItemPriority_Normal) {
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
	if (item->action == NULL) {
		CoTaskMemFree(item);
		return E_OUTOFMEMORY;
	}

	handler->lpVtbl->AddRef(handler);
	*action = item->action;

	if (options == WorkItemOptions_TimeSliced)
		hr = StandaloneThreadSubmitWork(item, priority, action);
	else
		hr = ThreadPoolSubmitWork(item, priority, action);

	if (FAILED(hr)) {
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
		IsEqualIID(RefIID, &IID_IThreadPoolStatics)) {
		*Object = This;
	}
	else {
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
	if (!Array) {
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


// System : Windows.System.Threading.ThreadpoolTimer


STATIC VOID CALLBACK ThreadpoolTimerCallback(
	IN PTP_CALLBACK_INSTANCE Instance,
	IN PVOID Parameter,
	IN PTP_TIMER hTimer)
{
	IThreadpoolTimer* timer = (IThreadpoolTimer*)Parameter;

	timer->Handler->lpVtbl->Invoke(timer->Handler, timer);

	if (!timer->IsRepeating)
		goto Quit;

	return;
Quit:
	if (timer->Destroyed)
		timer->Destroyed->lpVtbl->Invoke(timer->Destroyed, timer);

	return;
}

STATIC HRESULT ThreadpoolTimerStart(
	IN	IThreadpoolTimer* timer)
{
	if (timer->hTimer)
		return E_NOT_VALID_STATE;

	ThreadPool* pool;
	pool = &pools[1];

	if (!InitOnceExecuteOnce(&pool->init_once, ThreadPoolInitOnce, pool, NULL))
		return E_FAIL;

	timer->hTimer = CreateThreadpoolTimer(ThreadpoolTimerCallback,
										  timer, &pool->environment);

	if (!timer->IsRepeating) {
		LARGE_INTEGER timeL;
		timeL.QuadPart = -(INT64)timer->Interval;

		FILETIME time;
		time.dwHighDateTime = timeL.HighPart;
		time.dwLowDateTime = timeL.LowPart;

		SetThreadpoolTimer(timer->hTimer, &time, 0, 0);
	}
	else {
		SetThreadpoolTimer(timer->hTimer, NULL, (DWORD)(timer->Interval / 10 / 1000), 0);
	}
	return HRESULT_FROM_WIN32(GetLastError());
}

STATIC HRESULT ThreadpoolTimerStop(
	IN	IThreadpoolTimer* timer)
{
	if (timer->hTimer) {
		timer->IsRepeating = FALSE;

		SetThreadpoolTimer(timer->hTimer, NULL, 0, 0);
		WaitForThreadpoolTimerCallbacks(timer->hTimer, FALSE);
		CloseThreadpoolTimer(timer->hTimer);
	}

	timer->hTimer = NULL;

	timer->lpVtbl->Release(timer);
	return S_OK;
}

STATIC HRESULT CreateIThreadpoolTimer(
	IN ITimerElapsedHandler* Handler,
	IN ITimerElapsedHandler* Destroyed,
	IN UINT64 Interval,
	IN BOOL IsRepeating,
	OUT IThreadpoolTimer** object)
{
	if (object == NULL)
		return E_POINTER;

	IThreadpoolTimer* timer = (IThreadpoolTimer*)CoTaskMemAlloc(sizeof(IThreadpoolTimer));
	if (!timer)
		return E_OUTOFMEMORY;

	timer->lpVtbl = &CThreadpoolTimerVtbl;
	timer->RefCount = 1;
	timer->Interval = Interval;
	timer->IsRepeating = IsRepeating;
	timer->Handler = Handler;
	timer->Destroyed = Destroyed;

	Handler->lpVtbl->AddRef(Handler);
	if (Destroyed)
		Destroyed->lpVtbl->AddRef(Destroyed);

	HRESULT Result = ThreadpoolTimerStart(timer);

	if (FAILED(Result)) {
		CoTaskMemFree(timer);
		return Result;
	}

	*object = timer;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimer_QueryInterface(
	IN	IThreadpoolTimer* This,
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
		IsEqualIID(RefIID, &IID_IThreadPoolTimer)) {
		*Object = This;
		InterlockedIncrement(&This->RefCount);
	}
	else {
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

KXCOMAPI ULONG STDMETHODCALLTYPE ThreadpoolTimer_AddRef(
	IN	IThreadpoolTimer* This)
{
	return InterlockedIncrement(&This->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE ThreadpoolTimer_Release(
	IN	IThreadpoolTimer* This)
{
	ULONG NewRefCount;

	NewRefCount = InterlockedDecrement(&This->RefCount);

	if (NewRefCount == 0) {
		if (This->hTimer)
			DeleteTimerQueueTimer(NULL, This->hTimer, INVALID_HANDLE_VALUE);

		This->Handler->lpVtbl->Release(This->Handler);
		This->Destroyed->lpVtbl->Release(This->Destroyed);
		CoTaskMemFree(This);
	}

	return NewRefCount;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimer_GetIids(
	IN	IThreadpoolTimer* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 1;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array) {
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_IThreadPoolTimer;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimer_GetRuntimeClassName(
	IN	IThreadpoolTimer* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.System.ThreadPoolTimer";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimer_GetTrustLevel(
	IN	IThreadpoolTimer* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimer_get_Period(
	IN IThreadpoolTimer* thiz,
	OUT UINT64* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = thiz->Interval;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimer_get_Delay(
	IN IThreadpoolTimer* thiz,
	OUT UINT64* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = thiz->Interval;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimer_Cancel(
	IN IThreadpoolTimer* thiz)
{
	return ThreadpoolTimerStop(thiz);
}


IThreadpoolTimerVtbl CThreadpoolTimerVtbl = {
	ThreadpoolTimer_QueryInterface,
	ThreadpoolTimer_AddRef,
	ThreadpoolTimer_Release,

	ThreadpoolTimer_GetIids,
	ThreadpoolTimer_GetRuntimeClassName,
	ThreadpoolTimer_GetTrustLevel,

	ThreadpoolTimer_get_Period,
	ThreadpoolTimer_get_Delay,
	ThreadpoolTimer_Cancel
};



// System : Windows.System.Threading.ThreadpoolTimerStatics

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimerStatics_QueryInterface(
	IN	IThreadpoolTimerStatics* This,
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
		IsEqualIID(RefIID, &IID_IThreadPoolTimerStatics)) {
		*Object = This;
	}
	else {
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

KXCOMAPI ULONG STDMETHODCALLTYPE ThreadpoolTimerStatics_AddRef(
	IN	IThreadpoolTimerStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE ThreadpoolTimerStatics_Release(
	IN	IThreadpoolTimerStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimerStatics_GetIids(
	IN	IThreadpoolTimerStatics* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 1;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array) {
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_IThreadPoolTimerStatics;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimerStatics_GetRuntimeClassName(
	IN	IThreadpoolTimerStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.System.Threading.ThreadPoolTimerStatics";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimerStatics_GetTrustLevel(
	IN	IThreadpoolTimerStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimerStatics_CreatePeriodicTimer(
	IN IThreadpoolTimerStatics* thiz,
	IN ITimerElapsedHandler* handler,
	IN UINT64 period,
	OUT IThreadpoolTimer** timer)
{
	return CreateIThreadpoolTimer(handler, NULL, period, TRUE, timer);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimerStatics_CreateTimer(
	IN IThreadpoolTimerStatics* thiz,
	IN ITimerElapsedHandler* handler,
	IN UINT64 delay,
	OUT IThreadpoolTimer** timer)
{
	return CreateIThreadpoolTimer(handler, NULL, delay, FALSE, timer);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimerStatics_CreatePeriodicTimer1(
	IN IThreadpoolTimerStatics* thiz,
	IN ITimerElapsedHandler* handler,
	IN UINT64 period,
	IN ITimerElapsedHandler* destroyed,
	OUT IThreadpoolTimer** timer)
{
	return CreateIThreadpoolTimer(handler, destroyed, period, TRUE, timer);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE ThreadpoolTimerStatics_CreateTimer1(
	IN IThreadpoolTimerStatics* thiz,
	IN ITimerElapsedHandler* handler,
	IN UINT64 delay,
	IN ITimerElapsedHandler* destroyed,
	OUT IThreadpoolTimer** timer)
{
	return CreateIThreadpoolTimer(handler, destroyed, delay, FALSE, timer);
}

IThreadpoolTimerStaticsVtbl CThreadpoolTimerStaticsVtbl = {
	ThreadpoolTimerStatics_QueryInterface,
	ThreadpoolTimerStatics_AddRef,
	ThreadpoolTimerStatics_Release,

	ThreadpoolTimerStatics_GetIids,
	ThreadpoolTimerStatics_GetRuntimeClassName,
	ThreadpoolTimerStatics_GetTrustLevel,

	ThreadpoolTimerStatics_CreatePeriodicTimer,
	ThreadpoolTimerStatics_CreateTimer,
	ThreadpoolTimerStatics_CreatePeriodicTimer1,
	ThreadpoolTimerStatics_CreateTimer1
};

IThreadpoolTimerStatics CThreadpoolTimerStatics = {
	&CThreadpoolTimerStaticsVtbl
};
