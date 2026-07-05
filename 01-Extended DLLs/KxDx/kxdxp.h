#pragma once

#include <KexComm.h>
#include <KexDll.h>
#include <KxDx.h>

EXTERN PKEX_PROCESS_DATA KexData;

typedef interface IMFMediaBuffer IMFMediaBuffer;
typedef interface IMFDXGIDeviceManager IMFDXGIDeviceManager;

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