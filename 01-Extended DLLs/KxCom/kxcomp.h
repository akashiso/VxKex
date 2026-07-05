#pragma once

#include <KexComm.h>
#include <KexDll.h>
#include <KxCom.h>

EXTERN PKEX_PROCESS_DATA KexData;

KXCOMAPI HRESULT STDMETHODCALLTYPE CActivationFactory_QueryInterface(
	IN	IActivationFactory	*This,
	IN	REFIID				RefIID,
	OUT	PPVOID				Object);

// dispatcher.c

HRESULT DispatcherQueueCreateOnThread(
	IN	DWORD dwThreadId,
	OUT	IDispatcherQueue** Object);

HRESULT DispatcherQueueCreateOnNewThread(
	OUT	IDispatcherQueue** Object);

IDispatcherQueue* DispatcherQueueGetForThread(
	IN	DWORD dwThreadId);

// async.c

IAsyncAction* CreateAsyncAction();

void AsyncActionCompleted(
	IAsyncAction*,
	HRESULT);

// winrt.c

FORCEINLINE VOID InitializeEventHandlers(
	OUT IEventHandlerEntry** list)
{
	*list = NULL;
}

HRESULT EventHandlersAppend(
	 IEventHandlerEntry**, 
	 ITypedEventHandler*, 
	 EventRegistrationToken*);

HRESULT EventHandlersRemove(
	 IEventHandlerEntry**, 
	 EventRegistrationToken*);

void EventHandlersRemoveAll(
	IEventHandlerEntry**);

void EventHandlersNotify(
	IEventHandlerEntry*, 
	IInspectable* thiz);