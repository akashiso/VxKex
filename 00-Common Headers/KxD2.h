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

typedef interface ID2D1Factory1 ID2D1Factory1;

typedef interface ID2D1Device ID2D1Device;
typedef interface ID2D1DeviceContext ID2D1DeviceContext;
typedef interface ID2D1Bitmap1 ID2D1Bitmap1;
typedef interface ID2D1GradientStopCollection1 ID2D1GradientStopCollection1;
typedef interface ID2D1PathGeometry1 ID2D1PathGeometry1;
typedef interface ID2D1BitmapBrush1 ID2D1BitmapBrush1;
typedef interface ID2D1GeometryRealization ID2D1GeometryRealization;
typedef interface ID2D1Ink ID2D1Ink;
typedef interface ID2D1InkStyle ID2D1InkStyle;
typedef interface ID2D1GradientMesh ID2D1GradientMesh;
typedef interface ID2D1ImageBrush ID2D1ImageBrush;
typedef interface ID2D1StrokeStyle1 ID2D1StrokeStyle1;
typedef interface ID2D1ImageSource ID2D1ImageSource;
typedef interface ID2D1ImageSourceFromWic ID2D1ImageSourceFromWic;
typedef interface ID2D1TransformedImageSource ID2D1TransformedImageSource;
typedef interface ID2D1LookupTable3D ID2D1LookupTable3D;
typedef interface ID2D1SpriteBatch ID2D1SpriteBatch;
typedef interface ID2D1SvgGlyphStyle ID2D1SvgGlyphStyle;
typedef interface ID2D1SvgDocument ID2D1SvgDocument;
typedef interface ID2D1Properties ID2D1Properties;
typedef interface ID2D1CommandList ID2D1CommandList;
typedef interface ID2D1GdiMetafile ID2D1GdiMetafile;
typedef interface ID2D1PrintControl ID2D1PrintControl;
typedef interface IPrintDocumentPackageTarget IPrintDocumentPackageTarget;
typedef interface IWICImagingFactory IWICImagingFactory;
typedef interface ID2D1DrawingStateBlock1 ID2D1DrawingStateBlock1;

typedef interface ID2D1ColorContext ID2D1ColorContext;
typedef interface ID2D1ColorContext1 ID2D1ColorContext1;

typedef interface IDWriteFontFace IDWriteFontFace;

typedef interface D2D1_MATRIX_4X4_F D2D1_MATRIX_4X4_F;
typedef interface D2D1_GRADIENT_MESH_PATCH D2D1_GRADIENT_MESH_PATCH;
typedef interface D2D1_RENDERING_CONTROLS D2D1_RENDERING_CONTROLS;
typedef interface D2D1_STROKE_STYLE_PROPERTIES1 D2D1_STROKE_STYLE_PROPERTIES1;
typedef interface D2D1_DRAWING_STATE_DESCRIPTION1 D2D1_DRAWING_STATE_DESCRIPTION1;
typedef interface D2D1_PRINT_CONTROL_PROPERTIES D2D1_PRINT_CONTROL_PROPERTIES;
typedef interface D2D1_IMAGE_BRUSH_PROPERTIES D2D1_IMAGE_BRUSH_PROPERTIES;
typedef interface D2D1_BITMAP_BRUSH_PROPERTIES1 D2D1_BITMAP_BRUSH_PROPERTIES1;
typedef interface D2D1_EFFECT_INPUT_DESCRIPTION D2D1_EFFECT_INPUT_DESCRIPTION;
typedef interface D2D1_TRANSFORMED_IMAGE_SOURCE_PROPERTIES D2D1_TRANSFORMED_IMAGE_SOURCE_PROPERTIES;

typedef interface DWRITE_GLYPH_RUN_DESCRIPTION DWRITE_GLYPH_RUN_DESCRIPTION;

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

typedef enum DWRITE_READING_DIRECTION
{
	DWRITE_READING_DIRECTION_LEFT_TO_RIGHT = 0,
	DWRITE_READING_DIRECTION_RIGHT_TO_LEFT = 1,
	DWRITE_READING_DIRECTION_TOP_TO_BOTTOM = 2,
	DWRITE_READING_DIRECTION_BOTTOM_TO_TOP = 3
} DWRITE_READING_DIRECTION;

typedef enum DWRITE_FLOW_DIRECTION
{
	DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM = 0,
	DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP = 1,
	DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT = 2,
	DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT = 3
} DWRITE_FLOW_DIRECTION;

typedef enum DWRITE_TEXT_ANTIALIAS_MODE
{
	DWRITE_TEXT_ANTIALIAS_MODE_CLEARTYPE,
	DWRITE_TEXT_ANTIALIAS_MODE_GRAYSCALE
} DWRITE_TEXT_ANTIALIAS_MODE;

typedef enum DWRITE_RENDERING_MODE
{
	DWRITE_RENDERING_MODE_DEFAULT,
	DWRITE_RENDERING_MODE_ALIASED,
	DWRITE_RENDERING_MODE_GDI_CLASSIC,
	DWRITE_RENDERING_MODE_GDI_NATURAL,
	DWRITE_RENDERING_MODE_NATURAL,
	DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC,
	DWRITE_RENDERING_MODE_OUTLINE,
	DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC = DWRITE_RENDERING_MODE_GDI_CLASSIC,
	DWRITE_RENDERING_MODE_CLEARTYPE_GDI_NATURAL = DWRITE_RENDERING_MODE_GDI_NATURAL,
	DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL = DWRITE_RENDERING_MODE_NATURAL,
	DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL_SYMMETRIC = DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC
} DWRITE_RENDERING_MODE;

typedef enum DWRITE_TEXTURE_TYPE
{
	DWRITE_TEXTURE_ALIASED_1x1,
	DWRITE_TEXTURE_CLEARTYPE_3x1
} DWRITE_TEXTURE_TYPE;

typedef struct DWRITE_GLYPH_OFFSET
{
	FLOAT advanceOffset;
	FLOAT ascenderOffset;
} DWRITE_GLYPH_OFFSET;

typedef struct DWRITE_UNDERLINE
{
	FLOAT                    width;
	FLOAT                    thickness;
	FLOAT                    offset;
	FLOAT                    runHeight;
	DWRITE_READING_DIRECTION readingDirection;
	DWRITE_FLOW_DIRECTION    flowDirection;
	WCHAR const* localeName;
	DWRITE_MEASURING_MODE    measuringMode;
} DWRITE_UNDERLINE;

typedef struct DWRITE_STRIKETHROUGH
{
	FLOAT                    width;
	FLOAT                    thickness;
	FLOAT                    offset;
	DWRITE_READING_DIRECTION readingDirection;
	DWRITE_FLOW_DIRECTION    flowDirection;
	WCHAR const* localeName;
	DWRITE_MEASURING_MODE    measuringMode;
} DWRITE_STRIKETHROUGH;

typedef struct DWRITE_GLYPH_RUN
{
	IDWriteFontFace* fontFace;
	FLOAT                     fontEmSize;
	UINT32                    glyphCount;
	UINT16 const* glyphIndices;
	FLOAT const* glyphAdvances;
	DWRITE_GLYPH_OFFSET const* glyphOffsets;
	BOOL                      isSideways;
	UINT32                    bidiLevel;
} DWRITE_GLYPH_RUN;

typedef struct DWRITE_COLOR_GLYPH_RUN
{
	DWRITE_GLYPH_RUN glyphRun;
	DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription;
	FLOAT baselineOriginX;
	FLOAT baselineOriginY;
	D2D1_COLOR_F runColor;
	UINT16 paletteIndex;
} DWRITE_COLOR_GLYPH_RUN;

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

typedef enum D2D1_BLEND_MODE
{
	D2D1_BLEND_MODE_FORCE_DWORD = 0xffffffff

} D2D1_BLEND_MODE;

typedef enum D2D1_RENDERING_PRIORITY
{
	D2D1_RENDERING_PRIORITY_NORMAL = 0,
	D2D1_RENDERING_PRIORITY_LOW = 1,
	D2D1_RENDERING_PRIORITY_FORCE_DWORD = 0xffffffff

} D2D1_RENDERING_PRIORITY;

typedef enum D2D1_BUFFER_PRECISION
{
	D2D1_BUFFER_PRECISION_UNKNOWN = 0,
	D2D1_BUFFER_PRECISION_8BPC_UNORM = 1,
	D2D1_BUFFER_PRECISION_8BPC_UNORM_SRGB = 2,
	D2D1_BUFFER_PRECISION_16BPC_UNORM = 3,
	D2D1_BUFFER_PRECISION_16BPC_FLOAT = 4,
	D2D1_BUFFER_PRECISION_32BPC_FLOAT = 5,
	D2D1_BUFFER_PRECISION_FORCE_DWORD = 0xffffffff

} D2D1_BUFFER_PRECISION;

typedef enum D2D1_CHANGE_TYPE
{
	D2D1_CHANGE_TYPE_NONE = 0,
	D2D1_CHANGE_TYPE_PROPERTIES = 1,
	D2D1_CHANGE_TYPE_CONTEXT = 2,
	D2D1_CHANGE_TYPE_GRAPH = 3,
	D2D1_CHANGE_TYPE_FORCE_DWORD = 0xffffffff

} D2D1_CHANGE_TYPE;

typedef enum D2D1_FEATURE
{
	D2D1_FEATURE_DOUBLES = 0,
	D2D1_FEATURE_D3D10_X_HARDWARE_OPTIONS = 1,
	D2D1_FEATURE_FORCE_DWORD = 0xffffffff

} D2D1_FEATURE;

typedef enum D2D1_PROPERTY_TYPE
{
	D2D1_PROPERTY_TYPE_UNKNOWN = 0,
	D2D1_PROPERTY_TYPE_STRING = 1,
	D2D1_PROPERTY_TYPE_BOOL = 2,
	D2D1_PROPERTY_TYPE_UINT32 = 3,
	D2D1_PROPERTY_TYPE_INT32 = 4,
	D2D1_PROPERTY_TYPE_FLOAT = 5,
	D2D1_PROPERTY_TYPE_VECTOR2 = 6,
	D2D1_PROPERTY_TYPE_VECTOR3 = 7,
	D2D1_PROPERTY_TYPE_VECTOR4 = 8,
	D2D1_PROPERTY_TYPE_BLOB = 9,
	D2D1_PROPERTY_TYPE_IUNKNOWN = 10,
	D2D1_PROPERTY_TYPE_ENUM = 11,
	D2D1_PROPERTY_TYPE_ARRAY = 12,
	D2D1_PROPERTY_TYPE_CLSID = 13,
	D2D1_PROPERTY_TYPE_MATRIX_3X2 = 14,
	D2D1_PROPERTY_TYPE_MATRIX_4X3 = 15,
	D2D1_PROPERTY_TYPE_MATRIX_4X4 = 16,
	D2D1_PROPERTY_TYPE_MATRIX_5X4 = 17,
	D2D1_PROPERTY_TYPE_COLOR_CONTEXT = 18,
	D2D1_PROPERTY_TYPE_FORCE_DWORD = 0xffffffff

} D2D1_PROPERTY_TYPE;

typedef enum D2D1_CHANNEL_DEPTH
{
	D2D1_CHANNEL_DEPTH_DEFAULT = 0,
	D2D1_CHANNEL_DEPTH_1 = 1,
	D2D1_CHANNEL_DEPTH_4 = 4,
	D2D1_CHANNEL_DEPTH_FORCE_DWORD = 0xffffffff

} D2D1_CHANNEL_DEPTH;

typedef enum D2D1_COLOR_SPACE
{
	D2D1_COLOR_SPACE_CUSTOM = 0,
	D2D1_COLOR_SPACE_SRGB = 1,
	D2D1_COLOR_SPACE_SCRGB = 2,
	D2D1_COLOR_SPACE_FORCE_DWORD = 0xffffffff

} D2D1_COLOR_SPACE;

typedef enum D2D1_COLOR_CONTEXT_TYPE
{
	D2D1_COLOR_CONTEXT_TYPE_ICC = 0,
	D2D1_COLOR_CONTEXT_TYPE_SIMPLE = 1,
	D2D1_COLOR_CONTEXT_TYPE_DXGI = 2,
	D2D1_COLOR_CONTEXT_TYPE_FORCE_DWORD = 0xffffffff
} D2D1_COLOR_CONTEXT_TYPE;

typedef enum D2D1_BLEND_OPERATION
{
	D2D1_BLEND_OPERATION_ADD = 1,
	D2D1_BLEND_OPERATION_SUBTRACT = 2,
	D2D1_BLEND_OPERATION_REV_SUBTRACT = 3,
	D2D1_BLEND_OPERATION_MIN = 4,
	D2D1_BLEND_OPERATION_MAX = 5,
	D2D1_BLEND_OPERATION_FORCE_DWORD = 0xffffffff

} D2D1_BLEND_OPERATION;

typedef enum D2D1_DEVICE_CONTEXT_OPTIONS
{
	D2D1_DEVICE_CONTEXT_OPTIONS_NONE = 0,
	D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS = 1,
	D2D1_DEVICE_CONTEXT_OPTIONS_FORCE_DWORD = 0xffffffff

} D2D1_DEVICE_CONTEXT_OPTIONS;

typedef enum D2D1_COLOR_INTERPOLATION_MODE
{
	D2D1_COLOR_INTERPOLATION_MODE_STRAIGHT = 0,
	D2D1_COLOR_INTERPOLATION_MODE_PREMULTIPLIED = 1,
	D2D1_COLOR_INTERPOLATION_MODE_FORCE_DWORD = 0xffffffff

} D2D1_COLOR_INTERPOLATION_MODE;

typedef enum D2D1_PRIMITIVE_BLEND
{
	D2D1_PRIMITIVE_BLEND_SOURCE_OVER = 0,
	D2D1_PRIMITIVE_BLEND_COPY = 1,
	D2D1_PRIMITIVE_BLEND_MIN = 2,
	D2D1_PRIMITIVE_BLEND_ADD = 3,
	D2D1_PRIMITIVE_BLEND_MAX = 4,
	D2D1_PRIMITIVE_BLEND_FORCE_DWORD = 0xffffffff

} D2D1_PRIMITIVE_BLEND;

typedef enum D2D1_UNIT_MODE
{
	D2D1_UNIT_MODE_DIPS = 0,
	D2D1_UNIT_MODE_PIXELS = 1,
	D2D1_UNIT_MODE_FORCE_DWORD = 0xffffffff

} D2D1_UNIT_MODE;

typedef enum D2D1_INTERPOLATION_MODE
{
	D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR = 0,
	D2D1_INTERPOLATION_MODE_LINEAR = 1,
	D2D1_INTERPOLATION_MODE_CUBIC = 2,
	D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR = 3,
	D2D1_INTERPOLATION_MODE_ANISOTROPIC = 4,
	D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC = 5,
	D2D1_INTERPOLATION_MODE_FORCE_DWORD = 0xffffffff

} D2D1_INTERPOLATION_MODE;

typedef enum D2D1_COMPOSITE_MODE
{
	D2D1_COMPOSITE_MODE_SOURCE_OVER = 0,
	D2D1_COMPOSITE_MODE_DESTINATION_OVER = 1,
	D2D1_COMPOSITE_MODE_SOURCE_IN = 2,
	D2D1_COMPOSITE_MODE_DESTINATION_IN = 3,
	D2D1_COMPOSITE_MODE_SOURCE_OUT = 4,
	D2D1_COMPOSITE_MODE_DESTINATION_OUT = 5,
	D2D1_COMPOSITE_MODE_SOURCE_ATOP = 6,
	D2D1_COMPOSITE_MODE_DESTINATION_ATOP = 7,
	D2D1_COMPOSITE_MODE_XOR = 8,
	D2D1_COMPOSITE_MODE_PLUS = 9,
	D2D1_COMPOSITE_MODE_SOURCE_COPY = 10,
	D2D1_COMPOSITE_MODE_BOUNDED_SOURCE_COPY = 11,
	D2D1_COMPOSITE_MODE_MASK_INVERT = 12,
	D2D1_COMPOSITE_MODE_FORCE_DWORD = 0xffffffff

} D2D1_COMPOSITE_MODE;

typedef enum D2D1_INK_NIB_SHAPE
{
	D2D1_INK_NIB_SHAPE_ROUND = 0,
	D2D1_INK_NIB_SHAPE_SQUARE = 1,
	D2D1_INK_NIB_SHAPE_FORCE_DWORD = 0xffffffff

} D2D1_INK_NIB_SHAPE;

typedef enum D2D1_GAMMA1
{
	D2D1_GAMMA1_G22 = D2D1_GAMMA_2_2,
	D2D1_GAMMA1_G10 = D2D1_GAMMA_1_0,
	D2D1_GAMMA1_G2084 = 2,
	D2D1_GAMMA1_FORCE_DWORD = 0xffffffff

} D2D1_GAMMA1;

typedef enum D2D1_PIXEL_OPTIONS
{
	D2D1_PIXEL_OPTIONS_NONE = 0,
	D2D1_PIXEL_OPTIONS_TRIVIAL_SAMPLING = 1,
	D2D1_PIXEL_OPTIONS_FORCE_DWORD = 0xffffffff
} D2D1_PIXEL_OPTIONS;

typedef enum D2D1_VERTEX_OPTIONS
{
	D2D1_VERTEX_OPTIONS_NONE = 0,
	D2D1_VERTEX_OPTIONS_DO_NOT_CLEAR = 1,
	D2D1_VERTEX_OPTIONS_USE_DEPTH_BUFFER = 2,
	D2D1_VERTEX_OPTIONS_ASSUME_NO_OVERLAP = 4,
	D2D1_VERTEX_OPTIONS_FORCE_DWORD = 0xffffffff
} D2D1_VERTEX_OPTIONS;

typedef enum _D2D1_DRAW_TEXT_OPTIONS
{
	D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT = 0x00000004,
} _D2D1_DRAW_TEXT_OPTIONS;



typedef HRESULT(CALLBACK* PD2D1_PROPERTY_SET_FUNCTION)(
	_In_ IUnknown* effect,
	_In_reads_(dataSize) const BYTE* data,
	UINT32 dataSize
	);

typedef HRESULT(CALLBACK* PD2D1_PROPERTY_GET_FUNCTION)(
	_In_ const IUnknown* effect,
	_Out_writes_opt_(dataSize) BYTE* data,
	UINT32 dataSize,
	_Out_opt_ UINT32* actualSize
	);

typedef struct D2D1_INPUT_DESCRIPTION D2D1_INPUT_DESCRIPTION;
typedef struct D2D1_VERTEX_RANGE D2D1_VERTEX_RANGE;

typedef struct D2D1_PROPERTY_BINDING
{
	PCWSTR propertyName;
	PD2D1_PROPERTY_SET_FUNCTION setFunction;
	PD2D1_PROPERTY_GET_FUNCTION getFunction;

} D2D1_PROPERTY_BINDING;

typedef struct D2D1_SIMPLE_COLOR_PROFILE
{
	D2D1_POINT_2F redPrimary;
	D2D1_POINT_2F greenPrimary;
	D2D1_POINT_2F bluePrimary;
	D2D1_POINT_2F whitePointXZ;
	D2D1_GAMMA1 gamma;

} D2D1_SIMPLE_COLOR_PROFILE;

typedef struct D2D1_INK_POINT
{
	FLOAT x;
	FLOAT y;
	FLOAT radius;

} D2D1_INK_POINT;

typedef struct D2D1_INK_STYLE_PROPERTIES
{
	D2D1_INK_NIB_SHAPE nibShape;
	D2D1_MATRIX_3X2_F nibTransform;

} D2D1_INK_STYLE_PROPERTIES;

typedef struct D2D1_BITMAP_PROPERTIES1 D2D1_BITMAP_PROPERTIES1;
typedef struct D2D1_INPUT_ELEMENT_DESC D2D1_INPUT_ELEMENT_DESC;
typedef struct D2D1_RESOURCE_TEXTURE_PROPERTIES D2D1_RESOURCE_TEXTURE_PROPERTIES;
typedef struct D2D1_VERTEX_BUFFER_PROPERTIES D2D1_VERTEX_BUFFER_PROPERTIES;
typedef struct D2D1_CUSTOM_VERTEX_BUFFER_PROPERTIES D2D1_CUSTOM_VERTEX_BUFFER_PROPERTIES;
typedef struct D2D1_BLEND_DESCRIPTION D2D1_BLEND_DESCRIPTION;

typedef struct D2D_MATRIX_5X4_F
{
	union
	{
		struct
		{
			FLOAT _11, _12, _13, _14;
			FLOAT _21, _22, _23, _24;
			FLOAT _31, _32, _33, _34;
			FLOAT _41, _42, _43, _44;
			FLOAT _51, _52, _53, _54;
		};

		FLOAT m[5][4];
	};

} D2D_MATRIX_5X4_F;

typedef struct D2D1_LAYER_PARAMETERS1 D2D1_LAYER_PARAMETERS1;

typedef POINT D2D1_POINT_2L;
typedef RECT D2D1_RECT_L;

typedef interface IWICColorContext IWICColorContext;
typedef interface ID2D1Resource ID2D1Image;
typedef interface ID2D1VertexBuffer ID2D1VertexBuffer;
typedef interface ID2D1ResourceTexture ID2D1ResourceTexture;

typedef interface ID2D1Effect ID2D1Effect;

typedef struct ID2D1EffectVtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			ID2D1Effect* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			_Out_ void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		ID2D1Effect* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		ID2D1Effect* This);

	STDMETHOD_(UINT32, GetPropertyCount)(
		ID2D1Effect* This
		);

	STDMETHOD(GetPropertyName)(
		ID2D1Effect* This,
		UINT32 index,
		_Out_writes_(nameCount) PWSTR name,
		UINT32 nameCount
		);

	STDMETHOD_(UINT32, GetPropertyNameLength)(
		ID2D1Effect* This,
		UINT32 index
		);

	STDMETHOD_(D2D1_PROPERTY_TYPE, GetType)(
		ID2D1Effect* This,
		UINT32 index
		);

	STDMETHOD_(UINT32, GetPropertyIndex)(
		ID2D1Effect* This,
		_In_ PCWSTR name
		);

	STDMETHOD(SetValueByName)(
		ID2D1Effect* This,
		_In_ PCWSTR name,
		D2D1_PROPERTY_TYPE type,
		_In_reads_(dataSize) CONST BYTE* data,
		UINT32 dataSize
		);

	STDMETHOD(SetValue)(
		ID2D1Effect* This,
		UINT32 index,
		D2D1_PROPERTY_TYPE type,
		_In_reads_(dataSize) CONST BYTE* data,
		UINT32 dataSize
		);

	STDMETHOD(GetValueByName)(
		ID2D1Effect* This,
		_In_ PCWSTR name,
		D2D1_PROPERTY_TYPE type,
		_Out_writes_(dataSize) BYTE* data,
		UINT32 dataSize
		);

	STDMETHOD(GetValue)(
		ID2D1Effect* This,
		UINT32 index,
		D2D1_PROPERTY_TYPE type,
		_Out_writes_(dataSize) BYTE* data,
		UINT32 dataSize
		);

	STDMETHOD_(UINT32, GetValueSize)(
		ID2D1Effect* This,
		UINT32 index
		);

	STDMETHOD(GetSubProperties)(
		ID2D1Effect* This,
		UINT32 index,
		_COM_Outptr_result_maybenull_ ID2D1Effect** subProperties
		);

	STDMETHOD_(void, SetInput)(
		ID2D1Effect* This,
		UINT32 index,
		_In_opt_ ID2D1Image* input,
		BOOL invalidate
		);

	STDMETHOD(SetInputCount)(
		ID2D1Effect* This,
		UINT32 inputCount
		);

	STDMETHOD_(void, GetInput)(
		ID2D1Effect* This,
		UINT32 index,
		_Outptr_result_maybenull_ ID2D1Image** input
		);

	STDMETHOD_(UINT32, GetInputCount)(
		ID2D1Effect* This
		);

	STDMETHOD_(void, GetOutput)(
		ID2D1Effect* This,
		_Outptr_ ID2D1Image** outputImage
		);

	END_INTERFACE
} ID2D1EffectVtbl;

interface ID2D1Effect
{
	CONST_VTBL struct ID2D1EffectVtbl* lpVtbl;
};

typedef interface ID2D1TransformNode ID2D1TransformNode;

typedef struct ID2D1TransformNodeVtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			ID2D1TransformNode* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			_Out_ void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		ID2D1TransformNode* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		ID2D1TransformNode* This);

	STDMETHOD_(UINT32, GetInputCount)(
		ID2D1TransformNode* This
		);

	END_INTERFACE
} ID2D1TransformNodeVtbl;

interface ID2D1TransformNode
{
	CONST_VTBL struct ID2D1TransformNodeVtbl* lpVtbl;
};

typedef interface ID2D1TransformNode ID2D1BlendTransform;
typedef interface ID2D1TransformNode ID2D1BorderTransform;
typedef interface ID2D1TransformNode ID2D1OffsetTransform;
typedef interface ID2D1TransformNode ID2D1BoundsAdjustmentTransform;

typedef interface ID2D1DrawInfo ID2D1DrawInfo;

typedef interface ID2D1DrawInfoVtbl
{
	IUnknownVtbl Base;
	STDMETHOD(SetInputDescription)(ID2D1DrawInfo* This, UINT32 inputIndex, D2D1_INPUT_DESCRIPTION inputDescription);
	STDMETHOD(SetOutputBuffer)(ID2D1DrawInfo* This, D2D1_BUFFER_PRECISION bufferPrecision, D2D1_CHANNEL_DEPTH channelDepth);
	STDMETHOD_(void, SetCached)(ID2D1DrawInfo* This, BOOL isCached);
	STDMETHOD_(void, SetInstructionCountHint)(ID2D1DrawInfo* This, UINT32 instructionCount);
	STDMETHOD(SetPixelShaderConstantBuffer)(ID2D1DrawInfo* This, CONST BYTE* buffer, UINT32 bufferCount);
	STDMETHOD(SetResourceTexture)(ID2D1DrawInfo* This, UINT32 textureIndex, ID2D1ResourceTexture* resourceTexture);
	STDMETHOD(SetVertexShaderConstantBuffer)(ID2D1DrawInfo* This, CONST BYTE* buffer, UINT32 bufferCount);
	STDMETHOD(SetPixelShader)(ID2D1DrawInfo* This, REFGUID shaderId, D2D1_PIXEL_OPTIONS pixelOptions);
	STDMETHOD(SetVertexProcessing)(ID2D1DrawInfo* This, ID2D1VertexBuffer* vertexBuffer, D2D1_VERTEX_OPTIONS vertexOptions,
								   CONST D2D1_BLEND_DESCRIPTION* blendDescription, CONST D2D1_VERTEX_RANGE* vertexRange,
								   CONST GUID* vertexShader);
} ID2D1DrawInfoVtbl;

interface ID2D1DrawInfo
{
	CONST_VTBL struct ID2D1DrawInfoVtbl* lpVtbl;
};

typedef interface ID2D1DrawTransform ID2D1DrawTransform;

typedef interface ID2D1DrawTransformVtbl
{
	ID2D1TransformNodeVtbl node;
	STDMETHOD(MapOutputRectToInputRects)(ID2D1DrawTransform* Thi, CONST D2D1_RECT_L* outputRect, D2D1_RECT_L* inputRects,
										 UINT32 inputRectsCount);
	STDMETHOD(MapInputRectsToOutputRect)(ID2D1DrawTransform* Thi, CONST D2D1_RECT_L* inputRects, CONST D2D1_RECT_L* inputOpaqueSubRects,
										 UINT32 inputRectCount, D2D1_RECT_L* outputRect, D2D1_RECT_L* outputOpaqueSubRect);
	STDMETHOD(MapInvalidRect)(ID2D1DrawTransform* Thi, UINT32 inputIndex, D2D1_RECT_L invalidInputRect, D2D1_RECT_L* invalidOutputRect);
	STDMETHOD(SetDrawInfo)(ID2D1DrawTransform* Thi, ID2D1DrawInfo* drawInfo);
} ID2D1DrawTransformVtbl;

interface ID2D1DrawTransform
{
	CONST_VTBL struct ID2D1DrawTransformVtbl* lpVtbl;
};

typedef interface ID2D1TransformGraph ID2D1TransformGraph;

typedef struct ID2D1TransformGraphVtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			ID2D1TransformGraph* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			_Out_ void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		ID2D1TransformGraph* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		ID2D1TransformGraph* This);

	STDMETHOD_(UINT32, GetInputCount)(
		);

	STDMETHOD(SetSingleTransformNode)(
		ID2D1TransformGraph* This,
		_In_ ID2D1TransformNode* node
		);

	STDMETHOD(AddNode)(
		ID2D1TransformGraph* This,
		_In_ ID2D1TransformNode* node
		);

	STDMETHOD(RemoveNode)(
		ID2D1TransformGraph* This,
		_In_ ID2D1TransformNode* node
		);

	STDMETHOD(SetOutputNode)(
		ID2D1TransformGraph* This,
		_In_ ID2D1TransformNode* node
		);

	STDMETHOD(ConnectNode)(
		ID2D1TransformGraph* This,
		_In_ ID2D1TransformNode* fromNode,
		_In_ ID2D1TransformNode* toNode,
		UINT32 toNodeInputIndex
		);

	STDMETHOD(ConnectToEffectInput)(
		ID2D1TransformGraph* This,
		UINT32 toEffectInputIndex,
		_In_ ID2D1TransformNode* node,
		UINT32 toNodeInputIndex
		);

	STDMETHOD_(void, Clear)(
		ID2D1TransformGraph* This
		);

	STDMETHOD(SetPassthroughGraph)(
		ID2D1TransformGraph* This,
		UINT32 effectInputIndex
		);

	END_INTERFACE
} ID2D1TransformGraphVtbl;

interface ID2D1TransformGraph
{
	CONST_VTBL struct ID2D1TransformGraphVtbl* lpVtbl;
};

typedef interface ID2D1EffectContext ID2D1EffectContext;

typedef struct ID2D1EffectContextVtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			ID2D1EffectContext* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			_Out_ void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		ID2D1EffectContext* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		ID2D1EffectContext* This);

	void(STDMETHODCALLTYPE* GetDpi)(
		ID2D1EffectContext* This,
		_Out_ FLOAT* dpiX,
		_Out_ FLOAT* dpiY
		);

	HRESULT(STDMETHODCALLTYPE* CreateEffect)(
		ID2D1EffectContext* This,
		_In_ REFCLSID effectId,
		_COM_Outptr_ ID2D1Effect** effect
		);

	HRESULT(STDMETHODCALLTYPE* GetMaximumSupportedFeatureLevel)(
		ID2D1EffectContext* This,
		_In_reads_(featureLevelsCount) CONST D3D_FEATURE_LEVEL* featureLevels,
		UINT32 featureLevelsCount,
		_Out_ D3D_FEATURE_LEVEL* maximumSupportedFeatureLevel
		);

	HRESULT(STDMETHODCALLTYPE* CreateTransformNodeFromEffect)(
		ID2D1EffectContext* This,
		_In_ ID2D1Effect* effect,
		_COM_Outptr_ ID2D1TransformNode** transformNode
		);

	HRESULT(STDMETHODCALLTYPE* CreateBlendTransform)(
		ID2D1EffectContext* This,
		UINT32 numInputs,
		_In_ CONST D2D1_BLEND_DESCRIPTION* blendDescription,
		_COM_Outptr_ ID2D1BlendTransform** transform
		);

	HRESULT(STDMETHODCALLTYPE* CreateBorderTransform)(
		ID2D1EffectContext* This,
		D2D1_EXTEND_MODE extendModeX,
		D2D1_EXTEND_MODE extendModeY,
		_COM_Outptr_ ID2D1BorderTransform** transform
		);

	HRESULT(STDMETHODCALLTYPE* CreateOffsetTransform)(
		ID2D1EffectContext* This,
		D2D1_POINT_2L offset,
		_COM_Outptr_ ID2D1OffsetTransform** transform
		);

	HRESULT(STDMETHODCALLTYPE* CreateBoundsAdjustmentTransform)(
		ID2D1EffectContext* This,
		_In_ CONST D2D1_RECT_L* outputRectangle,
		_COM_Outptr_ ID2D1BoundsAdjustmentTransform** transform
		);

	HRESULT(STDMETHODCALLTYPE* LoadPixelShader)(
		ID2D1EffectContext* This,
		REFGUID shaderId,
		_In_reads_(shaderBufferCount) CONST BYTE* shaderBuffer,
		UINT32 shaderBufferCount
		);

	HRESULT(STDMETHODCALLTYPE* LoadVertexShader)(
		ID2D1EffectContext* This,
		REFGUID resourceId,
		_In_reads_(shaderBufferCount) CONST BYTE* shaderBuffer,
		UINT32 shaderBufferCount
		);

	HRESULT(STDMETHODCALLTYPE* LoadComputeShader)(
		ID2D1EffectContext* This,
		REFGUID resourceId,
		_In_reads_(shaderBufferCount) CONST BYTE* shaderBuffer,
		UINT32 shaderBufferCount
		);

	BOOL(STDMETHODCALLTYPE* IsShaderLoaded)(
		ID2D1EffectContext* This,
		REFGUID shaderId
		);

	HRESULT(STDMETHODCALLTYPE* CreateResourceTexture)(
		ID2D1EffectContext* This,
		_In_opt_ CONST GUID* resourceId,
		_In_ CONST D2D1_RESOURCE_TEXTURE_PROPERTIES* resourceTextureProperties,
		_In_reads_opt_(dataSize) CONST BYTE* data,
		_In_reads_opt_(resourceTextureProperties->dimensions - 1) CONST UINT32* strides,
		UINT32 dataSize,
		_COM_Outptr_ ID2D1ResourceTexture** resourceTexture
		);

	HRESULT(STDMETHODCALLTYPE* FindResourceTexture)(
		ID2D1EffectContext* This,
		_In_ CONST GUID* resourceId,
		_COM_Outptr_ ID2D1ResourceTexture** resourceTexture
		);

	HRESULT(STDMETHODCALLTYPE* CreateVertexBuffer)(
		ID2D1EffectContext* This,
		_In_ CONST D2D1_VERTEX_BUFFER_PROPERTIES* vertexBufferProperties,
		_In_opt_ CONST GUID* resourceId,
		_In_opt_ CONST D2D1_CUSTOM_VERTEX_BUFFER_PROPERTIES* customVertexBufferProperties,
		_COM_Outptr_ ID2D1VertexBuffer** buffer
		);

	HRESULT(STDMETHODCALLTYPE* FindVertexBuffer)(
		ID2D1EffectContext* This,
		_In_ CONST GUID* resourceId,
		_COM_Outptr_ ID2D1VertexBuffer** buffer
		);

	HRESULT(STDMETHODCALLTYPE* CreateColorContext)(
		ID2D1EffectContext* This,
		D2D1_COLOR_SPACE space,
		_In_reads_opt_(profileSize) CONST BYTE* profile,
		UINT32 profileSize,
		_COM_Outptr_ ID2D1ColorContext** colorContext
		);

	HRESULT(STDMETHODCALLTYPE* CreateColorContextFromFilename)(
		ID2D1EffectContext* This,
		_In_ PCWSTR filename,
		_COM_Outptr_ ID2D1ColorContext** colorContext
		);

	HRESULT(STDMETHODCALLTYPE* CreateColorContextFromWicColorContext)(
		ID2D1EffectContext* This,
		_In_ IWICColorContext* wicColorContext,
		_COM_Outptr_ ID2D1ColorContext** colorContext
		);

	HRESULT(STDMETHODCALLTYPE* CheckFeatureSupport)(
		ID2D1EffectContext* This,
		D2D1_FEATURE feature,
		_Out_writes_bytes_(featureSupportDataSize) void* featureSupportData,
		UINT32 featureSupportDataSize
		);

	BOOL(STDMETHODCALLTYPE* IsBufferPrecisionSupported)(
		ID2D1EffectContext* This,
		D2D1_BUFFER_PRECISION bufferPrecision
		);

	END_INTERFACE
} ID2D1EffectContextVtbl;

interface ID2D1EffectContext
{
	CONST_VTBL struct ID2D1EffectContextVtbl* lpVtbl;
};

typedef interface ID2D1EffectImpl ID2D1EffectImpl;

typedef struct ID2D1EffectImplVtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			ID2D1EffectImpl* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			_Out_ void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		ID2D1EffectImpl* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		ID2D1EffectImpl* This);

	HRESULT(STDMETHODCALLTYPE* Initialize)(
		ID2D1EffectImpl* This,
		_In_ ID2D1EffectContext* effectContext,
		_In_ ID2D1TransformGraph* transformGraph
		);

	HRESULT(STDMETHODCALLTYPE* PrepareForRender)(
		ID2D1EffectImpl* This,
		D2D1_CHANGE_TYPE changeType
		);

	HRESULT(STDMETHODCALLTYPE* SetGraph)(
		ID2D1EffectImpl* This,
		_In_ ID2D1TransformGraph* transformGraph
		);

	END_INTERFACE
} ID2D1EffectImplVtbl;

interface ID2D1EffectImpl
{
	CONST_VTBL struct ID2D1EffectImplVtbl* lpVtbl;
};

typedef HRESULT(CALLBACK* PD2D1_EFFECT_FACTORY)(
	_Outptr_ IUnknown** effectImpl
	);

typedef struct ID2D1Factory1Vtbl
{
	BEGIN_INTERFACE

	ID2D1FactoryVtbl Base;

	STDMETHOD(CreateDevice)(
		ID2D1Factory1* This,
		_In_ IDXGIDevice* dxgiDevice,
		_COM_Outptr_ ID2D1Device** d2dDevice
		);

	STDMETHOD(CreateStrokeStyle1)(
		ID2D1Factory1* This,
		_In_ CONST D2D1_STROKE_STYLE_PROPERTIES1* strokeStyleProperties,
		_In_reads_opt_(dashesCount) CONST FLOAT* dashes,
		UINT32 dashesCount,
		_COM_Outptr_ ID2D1StrokeStyle1** strokeStyle
		);

	STDMETHOD(CreatePathGeometry1)(
		ID2D1Factory1* This,
		_COM_Outptr_ ID2D1PathGeometry1** pathGeometry
		);

	STDMETHOD(CreateDrawingStateBlock1)(
		ID2D1Factory1* This,
		_In_opt_ CONST D2D1_DRAWING_STATE_DESCRIPTION1* drawingStateDescription,
		_In_opt_ IDWriteRenderingParams* textRenderingParams,
		_COM_Outptr_ ID2D1DrawingStateBlock1** drawingStateBlock
		);

	STDMETHOD(CreateGdiMetafile)(
		ID2D1Factory1* This,
		_In_ IStream* metafileStream,
		_COM_Outptr_ ID2D1GdiMetafile** metafile
		);

	STDMETHOD(RegisterEffectFromStream)(
		ID2D1Factory1* This,
		_In_ REFCLSID classId,
		_In_ IStream* propertyXml,
		_In_reads_opt_(bindingsCount) CONST D2D1_PROPERTY_BINDING* bindings,
		UINT32 bindingsCount,
		_In_ CONST PD2D1_EFFECT_FACTORY effectFactory
		);

	STDMETHOD(RegisterEffectFromString)(
		ID2D1Factory1* This,
		_In_ REFCLSID classId,
		_In_ PCWSTR propertyXml,
		_In_reads_opt_(bindingsCount) CONST D2D1_PROPERTY_BINDING* bindings,
		UINT32 bindingsCount,
		_In_ CONST PD2D1_EFFECT_FACTORY effectFactory
		);

	STDMETHOD(UnregisterEffect)(
		ID2D1Factory1* This,
		_In_ REFCLSID classId
		);

	STDMETHOD(GetRegisteredEffects)(
		ID2D1Factory1* This,
		_Out_writes_to_opt_(effectsCount, *effectsReturned) CLSID* effects,
		UINT32 effectsCount,
		_Out_opt_ UINT32* effectsReturned,
		_Out_opt_ UINT32* effectsRegistered
		);

	STDMETHOD(GetEffectProperties)(
		ID2D1Factory1* This,
		_In_ REFCLSID effectId,
		_COM_Outptr_ ID2D1Properties** properties
		);

	END_INTERFACE
} ID2D1Factory1Vtbl;

interface ID2D1Factory1
{
	CONST_VTBL struct ID2D1Factory1Vtbl* lpVtbl;
};

typedef struct ID2D1DeviceVtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			ID2D1Device* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			_Out_ void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		ID2D1Device* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		ID2D1Device* This);

	STDMETHOD_(void, GetFactory)(
		ID2D1Device* This,
		_Outptr_ ID2D1Factory** factory
		);

	STDMETHOD(CreateDeviceContext)(
		ID2D1Device* This,
		D2D1_DEVICE_CONTEXT_OPTIONS options,
		_COM_Outptr_ ID2D1DeviceContext** deviceContext
		);

	STDMETHOD(CreatePrintControl)(
		ID2D1Device* This,
		_In_ IWICImagingFactory* wicFactory,
		_In_ IPrintDocumentPackageTarget* documentTarget,
		_In_opt_ CONST D2D1_PRINT_CONTROL_PROPERTIES* printControlProperties,
		_COM_Outptr_ ID2D1PrintControl** printControl
		);

	STDMETHOD_(void, SetMaximumTextureMemory)(
		ID2D1Device* This,
		UINT64 maximumInBytes
		);

	STDMETHOD_(UINT64, GetMaximumTextureMemory)(
		ID2D1Device* This
		);

	STDMETHOD_(void, ClearResources)(
		ID2D1Device* This,
		UINT32 millisecondsSinceUse
		);

	END_INTERFACE
} ID2D1DeviceVtbl;

interface ID2D1Device
{
	CONST_VTBL struct ID2D1DeviceVtbl* lpVtbl;
};

typedef struct ID2D1DeviceContextVtbl
{
	BEGIN_INTERFACE

		ID2D1RenderTargetVtbl Base;

	STDMETHOD(CreateBitmap1)(
		ID2D1DeviceContext* This,
		D2D1_SIZE_U size,
		_In_opt_ CONST void* sourceData,
		UINT32 pitch,
		_In_ CONST D2D1_BITMAP_PROPERTIES1* bitmapProperties,
		_COM_Outptr_ ID2D1Bitmap1** bitmap
		);

	STDMETHOD(CreateBitmapFromWicBitmap1)(
		ID2D1DeviceContext* This,
		_In_ IWICBitmapSource* wicBitmapSource,
		_In_opt_ CONST D2D1_BITMAP_PROPERTIES1* bitmapProperties,
		_COM_Outptr_ ID2D1Bitmap1** bitmap
		);

	STDMETHOD(CreateColorContext)(
		ID2D1DeviceContext* This,
		D2D1_COLOR_SPACE space,
		_In_reads_opt_(profileSize) CONST BYTE* profile,
		UINT32 profileSize,
		_COM_Outptr_ ID2D1ColorContext** colorContext
		);

	STDMETHOD(CreateColorContextFromFilename)(
		ID2D1DeviceContext* This,
		_In_ PCWSTR filename,
		_COM_Outptr_ ID2D1ColorContext** colorContext
		);

	STDMETHOD(CreateColorContextFromWicColorContext)(
		ID2D1DeviceContext* This,
		_In_ IWICColorContext* wicColorContext,
		_COM_Outptr_ ID2D1ColorContext** colorContext
		);

	STDMETHOD(CreateBitmapFromDxgiSurface)(
		ID2D1DeviceContext* This,
		_In_ IDXGISurface* surface,
		_In_opt_ CONST D2D1_BITMAP_PROPERTIES1* bitmapProperties,
		_COM_Outptr_ ID2D1Bitmap1** bitmap
		);

	STDMETHOD(CreateEffect)(
		ID2D1DeviceContext* This,
		_In_ REFCLSID effectId,
		_COM_Outptr_ ID2D1Effect** effect
		);

	STDMETHOD(CreateGradientStopCollection1)(
		ID2D1DeviceContext* This,
		_In_reads_(straightAlphaGradientStopsCount) CONST D2D1_GRADIENT_STOP* straightAlphaGradientStops,
		_In_range_(>= , 1) UINT32 straightAlphaGradientStopsCount,
		D2D1_COLOR_SPACE preInterpolationSpace,
		D2D1_COLOR_SPACE postInterpolationSpace,
		D2D1_BUFFER_PRECISION bufferPrecision,
		D2D1_EXTEND_MODE extendMode,
		D2D1_COLOR_INTERPOLATION_MODE colorInterpolationMode,
		_COM_Outptr_ ID2D1GradientStopCollection1** gradientStopCollection1
		);

	STDMETHOD(CreateImageBrush)(
		ID2D1DeviceContext* This,
		_In_opt_ ID2D1Image* image,
		_In_ CONST D2D1_IMAGE_BRUSH_PROPERTIES* imageBrushProperties,
		_In_opt_ CONST D2D1_BRUSH_PROPERTIES* brushProperties,
		_COM_Outptr_ ID2D1ImageBrush** imageBrush
		);

	STDMETHOD(CreateBitmapBrush1)(
		ID2D1DeviceContext* This,
		_In_opt_ ID2D1Bitmap* bitmap,
		_In_opt_ CONST D2D1_BITMAP_BRUSH_PROPERTIES1* bitmapBrushProperties,
		_In_opt_ CONST D2D1_BRUSH_PROPERTIES* brushProperties,
		_COM_Outptr_ ID2D1BitmapBrush1** bitmapBrush
		);

	STDMETHOD(CreateCommandList)(
		ID2D1DeviceContext* This,
		_COM_Outptr_ ID2D1CommandList** commandList
		);

	STDMETHOD_(BOOL, IsDxgiFormatSupported)(
		ID2D1DeviceContext* This,
		DXGI_FORMAT format
		);

	STDMETHOD_(BOOL, IsBufferPrecisionSupported)(
		ID2D1DeviceContext* This,
		D2D1_BUFFER_PRECISION bufferPrecision
		);

	STDMETHOD(GetImageLocalBounds)(
		ID2D1DeviceContext* This,
		_In_ ID2D1Image* image,
		_Out_ D2D1_RECT_F* localBounds
		);

	STDMETHOD(GetImageWorldBounds)(
		ID2D1DeviceContext* This,
		_In_ ID2D1Image* image,
		_Out_ D2D1_RECT_F* worldBounds
		);

	STDMETHOD(GetGlyphRunWorldBounds)(
		ID2D1DeviceContext* This,
		D2D1_POINT_2F baselineOrigin,
		_In_ CONST DWRITE_GLYPH_RUN* glyphRun,
		DWRITE_MEASURING_MODE measuringMode,
		_Out_ D2D1_RECT_F* bounds
		);

	STDMETHOD_(void, GetDevice)(
		ID2D1DeviceContext* This,
		_Outptr_ ID2D1Device** device
		);

	STDMETHOD_(void, SetTarget)(
		ID2D1DeviceContext* This,
		_In_opt_ ID2D1Image* image
		);

	STDMETHOD_(void, GetTarget)(
		ID2D1DeviceContext* This,
		_Outptr_result_maybenull_ ID2D1Image** image
		);

	STDMETHOD_(void, SetRenderingControls)(
		ID2D1DeviceContext* This,
		_In_ CONST D2D1_RENDERING_CONTROLS* renderingControls
		);

	STDMETHOD_(void, GetRenderingControls)(
		ID2D1DeviceContext* This,
		_Out_ D2D1_RENDERING_CONTROLS* renderingControls
		);

	STDMETHOD_(void, SetPrimitiveBlend)(
		ID2D1DeviceContext* This,
		D2D1_PRIMITIVE_BLEND primitiveBlend
		);

	STDMETHOD_(D2D1_PRIMITIVE_BLEND, GetPrimitiveBlend)(
		ID2D1DeviceContext* This
		);

	STDMETHOD_(void, SetUnitMode)(
		ID2D1DeviceContext* This,
		D2D1_UNIT_MODE unitMode
		);

	STDMETHOD_(D2D1_UNIT_MODE, GetUnitMode)(
		ID2D1DeviceContext* This
		);

	STDMETHOD_(void, DrawGlyphRun1)(
		ID2D1DeviceContext* This,
		D2D1_POINT_2F baselineOrigin,
		_In_ CONST DWRITE_GLYPH_RUN* glyphRun,
		_In_opt_ CONST DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
		_In_ ID2D1Brush* foregroundBrush,
		DWRITE_MEASURING_MODE measuringMode
		);

	STDMETHOD_(void, DrawImage)(
		ID2D1DeviceContext* This,
		_In_ ID2D1Image* image,
		_In_opt_ CONST D2D1_POINT_2F* targetOffset,
		_In_opt_ CONST D2D1_RECT_F* imageRectangle,
		D2D1_INTERPOLATION_MODE interpolationMode,
		D2D1_COMPOSITE_MODE compositeMode
		);

	STDMETHOD_(void, DrawGdiMetafile)(
		ID2D1DeviceContext* This,
		_In_ ID2D1GdiMetafile* gdiMetafile,
		_In_opt_ CONST D2D1_POINT_2F* targetOffset
		);

	STDMETHOD_(void, DrawBitmap1)(
		ID2D1DeviceContext* This,
		_In_ ID2D1Bitmap* bitmap,
		_In_opt_ CONST D2D1_RECT_F* destinationRectangle,
		FLOAT opacity,
		D2D1_INTERPOLATION_MODE interpolationMode,
		_In_opt_ CONST D2D1_RECT_F* sourceRectangle,
		_In_opt_ CONST D2D1_MATRIX_4X4_F* perspectiveTransform
		);

	STDMETHOD_(void, PushLayer1)(
		ID2D1DeviceContext* This,
		_In_ CONST D2D1_LAYER_PARAMETERS1* layerParameters,
		_In_opt_ ID2D1Layer* layer
		);

	STDMETHOD(InvalidateEffectInputRectangle)(
		ID2D1DeviceContext* This,
		_In_ ID2D1Effect* effect,
		UINT32 input,
		_In_ CONST D2D1_RECT_F* inputRectangle
		);

	STDMETHOD(GetEffectInvalidRectangleCount)(
		ID2D1DeviceContext* This,
		_In_ ID2D1Effect* effect,
		_Out_ UINT32* rectangleCount
		);

	STDMETHOD(GetEffectInvalidRectangles)(
		ID2D1DeviceContext* This,
		_In_ ID2D1Effect* effect,
		_Out_writes_(rectanglesCount) D2D1_RECT_F* rectangles,
		UINT32 rectanglesCount
		);

	STDMETHOD(GetEffectRequiredInputRectangles)(
		ID2D1DeviceContext* This,
		_In_ ID2D1Effect* renderEffect,
		_In_opt_ CONST D2D1_RECT_F* renderImageRectangle,
		_In_reads_(inputCount) CONST D2D1_EFFECT_INPUT_DESCRIPTION* inputDescriptions,
		_Out_writes_(inputCount) D2D1_RECT_F* requiredInputRects,
		UINT32 inputCount
		);

	STDMETHOD_(void, FillOpacityMask1)(
		ID2D1DeviceContext* This,
		_In_ ID2D1Bitmap* opacityMask,
		_In_ ID2D1Brush* brush,
		_In_opt_ CONST D2D1_RECT_F* destinationRectangle,
		_In_opt_ CONST D2D1_RECT_F* sourceRectangle
		);

	END_INTERFACE
} ID2D1DeviceContextVtbl;

interface ID2D1DeviceContext
{
	CONST_VTBL struct ID2D1DeviceContextVtbl* lpVtbl;
};

// color.c

typedef struct ID2D1ColorContextVtbl
{
	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			ID2D1ColorContext* This,
			/* [in] */ REFIID riid,
			/* [annotation][iid_is][out] */
			_Out_ void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		ID2D1ColorContext* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		ID2D1ColorContext* This);

	STDMETHOD_(void, GetFactory)(
		ID2D1ColorContext* This,
		_Outptr_ ID2D1Factory** factory
		);

	STDMETHOD_(D2D1_COLOR_SPACE, GetColorSpace)(
		ID2D1ColorContext* This
		);

	STDMETHOD_(UINT32, GetProfileSize)(
		ID2D1ColorContext* This
		);

	STDMETHOD(GetProfile)(
		ID2D1ColorContext* This,
		_Out_writes_(profileSize) BYTE* profile,
		UINT32 profileSize
		);

	END_INTERFACE
} ID2D1ColorContextVtbl;

typedef struct ID2D1ColorContext
{
	const ID2D1ColorContextVtbl* lpVtbl;
} ID2D1ColorContext;


//
// GUIDs
//
DEFINE_GUID(IID_IDWriteFactory3, 0x9a1b41c3, 0xd3bb, 0x466a, 0x87, 0xfc, 0xfe, 0x67, 0x55, 0x6a, 0x3b, 0x65);
DEFINE_GUID(IID_IDWriteTextLayout3, 0x07ddcd52, 0x020e, 0x4de8, 0xac, 0x33, 0x6c, 0x95, 0x3d, 0x83, 0xf9, 0x2d);
DEFINE_GUID(IID_IDWriteTextFormat3, 0x6d3b5641, 0xe550, 0x430d, 0xa8, 0x5b, 0xb7, 0xbf, 0x48, 0xa9, 0x34, 0x27);
DEFINE_GUID(IID_IDWriteFontFace2, 0xd8b768ff, 0x64bc, 0x4e66, 0x98, 0x2b, 0xec, 0x8e, 0x87, 0xf6, 0x93, 0xf7);
DEFINE_GUID(IID_IDWriteFontFace3, 0xd37d7598, 0x09be, 0x4222, 0xa2, 0x36, 0x20, 0x81, 0x34, 0x1c, 0xc1, 0xf2);
DEFINE_GUID(IID_IDWriteTextRenderer, 0xef8a8135, 0x5cc6, 0x45fe, 0x88, 0x25, 0xc5, 0xa0, 0x72, 0x4e, 0xb8, 0x19);
DEFINE_GUID(IID_IDWritePixelSnapping, 0xeaf3a2da, 0xecf4, 0x4d24, 0xb6, 0x44, 0xb3, 0x4f, 0x68, 0x42, 0x02, 0x4b);

DEFINE_GUID(IID_ID2D1Factory, 0x2cd90691, 0x12e2, 0x11dc, 0x9f, 0xed, 0x00, 0x11, 0x43, 0xa0, 0x55, 0xf9);
DEFINE_GUID(IID_ID2D1Factory1, 0xbb12d362, 0xdaee, 0x4b9a, 0xaa, 0x1d, 0x14, 0xba, 0x40, 0x1c, 0xfa, 0x1f);
DEFINE_GUID(IID_ID2D1Factory2, 0x94f81a73, 0x9212, 0x4376, 0x9c, 0x58, 0xb1, 0x6a, 0x3a, 0x0d, 0x39, 0x92);
DEFINE_GUID(IID_ID2D1Factory3, 0x0869759f, 0x4f00, 0x413f, 0xb0, 0x3e, 0x2b, 0xda, 0x45, 0x40, 0x4d, 0x0f);
DEFINE_GUID(IID_ID2D1Factory4, 0xbd4ec2d2, 0x0662, 0x4bee, 0xba, 0x8e, 0x6f, 0x29, 0xf0, 0x32, 0xe0, 0x96);
DEFINE_GUID(IID_ID2D1Factory5, 0xc4349994, 0x838e, 0x4b0f, 0x8c, 0xab, 0x44, 0x99, 0x7d, 0x9e, 0xea, 0xcc);
DEFINE_GUID(IID_ID2D1Factory6, 0xf9976f46, 0xf642, 0x44c1, 0x97, 0xca, 0xda, 0x32, 0xea, 0x2a, 0x26, 0x35);
DEFINE_GUID(IID_ID2D1Factory7, 0xbdc2bdd3, 0xb96c, 0x4de6, 0xbd, 0xf7, 0x99, 0xd4, 0x74, 0x54, 0x54, 0xde);

DEFINE_GUID(IID_ID2D1Device, 0x47dd575d, 0xac05, 0x4cdd, 0x80, 0x49, 0x9b, 0x02, 0xcd, 0x16, 0xf4, 0x4c);
DEFINE_GUID(IID_ID2D1Device1, 0xd21768e1, 0x23a4, 0x4823, 0xa1, 0x4b, 0x7c, 0x3e, 0xba, 0x85, 0xd6, 0x58);
DEFINE_GUID(IID_ID2D1Device2, 0xa44472e1, 0x8dfb, 0x4e60, 0x84, 0x92, 0x6e, 0x28, 0x61, 0xc9, 0xca, 0x8b);
DEFINE_GUID(IID_ID2D1Device3, 0x852f2087, 0x802c, 0x4037, 0xab, 0x60, 0xff, 0x2e, 0x7e, 0xe6, 0xfc, 0x01);
DEFINE_GUID(IID_ID2D1Device4, 0xd7bdb159, 0x5683, 0x4a46, 0xbc, 0x9c, 0x72, 0xdc, 0x72, 0x0b, 0x85, 0x8b);
DEFINE_GUID(IID_ID2D1Device5, 0xd55ba0a4, 0x6405, 0x4694, 0xae, 0xf5, 0x08, 0xee, 0x1a, 0x43, 0x58, 0xb4);
DEFINE_GUID(IID_ID2D1Device6, 0x7bfef914, 0x2d75, 0x4bad, 0xbe, 0x87, 0xe1, 0x8d, 0xdb, 0x07, 0x7b, 0x6d);

DEFINE_GUID(IID_ID2D1DeviceContext, 0xe8f7fe7a, 0x191c, 0x466d, 0xad, 0x95, 0x97, 0x56, 0x78, 0xbd, 0xa9, 0x98);
DEFINE_GUID(IID_ID2D1DeviceContext1, 0xd37f57e4, 0x6908, 0x459f, 0xa1, 0x99, 0xe7, 0x2f, 0x24, 0xf7, 0x99, 0x87);
DEFINE_GUID(IID_ID2D1DeviceContext2, 0x394ea6a3, 0x0c34, 0x4321, 0x95, 0x0b, 0x6c, 0xa2, 0x0f, 0x0b, 0xe6, 0xc7);
DEFINE_GUID(IID_ID2D1DeviceContext3, 0x235a7496, 0x8351, 0x414c, 0xbc, 0xd4, 0x66, 0x72, 0xab, 0x2d, 0x8e, 0x00);
DEFINE_GUID(IID_ID2D1DeviceContext4, 0x8c427831, 0x3d90, 0x4476, 0xb6, 0x47, 0xc4, 0xfa, 0xe3, 0x49, 0xe4, 0xdb);
DEFINE_GUID(IID_ID2D1DeviceContext5, 0x7836d248, 0x68cc, 0x4df6, 0xb9, 0xe8, 0xde, 0x99, 0x1b, 0xf6, 0x2e, 0xb7);
DEFINE_GUID(IID_ID2D1DeviceContext6, 0x985f7e37, 0x4ed0, 0x4a19, 0x98, 0xa3, 0x15, 0xb0, 0xed, 0xfd, 0xe3, 0x06);

DEFINE_GUID(IID_ID2D1EffectImpl, 0xa248fd3f, 0x3e6c, 0x4e63, 0x9f, 0x03, 0x7f, 0x68, 0xec, 0xc9, 0x1d, 0xb9);
DEFINE_GUID(IID_ID2D1Transform, 0xef1a287d, 0x342a, 0x4f76, 0x8f, 0xdb, 0xda, 0x0d, 0x6e, 0xa9, 0xf9, 0x2b);
DEFINE_GUID(IID_ID2D1TransformNode, 0xb2efe1e7, 0x729f, 0x4102, 0x94, 0x9f, 0x50, 0x5f, 0xa2, 0x1b, 0xf6, 0x66);
DEFINE_GUID(IID_ID2D1DrawTransform, 0x36bfdcb6, 0x9739, 0x435d, 0xa3, 0x0d, 0xa6, 0x53, 0xbe, 0xff, 0x6a, 0x6f);
DEFINE_GUID(IID_ID2D1Effect, 0x28211a43, 0x7d89, 0x476f, 0x81, 0x81, 0x2d, 0x61, 0x59, 0xb2, 0x20, 0xad);
DEFINE_GUID(IID_ID2D1EffectContext, 0x3d9f916b, 0x27dc, 0x4ad7, 0xb4, 0xf1, 0x64, 0x94, 0x53, 0x40, 0xf5, 0x63);
DEFINE_GUID(IID_ID2D1EffectContext1, 0x84ab595a, 0xfc81, 0x4546, 0xba, 0xcd, 0xe8, 0xef, 0x4d, 0x8a, 0xbe, 0x7a);
DEFINE_GUID(IID_ID2D1EffectContext2, 0x577ad2a0, 0x9fc7, 0x4dda, 0x8b, 0x18, 0xda, 0xb8, 0x10, 0x14, 0x00, 0x52);

DEFINE_GUID(IID_ID2D1Multithread, 0x31e6e7bc, 0xe0ff, 0x4d46, 0x8c, 0x64, 0xa0, 0xa8, 0xc4, 0x1c, 0x15, 0xd3);

DEFINE_GUID(IID_ID3D11Device, 0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40);
DEFINE_GUID(IID_IDXGIDevice2, 0x05008617, 0xfbfd, 0x4051, 0xa7, 0x90, 0x14, 0x48, 0x84, 0xb4, 0xf6, 0xa9);
DEFINE_GUID(IID_ID2D1ColorContext, 0x1c4820bb, 0x5771, 0x4518, 0xa5, 0x81, 0x2f, 0xe4, 0xdd, 0x0e, 0xc6, 0x57);
DEFINE_GUID(IID_ID2D1ColorContext1, 0x1ab42875, 0xc57f, 0x4be9, 0xbd, 0x85, 0x9c, 0xd7, 0x8d, 0x6f, 0x55, 0xee);
DEFINE_GUID(IID_ID2D1GeometryRealization, 0xa16907d7, 0xbc02, 0x4801, 0x99, 0xe8, 0x8c, 0xf7, 0xf4, 0x85, 0xf7, 0x74);

// Built in effect CLSIDs
DEFINE_GUID(CLSID_D2D1Contrast, 0xb648a78a, 0x0ed5, 0x4f80, 0xa9, 0x4a, 0x8e, 0x82, 0x5a, 0xca, 0x6b, 0x77);
DEFINE_GUID(CLSID_D2D1Emboss, 0xb1c5eb2b, 0x0348, 0x43f0, 0x81, 0x07, 0x49, 0x57, 0xca, 0xcb, 0xa2, 0xae);
DEFINE_GUID(CLSID_D2D1Invert, 0xe0c3784d, 0xcb39, 0x4e84, 0xb6, 0xfd, 0x6b, 0x72, 0xf0, 0x81, 0x02, 0x63);
DEFINE_GUID(CLSID_D2D1Exposure, 0xb56c8cfa, 0xf634, 0x41ee, 0xbe, 0xe0, 0xff, 0xa6, 0x17, 0x10, 0x60, 0x04);
DEFINE_GUID(CLSID_D2D1Grayscale, 0x36DDE0EB, 0x3725, 0x42E0, 0x83, 0x6D, 0x52, 0xFB, 0x20, 0xAE, 0xE6, 0x44);
DEFINE_GUID(CLSID_D2D1Opacity, 0x811d79a4, 0xde28, 0x4454, 0x80, 0x94, 0xc6, 0x46, 0x85, 0xf8, 0xbd, 0x4c);
DEFINE_GUID(CLSID_D2D1AlphaMask, 0xc80ecff0, 0x3fd5, 0x4f05, 0x83, 0x28, 0xc5, 0xd1, 0x72, 0x4b, 0x4f, 0x0a);
DEFINE_GUID(CLSID_D2D1RgbToHue, 0x23f3e5ec, 0x91e8, 0x4d3d, 0xad, 0x0a, 0xaf, 0xad, 0xc1, 0x00, 0x4a, 0xa1);
DEFINE_GUID(CLSID_D2D1HueToRgb, 0x7b78a6bd, 0x0141, 0x4def, 0x8a, 0x52, 0x63, 0x56, 0xee, 0x0c, 0xbd, 0xd5);
DEFINE_GUID(CLSID_D2D1TemperatureTint, 0x89176087, 0x8AF9, 0x4A08, 0xAE, 0xB1, 0x89, 0x5F, 0x38, 0xDB, 0x17, 0x66);
DEFINE_GUID(CLSID_D2D1Tint, 0x36312b17, 0xf7dd, 0x4014, 0x91, 0x5d, 0xff, 0xca, 0x76, 0x8c, 0xf2, 0x11);
DEFINE_GUID(CLSID_D2D1CrossFade, 0x12f575e8, 0x4db1, 0x485f, 0x9a, 0x84, 0x03, 0xa0, 0x7d, 0xd3, 0x82, 0x9f);
DEFINE_GUID(CLSID_D2D1Sepia, 0x3a1af410, 0x5f1d, 0x4dbe, 0x84, 0xdf, 0x91, 0x5d, 0xa7, 0x9b, 0x71, 0x53);
DEFINE_GUID(CLSID_D2D1WhiteLevelAdjustment, 0x44a1cadb, 0x6cdd, 0x4818, 0x8f, 0xf4, 0x26, 0xc1, 0xcf, 0xe9, 0x5b, 0xdb);

DEFINE_GUID(CLSID_D2D1ColorManagement, 0x1A28524C, 0xFDD6, 0x4AA4, 0xAE, 0x8F, 0x83, 0x7E, 0xB8, 0x26, 0x7B, 0x37);
DEFINE_GUID(CLSID_D2D1DistantDiffuse, 0x3e7efd62, 0xa32d, 0x46d4, 0xa8, 0x3c, 0x52, 0x78, 0x88, 0x9a, 0xc9, 0x54);
DEFINE_GUID(CLSID_D2D1Composite, 0x48fc9f51, 0xf6ac, 0x48f1, 0x8b, 0x58, 0x3b, 0x28, 0xac, 0x46, 0xf7, 0x6d);
DEFINE_GUID(CLSID_D2D1ColorMatrix, 0x921f03d6, 0x641c, 0x47df, 0x85, 0x2d, 0xb4, 0xbb, 0x61, 0x53, 0xae, 0x11);
DEFINE_GUID(CLSID_D2D1ArithmeticComposite, 0xfc151437, 0x049a, 0x4784, 0xa2, 0x4a, 0xf1, 0xc4, 0xda, 0xf2, 0x09, 0x87);



typedef HRESULT(WINAPI* D2D1DX_D2D1CreateFactory_PFN)(
	D2D1_FACTORY_TYPE			factoryType,
	REFIID						riid,
	const D2D1_FACTORY_OPTIONS* pFactoryOptions,
	void** ppIFactory
	);

typedef enum _DWRITE_FACTORY_TYPE
{
	DWRITE_FACTORY_TYPE_SHARED,
	DWRITE_FACTORY_TYPE_ISOLATED
} TYPEDEF_TYPE_NAME(DWRITE_FACTORY_TYPE);

DECLSPEC_IMPORT HRESULT WINAPI DWriteCoreCreateFactory(
	IN	DWRITE_FACTORY_TYPE	factoryType,
	IN	REFIID              iid,
	OUT	IUnknown** factory);