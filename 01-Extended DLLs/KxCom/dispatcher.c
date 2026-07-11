#include "buildcfg.h"
#include "kxcomp.h"

//
// WinRT 
//

// System : Windows.System.IDispatcherQueueController

#define KEXDQMSG_CMD_ENQUEUE 0xBADFACE0
#define KEXDQMSG_CMD_TIMER_TICK 0xBADFACE1
#define KEXDQMSG_CMD_SHUTDOWN 0xBADFACE2
#define KEXDQMSG_CMD_SHUTDOWN_READY 0xBADFACE3
#define KEXDQMSG_MASK 0xBADFACE0

typedef struct DispatcherQueueTask
{
	IDispatcherQueue* queue;
	IDelegate* call;
} DispatcherQueueTask;


UINT uDispatcherMsg = 0;

IDispatcherQueue* queues = NULL;
RTL_SRWLOCK queuesLock = { 0 };

STATIC VOID EnsureMessageRegistered()
{
	if (uDispatcherMsg == 0)
		uDispatcherMsg = RegisterWindowMessage(L"VxKex_Internal_DispatcherQueue");
}

STATIC LRESULT CALLBACK DispatcherQueueGetMsgProc(
	IN int    code,
	IN WPARAM wParam,
	IN LPARAM lParam
)
{
	MSG* msg = (MSG*)lParam;

	if (code == HC_ACTION && wParam == 1
		&& msg->message == uDispatcherMsg)
	{
		DispatcherQueueTask* task = (DispatcherQueueTask*)msg->lParam;
		IDispatcherQueue* q = task->queue;
		IDispatcherQueueTimer* timer;

		switch (msg->wParam)
		{
			case KEXDQMSG_CMD_ENQUEUE:
				task->call->lpVtbl->Invoke(task->call);
				task->call->lpVtbl->Release(task->call);
				CoTaskMemFree(task);
				break;
			case KEXDQMSG_CMD_TIMER_TICK:
				timer = (IDispatcherQueueTimer*)msg->lParam;
				EventHandlersNotify(timer->tick, (IInspectable*)timer);
				timer->lpVtbl->Release(timer);
				break;
			case KEXDQMSG_CMD_SHUTDOWN:
				EventHandlersNotify(q->shutdownStart, (IInspectable*)q);
				EventHandlersRemoveAll(&q->shutdownStart);
				PostThreadMessage(GetCurrentThreadId(), uDispatcherMsg, KEXDQMSG_CMD_SHUTDOWN_READY, lParam);
				break;
			case KEXDQMSG_CMD_SHUTDOWN_READY:
				if (q->Hook)
					UnhookWindowsHookEx(q->Hook);
				if (q->hThread)
					CloseHandle(q->hThread);
				q->Hook = NULL;
				q->hThread = NULL;

				EventHandlersNotify(q->shutdownCompleted, (IInspectable*)q);
				EventHandlersRemoveAll(&q->shutdownCompleted);

				AsyncActionCompleted((IAsyncAction*)lParam, S_OK);
				break;
		}
		return S_OK;
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}

HRESULT DispatcherQueueCreateOnThread(
	IN DWORD dwThreadId,
	OUT IDispatcherQueue** Object)
{
	EnsureMessageRegistered();

	HHOOK hook = NULL;
	HANDLE hThread = NULL;
	IDispatcherQueue* queue = NULL;

	hook = SetWindowsHookEx(WH_GETMESSAGE, DispatcherQueueGetMsgProc, NULL, dwThreadId);
	if (hook == NULL)
		goto Failed;

	hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, dwThreadId);
	if (hThread == NULL)
		goto Failed;

	queue = (IDispatcherQueue*)CoTaskMemAlloc(sizeof(IDispatcherQueue));
	if (queue == NULL)
		goto Failed;

	RtlAcquireSRWLockExclusive(&queuesLock);

	queue->lpVtbl = &CDispatcherQueueVtbl;
	queue->Parent = queue;
	queue->cDispatcherQueue2.lpVtbl = &CDispatcherQueue2Vtbl;
	queue->cDispatcherQueue2.Parent = queue;
	queue->dwThreadId = dwThreadId;
	queue->hThread = hThread;
	queue->Hook = hook;
	queue->RefCount = 1;
	queue->pPrev = queues;
	InitializeEventHandlers(&queue->shutdownStart);
	InitializeEventHandlers(&queue->shutdownCompleted);
	queues = queue;

	RtlReleaseSRWLockExclusive(&queuesLock);
	*Object = queue;
	return S_OK;

Failed:
	if (hook)
		UnhookWindowsHookEx(hook);

	if (hThread)
		CloseHandle(hThread);

	if (queue)
		CoTaskMemFree(queue);

	if (hook == NULL || hThread == NULL)
		return HRESULT_FROM_WIN32(GetLastError());
	else if (queue == NULL)
		return E_OUTOFMEMORY;

	return E_FAIL;
}

STATIC DWORD WINAPI DispatcherQueueDedicatedThreadProc(
	IN LPVOID lpParameter
)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE); // Create message queue

	HANDLE msgQueueCreatedEvent = (HANDLE)lpParameter;
	SetEvent(msgQueueCreatedEvent);

	while (GetMessage(&msg, (HWND)-1, 0, 0) != 0)
	{
		if (msg.message == uDispatcherMsg)
		{
			DispatcherQueueTask* task = (DispatcherQueueTask*)msg.lParam;
			IDispatcherQueue* q = task->queue;
			IDispatcherQueueTimer* timer;

			switch (msg.wParam)
			{
				case KEXDQMSG_CMD_ENQUEUE:
					task->call->lpVtbl->Invoke(task->call);
					task->call->lpVtbl->Release(task->call);
					CoTaskMemFree(task);
					break;
				case KEXDQMSG_CMD_TIMER_TICK:
					timer = (IDispatcherQueueTimer*)msg.lParam;
					EventHandlersNotify(timer->tick, (IInspectable*)timer);
					timer->lpVtbl->Release(timer);
					break;
				case KEXDQMSG_CMD_SHUTDOWN:
					EventHandlersNotify(q->shutdownStart, (IInspectable*)q);
					EventHandlersRemoveAll(&q->shutdownStart);

					if (q->Hook)
						UnhookWindowsHookEx(q->Hook);
					if (q->hThread)
						CloseHandle(q->hThread);
					q->Hook = NULL;
					q->hThread = NULL;

					EventHandlersNotify(q->shutdownCompleted, (IInspectable*)q);
					EventHandlersRemoveAll(&q->shutdownCompleted);

					AsyncActionCompleted((IAsyncAction*)msg.lParam, S_OK);
					PostQuitMessage(0);
					break;
			}
		}
	}

	CoUninitialize();
	return 0;
}

HRESULT DispatcherQueueCreateOnNewThread(
	OUT	IDispatcherQueue** Object)
{
	EnsureMessageRegistered();

	DWORD threadId;
	HANDLE hThread = NULL;
	HANDLE msgQueueCreatedEvent = NULL;
	IDispatcherQueue* queue = NULL;

	if ((msgQueueCreatedEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		goto Failed;

	if ((hThread = CreateThread(NULL, 0, DispatcherQueueDedicatedThreadProc, msgQueueCreatedEvent, 0, &threadId)) == NULL)
		goto Failed;

	queue = (IDispatcherQueue*)CoTaskMemAlloc(sizeof(IDispatcherQueue));
	if (queue == NULL)
		goto Failed;

	RtlAcquireSRWLockExclusive(&queuesLock);

	queue->lpVtbl = &CDispatcherQueueVtbl;
	queue->Parent = queue;
	queue->cDispatcherQueue2.lpVtbl = &CDispatcherQueue2Vtbl;
	queue->cDispatcherQueue2.Parent = queue;
	queue->dwThreadId = threadId;
	queue->hThread = hThread;
	queue->Hook = NULL;
	queue->RefCount = 1;
	queue->pPrev = queues;
	InitializeEventHandlers(&queue->shutdownStart);
	InitializeEventHandlers(&queue->shutdownCompleted);
	queues = queue;

	RtlReleaseSRWLockExclusive(&queuesLock);
	*Object = queue;

	WaitForSingleObject(msgQueueCreatedEvent, INFINITE);
	CloseHandle(msgQueueCreatedEvent);
	return S_OK;

Failed:
	if (msgQueueCreatedEvent)
		CloseHandle(msgQueueCreatedEvent);

	if (hThread)
		CloseHandle(hThread);

	if (queue)
		CoTaskMemFree(queue);

	if (msgQueueCreatedEvent == NULL || hThread == NULL)
		return HRESULT_FROM_WIN32(GetLastError());
	else if (queue == NULL)
		return E_OUTOFMEMORY;

	return E_FAIL;
}

IDispatcherQueue* DispatcherQueueGetForThread(
	IN	DWORD dwThreadId)
{
	RtlAcquireSRWLockShared(&queuesLock);

	IDispatcherQueue* q = queues;
	while (q)
	{
		if (q->dwThreadId == dwThreadId)
			break;

		q = q->pPrev;
	}
	RtlReleaseSRWLockShared(&queuesLock);
	return q;
}

STATIC BOOL DispatcherQueueIsThreadAlive(
	IN	IDispatcherQueue* q)
{
	if (q->Parent->hThread == NULL)
		return FALSE;

	DWORD code;
	if (GetExitCodeThread(q->Parent->hThread, &code))
		if (code == STILL_ACTIVE)
			return TRUE;

	return FALSE;
}

STATIC HRESULT DispatcherQueueEnqueue(
	IN	IDispatcherQueue* q,
	IN IDelegate* callback)
{
	DispatcherQueueTask* task = (DispatcherQueueTask*)CoTaskMemAlloc(sizeof(DispatcherQueueTask));
	if (task == NULL)
		return E_OUTOFMEMORY;

	callback->lpVtbl->AddRef(callback);

	task->queue = q->Parent;
	task->call = callback;
	if (!PostThreadMessage(q->Parent->dwThreadId, uDispatcherMsg, KEXDQMSG_CMD_ENQUEUE, (LPARAM)task))
		return HRESULT_FROM_WIN32(GetLastError());
	return S_OK;
}

STATIC ULONG DispatcherQueueRelease(
	IN	IDispatcherQueue* q)
{
	ULONG NewRefCount;
	NewRefCount = InterlockedDecrement(&q->Parent->RefCount);

	if (NewRefCount == 0)
	{
		RtlAcquireSRWLockExclusive(&queuesLock);

		IDispatcherQueue* q0 = queues;
		IDispatcherQueue* qPrev = NULL;
		while (q0)
		{
			if (q0 == q)
				break;

			qPrev = q0;
			q0 = q0->pPrev;
		}

		if (qPrev != NULL)
			qPrev->pPrev = q->pPrev;
		else
			queues = q->pPrev;

		if (q->Hook)
			UnhookWindowsHookEx(q->Hook);
		if (q->hThread)
			CloseHandle(q->hThread);

		RtlReleaseSRWLockExclusive(&queuesLock);
		EventHandlersRemoveAll(&q->shutdownStart);
		EventHandlersRemoveAll(&q->shutdownCompleted);
		CoTaskMemFree(q);
	}

	return NewRefCount;
}


STATIC HRESULT DispatcherQueueCreateTimer(
	IN	IDispatcherQueue* queue,
	OUT IDispatcherQueueTimer** Object)
{
	if (Object == NULL) 
		return E_POINTER;

	IDispatcherQueueTimer* timer = (IDispatcherQueueTimer*)CoTaskMemAlloc(sizeof(IDispatcherQueueTimer));
	if (!timer) 
		return E_OUTOFMEMORY;

	timer->lpVtbl = &CDispatcherQueueTimerVtbl;
	timer->RefCount = 1;
	timer->queue = queue->Parent;
	timer->queue->lpVtbl->AddRef(timer->queue);
	timer->Interval = 0;
	timer->IsRunning = FALSE;
	timer->IsRepeating = FALSE;
	timer->hTimer = NULL;
	InitializeEventHandlers(&timer->tick);

	*Object = timer;
	return S_OK;
}

STATIC VOID CALLBACK TimerQueueCallback(
	IN PVOID Parameter,
	IN BOOLEAN TimerOrWaitFired)
{
	IDispatcherQueueTimer* timer = (IDispatcherQueueTimer*)Parameter;
	if (!TimerOrWaitFired)
		return;

	BOOL shouldTick = timer->IsRunning;
	IDispatcherQueue* queue = timer->queue;

	if (!shouldTick || !DispatcherQueueIsThreadAlive(queue))
		goto Quit;

	BOOL succeed = PostThreadMessage(queue->Parent->dwThreadId, uDispatcherMsg, KEXDQMSG_CMD_TIMER_TICK, (LPARAM)timer);
	if (!succeed)
		goto Quit;
	else
		timer->lpVtbl->AddRef(timer);
	
	if (!timer->IsRepeating)
		goto Quit;

	return;
Quit:
	timer->IsRunning = FALSE;
	return;
}

STATIC HRESULT DispatcherQueueTimerStart(
	IN	IDispatcherQueueTimer* timer)
{
	if (timer->hTimer)
		return E_NOT_VALID_STATE;

	timer->IsRunning = TRUE;
	if (!CreateTimerQueueTimer(&timer->hTimer, NULL, TimerQueueCallback,
							   timer, timer->Interval,
							   timer->IsRepeating ? timer->Interval : 0,
							   WT_EXECUTEDEFAULT))
	{
		timer->IsRunning = FALSE;
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}

STATIC HRESULT DispatcherQueueTimerStop(
	IN	IDispatcherQueueTimer* timer)
{
	timer->IsRunning = FALSE;

	if (timer->hTimer)
		if (!DeleteTimerQueueTimer(NULL, timer->hTimer, INVALID_HANDLE_VALUE))
			return HRESULT_FROM_WIN32(GetLastError());

	timer->hTimer = NULL;
	return S_OK;
}

STATIC HRESULT DispatcherQueueTimerUpdateParameter(
	IN	IDispatcherQueueTimer* timer)
{
	if (timer->hTimer)
		if (!ChangeTimerQueueTimer(NULL, timer->hTimer, timer->Interval, timer->IsRepeating ? timer->Interval : 0))
			return HRESULT_FROM_WIN32(GetLastError());

	return S_OK;
}

STATIC ULONG DispatcherQueueTimerRelease(
	IN	IDispatcherQueueTimer* timer)
{
	ULONG NewRefCount;
	NewRefCount = InterlockedDecrement(&timer->RefCount);

	if (NewRefCount == 0)
	{
		if (timer->hTimer)
			DeleteTimerQueueTimer(NULL, timer->hTimer, INVALID_HANDLE_VALUE);

		timer->queue->lpVtbl->Release(timer->queue);
		EventHandlersRemoveAll(&timer->tick);
		CoTaskMemFree(timer);
	}

	return NewRefCount;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueController_QueryInterface(
	IN	IDispatcherQueueController* This,
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
		IsEqualIID(RefIID, &IID_IDispatcherQueueController))
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

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueueController_AddRef(
	IN	IDispatcherQueueController* This)
{
	return InterlockedIncrement(&This->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueueController_Release(
	IN	IDispatcherQueueController* This)
{
	ULONG NewRefCount;

	NewRefCount = InterlockedDecrement(&This->RefCount);

	if (NewRefCount == 0)
	{
		IUnknown_Release((IUnknown*)This->queue);
		CoTaskMemFree(This);
	}

	return NewRefCount;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueController_GetIids(
	IN	IDispatcherQueueController* This,
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
	Array[0] = IID_IDispatcherQueueController;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueController_GetRuntimeClassName(
	IN	IDispatcherQueueController* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.System.DispatcherQueueController";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueController_GetTrustLevel(
	IN	IDispatcherQueueController* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueController_get_DispatcherQueue(
	IN IDispatcherQueueController* thiz,
	OUT IDispatcherQueue** out)
{
	if (out == NULL)
		return E_POINTER;

	*out = thiz->queue;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueController_ShutdownQueueAsync(
	IN IDispatcherQueueController* thiz,
	OUT IAsyncAction** out)
{
	if (out == NULL)
		return E_POINTER;

	IAsyncAction* action = CreateAsyncAction();

	if (!PostThreadMessage(thiz->queue->dwThreadId, uDispatcherMsg, KEXDQMSG_CMD_SHUTDOWN, (LPARAM)action))
	{
		IUnknown_Release(action);
		return HRESULT_FROM_WIN32(GetLastError());
	}

	*out = action;
	return S_OK;
}


IDispatcherQueueControllerVtbl CDispatcherQueueControllerVtbl = {
	DispatcherQueueController_QueryInterface,
	DispatcherQueueController_AddRef,
	DispatcherQueueController_Release,

	DispatcherQueueController_GetIids,
	DispatcherQueueController_GetRuntimeClassName,
	DispatcherQueueController_GetTrustLevel,

	DispatcherQueueController_get_DispatcherQueue,
	DispatcherQueueController_ShutdownQueueAsync,
};

// System : Windows.System.IDispatcherQueue

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_QueryInterface(
	IN	IDispatcherQueue* This,
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
		IsEqualIID(RefIID, &IID_IDispatcherQueue))
	{
		*Object = This->Parent;
		InterlockedIncrement(&This->Parent->RefCount);
	}
	else if (IsEqualIID(RefIID, &IID_IDispatcherQueue2))
	{
		*Object = &This->cDispatcherQueue2;
		InterlockedIncrement(&This->Parent->RefCount);
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

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueue_AddRef(
	IN	IDispatcherQueue* This)
{
	return InterlockedIncrement(&This->Parent->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueue_Release(
	IN	IDispatcherQueue* This)
{
	return DispatcherQueueRelease(This);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_GetIids(
	IN	IDispatcherQueue* This,
	OUT	PULONG		NumberOfIids,
	OUT	IID** IidArray)
{
	IID* Array;
	ULONG Count;

	ASSERT(NumberOfIids != NULL);
	ASSERT(IidArray != NULL);

	Count = 2;

	Array = (IID*)CoTaskMemAlloc(Count * sizeof(IID));
	if (!Array)
	{
		return E_OUTOFMEMORY;
	}

	*NumberOfIids = Count;
	Array[0] = IID_IDispatcherQueue;
	Array[1] = IID_IDispatcherQueue2;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_GetRuntimeClassName(
	IN	IDispatcherQueue* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.System.DispatcherQueue";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_GetTrustLevel(
	IN	IDispatcherQueue* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_CreateTimer(
	IN	IDispatcherQueue* This,
	OUT	PPVOID out)
{
	return DispatcherQueueCreateTimer(This, (IDispatcherQueueTimer**)out);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_TryEnqueue(
	IN	IDispatcherQueue* This,
	IN	IDelegate* in,
	OUT	BOOL* out)
{
	if (out == NULL || in == NULL)
		return E_POINTER;

	if (!DispatcherQueueIsThreadAlive(This))
	{
		*out = FALSE;
		return S_OK;
	}

	HRESULT Result;
	Result = DispatcherQueueEnqueue(This, in);
	if (FAILED(Result))
	{
		*out = FALSE;
		return Result == E_FAIL ? S_OK : Result;
	}

	*out = TRUE;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_TryEnqueueWithPriority(
	IN	IDispatcherQueue* This,
	IN	INT p,
	IN	IDelegate* in,
	OUT	BOOL* out)
{
	if (out == NULL || in == NULL)
		return E_POINTER;

	return DispatcherQueue_TryEnqueue(This, in, out);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_add_ShutdownStarting(
	IN	IDispatcherQueue* This,
	IN	ITypedEventHandler* in,
	OUT	EventRegistrationToken* token)
{
	return EventHandlersAppend(&This->shutdownStart, in, token);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_remove_ShutdownStarting(
	IN	IDispatcherQueue* This,
	IN	EventRegistrationToken token)
{
	return EventHandlersRemove(&This->shutdownStart, &token);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_add_ShutdownCompleted(
	IN	IDispatcherQueue* This,
	IN	ITypedEventHandler* in,
	OUT	EventRegistrationToken* token)
{
	return EventHandlersAppend(&This->shutdownCompleted, in, token);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue_remove_ShutdownCompleted(
	IN	IDispatcherQueue* This,
	IN	EventRegistrationToken token)
{
	return EventHandlersRemove(&This->shutdownCompleted, &token);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueue2_get_HasThreadAccess(
	IN	IDispatcherQueue* This,
	IN	BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	// 
	// This = DispatcherQueue2
	//

	*out = DispatcherQueueIsThreadAlive(This->Parent);
	return S_OK;
}


IDispatcherQueueVtbl CDispatcherQueueVtbl = {
	DispatcherQueue_QueryInterface,
	DispatcherQueue_AddRef,
	DispatcherQueue_Release,

	DispatcherQueue_GetIids,
	DispatcherQueue_GetRuntimeClassName,
	DispatcherQueue_GetTrustLevel,

	DispatcherQueue_CreateTimer,
	DispatcherQueue_TryEnqueue,
	DispatcherQueue_TryEnqueueWithPriority,
	DispatcherQueue_add_ShutdownStarting,
	DispatcherQueue_remove_ShutdownStarting,
	DispatcherQueue_add_ShutdownCompleted,
	DispatcherQueue_remove_ShutdownCompleted
};

IDispatcherQueue2Vtbl CDispatcherQueue2Vtbl = {
	DispatcherQueue_QueryInterface,
	DispatcherQueue_AddRef,
	DispatcherQueue_Release,

	DispatcherQueue_GetIids,
	DispatcherQueue_GetRuntimeClassName,
	DispatcherQueue_GetTrustLevel,

	DispatcherQueue2_get_HasThreadAccess,
};


// System : Windows.System.IDispatcherQueueStatics

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueStatics_QueryInterface(
	IN	IDispatcherQueueStatics* This,
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
		IsEqualIID(RefIID, &IID_IDispatcherQueueStatics))
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

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueueStatics_AddRef(
	IN	IDispatcherQueueStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueueStatics_Release(
	IN	IDispatcherQueueStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueStatics_GetIids(
	IN	IDispatcherQueueStatics* This,
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
	Array[0] = IID_IDispatcherQueueStatics;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueStatics_GetRuntimeClassName(
	IN	IDispatcherQueueStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.System.DispatcherQueueStatics";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueStatics_GetTrustLevel(
	IN	IDispatcherQueueStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueStatics_GetForCurrentThread(
	IN	IDispatcherQueueStatics* This,
	OUT	IDispatcherQueue** out)
{
	if (out == NULL)
		return E_POINTER;

	*out = DispatcherQueueGetForThread(GetCurrentThreadId());

	if (*out) IUnknown_AddRef((IUnknown*)(*out));
	return S_OK;
}

IDispatcherQueueStaticsVtbl CDispatcherQueueStaticsVtbl = {
	DispatcherQueueStatics_QueryInterface,
	DispatcherQueueStatics_AddRef,
	DispatcherQueueStatics_Release,

	DispatcherQueueStatics_GetIids,
	DispatcherQueueStatics_GetRuntimeClassName,
	DispatcherQueueStatics_GetTrustLevel,

	DispatcherQueueStatics_GetForCurrentThread,
};

IDispatcherQueueStatics CDispatcherQueueStatics = {
	&CDispatcherQueueStaticsVtbl
};

// System : Windows.System.IDispatcherQueueControllerStatics
//


KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueControllerStatics_QueryInterface(
	IN	IDispatcherQueueControllerStatics* This,
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
		IsEqualIID(RefIID, &IID_IDispatcherQueueControllerStatics))
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

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueueControllerStatics_AddRef(
	IN	IDispatcherQueueControllerStatics* This)
{
	return 1;
}

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueueControllerStatics_Release(
	IN	IDispatcherQueueControllerStatics* This)
{
	return 1;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueControllerStatics_GetIids(
	IN	IDispatcherQueueControllerStatics* This,
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
	Array[0] = IID_IDispatcherQueueControllerStatics;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueControllerStatics_GetRuntimeClassName(
	IN	IDispatcherQueueControllerStatics* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.System.DispatcherQueueControllerStatics";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueControllerStatics_GetTrustLevel(
	IN	IDispatcherQueueControllerStatics* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueControllerStatics_CreateOnDedicatedThread(
	IN IDispatcherQueueControllerStatics* thiz,
	OUT IDispatcherQueueController** out)
{
	if (out == NULL)
		return E_POINTER;

	HRESULT Result;
	IDispatcherQueue* queue;
	IDispatcherQueueController* dqc;

	dqc = (IDispatcherQueueController*)CoTaskMemAlloc(sizeof(IDispatcherQueueController));
	if (!dqc)
		return E_OUTOFMEMORY;

	dqc->lpVtbl = &CDispatcherQueueControllerVtbl;
	dqc->RefCount = 1;

	Result = DispatcherQueueCreateOnNewThread(&queue);
	if (FAILED(Result))
	{
		CoTaskMemFree(dqc);
		return Result;
	}

	dqc->queue = queue;

	*out = dqc;
	return S_OK;
}

IDispatcherQueueControllerStaticsVtbl CDispatcherQueueControllerStaticsVtbl = {
	DispatcherQueueControllerStatics_QueryInterface,
	DispatcherQueueControllerStatics_AddRef,
	DispatcherQueueControllerStatics_Release,

	DispatcherQueueControllerStatics_GetIids,
	DispatcherQueueControllerStatics_GetRuntimeClassName,
	DispatcherQueueControllerStatics_GetTrustLevel,

	DispatcherQueueControllerStatics_CreateOnDedicatedThread,
};

IDispatcherQueueControllerStatics CDispatcherQueueControllerStatics = {
	&CDispatcherQueueControllerStaticsVtbl
};

// System : Windows.System.IDispatcherQueueTimer
//

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_QueryInterface(
	IN	IDispatcherQueueTimer* This,
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
		IsEqualIID(RefIID, &IID_IDispatcherQueueTimer))
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

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueueTimer_AddRef(
	IN	IDispatcherQueueTimer* This)
{
	return InterlockedIncrement(&This->RefCount);
}

KXCOMAPI ULONG STDMETHODCALLTYPE DispatcherQueueTimer_Release(
	IN	IDispatcherQueueTimer* This)
{
	return DispatcherQueueTimerRelease(This);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_GetIids(
	IN	IDispatcherQueueTimer* This,
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
	Array[0] = IID_IDispatcherQueueTimer;

	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_GetRuntimeClassName(
	IN	IDispatcherQueueTimer* This,
	OUT	HSTRING* ClassName)
{
	PCWSTR Name = L"Windows.System.DispatcherQueueTimer";
	ASSERT(ClassName != NULL);
	return WindowsCreateString(Name, (ULONG)wcslen(Name), ClassName);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_GetTrustLevel(
	IN	IDispatcherQueueTimer* This,
	OUT	TrustLevel* Level)
{
	ASSERT(Level != NULL);
	*Level = BaseTrust;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_get_Interval(
	IN IDispatcherQueueTimer* thiz,
	OUT INT64* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = thiz->Interval;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_put_Interval(
	IN IDispatcherQueueTimer* thiz,
	IN INT64 in)
{
	if (in < 0)
		return E_INVALIDARG;

	thiz->Interval = (ULONG)in;
	return DispatcherQueueTimerUpdateParameter(thiz);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_get_IsRunning(
	IN IDispatcherQueueTimer* thiz,
	OUT BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = thiz->IsRunning;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_get_IsRepeating(
	IN IDispatcherQueueTimer* thiz,
	OUT BOOL* out)
{
	if (out == NULL)
		return E_POINTER;

	*out = thiz->IsRepeating;
	return S_OK;
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_put_IsRepeating(
	IN IDispatcherQueueTimer* thiz,
	IN BOOL in)
{
	if (in < 0)
		return E_INVALIDARG;

	thiz->IsRepeating = in;
	return DispatcherQueueTimerUpdateParameter(thiz);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_Start(
	IN IDispatcherQueueTimer* thiz)
{
	return DispatcherQueueTimerStart(thiz);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_Stop(
	IN IDispatcherQueueTimer* thiz)
{
	return DispatcherQueueTimerStop(thiz);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_add_Tick(
	IN	IDispatcherQueueTimer* This,
	IN	ITypedEventHandler* in,
	OUT	EventRegistrationToken* token)
{
	return EventHandlersAppend(&This->tick, in, token);
}

KXCOMAPI HRESULT STDMETHODCALLTYPE DispatcherQueueTimer_remove_Tick(
	IN	IDispatcherQueueTimer* This,
	IN	EventRegistrationToken token)
{
	return EventHandlersRemove(&This->tick, &token);
}

IDispatcherQueueTimerVtbl CDispatcherQueueTimerVtbl = {
	DispatcherQueueTimer_QueryInterface,
	DispatcherQueueTimer_AddRef,
	DispatcherQueueTimer_Release,

	DispatcherQueueTimer_GetIids,
	DispatcherQueueTimer_GetRuntimeClassName,
	DispatcherQueueTimer_GetTrustLevel,

	DispatcherQueueTimer_get_Interval,
	DispatcherQueueTimer_put_Interval,
	DispatcherQueueTimer_get_IsRunning,
	DispatcherQueueTimer_get_IsRepeating,
	DispatcherQueueTimer_put_IsRepeating,
	DispatcherQueueTimer_Start,
	DispatcherQueueTimer_Stop,
	DispatcherQueueTimer_add_Tick,
	DispatcherQueueTimer_remove_Tick
};