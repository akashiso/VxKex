#pragma once
#include <KexComm.h>
#include <dxgi.h>
#include <d2d1.h>
#include <dxgitype.h>
#include <d3d9types.h>
#include <d3d11.h>

//
// Structure & enum
//

typedef ID3D11DeviceContext ID3D11DeviceContext1;
typedef ID3D11DeviceContext ID3D11DeviceContext2;

typedef interface ID3D11Texture2D1 ID3D11Texture2D1;
typedef interface ID3D11Texture3D1 ID3D11Texture3D1;
typedef interface ID3D11RasterizerState2 ID3D11RasterizerState2;
typedef interface ID3D11ShaderResourceView1 ID3D11ShaderResourceView1;
typedef interface ID3D11UnorderedAccessView1 ID3D11UnorderedAccessView1;
typedef interface ID3D11RenderTargetView1 ID3D11RenderTargetView1;
typedef interface ID3D11Query1 ID3D11Query1;
typedef interface ID3D11DeviceContext3 ID3D11DeviceContext3;
typedef interface ID3D11Fence ID3D11Fence;
typedef interface ID3D11BlendState1 ID3D11BlendState1;
typedef interface ID3D11RasterizerState1 ID3D11RasterizerState1;
typedef interface ID3DDeviceContextState ID3DDeviceContextState;

typedef struct D3D11_PACKED_MIP_DESC D3D11_PACKED_MIP_DESC;
typedef struct D3D11_TILE_SHAPE D3D11_TILE_SHAPE;
typedef struct D3D11_SUBRESOURCE_TILING D3D11_SUBRESOURCE_TILING;
typedef struct D3D11_TEXTURE2D_DESC1 D3D11_TEXTURE2D_DESC1;
typedef struct D3D11_TEXTURE3D_DESC1 D3D11_TEXTURE3D_DESC1;
typedef struct D3D11_RASTERIZER_DESC2 D3D11_RASTERIZER_DESC2;
typedef struct D3D11_SHADER_RESOURCE_VIEW_DESC1 D3D11_SHADER_RESOURCE_VIEW_DESC1;
typedef struct D3D11_UNORDERED_ACCESS_VIEW_DESC1 D3D11_UNORDERED_ACCESS_VIEW_DESC1;
typedef struct D3D11_RENDER_TARGET_VIEW_DESC1 D3D11_RENDER_TARGET_VIEW_DESC1;
typedef struct D3D11_QUERY_DESC1 D3D11_QUERY_DESC1;
typedef struct D3D11_BLEND_DESC1 D3D11_BLEND_DESC1;
typedef struct D3D11_RASTERIZER_DESC1 D3D11_RASTERIZER_DESC1;

typedef struct D3D11_TILED_RESOURCE_COORDINATE D3D11_TILED_RESOURCE_COORDINATE;
typedef struct D3D11_TILE_REGION_SIZE D3D11_TILE_REGION_SIZE;

typedef enum _DWRITE_GLYPH_IMAGE_FORMATS
{
	DWRITE_GLYPH_IMAGE_FORMATS_NONE = 0x00000000,
	DWRITE_GLYPH_IMAGE_FORMATS_TRUETYPE = 0x00000001,
	DWRITE_GLYPH_IMAGE_FORMATS_CFF = 0x00000002,
	DWRITE_GLYPH_IMAGE_FORMATS_COLR = 0x00000004,
	DWRITE_GLYPH_IMAGE_FORMATS_SVG = 0x00000008,
	DWRITE_GLYPH_IMAGE_FORMATS_PNG = 0x00000010,
	DWRITE_GLYPH_IMAGE_FORMATS_JPEG = 0x00000020,
	DWRITE_GLYPH_IMAGE_FORMATS_TIFF = 0x00000040,
	DWRITE_GLYPH_IMAGE_FORMATS_PREMULTIPLIED_B8G8R8A8 = 0x00000080,
} TYPEDEF_TYPE_NAME(DWRITE_GLYPH_IMAGE_FORMATS);

typedef enum _DXGI_FORMAT
{
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM = 115,
	DXGI_FORMAT_P208 = 130,
	DXGI_FORMAT_V208 = 131,
	DXGI_FORMAT_V408 = 132,
	DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
	DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
} TYPEDEF_TYPE_NAME(DXGI_FORMAT);

typedef enum _DXGI_COLOR_SPACE_TYPE
{
	DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709 = 0,
	DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709 = 1,
	DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709 = 2,
	DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P2020 = 3,
	DXGI_COLOR_SPACE_RESERVED = 4,
	DXGI_COLOR_SPACE_YCBCR_FULL_G22_NONE_P709_X601 = 5,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P601 = 6,
	DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P601 = 7,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P709 = 8,
	DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P709 = 9,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P2020 = 10,
	DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P2020 = 11,
	DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 = 12,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_G2084_LEFT_P2020 = 13,
	DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020 = 14,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_TOPLEFT_P2020 = 15,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_G2084_TOPLEFT_P2020 = 16,
	DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P2020 = 17,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_GHLG_TOPLEFT_P2020 = 18,
	DXGI_COLOR_SPACE_YCBCR_FULL_GHLG_TOPLEFT_P2020 = 19,
	DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P709 = 20,
	DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P2020 = 21,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P709 = 22,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P2020 = 23,
	DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_TOPLEFT_P2020 = 24,
	DXGI_COLOR_SPACE_CUSTOM = 0xFFFFFFFF
} TYPEDEF_TYPE_NAME(DXGI_COLOR_SPACE_TYPE);

typedef enum _DXGI_SCALING
{
	DXGI_SCALING_STRETCH = 0,
	DXGI_SCALING_NONE = 1,
	DXGI_SCALING_ASPECT_RATIO_STRETCH = 2
} TYPEDEF_TYPE_NAME(DXGI_SCALING);

typedef enum _DXGI_ALPHA_MODE
{
	DXGI_ALPHA_MODE_UNSPECIFIED = 0,
	DXGI_ALPHA_MODE_PREMULTIPLIED = 1,
	DXGI_ALPHA_MODE_STRAIGHT = 2,
	DXGI_ALPHA_MODE_IGNORE = 3,
	DXGI_ALPHA_MODE_FORCE_DWORD = 0xffffffff
} TYPEDEF_TYPE_NAME(DXGI_ALPHA_MODE);

typedef	enum _DXGI_SWAP_EFFECT
{
	DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL = 2, // win8
	DXGI_SWAP_EFFECT_FLIP_DISCARD = 3 // win10
} TYPEDEF_TYPE_NAME(DXGI_SWAP_EFFECT);

typedef enum _DXGI_FEATURE
{
	DXGI_FEATURE_PRESENT_ALLOW_TEARING = 0
} TYPEDEF_TYPE_NAME(DXGI_FEATURE);

typedef enum _DXGI_GPU_PREFERENCE
{
	DXGI_GPU_PREFERENCE_UNSPECIFIED = 0,
	DXGI_GPU_PREFERENCE_MINIMUM_POWER = (DXGI_GPU_PREFERENCE_UNSPECIFIED + 1),
	DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE = (DXGI_GPU_PREFERENCE_MINIMUM_POWER + 1)
} TYPEDEF_TYPE_NAME(DXGI_GPU_PREFERENCE);

typedef enum _DXGI_GRAPHICS_PREEMPTION_GRANULARITY
{
	DXGI_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY = 0,
	DXGI_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY = 1,
	DXGI_GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY = 2,
	DXGI_GRAPHICS_PREEMPTION_PIXEL_BOUNDARY = 3,
	DXGI_GRAPHICS_PREEMPTION_INSTRUCTION_BOUNDARY = 4
} TYPEDEF_TYPE_NAME(DXGI_GRAPHICS_PREEMPTION_GRANULARITY);

typedef enum _DXGI_COMPUTE_PREEMPTION_GRANULARITY
{
	DXGI_COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY = 0,
	DXGI_COMPUTE_PREEMPTION_DISPATCH_BOUNDARY = 1,
	DXGI_COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY = 2,
	DXGI_COMPUTE_PREEMPTION_THREAD_BOUNDARY = 3,
	DXGI_COMPUTE_PREEMPTION_INSTRUCTION_BOUNDARY = 4
} TYPEDEF_TYPE_NAME(DXGI_COMPUTE_PREEMPTION_GRANULARITY);

typedef enum _DXGI_MEMORY_SEGMENT_GROUP
{
	DXGI_MEMORY_SEGMENT_GROUP_LOCAL = 0,
	DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL = 1
} TYPEDEF_TYPE_NAME(DXGI_MEMORY_SEGMENT_GROUP);

typedef enum _DXGI_ADAPTER_FLAG3
{
	DXGI_ADAPTER_FLAG3_NONE = 0,
	DXGI_ADAPTER_FLAG3_REMOTE = 1,
	DXGI_ADAPTER_FLAG3_SOFTWARE = 2,
	DXGI_ADAPTER_FLAG3_ACG_COMPATIBLE = 4,
	DXGI_ADAPTER_FLAG3_SUPPORT_MONITORED_FENCES = 8,
	DXGI_ADAPTER_FLAG3_SUPPORT_NON_MONITORED_FENCES = 0x10,
	DXGI_ADAPTER_FLAG3_KEYED_MUTEX_CONFORMANCE = 0x20,
	DXGI_ADAPTER_FLAG3_FORCE_DWORD = 0xffffffff
} TYPEDEF_TYPE_NAME(DXGI_ADAPTER_FLAG3);

typedef enum _DXGI_OFFER_RESOURCE_FLAGS
{
	DXGI_OFFER_RESOURCE_FLAG_ALLOW_DECOMMIT = 0x1
} 	DXGI_OFFER_RESOURCE_FLAGS;

typedef enum _DXGI_OFFER_RESOURCE_PRIORITY
{
	DXGI_OFFER_RESOURCE_PRIORITY_LOW = 1,
	DXGI_OFFER_RESOURCE_PRIORITY_NORMAL = (DXGI_OFFER_RESOURCE_PRIORITY_LOW + 1),
	DXGI_OFFER_RESOURCE_PRIORITY_HIGH = (DXGI_OFFER_RESOURCE_PRIORITY_NORMAL + 1)
} 	DXGI_OFFER_RESOURCE_PRIORITY;

typedef enum _DXGI_RECLAIM_RESOURCE_RESULTS
{
	DXGI_RECLAIM_RESOURCE_RESULT_OK = 0,
	DXGI_RECLAIM_RESOURCE_RESULT_DISCARDED = 1,
	DXGI_RECLAIM_RESOURCE_RESULT_NOT_COMMITTED = 2
} 	DXGI_RECLAIM_RESOURCE_RESULTS;

typedef enum D3D11_FENCE_FLAG
{
	D3D11_FENCE_FLAG_NONE = 0,
	D3D11_FENCE_FLAG_SHARED = 0x2,
	D3D11_FENCE_FLAG_SHARED_CROSS_ADAPTER = 0x4,
	D3D11_FENCE_FLAG_NON_MONITORED = 0x8
} 	D3D11_FENCE_FLAG;

typedef enum D3D11_CONTEXT_TYPE
{
	D3D11_CONTEXT_TYPE_ALL = 0,
	D3D11_CONTEXT_TYPE_3D = 1,
	D3D11_CONTEXT_TYPE_COMPUTE = 2,
	D3D11_CONTEXT_TYPE_COPY = 3,
	D3D11_CONTEXT_TYPE_VIDEO = 4
} 	D3D11_CONTEXT_TYPE;

typedef struct _DXGI_SWAP_CHAIN_DESC1
{
	UINT Width;
	UINT Height;
	DXGI_FORMAT Format;
	BOOL Stereo;
	DXGI_SAMPLE_DESC SampleDesc;
	DXGI_USAGE BufferUsage;
	UINT BufferCount;
	DXGI_SCALING Scaling;
	DXGI_SWAP_EFFECT SwapEffect;
	DXGI_ALPHA_MODE AlphaMode;
	UINT Flags;
} TYPEDEF_TYPE_NAME(DXGI_SWAP_CHAIN_DESC1);

typedef struct _DXGI_SWAP_CHAIN_FULLSCREEN_DESC
{
	DXGI_RATIONAL RefreshRate;
	DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
	DXGI_MODE_SCALING Scaling;
	BOOL Windowed;
} TYPEDEF_TYPE_NAME(DXGI_SWAP_CHAIN_FULLSCREEN_DESC);

typedef struct _DXGI_PRESENT_PARAMETERS
{
	UINT DirtyRectsCount;
	RECT* pDirtyRects;
	RECT* pScrollRect;
	POINT* pScrollOffset;
} TYPEDEF_TYPE_NAME(DXGI_PRESENT_PARAMETERS);

typedef struct _DXGI_ADAPTER_DESC2
{
	WCHAR Description[128];
	UINT VendorId;
	UINT DeviceId;
	UINT SubSysId;
	UINT Revision;
	SIZE_T DedicatedVideoMemory;
	SIZE_T DedicatedSystemMemory;
	SIZE_T SharedSystemMemory;
	LUID AdapterLuid;
	UINT Flags;
	DXGI_GRAPHICS_PREEMPTION_GRANULARITY GraphicsPreemptionGranularity;
	DXGI_COMPUTE_PREEMPTION_GRANULARITY ComputePreemptionGranularity;
} TYPEDEF_TYPE_NAME(DXGI_ADAPTER_DESC2);

typedef struct _DXGI_ADAPTER_DESC3
{
	WCHAR Description[128];
	UINT VendorId;
	UINT DeviceId;
	UINT SubSysId;
	UINT Revision;
	SIZE_T DedicatedVideoMemory;
	SIZE_T DedicatedSystemMemory;
	SIZE_T SharedSystemMemory;
	LUID AdapterLuid;
	DXGI_ADAPTER_FLAG3 Flags;
	DXGI_GRAPHICS_PREEMPTION_GRANULARITY GraphicsPreemptionGranularity;
	DXGI_COMPUTE_PREEMPTION_GRANULARITY ComputePreemptionGranularity;
} TYPEDEF_TYPE_NAME(DXGI_ADAPTER_DESC3);

typedef struct DXGI_QUERY_VIDEO_MEMORY_INFO
{
	UINT64 Budget;
	UINT64 CurrentUsage;
	UINT64 AvailableForReservation;
	UINT64 CurrentReservation;
} TYPEDEF_TYPE_NAME(DXGI_QUERY_VIDEO_MEMORY_INFO);

typedef D3DCOLORVALUE DXGI_RGBA;

typedef struct DXGI_MATRIX_3X2_F
{
	FLOAT _11;
	FLOAT _12;
	FLOAT _21;
	FLOAT _22;
	FLOAT _31;
	FLOAT _32;
} 	DXGI_MATRIX_3X2_F;

typedef struct DXGI_OUTPUT_DESC1
{
	WCHAR DeviceName[32];
	RECT DesktopCoordinates;
	BOOL AttachedToDesktop;
	DXGI_MODE_ROTATION Rotation;
	HMONITOR Monitor;
	UINT BitsPerColor;
	DXGI_COLOR_SPACE_TYPE ColorSpace;
	FLOAT RedPrimary[2];
	FLOAT GreenPrimary[2];
	FLOAT BluePrimary[2];
	FLOAT WhitePoint[2];
	FLOAT MinLuminance;
	FLOAT MaxLuminance;
	FLOAT MaxFullFrameLuminance;
} 	DXGI_OUTPUT_DESC1;

//
// IDXGIFactory2
//

typedef interface IDXGIOutputDuplication IDXGIOutputDuplication;
typedef interface IDXGIFactory2 IDXGIFactory2;

typedef struct IDXGIFactory2Vtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			IDXGIFactory2* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			_Out_ void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		IDXGIFactory2* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		IDXGIFactory2* This);

	HRESULT(STDMETHODCALLTYPE* SetPrivateData)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  REFGUID Name,
		/* [in] */ UINT DataSize,
		/* [annotation][in] */
		_In_ const void* pData);

	HRESULT(STDMETHODCALLTYPE* SetPrivateDataInterface)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  REFGUID Name,
		/* [annotation][in] */
		_In_opt_  const IUnknown* pUnknown);

	HRESULT(STDMETHODCALLTYPE* GetPrivateData)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  REFGUID Name,
		/* [annotation][out][in] */
		_Inout_  UINT* pDataSize,
		/* [annotation][out] */
		_Out_ void* pData);

	HRESULT(STDMETHODCALLTYPE* GetParent)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  REFIID riid,
		/* [annotation][retval][out] */
		_Out_ void** ppParent);

	HRESULT(STDMETHODCALLTYPE* EnumAdapters)(
		IDXGIFactory2* This,
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		_Out_ IDXGIAdapter** ppAdapter);

	HRESULT(STDMETHODCALLTYPE* MakeWindowAssociation)(
		IDXGIFactory2* This,
		HWND WindowHandle,
		UINT Flags);

	HRESULT(STDMETHODCALLTYPE* GetWindowAssociation)(
		IDXGIFactory2* This,
		/* [annotation][out] */
		_Out_  HWND* pWindowHandle);

	HRESULT(STDMETHODCALLTYPE* CreateSwapChain)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  DXGI_SWAP_CHAIN_DESC* pDesc,
		/* [annotation][out] */
		_Out_ IDXGISwapChain** ppSwapChain);

	HRESULT(STDMETHODCALLTYPE* CreateSoftwareAdapter)(
		IDXGIFactory2* This,
		/* [in] */ HMODULE Module,
		/* [annotation][out] */
		_Out_ IDXGIAdapter** ppAdapter);

	HRESULT(STDMETHODCALLTYPE* EnumAdapters1)(
		IDXGIFactory2* This,
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		_Out_ IDXGIAdapter1** ppAdapter);

	BOOL(STDMETHODCALLTYPE* IsCurrent)(
		IDXGIFactory2* This);

	BOOL(STDMETHODCALLTYPE* IsWindowedStereoEnabled)(
		IDXGIFactory2* This);

	HRESULT(STDMETHODCALLTYPE* CreateSwapChainForHwnd)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  HWND hWnd,
		/* [annotation][in] */
		_In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
		/* [annotation][in] */
		_In_opt_  const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc,
		/* [annotation][in] */
		_In_opt_  IDXGIOutput* pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain** ppSwapChain);

	HRESULT(STDMETHODCALLTYPE* CreateSwapChainForCoreWindow)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  IUnknown* pWindow,
		/* [annotation][in] */
		_In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
		/* [annotation][in] */
		_In_opt_  IDXGIOutput* pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain** ppSwapChain);

	HRESULT(STDMETHODCALLTYPE* GetSharedResourceAdapterLuid)(
		IDXGIFactory2* This,
		/* [annotation] */
		_In_  HANDLE hResource,
		/* [annotation] */
		_Out_  LUID* pLuid);

	HRESULT(STDMETHODCALLTYPE* RegisterStereoStatusWindow)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  HWND WindowHandle,
		/* [annotation][in] */
		_In_  UINT wMsg,
		/* [annotation][out] */
		_Out_  DWORD* pdwCookie);

	HRESULT(STDMETHODCALLTYPE* RegisterStereoStatusEvent)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  HANDLE hEvent,
		/* [annotation][out] */
		_Out_  DWORD* pdwCookie);

	void (STDMETHODCALLTYPE* UnregisterStereoStatus)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  DWORD dwCookie);

	HRESULT(STDMETHODCALLTYPE* RegisterOcclusionStatusWindow)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  HWND WindowHandle,
		/* [annotation][in] */
		_In_  UINT wMsg,
		/* [annotation][out] */
		_Out_  DWORD* pdwCookie);

	HRESULT(STDMETHODCALLTYPE* RegisterOcclusionStatusEvent)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  HANDLE hEvent,
		/* [annotation][out] */
		_Out_  DWORD* pdwCookie);

	void (STDMETHODCALLTYPE* UnregisterOcclusionStatus)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  DWORD dwCookie);

	HRESULT(STDMETHODCALLTYPE* CreateSwapChainForComposition)(
		IDXGIFactory2* This,
		/* [annotation][in] */
		_In_  IUnknown* pDevice,
		/* [annotation][in] */
		_In_  const DXGI_SWAP_CHAIN_DESC1* pDesc,
		/* [annotation][in] */
		_In_opt_  IDXGIOutput* pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain** ppSwapChain);

	END_INTERFACE
} IDXGIFactory2Vtbl;

interface IDXGIFactory2
{
	CONST_VTBL struct IDXGIFactory2Vtbl* lpVtbl;
};

#define IDXGIFactory2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDXGIFactory2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDXGIFactory2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDXGIFactory2_SetPrivateData(This,Name,DataSize,pData)	\
    ( (This)->lpVtbl -> SetPrivateData(This,Name,DataSize,pData) ) 

#define IDXGIFactory2_SetPrivateDataInterface(This,Name,pUnknown)	\
    ( (This)->lpVtbl -> SetPrivateDataInterface(This,Name,pUnknown) ) 

#define IDXGIFactory2_GetPrivateData(This,Name,pDataSize,pData)	\
    ( (This)->lpVtbl -> GetPrivateData(This,Name,pDataSize,pData) ) 

#define IDXGIFactory2_GetParent(This,riid,ppParent)	\
    ( (This)->lpVtbl -> GetParent(This,riid,ppParent) ) 


#define IDXGIFactory2_EnumAdapters(This,Adapter,ppAdapter)	\
    ( (This)->lpVtbl -> EnumAdapters(This,Adapter,ppAdapter) ) 

#define IDXGIFactory2_MakeWindowAssociation(This,WindowHandle,Flags)	\
    ( (This)->lpVtbl -> MakeWindowAssociation(This,WindowHandle,Flags) ) 

#define IDXGIFactory2_GetWindowAssociation(This,pWindowHandle)	\
    ( (This)->lpVtbl -> GetWindowAssociation(This,pWindowHandle) ) 

#define IDXGIFactory2_CreateSwapChain(This,pDevice,pDesc,ppSwapChain)	\
    ( (This)->lpVtbl -> CreateSwapChain(This,pDevice,pDesc,ppSwapChain) ) 

#define IDXGIFactory2_CreateSoftwareAdapter(This,Module,ppAdapter)	\
    ( (This)->lpVtbl -> CreateSoftwareAdapter(This,Module,ppAdapter) ) 


#define IDXGIFactory2_EnumAdapters1(This,Adapter,ppAdapter)	\
    ( (This)->lpVtbl -> EnumAdapters1(This,Adapter,ppAdapter) ) 

#define IDXGIFactory2_IsCurrent(This)	\
    ( (This)->lpVtbl -> IsCurrent(This) ) 


#define IDXGIFactory2_IsWindowedStereoEnabled(This)	\
    ( (This)->lpVtbl -> IsWindowedStereoEnabled(This) ) 

#define IDXGIFactory2_CreateSwapChainForHwnd(This,pDevice,hWnd,pDesc,pFullscreenDesc,pRestrictToOutput,ppSwapChain)	\
    ( (This)->lpVtbl -> CreateSwapChainForHwnd(This,pDevice,hWnd,pDesc,pFullscreenDesc,pRestrictToOutput,ppSwapChain) ) 

#define IDXGIFactory2_CreateSwapChainForCoreWindow(This,pDevice,pWindow,pDesc,pRestrictToOutput,ppSwapChain)	\
    ( (This)->lpVtbl -> CreateSwapChainForCoreWindow(This,pDevice,pWindow,pDesc,pRestrictToOutput,ppSwapChain) ) 

#define IDXGIFactory2_GetSharedResourceAdapterLuid(This,hResource,pLuid)	\
    ( (This)->lpVtbl -> GetSharedResourceAdapterLuid(This,hResource,pLuid) ) 

#define IDXGIFactory2_RegisterStereoStatusWindow(This,WindowHandle,wMsg,pdwCookie)	\
    ( (This)->lpVtbl -> RegisterStereoStatusWindow(This,WindowHandle,wMsg,pdwCookie) ) 

#define IDXGIFactory2_RegisterStereoStatusEvent(This,hEvent,pdwCookie)	\
    ( (This)->lpVtbl -> RegisterStereoStatusEvent(This,hEvent,pdwCookie) ) 

#define IDXGIFactory2_UnregisterStereoStatus(This,dwCookie)	\
    ( (This)->lpVtbl -> UnregisterStereoStatus(This,dwCookie) ) 

#define IDXGIFactory2_RegisterOcclusionStatusWindow(This,WindowHandle,wMsg,pdwCookie)	\
    ( (This)->lpVtbl -> RegisterOcclusionStatusWindow(This,WindowHandle,wMsg,pdwCookie) ) 

#define IDXGIFactory2_RegisterOcclusionStatusEvent(This,hEvent,pdwCookie)	\
    ( (This)->lpVtbl -> RegisterOcclusionStatusEvent(This,hEvent,pdwCookie) ) 

#define IDXGIFactory2_UnregisterOcclusionStatus(This,dwCookie)	\
    ( (This)->lpVtbl -> UnregisterOcclusionStatus(This,dwCookie) ) 

#define IDXGIFactory2_CreateSwapChainForComposition(This,pDevice,pDesc,pRestrictToOutput,ppSwapChain)	\
    ( (This)->lpVtbl -> CreateSwapChainForComposition(This,pDevice,pDesc,pRestrictToOutput,ppSwapChain) ) 

//
// IMFMediaBuffer
//

typedef interface IMFMediaBuffer IMFMediaBuffer;

typedef struct IMFMediaBufferVtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			IMFMediaBuffer* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			__RPC__deref_out  void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		IMFMediaBuffer* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		IMFMediaBuffer* This);

	HRESULT(STDMETHODCALLTYPE* Lock)(
		IMFMediaBuffer* This,
		/* [annotation][out] */
		__deref_out_bcount_part(*pcbMaxLength, *pcbCurrentLength)  BYTE** ppbBuffer,
		/* [annotation][out] */
		__out_opt  DWORD* pcbMaxLength,
		/* [annotation][out] */
		__out_opt  DWORD* pcbCurrentLength);

	HRESULT(STDMETHODCALLTYPE* Unlock)(
		IMFMediaBuffer* This);

	HRESULT(STDMETHODCALLTYPE* GetCurrentLength)(
		IMFMediaBuffer* This,
		/* [annotation][out] */
		__out  DWORD* pcbCurrentLength);

	HRESULT(STDMETHODCALLTYPE* SetCurrentLength)(
		IMFMediaBuffer* This,
		/* [in] */ DWORD cbCurrentLength);

	HRESULT(STDMETHODCALLTYPE* GetMaxLength)(
		IMFMediaBuffer* This,
		/* [annotation][out] */
		__out  DWORD* pcbMaxLength);

	END_INTERFACE
} IMFMediaBufferVtbl;

interface IMFMediaBuffer
{
	CONST_VTBL struct IMFMediaBufferVtbl* lpVtbl;
};

typedef interface IMFDXGIDeviceManager IMFDXGIDeviceManager;

typedef struct IMFDXGIDeviceManagerVtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			IMFDXGIDeviceManager* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			__RPC__deref_out  void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		IMFDXGIDeviceManager* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		IMFDXGIDeviceManager* This);

	HRESULT(STDMETHODCALLTYPE* CloseDeviceHandle)(
		IMFDXGIDeviceManager* This,
		/* [annotation] */
		_In_  HANDLE hDevice);

	HRESULT(STDMETHODCALLTYPE* GetVideoService)(
		IMFDXGIDeviceManager* This,
		/* [annotation] */
		_In_  HANDLE hDevice,
		/* [annotation] */
		_In_  REFIID riid,
		/* [annotation] */
		__RPC__deref_out  void** ppService);

	HRESULT(STDMETHODCALLTYPE* LockDevice)(
		IMFDXGIDeviceManager* This,
		/* [annotation] */
		_In_  HANDLE hDevice,
		/* [annotation] */
		_In_  REFIID riid,
		/* [annotation] */
		__RPC__deref_out  void** ppUnkDevice,
		/* [annotation] */
		_In_  BOOL fBlock);

	HRESULT(STDMETHODCALLTYPE* OpenDeviceHandle)(
		IMFDXGIDeviceManager* This,
		/* [annotation] */
		_Out_  HANDLE* phDevice);

	HRESULT(STDMETHODCALLTYPE* ResetDevice)(
		IMFDXGIDeviceManager* This,
		/* [annotation] */
		_In_  IUnknown* pUnkDevice,
		/* [annotation] */
		_In_  UINT resetToken);

	HRESULT(STDMETHODCALLTYPE* TestDevice)(
		IMFDXGIDeviceManager* This,
		/* [annotation] */
		_In_  HANDLE hDevice);

	HRESULT(STDMETHODCALLTYPE* UnlockDevice)(
		IMFDXGIDeviceManager* This,
		/* [annotation] */
		_In_  HANDLE hDevice,
		/* [annotation] */
		_In_  BOOL fSaveState);

	END_INTERFACE
} IMFDXGIDeviceManagerVtbl;

interface IMFDXGIDeviceManager
{
	CONST_VTBL struct IMFDXGIDeviceManagerVtbl* lpVtbl;
};


typedef interface IDXGIFactoryMedia IDXGIFactoryMedia;

typedef struct
{
	IUnknownVtbl Base;

	HRESULT(STDMETHODCALLTYPE* CreateSwapChainForCompositionSurfaceHandle)(IDXGIFactoryMedia* This, IUnknown* Device, HANDLE Surface, CONST DXGI_SWAP_CHAIN_DESC1* Desc, IDXGIOutput* RestrictToOutput, IDXGISwapChain** SwapChain);
	HRESULT(STDMETHODCALLTYPE* CreateDecodeSwapChainForCompositionSurfaceHandle)(IDXGIFactoryMedia* This, IUnknown* Device, HANDLE Surface, PVOID Desc, IDXGIResource* YuvDecodeBuffers, IDXGIOutput* RestrictToOutput, IUnknown** SwapChain);
} IDXGIFactoryMediaVtbl;

interface IDXGIFactoryMedia
{
	IDXGIFactoryMediaVtbl* lpVtbl;
};

typedef ID3D11Device ID3D11Device1;

typedef struct ID3D11Device1Vtbl
{
	ID3D11DeviceVtbl Base;

	void (STDMETHODCALLTYPE* GetImmediateContext1)(
		ID3D11Device1* This,
		_Outptr_  ID3D11DeviceContext1** ppImmediateContext);

	HRESULT(STDMETHODCALLTYPE* CreateDeferredContext1)(
		ID3D11Device1* This,
		UINT ContextFlags,
		_COM_Outptr_opt_  ID3D11DeviceContext1** ppDeferredContext);

	HRESULT(STDMETHODCALLTYPE* CreateBlendState1)(
		ID3D11Device1* This,
		_In_  const D3D11_BLEND_DESC1* pBlendStateDesc,
		_COM_Outptr_opt_  ID3D11BlendState1** ppBlendState);

	HRESULT(STDMETHODCALLTYPE* CreateRasterizerState1)(
		ID3D11Device1* This,
		_In_  const D3D11_RASTERIZER_DESC1* pRasterizerDesc,
		_COM_Outptr_opt_  ID3D11RasterizerState1** ppRasterizerState);

	HRESULT(STDMETHODCALLTYPE* CreateDeviceContextState)(
		ID3D11Device1* This,
		UINT Flags,
		_In_reads_(FeatureLevels)  const D3D_FEATURE_LEVEL* pFeatureLevels,
		UINT FeatureLevels,
		UINT SDKVersion,
		REFIID EmulatedInterface,
		_Out_opt_  D3D_FEATURE_LEVEL* pChosenFeatureLevel,
		_Out_opt_  ID3DDeviceContextState** ppContextState);

	HRESULT(STDMETHODCALLTYPE* OpenSharedResource1)(
		ID3D11Device1* This,
		_In_  HANDLE hResource,
		_In_  REFIID returnedInterface,
		_COM_Outptr_  void** ppResource);

	HRESULT(STDMETHODCALLTYPE* OpenSharedResourceByName)(
		ID3D11Device1* This,
		_In_  LPCWSTR lpName,
		_In_  DWORD dwDesiredAccess,
		_In_  REFIID returnedInterface,
		_COM_Outptr_  void** ppResource);
} ID3D11Device1Vtbl;

DEFINE_GUID(IID_IDXGIDisplayControl, 0xea9dbf1a, 0xc88e, 0x4486, 0x85, 0x4a, 0x98, 0xaa, 0x01, 0x38, 0xf3, 0x0c);
DEFINE_GUID(IID_IDXGIOutputDuplication, 0x191cfac3, 0xa341, 0x470d, 0xb2, 0x6e, 0xa8, 0x64, 0xf4, 0x28, 0x31, 0x9c);
DEFINE_GUID(IID_IDXGISurface2, 0xaba496dd, 0xb617, 0x4cb8, 0xa8, 0x66, 0xbc, 0x44, 0xd7, 0xeb, 0x1f, 0xa2);
DEFINE_GUID(IID_IDXGIResource1, 0x30961379, 0x4609, 0x4a41, 0x99, 0x8e, 0x54, 0xfe, 0x56, 0x7e, 0xe0, 0xc1);
DEFINE_GUID(IID_IDXGIDevice, 0x54ec77fa, 0x1377, 0x44e6, 0x8c, 0x32, 0x88, 0xfd, 0x5f, 0x44, 0xc8, 0x4c);
DEFINE_GUID(IID_IDXGIDevice1, 0x77db970f, 0x6276, 0x48ba, 0xba, 0x28, 0x07, 0x01, 0x43, 0xb4, 0x39, 0x2c);
DEFINE_GUID(IID_IDXGIDevice2, 0x05008617, 0xfbfd, 0x4051, 0xa7, 0x90, 0x14, 0x48, 0x84, 0xb4, 0xf6, 0xa9);
DEFINE_GUID(IID_IDXGIDevice3, 0x6007896c, 0x3244, 0x4afd, 0xbf, 0x18, 0xa6, 0xd3, 0xbe, 0xda, 0x50, 0x23);
DEFINE_GUID(IID_IDXGIDevice4, 0x95B4F95F, 0xD8DA, 0x4CA4, 0x9E, 0xE6, 0x3B, 0x76, 0xD5, 0x96, 0x8A, 0x10);
DEFINE_GUID(IID_IDXGIAdapter2, 0x0AA1AE0A, 0xFA0E, 0x4B84, 0x86, 0x44, 0xE0, 0x5F, 0xF8, 0xE5, 0xAC, 0xB5);
DEFINE_GUID(IID_IDXGIAdapter3, 0x645967A4, 0x1392, 0x4310, 0xA7, 0x98, 0x80, 0x53, 0xCE, 0x3E, 0x93, 0xFD);
DEFINE_GUID(IID_IDXGIAdapter4, 0x3c8d99d1, 0x4fbf, 0x4181, 0xa8, 0x2c, 0xaf, 0x66, 0xbf, 0x7b, 0xd2, 0x4e);
DEFINE_GUID(IID_IDXGIOutput1, 0x00cddea8, 0x939b, 0x4b83, 0xa3, 0x40, 0xa6, 0x85, 0x22, 0x66, 0x66, 0xcc);
DEFINE_GUID(IID_IDXGIOutput2, 0x595e39d1, 0x2724, 0x4663, 0x99, 0xb1, 0xda, 0x96, 0x9d, 0xe2, 0x83, 0x64);
DEFINE_GUID(IID_IDXGIOutput3, 0x8a6bb301, 0x7e7e, 0x41F4, 0xa8, 0xe0, 0x5b, 0x32, 0xf7, 0xf9, 0x9b, 0x18);
DEFINE_GUID(IID_IDXGIOutput4, 0xdc7dca35, 0x2196, 0x414d, 0x9F, 0x53, 0x61, 0x78, 0x84, 0x03, 0x2a, 0x60);
DEFINE_GUID(IID_IDXGIOutput5, 0x80A07424, 0xAB52, 0x42EB, 0x83, 0x3C, 0x0C, 0x42, 0xFD, 0x28, 0x2D, 0x98);
DEFINE_GUID(IID_IDXGIOutput6, 0x068346e8, 0xaaec, 0x4b84, 0xad, 0xd7, 0x13, 0x7f, 0x51, 0x3f, 0x77, 0xa1);
DEFINE_GUID(IID_IDXGISwapChain1, 0x790a45f7, 0x0d42, 0x4876, 0x98, 0x3a, 0x0a, 0x55, 0xcf, 0xe6, 0xf4, 0xaa);
DEFINE_GUID(IID_IDXGISwapChain2, 0xa8be2ac4, 0x199f, 0x4946, 0xb3, 0x31, 0x79, 0x59, 0x9f, 0xb9, 0x8d, 0xe7);
DEFINE_GUID(IID_IDXGISwapChain3, 0x94d99bdb, 0xf1f8, 0x4ab0, 0xb2, 0x36, 0x7d, 0xa0, 0x17, 0x0e, 0xda, 0xb1);
DEFINE_GUID(IID_IDXGISwapChain4, 0x3D585D5A, 0xBD4A, 0x489E, 0xB1, 0xF4, 0x3D, 0xBC, 0xB6, 0x45, 0x2F, 0xFB);
//DEFINE_GUID(IID_ID3D11Texture2D,0x6f15aaf2,0xd208,0x4e89,0x9a,0xb4,0x48,0x95,0x35,0xd3,0x4f,0x9c);

DEFINE_GUID(IID_IDXGIFactory, 0x7b7166ec, 0x21c7, 0x44ae, 0xb2, 0x1a, 0xc9, 0xae, 0x32, 0x1a, 0xe3, 0x69);
DEFINE_GUID(IID_IDXGIFactory1, 0x770aae78, 0xf26f, 0x4dba, 0xa8, 0x29, 0x25, 0x3c, 0x83, 0xd1, 0xb3, 0x87);
DEFINE_GUID(IID_IDXGIFactory2, 0x50c83a1c, 0xe072, 0x4c48, 0x87, 0xb0, 0x36, 0x30, 0xfa, 0x36, 0xa6, 0xd0);
DEFINE_GUID(IID_IDXGIFactory3, 0x25483823, 0xcd46, 0x4c7d, 0x86, 0xca, 0x47, 0xaa, 0x95, 0xb8, 0x37, 0xbd);
DEFINE_GUID(IID_IDXGIFactory4, 0x1bc6ea02, 0xef36, 0x464f, 0xbf, 0x0c, 0x21, 0xca, 0x39, 0xe5, 0x16, 0x8a);
DEFINE_GUID(IID_IDXGIFactory5, 0x7632e1f5, 0xee65, 0x4dca, 0x87, 0xfd, 0x84, 0xcd, 0x75, 0xf8, 0x83, 0x8d);
DEFINE_GUID(IID_IDXGIFactory6, 0xc1b6694f, 0xff09, 0x44a9, 0xb0, 0x3c, 0x77, 0x90, 0x0a, 0x0a, 0x1d, 0x17);
DEFINE_GUID(IID_IDXGIFactory7, 0xa4966eed, 0x76db, 0x44da, 0x84, 0xc1, 0xee, 0x9a, 0x7a, 0xfb, 0x20, 0xa8);
DEFINE_GUID(IID_IDXGIFactoryMedia, 0x41e7d1f2, 0xa591, 0x4f7b, 0xa2, 0xe5, 0xfa, 0x9c, 0x84, 0x3e, 0x1c, 0x12);

DEFINE_GUID(IID_ID3D11Device2, 0x9d06dffa, 0xd1e5, 0x4d07, 0x83, 0xa8, 0x1b, 0xb1, 0x23, 0xf2, 0xf8, 0x41);
DEFINE_GUID(IID_ID3D11Device3, 0xA05C8C37, 0xD2C6, 0x4732, 0xB3, 0xA0, 0x9C, 0xE0, 0xB0, 0xDC, 0x9A, 0xE6);
DEFINE_GUID(IID_ID3D11Device4, 0x8992ab71, 0x02e6, 0x4b8d, 0xba, 0x48, 0xb0, 0x56, 0xdc, 0xda, 0x42, 0xc4);
DEFINE_GUID(IID_ID3D11Device5, 0x8ffde202, 0xa0e7, 0x45df, 0x9e, 0x01, 0xe8, 0x37, 0x80, 0x1b, 0x5e, 0xa0);

DEFINE_GUID(IID_ID3D11DeviceContext2, 0x420d5b32, 0xb90c, 0x4da4, 0xbe, 0xf0, 0x35, 0x9f, 0x6a, 0x24, 0xa8, 0x3a);
DEFINE_GUID(IID_ID3D11DeviceContext3, 0xb4e3c01d, 0xe79e, 0x4637, 0x91, 0xb2, 0x51, 0x0e, 0x9f, 0x4c, 0x9b, 0x8f);
DEFINE_GUID(IID_ID3D11DeviceContext4, 0x917600da, 0xf58c, 0x4c33, 0x98, 0xd8, 0x3e, 0x15, 0xb3, 0x90, 0xfa, 0x24);

typedef struct IIDXGIFactory2To7
{
	IDXGIFactory2Vtbl* lpVtbl;
	IDXGIFactory2* Thi;
	UINT CreationFlags;
} IIDXGIFactory2To7;

typedef struct IIDXGIAdapter
{
	IDXGIAdapter1Vtbl* lpVtbl;
	IDXGIAdapter1* Thi;
} IIDXGIAdapter;

typedef struct IIDXGISwapchain
{
	IDXGISwapChainVtbl* lpVtbl;
	IDXGISwapChain* Thi;
} IIDXGISwapchain;

typedef struct IIDXGIOutput
{
	IDXGIOutputVtbl* lpVtbl;
	IDXGIOutput* Thi;
} IIDXGIOutput;

typedef struct IIDXGIDevice
{
	IDXGIDeviceVtbl* lpVtbl;
	IDXGIDevice* Thi;
} IIDXGIDevice;

typedef struct IID3D11Device
{
	ID3D11Device1Vtbl* lpVtbl;
	ID3D11Device* Thi;
} IID3D11Device;

typedef struct IID3D11DeviceContext
{
	ID3D11DeviceContextVtbl* lpVtbl;
	ID3D11DeviceContext* Thi;
} IID3D11DeviceContext;

KXDXAPI IDXGIDevice* STDMETHODCALLTYPE KexWrapID3D11RawDevice(IDXGIDevice* pAdapt);