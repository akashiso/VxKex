#include "buildcfg.h"
#include "kxdxp.h"

//
// Roti-Poulet/DirectX12On7
//

VOID IID3D12Interop_WaitForGPUIdle(IID3D12Interop* interop, DWORD timeoutMs)
{
	UINT64 target;
	HRESULT hr;

	if (!interop || !interop->pFence || !interop->hFenceEvent || !interop->pQueue)
		return;

	target = ++interop->fenceValue;
	hr = interop->pQueue->lpVtbl->Signal(interop->pQueue, interop->pFence, target);
	if (FAILED(hr)) {
		KexLogWarningEvent(L"Signal failed 0x%08X : %s.\r\n"
						   L"Proceeding without GPU sync.",
						   hr, Win32ErrorAsString(hr));
		return;
	}

	if (interop->pFence->lpVtbl->GetCompletedValue(interop->pFence) < target) {
		hr = interop->pFence->lpVtbl->SetEventOnCompletion(interop->pFence, target, interop->hFenceEvent);

		if (FAILED(hr)) {
			KexLogWarningEvent(L"SetEventOnCompletion failed 0x%08X : %s.\r\n"
							   L"Proceeding without GPU sync.",
							   hr, Win32ErrorAsString(hr));
			return;
		}
		if (WaitForSingleObject(interop->hFenceEvent, timeoutMs) != WAIT_OBJECT_0) {
			KexLogWarningEvent(L"Timed out after %ums waiting for fence : %llu.\r\n"
							   L"Device may already be removed.",
							   timeoutMs, (unsigned long long)target);
			return;
		}
	}

	return;
}

VOID DestroyID3D12Interop(IID3D12Interop* interop)
{
	IID3D12Interop_WaitForGPUIdle(interop, 5000);

	for (UINT i = 0; i < interop->BufferCount; i++) {
		if (interop->pFrameBuffers[i]) {
			IUnknown_Release((IUnknown*)interop->pFrameBuffers[i]);
		}
	}

	if (interop->pFence)
		IUnknown_Release((IUnknown*)interop->pFence);

	if (interop->hFenceEvent && interop->hFenceEvent != INVALID_HANDLE_VALUE)
		CloseHandle(interop->hFenceEvent);

	if (interop->hFrameLatencyWaitable && interop->hFrameLatencyWaitable != INVALID_HANDLE_VALUE)
		CloseHandle(interop->hFrameLatencyWaitable);

	if (interop->pDownlevel)
		IUnknown_Release((IUnknown*)interop->pDownlevel);

	if (interop->pCmdAllocator)
		IUnknown_Release((IUnknown*)interop->pCmdAllocator);

	if (interop->pCmdList)
		IUnknown_Release((IUnknown*)interop->pCmdList);

	if (interop->pDevice)
		IUnknown_Release((IUnknown*)interop->pDevice);
}

HRESULT InitializeID3D12Interop(
	ID3D12CommandQueue* pQueue,
	const DXGI_SWAP_CHAIN_DESC1* pDesc,
	HWND hwnd,
	IID3D12Interop* interop)
{
	ID3D12CommandQueueDownlevel* pDownlevel = NULL;
	ID3D12CommandAllocator* pCmdAllocator = NULL;
	ID3D12CommandList* pCmdList = NULL;
	ID3D12Device* pDevice = NULL;

	if (!pDesc || pDesc->BufferCount == 0)
		return E_INVALIDARG;

	if (pDesc->BufferCount > D3D12_INTEROP_MAX_BUFFERS) {
		KexLogWarningEvent(L"Too many swap chain buffers are requested. (BufferCount = %d > %d)",
						   pDesc->BufferCount, D3D12_INTEROP_MAX_BUFFERS);

		return E_INVALIDARG;
	}

	HRESULT hr;

	hr = pQueue->lpVtbl->Parent.GetDevice((ID3D12DeviceChild*)pQueue, &IID_ID3D12Device, &pDevice);
	if (FAILED(hr)) goto Failed;

	hr = IUnknown_QueryInterface((IUnknown*)pQueue, &IID_ID3D12CommandQueueDownlevel, &pDownlevel);
	if (FAILED(hr)) goto Failed;

	hr = pDevice->lpVtbl->CreateCommandAllocator(pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, (void**)&pCmdAllocator);
	if (FAILED(hr)) goto Failed;

	hr = pDevice->lpVtbl->CreateCommandList(pDevice, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCmdAllocator, NULL, &IID_ID3D12GraphicsCommandList, (void**)&pCmdList);
	if (FAILED(hr)) goto Failed;

	pCmdList->lpVtbl->Close(pCmdList);

	ZeroMemory(interop, sizeof(IID3D12Interop));

	interop->BufferCount = pDesc->BufferCount;
	interop->Format = pDesc->Format;
	interop->Width = pDesc->Width;
	interop->Height = pDesc->Height;
	interop->hwnd = hwnd;

	interop->pDevice = pDevice;
	interop->pCmdAllocator = pCmdAllocator;
	interop->pCmdList = pCmdList;
	interop->pDownlevel = pDownlevel;
	interop->pQueue = pQueue;

	interop->maxFrameLatency = 1;
	interop->hFrameLatencyWaitable = CreateEventW(NULL, TRUE, TRUE, NULL);

	if (interop->Width == 0 || interop->Height == 0) {
		RECT wndRect;
		GetClientRect(hwnd, &wndRect);

		UINT width = wndRect.right - wndRect.left;
		UINT height = wndRect.bottom - wndRect.top;

		interop->Width = interop->Width == 0 ? width : interop->Width;
		interop->Height = interop->Height == 0 ? height : interop->Height;
	}

	hr = pDevice->lpVtbl->CreateFence(pDevice, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, &interop->pFence);
	if (FAILED(hr)) goto Failed;

	interop->fenceValue = 0;
	interop->hFenceEvent = CreateEventW(NULL, FALSE, FALSE, NULL);

	for (UINT i = 0; i < interop->BufferCount; i++) {
		D3D12_HEAP_PROPERTIES hp;
		ZeroMemory(&hp, sizeof(hp));
		hp.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		rd.Width = interop->Width;
		rd.Height = interop->Height;
		rd.DepthOrArraySize = 1;
		rd.MipLevels = 1;
		rd.Format = interop->Format;
		rd.SampleDesc.Count = 1;
		rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		rd.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE cv;
		ZeroMemory(&cv, sizeof(cv));
		cv.Format = interop->Format;

		ID3D12Resource* pRes = NULL;
		hr = pDevice->lpVtbl->CreateCommittedResource(pDevice, &hp, D3D12_HEAP_FLAG_NONE, &rd,
													  D3D12_RESOURCE_STATE_RENDER_TARGET, &cv, &IID_ID3D12Resource, &pRes);
		if (FAILED(hr)) goto Failed;

		interop->pFrameBuffers[i] = pRes;
	}

	return S_OK;

Failed:
	if (pDownlevel)
		IUnknown_Release((IUnknown*)pDownlevel);

	if (pCmdAllocator)
		IUnknown_Release((IUnknown*)pCmdAllocator);

	if (pCmdList)
		IUnknown_Release((IUnknown*)pCmdList);

	if (pDevice)
		IUnknown_Release((IUnknown*)pDevice);

	interop->pDevice = NULL;
	interop->pCmdAllocator = NULL;
	interop->pCmdList = NULL;
	interop->pDownlevel = NULL;

	DestroyID3D12Interop(interop);

	return hr;
}

HRESULT IID3D12Interop_GetBuffer(IID3D12Interop* interop, UINT Buffer, REFIID riid, void** pp)
{
	if (pp == NULL)
		return E_POINTER;

	if (Buffer >= interop->BufferCount)
		return DXGI_ERROR_INVALID_CALL;

	if (!interop->pFrameBuffers[Buffer])
		return E_FAIL;

	if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_ID3D12Resource)) {
		IUnknown_AddRef((IUnknown*)interop->pFrameBuffers[Buffer]);
		*pp = interop->pFrameBuffers[Buffer];
		return S_OK;
	}

	HRESULT hr = IUnknown_QueryInterface((IUnknown*)interop->pFrameBuffers[Buffer], riid, pp);
	return hr;
}

VOID IID3D12Interop_SignalFrameLatencyFallback(IID3D12Interop* interop)
{
	if (interop && interop->hFrameLatencyWaitable && interop->hFrameLatencyWaitable != INVALID_HANDLE_VALUE)
		SetEvent(interop->hFrameLatencyWaitable);
}

UINT IID3D12Interop_GetRefreshHz(HWND hwnd)
{
	HMONITOR hMon;
	MONITORINFOEXA mi;
	DEVMODEA dm;

	hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	if (!hMon) return 60;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);

	if (!GetMonitorInfoA(hMon, (MONITORINFO*)&mi))
		return 60;

	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	if (!EnumDisplaySettingsA(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm))
		return 60;

	if (dm.dmDisplayFrequency <= 1)
		return 60;

	return (UINT)dm.dmDisplayFrequency;
}

VOID IID3D12Interop_PaceFrame(IID3D12Interop* interop, UINT effectiveInterval)
{
	LARGE_INTEGER now;
	LONGLONG targetTicks, elapsed;

	if (effectiveInterval == 0)
		return;

	if (!interop->paceQpcFreq.QuadPart)
		QueryPerformanceFrequency(&interop->paceQpcFreq);

	if (!interop->paceRefreshHz)
		interop->paceRefreshHz = IID3D12Interop_GetRefreshHz(interop->hwnd);

	QueryPerformanceCounter(&now);
	if (!interop->paceHasLast) {
		interop->paceLastQpc = now;
		interop->paceHasLast = TRUE;
		return;
	}

	targetTicks = (interop->paceQpcFreq.QuadPart * (LONGLONG)effectiveInterval) / (LONGLONG)interop->paceRefreshHz;
	elapsed = now.QuadPart - interop->paceLastQpc.QuadPart;

	if (elapsed < targetTicks) {
		LONGLONG remainingTicks = targetTicks - elapsed;
		DWORD sleepMs = (DWORD)((remainingTicks * 1000) / interop->paceQpcFreq.QuadPart);

		if (sleepMs > 2)
			Sleep(sleepMs - 2);

		do {
			QueryPerformanceCounter(&now);
			elapsed = now.QuadPart - interop->paceLastQpc.QuadPart;
		} while (elapsed < targetTicks);
	}

	interop->paceLastQpc = now;
}

HRESULT IID3D12Interop_Present(IID3D12Interop* interop, UINT SyncInterval, UINT Flags)
{
	UINT idx = interop->presentIndex % interop->BufferCount;
	HRESULT hr;

	if (IsIconic(interop->hwnd)) {
		IID3D12Interop_SignalFrameLatencyFallback(interop);
		return S_OK;
	}

	if (!interop->pDownlevel || !interop->pCmdList || !interop->pFrameBuffers[idx]) {
		IID3D12Interop_SignalFrameLatencyFallback(interop);
		return E_FAIL;
	}

	interop->pCmdAllocator->lpVtbl->Reset(interop->pCmdAllocator);
	interop->pCmdList->lpVtbl->Reset(interop->pCmdList, interop->pCmdAllocator, NULL);

	IID3D12Interop_PaceFrame(interop, SyncInterval);

	if (interop->hFrameLatencyWaitable && interop->hFrameLatencyWaitable != INVALID_HANDLE_VALUE)
		ResetEvent(interop->hFrameLatencyWaitable);

	hr = interop->pDownlevel->lpVtbl->Present(interop->pDownlevel, interop->pCmdList,
											  interop->pFrameBuffers[idx],
											  interop->hwnd,
											  D3D12_DOWNLEVEL_PRESENT_FLAG_NONE);
	if (FAILED(hr)) {
		IID3D12Interop_SignalFrameLatencyFallback(interop);
		return hr;
	}

	IID3D12Interop_SignalFrameLatencyFallback(interop);
	interop->presentIndex++;

	return S_OK;
}


HRESULT STDMETHODCALLTYPE IID3D12Swapchain_QueryInterface(
	IID3D12Swapchain* This,
	REFIID riid,
	void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;

	*ppvObject = NULL;

	if (IsEqualIID(riid, &IID_IUnknown) ||
		IsEqualIID(riid, &IID_IDXGIObject) ||
		IsEqualIID(riid, &IID_IDXGIDeviceSubObject) ||
		IsEqualIID(riid, &IID_IDXGISwapChain) ||
		IsEqualIID(riid, &IID_IDXGISwapChain1) ||
		IsEqualIID(riid, &IID_IDXGISwapChain2) ||
		IsEqualIID(riid, &IID_IDXGISwapChain3) ||
		IsEqualIID(riid, &IID_IDXGISwapChain4)) {
		*ppvObject = This;
		InterlockedIncrement(&This->RefCount);

		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE IID3D12Swapchain_AddRef(
	IID3D12Swapchain* This)
{
	return InterlockedIncrement(&This->RefCount);
}

ULONG STDMETHODCALLTYPE IID3D12Swapchain_Release(
	IID3D12Swapchain* This)
{
	ULONG RefCount;
	RefCount = InterlockedDecrement(&This->RefCount);

	if (RefCount == 0) {
		DestroyID3D12Interop(&This->interop);

		IDXGIPrivateData* prev = This->pPrivateData;
		IDXGIPrivateData* cur = prev;

		while (prev != NULL) {
			cur = prev;
			prev = prev->Prev;

			if (cur->IsInterface)
				IUnknown_Release((IUnknown*)cur->Data);

			CoTaskMemFree(cur);
		}

		CoTaskMemFree(This);
	}
	return RefCount;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetPrivateData(
	IID3D12Swapchain* This,
	REFGUID guid,
	UINT data_size,
	const void* pData)
{
	IDXGIPrivateData* data = (IDXGIPrivateData*)CoTaskMemAlloc(sizeof(IDXGIPrivateData) + data_size);
	if (data == NULL)
		return E_OUTOFMEMORY;

	data->Data = ((char*)data) + sizeof(IDXGIPrivateData);
	data->Guid = guid;
	data->IsInterface = FALSE;
	data->DataSize = data_size;
	data->Prev = This->pPrivateData;

	CopyMemory(data->Data, pData, data_size);
	This->pPrivateData = data;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetPrivateDataInterface(
	IID3D12Swapchain* This,
	REFGUID guid,
	const IUnknown* object)
{
	IDXGIPrivateData* data = (IDXGIPrivateData*)CoTaskMemAlloc(sizeof(IDXGIPrivateData));
	if (data == NULL)
		return E_OUTOFMEMORY;

	IUnknown_AddRef((IUnknown*)object);

	data->Data = (IUnknown*)object;
	data->Guid = guid;
	data->IsInterface = TRUE;
	data->DataSize = sizeof(void*);
	data->Prev = This->pPrivateData;

	This->pPrivateData = data;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetPrivateData(
	IID3D12Swapchain* This,
	REFGUID guid,
	UINT* data_size,
	void* data)
{
	IDXGIPrivateData* prev = This->pPrivateData;

	while (prev != NULL) {
		if (IsEqualGUID(prev->Guid, guid))
			break;

		prev = prev->Prev;
	}

	if (prev == NULL)
		return DXGI_ERROR_NOT_FOUND;

	if (prev->DataSize > *data_size)
		return DXGI_ERROR_MORE_DATA;

	if (prev->IsInterface) {
		IUnknown_AddRef((IUnknown*)prev->Data);
		*((PPVOID)data) = prev->Data;

		return S_OK;
	}

	CopyMemory(data, prev->Data, prev->DataSize);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetParent(
	IID3D12Swapchain* This,
	REFIID riid,
	void** parent)
{
	return IUnknown_QueryInterface((IUnknown*)This->pParent, riid, parent);
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetDevice(
	IID3D12Swapchain* This,
	REFIID riid,
	void** device)
{
	return IUnknown_QueryInterface((IUnknown*)This->interop.pDevice, riid, device);
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_Present(
	IID3D12Swapchain* This,
	UINT sync_interval,
	UINT flags)
{
	return IID3D12Interop_Present(&This->interop, sync_interval, flags);
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetBuffer(
	IID3D12Swapchain* This,
	UINT buffer_idx,
	REFIID riid,
	void** surface)
{
	return IID3D12Interop_GetBuffer(&This->interop, buffer_idx, riid, surface);
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetFullscreenState(
	IID3D12Swapchain* This,
	BOOL fullscreen,
	IDXGIOutput* target)
{
	IID3D12Interop* interop = &This->interop;

	if (!fullscreen) {
		if (This->fsBorderlessActive) {
			SetWindowLongPtrW(interop->hwnd, GWL_STYLE, This->savedStyle);
			SetWindowLongPtrW(interop->hwnd, GWL_EXSTYLE, This->savedExStyle);
			SetWindowPos(interop->hwnd, HWND_NOTOPMOST,
						 This->savedRect.left, This->savedRect.top,
						 This->savedRect.right - This->savedRect.left,
						 This->savedRect.bottom - This->savedRect.top,
						 SWP_FRAMECHANGED | SWP_NOACTIVATE);
			This->fsBorderlessActive = FALSE;
		}
		return S_OK;
	}

	HMONITOR hMon = MonitorFromWindow(interop->hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);

	if (hMon && GetMonitorInfo(hMon, &mi)) {
		if (!This->fsBorderlessActive) {
			This->savedStyle = GetWindowLongPtrW(interop->hwnd, GWL_STYLE);
			This->savedExStyle = GetWindowLongPtrW(interop->hwnd, GWL_EXSTYLE);
			GetWindowRect(interop->hwnd, &This->savedRect);
		}

		LONG_PTR newStyle = This->savedStyle &
			~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		newStyle |= WS_POPUP;

		SetWindowLongPtrW(interop->hwnd, GWL_STYLE, newStyle);

		SetWindowPos(interop->hwnd, HWND_TOP,
					 mi.rcMonitor.left, mi.rcMonitor.top,
					 mi.rcMonitor.right - mi.rcMonitor.left,
					 mi.rcMonitor.bottom - mi.rcMonitor.top,
					 SWP_FRAMECHANGED | SWP_NOACTIVATE);
		This->fsBorderlessActive = TRUE;
		return S_OK;
	}

	return DXGI_ERROR_NOT_CURRENTLY_AVAILABLE;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetFullscreenState(
	IID3D12Swapchain* This,
	BOOL* fullscreen,
	IDXGIOutput** target)
{
	if (fullscreen) *fullscreen = This->fsBorderlessActive;
	if (target) *target = NULL;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetDesc(
	IID3D12Swapchain* This,
	DXGI_SWAP_CHAIN_DESC* desc)
{
	if (desc == NULL)
		return E_POINTER;

	ZeroMemory(desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc->BufferDesc.Width = This->interop.Width;
	desc->BufferDesc.Height = This->interop.Height;
	desc->BufferDesc.Format = This->interop.Format;
	desc->BufferDesc.RefreshRate.Numerator = 60;
	desc->BufferDesc.RefreshRate.Denominator = 1;
	desc->BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc->BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc->SampleDesc.Count = 1;
	desc->SampleDesc.Quality = 0;
	desc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc->BufferCount = This->interop.BufferCount;
	desc->OutputWindow = This->interop.hwnd;
	desc->Windowed = TRUE;
	desc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc->Flags = 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_ResizeBuffers(
	IID3D12Swapchain* This,
	UINT buffer_count,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	UINT flags)
{
	IID3D12Interop* interop = &This->interop;
	HRESULT hr;

	IID3D12Interop_WaitForGPUIdle(interop, 5000);

	UINT newCount = (buffer_count != 0) ? buffer_count : interop->BufferCount;
	UINT newWidth = (width != 0) ? width : interop->Width;
	UINT newHeight = (height != 0) ? height : interop->Height;
	UINT newFormat = (format != 0) ? format : interop->Format;

	for (UINT i = 0; i < interop->BufferCount; i++) {
		if (interop->pFrameBuffers[i]) {
			IUnknown_Release((IUnknown*)interop->pFrameBuffers[i]);
		}
	}

	interop->Width = newWidth;
	interop->Height = newHeight;
	interop->Format = newFormat;
	interop->BufferCount = newCount;

	for (UINT i = 0; i < interop->BufferCount; i++) {
		D3D12_HEAP_PROPERTIES hp;
		ZeroMemory(&hp, sizeof(hp));
		hp.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		rd.Width = interop->Width;
		rd.Height = interop->Height;
		rd.DepthOrArraySize = 1;
		rd.MipLevels = 1;
		rd.Format = interop->Format;
		rd.SampleDesc.Count = 1;
		rd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		rd.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE cv;
		ZeroMemory(&cv, sizeof(cv));
		cv.Format = interop->Format;

		ID3D12Resource* pRes = NULL;
		hr = interop->pDevice->lpVtbl->CreateCommittedResource(interop->pDevice, &hp, D3D12_HEAP_FLAG_NONE, &rd,
															   D3D12_RESOURCE_STATE_RENDER_TARGET, &cv, &IID_ID3D12Resource, &pRes);
		if (FAILED(hr)) {
			for (UINT j = 0; j < i; j++) {
				if (interop->pFrameBuffers[j]) {
					IUnknown_Release((IUnknown*)interop->pFrameBuffers[j]);
					interop->pFrameBuffers[j] = NULL;
				}
			}

			return hr;
		}

		interop->pFrameBuffers[i] = pRes;
	}

	interop->presentIndex = 0;
	return hr;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_ResizeTarget(
	IID3D12Swapchain* This,
	const DXGI_MODE_DESC* mode)
{
	if (mode && This->interop.hwnd && (mode->Width > 0) && (mode->Height > 0)) {
		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = (LONG)mode->Width;
		rc.bottom = (LONG)mode->Height;

		AdjustWindowRectEx(&rc, (DWORD)GetWindowLongPtrW(This->interop.hwnd, GWL_STYLE),
						   FALSE, (DWORD)GetWindowLongPtrW(This->interop.hwnd, GWL_EXSTYLE));

		SetWindowPos(This->interop.hwnd, NULL, 0, 0,
					 rc.right - rc.left, rc.bottom - rc.top,
					 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetContainingOutput(
	IID3D12Swapchain* This,
	IDXGIOutput** output)
{
	HMONITOR hMon = MonitorFromWindow(This->interop.hwnd, MONITOR_DEFAULTTOPRIMARY);
	IDXGIFactory1* pRealFactory = NULL;
	HRESULT hr = E_FAIL;
	hr = CreateDXGIFactory1(&IID_IDXGIFactory1, (void**)&pRealFactory);

	if (SUCCEEDED(hr) && pRealFactory) {
		IDXGIAdapter1* pAdapter = NULL;
		IDXGIOutput* pFoundOutput = NULL;
		UINT i = 0;

		while (!pFoundOutput && IDXGIFactory1_EnumAdapters1(pRealFactory, i, &pAdapter) == S_OK) {
			IDXGIOutput* pOut = NULL;
			UINT j = 0;

			while (IDXGIAdapter1_EnumOutputs(pAdapter, j, &pOut) == S_OK) {
				DXGI_OUTPUT_DESC outputDesc;

				if (SUCCEEDED(IDXGIOutput_GetDesc(pOut, &outputDesc)) && (outputDesc.Monitor == hMon || !pFoundOutput)) {
					if (pFoundOutput)
						IDXGIOutput_Release(pFoundOutput);

					IDXGIOutput_AddRef(pOut);
					pFoundOutput = pOut;

					if (outputDesc.Monitor == hMon) {
						IDXGIOutput_Release(pOut);
						break;
					}
				}

				IDXGIOutput_Release(pOut);
				j++;
			}

			IDXGIAdapter_Release(pAdapter);
			i++;
		}
		IDXGIFactory1_Release(pRealFactory);

		if (pFoundOutput) {
			*output = pFoundOutput;
			IDXGIOutput_Release(pFoundOutput);
		}
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetFrameStatistics(
	IID3D12Swapchain* This,
	DXGI_FRAME_STATISTICS* stats)
{
	if (stats == NULL)
		return E_POINTER;

	ZeroMemory(stats, sizeof(DXGI_FRAME_STATISTICS));
	stats->PresentCount = This->interop.presentIndex;
	stats->PresentRefreshCount = This->interop.presentIndex;
	stats->SyncRefreshCount = This->interop.presentIndex;
	stats->SyncQPCTime = This->interop.lastPresentQpc;
	stats->SyncGPUTime = This->interop.lastPresentQpc;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetLastPresentCount(
	IID3D12Swapchain* This,
	UINT* last_present_count)
{
	if (last_present_count == NULL)
		return E_POINTER;

	*last_present_count = This->interop.presentIndex;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetDesc1(
	IID3D12Swapchain* This,
	DXGI_SWAP_CHAIN_DESC1* pDesc)
{
	if (pDesc == NULL)
		return E_POINTER;

	ZeroMemory(pDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	pDesc->Width = This->interop.Width;
	pDesc->Height = This->interop.Height;
	pDesc->Format = This->interop.Format;
	pDesc->SampleDesc.Count = 1;
	pDesc->SampleDesc.Quality = 0;
	pDesc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	pDesc->BufferCount = This->interop.BufferCount;
	pDesc->Flags = 0;
	pDesc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	pDesc->AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	pDesc->Scaling = DXGI_SCALING_STRETCH;
	pDesc->Stereo = FALSE;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetFullscreenDesc(
	IID3D12Swapchain* This,
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pDesc)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetHwnd(
	IID3D12Swapchain* This,
	HWND* pHwnd)
{
	if (pHwnd == NULL)
		return E_POINTER;

	*pHwnd = This->interop.hwnd;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetCoreWindow(
	IID3D12Swapchain* This,
	REFIID refiid,
	void** ppUnk)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_Present1(
	IID3D12Swapchain* This,
	UINT SyncInterval,
	UINT PresentFlags,
	const DXGI_PRESENT_PARAMETERS* pPresentParameters)
{
	return IID3D12Swapchain_Present(This, SyncInterval, PresentFlags);
}

BOOL STDMETHODCALLTYPE IID3D12Swapchain_IsTemporaryMonoSupported(
	IID3D12Swapchain* This)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetRestrictToOutput(
	IID3D12Swapchain* This,
	IDXGIOutput** ppRestrictToOutput)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetBackgroundColor(
	IID3D12Swapchain* This,
	const DXGI_RGBA* pColor)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetBackgroundColor(
	IID3D12Swapchain* This,
	DXGI_RGBA* pColor)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetRotation(
	IID3D12Swapchain* This,
	DXGI_MODE_ROTATION Rotation)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetRotation(
	IID3D12Swapchain* This,
	DXGI_MODE_ROTATION* pRotation)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetSourceSize(
	IID3D12Swapchain* This,
	UINT Width,
	UINT Height)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetSourceSize(
	IID3D12Swapchain* This,
	OUT  UINT* pWidth,
	OUT  UINT* pHeight)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetMaximumFrameLatency(
	IID3D12Swapchain* This,
	UINT MaxLatency)
{
	if (MaxLatency == 0 || MaxLatency > 16)
		return E_INVALIDARG;

	This->interop.maxFrameLatency = MaxLatency;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetMaximumFrameLatency(
	IID3D12Swapchain* This,
	OUT  UINT* pMaxLatency)
{
	if (!pMaxLatency)
		return E_POINTER;

	*pMaxLatency = This->interop.maxFrameLatency;
	return S_OK;
}

HANDLE STDMETHODCALLTYPE IID3D12Swapchain_GetFrameLatencyWaitableObject(
	IID3D12Swapchain* This)
{
	return This->interop.hFrameLatencyWaitable;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetMatrixTransform(
	IID3D12Swapchain* This,
	const DXGI_MATRIX_3X2_F* pMatrix)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_GetMatrixTransform(
	IID3D12Swapchain* This,
	OUT  DXGI_MATRIX_3X2_F* pMatrix)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

UINT STDMETHODCALLTYPE IID3D12Swapchain_GetCurrentBackBufferIndex(
	IID3D12Swapchain* This)
{
	return This->interop.presentIndex % This->interop.BufferCount;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_CheckColorSpaceSupport(
	IID3D12Swapchain* This,
	IN  DXGI_COLOR_SPACE_TYPE ColorSpace,
	OUT  UINT* pColorSpaceSupport)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetColorSpace1(
	IID3D12Swapchain* This,
	IN  DXGI_COLOR_SPACE_TYPE ColorSpace)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_ResizeBuffers1(
	IID3D12Swapchain* This,
	IN  UINT BufferCount,
	IN  UINT Width,
	IN  UINT Height,
	IN  DXGI_FORMAT Format,
	IN  UINT SwapChainFlags,
	IN  const UINT* pCreationNodeMask,
	IN  IUnknown* const* ppPresentQueue)
{
	return IID3D12Swapchain_ResizeBuffers(This, BufferCount, Width, Height, Format, SwapChainFlags);
}

HRESULT STDMETHODCALLTYPE IID3D12Swapchain_SetHDRMetaData(
	IID3D12Swapchain* This,
	IN  UINT Type,
	IN  UINT Size,
	IN  void* pMetaData)
{
	KexLogWarningEvent(L"Unimplemented function %s called", __FUNCTIONW__);
	return E_NOTIMPL;
}

PVOID IID3D12SwapchainVtbl[] = {
	IID3D12Swapchain_QueryInterface,
	IID3D12Swapchain_AddRef,
	IID3D12Swapchain_Release,
	IID3D12Swapchain_SetPrivateData,
	IID3D12Swapchain_SetPrivateDataInterface,
	IID3D12Swapchain_GetPrivateData,
	IID3D12Swapchain_GetParent,
	IID3D12Swapchain_GetDevice,
	IID3D12Swapchain_Present,
	IID3D12Swapchain_GetBuffer,
	IID3D12Swapchain_SetFullscreenState,
	IID3D12Swapchain_GetFullscreenState,
	IID3D12Swapchain_GetDesc,
	IID3D12Swapchain_ResizeBuffers,
	IID3D12Swapchain_ResizeTarget,
	IID3D12Swapchain_GetContainingOutput,
	IID3D12Swapchain_GetFrameStatistics,
	IID3D12Swapchain_GetLastPresentCount,
	IID3D12Swapchain_GetDesc1,
	IID3D12Swapchain_GetFullscreenDesc,
	IID3D12Swapchain_GetHwnd,
	IID3D12Swapchain_GetCoreWindow,
	IID3D12Swapchain_Present1,
	IID3D12Swapchain_IsTemporaryMonoSupported,
	IID3D12Swapchain_GetRestrictToOutput,
	IID3D12Swapchain_SetBackgroundColor,
	IID3D12Swapchain_GetBackgroundColor,
	IID3D12Swapchain_SetRotation,
	IID3D12Swapchain_GetRotation,
	IID3D12Swapchain_SetSourceSize,
	IID3D12Swapchain_GetSourceSize,
	IID3D12Swapchain_SetMaximumFrameLatency,
	IID3D12Swapchain_GetMaximumFrameLatency,
	IID3D12Swapchain_GetFrameLatencyWaitableObject,
	IID3D12Swapchain_SetMatrixTransform,
	IID3D12Swapchain_GetMatrixTransform,
	IID3D12Swapchain_GetCurrentBackBufferIndex,
	IID3D12Swapchain_CheckColorSpaceSupport,
	IID3D12Swapchain_SetColorSpace1,
	IID3D12Swapchain_ResizeBuffers1,
	IID3D12Swapchain_SetHDRMetaData
};

HRESULT CreateIID3D12Swapchain(
	ID3D12CommandQueue* pQueue,
	const DXGI_SWAP_CHAIN_DESC1* pDesc,
	HWND hwnd,
	IDXGIFactory* factory,
	IID3D12Swapchain** out)
{
	IID3D12Swapchain* swapchain = (IID3D12Swapchain*)CoTaskMemAlloc(sizeof(IID3D12Swapchain));
	if (swapchain == NULL)
		return E_OUTOFMEMORY;

	ZeroMemory(swapchain, sizeof(IID3D12Swapchain));
	swapchain->lpVtbl = IID3D12SwapchainVtbl;
	swapchain->RefCount = 1;

	HRESULT hr = InitializeID3D12Interop(pQueue, pDesc, hwnd, &swapchain->interop);

	if (FAILED(hr)) {
		KexLogErrorEvent(L"Failed to initialize DXGISwapChain for D3D12."
						 L"HRESULT error code: 0x%08lx: %s",
						 hr, Win32ErrorAsString(hr));

		CoTaskMemFree(swapchain);
		swapchain = NULL;
	}
	else {
		KexLogInformationEvent(L"Successfully initialized DXGISwapChain for D3D12.");
	}

	*out = swapchain;
	return hr;
}