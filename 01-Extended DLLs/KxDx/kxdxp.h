#pragma once

#include <KexComm.h>
#include <KexDll.h>
#include <KxDx.h>

EXTERN PKEX_PROCESS_DATA KexData;

typedef interface IMFMediaBuffer IMFMediaBuffer;
typedef interface IMFDXGIDeviceManager IMFDXGIDeviceManager;

#define D3D12_INTEROP_MAX_BUFFERS 4

typedef struct IID3D12Interop
{
	ID3D12Device* pDevice;
	ID3D12CommandQueue* pQueue;
	ID3D12CommandQueueDownlevel* pDownlevel;
	ID3D12CommandAllocator* pCmdAllocator;
	ID3D12CommandList* pCmdList;
	UINT  BufferCount;
	UINT  Width, Height;
	DXGI_FORMAT Format;

	UINT  presentIndex;
	LARGE_INTEGER lastPresentQpc;
	HWND  hwnd;

	UINT          paceRefreshHz;
	LARGE_INTEGER paceQpcFreq;
	LARGE_INTEGER paceLastQpc;
	BOOL          paceHasLast;

	ID3D12Resource* pFrameBuffers[D3D12_INTEROP_MAX_BUFFERS];

	ID3D12Fence* pFence;
	HANDLE  hFenceEvent;
	HANDLE  hFrameLatencyWaitable;
	UINT64  fenceValue;
	UINT    maxFrameLatency;
} IID3D12Interop;

typedef struct IDXGIPrivateData IDXGIPrivateData;

typedef struct IDXGIPrivateData
{
	IDXGIPrivateData* Prev;
	const GUID* Guid;
	void* Data;

	UINT DataSize;
	BOOL IsInterface;
} IDXGIPrivateData;

typedef struct IID3D12Swapchain
{
	PPVOID lpVtbl;
	ULONG RefCount;

	IDXGIFactory* pParent;
	IDXGIPrivateData* pPrivateData;

	IID3D12Interop interop;

	BOOL          fsBorderlessActive;
	LONG_PTR      savedStyle;
	LONG_PTR      savedExStyle;
	RECT          savedRect;
} IID3D12Swapchain;

HRESULT WINAPI CreateIDXGIFactoryMedia(
	OUT	PPVOID	FactoryMedia);

HRESULT WINAPI CreateDXGIFactory2(
	IN	ULONG	Flags,
	IN	REFIID	RefIID,
	OUT	PPVOID	Factory);

IDXGIAdapter1* WrapIDXGIAdapter(IDXGIAdapter1* pAdapt);
IDXGISwapChain* WrapIDXGISwapchain(IDXGISwapChain* pAdapt);
IDXGIOutput* WrapIDXGIOutput(IDXGIOutput* pAdapt);
IDXGIDevice* WrapIDXGIDevice(IDXGIDevice* pAdapt);

ID3D11DeviceContext* WrapID3D11DeviceContext(ID3D11DeviceContext* pAdapt);

IIDXGIFactory2To7* CreateIIDXGIFactory2To7(IDXGIFactory2* pFactory2, UINT CreationFlags);

HRESULT CreateIID3D12Swapchain(
	ID3D12CommandQueue* pQueue,
	const DXGI_SWAP_CHAIN_DESC1* pDesc,
	HWND hwnd,
	IDXGIFactory* factory,
	IID3D12Swapchain** out);