#pragma once

#include <KexComm.h>
#include <ShlDisp.h>

#ifndef KXCOMAPI
#  define KXCOMAPI
#else
#  ifdef KXCOM_WANT_INITGUID
#    include <InitGuid.h>
#  endif
#endif

#define EXCEPTION_RO_ORIGINATEERROR		0x40080201
#define EXCEPTION_RO_TRANSFORMERROR		0x40080202

#define E_STRING_NOT_NULL_TERMINATED	((HRESULT) 0x80000017L)
#define E_BOUNDS						((HRESULT) 0x8000000BL)
#define HRESULT_ARITHMETIC_OVERFLOW		((HRESULT) 0x80070216L)

#define WRHF_STRING_BUFFER_MAGIC						0xF8B1A8BE

#define WRHF_NONE										0x0
#define WRHF_STRING_REFERENCE							0x1
#define WRHF_VALID_UNICODE_FORMAT_INFO					0x2
#define WRHF_WELL_FORMED_UNICODE						0x4
#define WRHF_HAS_EMBEDDED_NULLS							0x8
#define WRHF_EMBEDDED_NULLS_COMPUTED					0x10
#define WRHF_RESERVED_FOR_PREALLOCATED_STRING_BUFFER	0x80000000

typedef enum _DISPATCHERQUEUE_THREAD_TYPE
{
	DQTYPE_THREAD_DEDICATED = 1,
	DQTYPE_THREAD_CURRENT = 2
} TYPEDEF_TYPE_NAME(DISPATCHERQUEUE_THREAD_TYPE);

typedef enum _DISPATCHERQUEUE_THREAD_APARTMENTTYPE
{
	DQTAT_COM_NONE = 0,
	DQTAT_COM_ASTA = 1,
	DQTAT_COM_STA = 2
} TYPEDEF_TYPE_NAME(DISPATCHERQUEUE_THREAD_APARTMENTTYPE);

typedef struct _DispatcherQueueOptions
{
	DWORD									dwSize;
	DISPATCHERQUEUE_THREAD_TYPE				threadType;
	DISPATCHERQUEUE_THREAD_APARTMENTTYPE	apartmentType;
} TYPEDEF_TYPE_NAME(DispatcherQueueOptions);

typedef struct _HSTRING_HEADER
{
	ULONG	Flags;		// WRHF_*
	ULONG	Length;		// Does not include null terminator
	ULONG	Padding1;
	ULONG	Padding2;
	PCWSTR	StringRef;
} HSTRING_HEADER, * HSTRING;

typedef struct _HSTRING_ALLOCATED
{
	HSTRING_HEADER	Header;
	VOLATILE LONG	RefCount;
	WCHAR			Data[1];
} HSTRING_ALLOCATED;

typedef enum _RO_INIT_TYPE
{
	RO_INIT_SINGLETHREADED,
	RO_INIT_MULTITHREADED
} TYPEDEF_TYPE_NAME(RO_INIT_TYPE);

typedef enum
{
	RO_ERROR_REPORTING_NONE = 0x00000000,
	RO_ERROR_REPORTING_SUPPRESSEXCEPTIONS = 0x00000001,
	RO_ERROR_REPORTING_FORCEEXCEPTIONS = 0x00000002,
	RO_ERROR_REPORTING_USESETERRORINFO = 0x00000004,
	RO_ERROR_REPORTING_SUPPRESSSETERRORINFO = 0x00000008,
} RO_ERROR_REPORTING_FLAGS;

typedef HSTRING_ALLOCATED* HSTRING_BUFFER, ** PHSTRING_BUFFER;

typedef HANDLE TYPEDEF_TYPE_NAME(CO_MTA_USAGE_COOKIE);

typedef enum _TrustLevel
{
	BaseTrust,
	PartialTrust,
	FullTrust
} TrustLevel;

typedef enum _DayOfWeek
{
	DayOfWeek_Sunday = 0,
	DayOfWeek_Monday = 1,
	DayOfWeek_Tuesday = 2,
	DayOfWeek_Wednesday = 3,
	DayOfWeek_Thursday = 4,
	DayOfWeek_Friday = 5,
	DayOfWeek_Saturday = 6,
} DayOfWeek;

typedef enum WorkItemOptions
{
	WorkItemOptions_None = 0x0,
	WorkItemOptions_TimeSliced = 0x1
} WorkItemOptions;

typedef enum WorkItemPriority
{
	WorkItemPriority_Low = -1,
	WorkItemPriority_Normal = 0,
	WorkItemPriority_High = 1
} WorkItemPriority;

typedef struct _EventRegistrationToken
{
	UINT value;
} EventRegistrationToken;

// {82BA7092-4C88-427D-A7BC-16DD93FEB67E}
DEFINE_GUID(IID_IRestrictedErrorInfo, 0x82BA7092, 0x4C88, 0x427D, 0xA7, 0xBC, 0x16, 0xDD, 0x93, 0xFE, 0xB6, 0x7E);

// {66818B96-DC17-4C12-8CA1-8E1FBAA5BF80}
DEFINE_GUID(IID_IInternalErrorInfo, 0x66818B96, 0xDC17, 0x4C12, 0x8C, 0xA1, 0x8E, 0x1F, 0xBA, 0xA5, 0xBF, 0x80);

// {94EA2B94-E9CC-49E0-C0FF-EE64CA8F5B90}
DEFINE_GUID(IID_IAgileObject, 0x94EA2B94, 0xE9CC, 0x49E0, 0xC0, 0xFF, 0xEE, 0x64, 0xCA, 0x8F, 0x5B, 0x90);

// {00000035-0000-0000-C000-000000000046}
DEFINE_GUID(IID_IActivationFactory, 0x00000035, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);

// {AF86E2E0-B12D-4C6A-9C5A-D7AA65101E90}
DEFINE_GUID(IID_IInspectable, 0xAF86E2E0, 0xB12D, 0x4C6A, 0x9C, 0x5A, 0xD7, 0xAA, 0x65, 0x10, 0x1E, 0x90);

// {BBE1FA4C-B0E3-4583-BAEF-1F1B2E483E56}
DEFINE_GUID(IID_IVectorView, 0xBBE1FA4C, 0xB0E3, 0x4583, 0xBA, 0xEF, 0x1F, 0x1B, 0x2E, 0x48, 0x3E, 0x56);

// {01BF4326-ED37-4E96-B0E9-C1340D1EA158}
DEFINE_GUID(IID_IGlobalizationPreferencesStatics, 0x01BF4326, 0xED37, 0x4E96, 0xB0, 0xE9, 0xC1, 0x34, 0x0D, 0x1E, 0xA1, 0x58);

// {3694DBF9-8F68-44BE-8FF5-195C98EDE8A6}
DEFINE_GUID(IID_IUIViewSettingsInterop, 0x3694DBF9, 0x8F68, 0x44BE, 0x8F, 0xF5, 0x19, 0x5C, 0x98, 0xED, 0xE8, 0xA6);

// {C63657F6-8850-470D-88F8-455E16EA2C26}
DEFINE_GUID(IID_IUIViewSettings, 0xC63657F6, 0x8850, 0x470D, 0x88, 0xF8, 0x45, 0x5E, 0x16, 0xEA, 0x2C, 0x26);

// {85361600-1C63-4627-BCB1-3A89E0BC9C55}
DEFINE_GUID(IID_IUISettings, 0x85361600, 0x1C63, 0x4627, 0xBC, 0xB1, 0x3A, 0x89, 0xE0, 0xBC, 0x9C, 0x55);

// {BAD82401-2721-44F9-BB91-2BB228BE442F}
DEFINE_GUID(IID_IUISettings2, 0xBAD82401, 0x2721, 0x44F9, 0xBB, 0x91, 0x2B, 0xB2, 0x28, 0xBE, 0x44, 0x2F);

// {03021BE4-5254-4781-8194-5168F7D06D7B}
DEFINE_GUID(IID_IUISettings3, 0x03021BE4, 0x5254, 0x4781, 0x81, 0x94, 0x51, 0x68, 0xF7, 0xD0, 0x6D, 0x7B);

// {52BB3002-919B-4D6B-9B78-8DD66FF4B93B}
DEFINE_GUID(IID_IUISettings4, 0x52BB3002, 0x919B, 0x4D6B, 0x9B, 0x78, 0x8D, 0xD6, 0x6F, 0xF4, 0xB9, 0x3B);

// {5349D588-0CB5-5F05-BD34-706B3231F0BD}
DEFINE_GUID(IID_IUISettings5, 0x5349D588, 0x0CB5, 0x5F05, 0xBD, 0x34, 0x70, 0x6B, 0x32, 0x31, 0xF0, 0xBD);

// {AEF19BD7-FE31-5A04-ADA4-469AAEC6DFA9}
DEFINE_GUID(IID_IUISettings6, 0xAEF19BD7, 0xFE31, 0x5A04, 0xAD, 0xA4, 0x46, 0x9A, 0xAE, 0xC6, 0xDF, 0xA9);

// {44A9796F-723E-4FDF-A218-033E75B0C084}
DEFINE_GUID(IID_IUriRuntimeClassFactory, 0x44A9796F, 0x723E, 0x4FDF, 0xA2, 0x18, 0x03, 0x3E, 0x75, 0xB0, 0xC0, 0x84);

// {9E365E57-48B2-4160-956F-C7385120BBFC}
DEFINE_GUID(IID_IUriRuntimeClass, 0x9E365E57, 0x48B2, 0x4160, 0x95, 0x6F, 0xC7, 0x38, 0x51, 0x20, 0xBB, 0xFC);

// {277151C3-9E3E-42F6-91A4-5DFDEB232451}
DEFINE_GUID(IID_ILauncherStatics, 0x277151C3, 0x9E3E, 0x42F6, 0x91, 0xA4, 0x5D, 0xFD, 0xEB, 0x23, 0x24, 0x51);

// Windows.Networking.Connectivity.INetworkInformationStatics
// {5074F851-950D-4165-9C15-365619481EEA}
DEFINE_GUID(IID_INetworkInformationStatics, 0x5074F851, 0x950D, 0x4165, 0x9C, 0x15, 0x36, 0x56, 0x19, 0x48, 0x1E, 0xEA);

// Windows.Networking.Connectivity.IConnectionProfile
// {71BA143C-598E-49D0-84EB-8FEBAEDCC195}
DEFINE_GUID(IID_IConnectionProfile, 0x71BA143C, 0x598E, 0x49D0, 0x84, 0xEB, 0x8F, 0xEB, 0xAE, 0xDC, 0xC1, 0x95);

// Windows.Networking.Connectivity.IConnectionProfile2
// {E2045145-4C9F-400C-9150-7EC7D6E2888A}
DEFINE_GUID(IID_IConnectionProfile2, 0xE2045145, 0x4C9F, 0x400C, 0x91, 0x50, 0x7E, 0xC7, 0xD6, 0xE2, 0x88, 0x8A);

// Windows.Networking.Connectivity.IConnectionProfile3
// {578C2528-4CD9-4161-8045-201CFD5B115C}
DEFINE_GUID(IID_IConnectionProfile3, 0x578C2528, 0x4CD9, 0x4161, 0x80, 0x45, 0x20, 0x1C, 0xFD, 0x5B, 0x11, 0x5C);

// Windows.Networking.Connectivity.IConnectionProfile4
// {7A2D42CD-81E0-4AE6-ABED-AB9CA13EB714}
DEFINE_GUID(IID_IConnectionProfile4, 0x7A2D42CD, 0x81E0, 0x4AE6, 0xAB, 0xED, 0xAB, 0x9C, 0xA1, 0x3E, 0xB7, 0x14);

// Windows.Networking.Connectivity.IConnectionProfile5
// {85361EC7-9C73-4BE0-8F14-578EEC71EE0E}
DEFINE_GUID(IID_IConnectionProfile5, 0x85361EC7, 0x9C73, 0x4BE0, 0x8F, 0x14, 0x57, 0x8E, 0xEC, 0x71, 0xEE, 0x0E);

// Windows.Networking.Connectivity.IConnectionProfile6
// {DC27DFE2-7A6F-5D0E-9589-2FE2E5B6F9AA}
DEFINE_GUID(IID_IConnectionProfile6, 0xDC27DFE2, 0x7A6F, 0x5D0E, 0x95, 0x89, 0x2F, 0xE2, 0xE5, 0xB6, 0xF9, 0xAA);

// Windows.Networking.Connectivity.IConnectionCost
// {BAD7D829-3416-4B10-A202-BAC0B075BDAE}
DEFINE_GUID(IID_IConnectionCost, 0xBAD7D829, 0x3416, 0x4B10, 0xA2, 0x02, 0xBA, 0xC0, 0xB0, 0x75, 0xBD, 0xAE);

// Windows.Networking.Connectivity.IConnectionCost2
// {8E113A05-E209-4549-BB25-5E0DB691CB05}
DEFINE_GUID(IID_IConnectionCost2, 0x8E113A05, 0xE209, 0x4549, 0xBB, 0x25, 0x5E, 0x0D, 0xB6, 0x91, 0xCB, 0x05);

// Windows.System.IDispatcherQueue
// {603E88E4-A338-4FFE-A457-A5CFB9CEB899}
DEFINE_GUID(IID_IDispatcherQueue, 0x603E88E4, 0xA338, 0x4FFE, 0xA4, 0x57, 0xA5, 0xCF, 0xB9, 0xCE, 0xB8, 0x99);

// Windows.System.IDispatcherQueue2
// {c822c647-30ef-506e-bd1e-a647ae6675ff}
DEFINE_GUID(IID_IDispatcherQueue2, 0xc822c647, 0x30ef, 0x506e, 0xbd, 0x1e, 0xa6, 0x47, 0xae, 0x66, 0x75, 0xff);

// Windows.System.IDispatcherQueueTimer
// {5feabb1d-a31c-4727-b1ac-37454649d56a}
DEFINE_GUID(IID_IDispatcherQueueTimer, 0x5feabb1d, 0xa31c, 0x4727, 0xb1, 0xac, 0x37, 0x45, 0x46, 0x49, 0xd5, 0x6a);

// Windows.System.IDispatcherQueueStatics
// {A96D83D7-9371-4517-9245-D0824AC12C74}
DEFINE_GUID(IID_IDispatcherQueueStatics, 0xA96D83D7, 0x9371, 0x4517, 0x92, 0x45, 0xD0, 0x82, 0x4A, 0xC1, 0x2C, 0x74);

// Windows.System.IDispatcherQueueController
// {22F34E66-50DB-4E36-A98D-61C01B384D20}
DEFINE_GUID(IID_IDispatcherQueueController, 0x22F34E66, 0x50DB, 0x4E36, 0xA9, 0x8D, 0x61, 0xC0, 0x1B, 0x38, 0x4D, 0x20);

// Windows.System.IDispatcherQueueControllerStatics
// {0A6C98E0-5198-49A2-A313-3F70D1F13C27}
DEFINE_GUID(IID_IDispatcherQueueControllerStatics, 0x0A6C98E0, 0x5198, 0x49A2, 0xA3, 0x13, 0x3F, 0x70, 0xD1, 0xF1, 0x3C, 0x27);

// Windows.Foundation.IAsyncAction
// {5A648006-843A-4DA9-865B-9D26E5DFAD7B}
DEFINE_GUID(IID_IAsyncAction, 0x5A648006, 0x843A, 0x4DA9, 0x86, 0x5B, 0x9D, 0x26, 0xE5, 0xDF, 0xAD, 0x7B);

// Windows.Foundation.IAsyncInfo
// {00000036-0000-0000-C000-000000000046}
DEFINE_GUID(IID_IAsyncInfo, 0x00000036, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);

// Windows.Gaming.UI.IGameBarStatics
// {1DB9A292-CC78-4173-BE45-B61E67283EA7}
DEFINE_GUID(IID_IGameBarStatics, 0x1DB9A292, 0xCC78, 0x4173, 0xBE, 0x45, 0xB6, 0x1E, 0x67, 0x28, 0x3E, 0xA7);

// Windows.UI.ViewManagement.IAccessibilitySettings
// {fe0e8147-c4c0-4562-b962-1327b52ad5b9}
DEFINE_GUID(IID_IAccessibilitySettings, 0xfe0e8147, 0xc4c0, 0x4562, 0xb9, 0x62, 0x13, 0x27, 0xb5, 0x2a, 0xd5, 0xb9);

// Windows.UI.ViewManagement.IInputPane
// {640ada70-06f3-4c87-a678-9829c9127c28}
DEFINE_GUID(IID_IInputPane, 0x640ada70, 0x06f3, 0x4c87, 0xa6, 0x78, 0x98, 0x29, 0xc9, 0x12, 0x7c, 0x28);

// Windows.UI.ViewManagement.IInputPane2
// {8a6b3f26-7090-4793-944c-c3f2cde26276}
DEFINE_GUID(IID_IInputPane2, 0x8a6b3f26, 0x7090, 0x4793, 0x94, 0x4c, 0xc3, 0xf2, 0xcd, 0xe2, 0x62, 0x76);

// Windows.UI.ViewManagement.IInputPaneStatics
// {95f4af3a-ef47-424a-9741-fd2815eba2bd}
DEFINE_GUID(IID_IInputPaneStatics, 0x95f4af3a, 0xef47, 0x424a, 0x97, 0x41, 0xfd, 0x28, 0x15, 0xeb, 0xa2, 0xbd);

// Windows.UI.ViewManagement.IInputPaneStatics2
// {1b63529b-d9ec-4531-8445-71bab9fb828e}
DEFINE_GUID(IID_IInputPaneStatics2, 0x1b63529b, 0xd9ec, 0x4531, 0x84, 0x45, 0x71, 0xba, 0xb9, 0xfb, 0x82, 0x8e);

// Windows.UI.ViewManagement.IInputPaneInterop
// {75cf2c57-9195-4931-8332-f0b409e916af}
DEFINE_GUID(IID_IInputPaneInterop, 0x75cf2c57, 0x9195, 0x4931, 0x83, 0x32, 0xf0, 0xb4, 0x09, 0xe9, 0x16, 0xaf);

// Windows.UI.IColorHelperStatics
// {8504dbea-fb6a-4144-a6c2-33499c9284f5}
DEFINE_GUID(IID_IColorHelperStatics, 0x8504dbea, 0xfb6a, 0x4144, 0xa6, 0xc2, 0x33, 0x49, 0x9c, 0x92, 0x84, 0xf5);

// Windows.UI.IColorHelperStatics2
// {24d9af02-6eb0-4b94-855c-fcf0818d9a16}
DEFINE_GUID(IID_IColorHelperStatics2, 0x24d9af02, 0x6eb0, 0x4b94, 0x85, 0x5c, 0xfc, 0xf0, 0x81, 0x8d, 0x9a, 0x16);

// Windows.System.Threading.ThreadPoolStatics
// {b6bf67dd-84bd-44f8-ac1c-93ebcb9dba91}
DEFINE_GUID(IID_IThreadPoolStatics, 0xb6bf67dd, 0x84bd, 0x44f8, 0xac, 0x1c, 0x93, 0xeb, 0xcb, 0x9d, 0xba, 0x91);

// Windows.Globalization.ILanguageFactory
// {9b0252ac-0c27-44f8-b792-9793fb66c63e}
DEFINE_GUID(IID_ILanguageFactory, 0x9b0252ac, 0x0c27, 0x44f8, 0xb7, 0x92, 0x97, 0x93, 0xfb, 0x66, 0xc6, 0x3e);

// Windows.Globalization.ILanguage
// {ea79a752-f7c2-4265-b1bd-c4dec4e4f080}
DEFINE_GUID(IID_ILanguage, 0xea79a752, 0xf7c2, 0x4265, 0xb1, 0xbd, 0xc4, 0xde, 0xc4, 0xe4, 0xf0, 0x80);

// Windows.Globalization.ILanguage2
// {6a47e5b5-d94d-4886-a404-a5a5b9d5b494}
DEFINE_GUID(IID_ILanguage2, 0x6a47e5b5, 0xd94d, 0x4886, 0xa4, 0x04, 0xa5, 0xa5, 0xb9, 0xd5, 0xb4, 0x94);

// Windows.Globalization.IApplicationLanguagesStatics
// {75b40847-0a4c-4a92-9565-fd63c95f7aed}
DEFINE_GUID(IID_IApplicationLanguagesStatics, 0x75b40847, 0x0a4c, 0x4a92, 0x95, 0x65, 0xfd, 0x63, 0xc9, 0x5f, 0x7a, 0xed);

// Windows.System.Profile.SystemManufacturers.ISmbiosInformationStatics
// {080cca7c-637c-48c4-b728-f9273812db8e}
DEFINE_GUID(IID_ISmbiosInformationStatics, 0x080cca7c, 0x637c, 0x48c4, 0xb7, 0x28, 0xf9, 0x27, 0x38, 0x12, 0xdb, 0x8e);

// Windows.Networking.IHostNameStatics
// {f68cd4bf-a388-4e8b-91ea-54dd6dd901c0}
DEFINE_GUID(IID_IHostNameStatics, 0xf68cd4bf, 0xa388, 0x4e8b, 0x91, 0xea, 0x54, 0xdd, 0x6d, 0xd9, 0x01, 0xc0);

// Windows.Networking.IHostNameFactory
// {458c23ed-712f-4576-adf1-c20b2c643558}
DEFINE_GUID(IID_IHostNameFactory, 0x458c23ed, 0x712f, 0x4576, 0xad, 0xf1, 0xc2, 0x0b, 0x2c, 0x64, 0x35, 0x58);

// Windows.Networking.IHostName
// {bf8ecaad-ed96-49a7-9084-d416cae88dcb}
DEFINE_GUID(IID_IHostName, 0xbf8ecaad, 0xed96, 0x49a7, 0x90, 0x84, 0xd4, 0x16, 0xca, 0xe8, 0x8d, 0xcb);


typedef struct _IInspectable IInspectable;

typedef struct _IInspectableVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IInspectable*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IInspectable*);
	ULONG(STDMETHODCALLTYPE* Release)(IInspectable*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids)(IInspectable*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName)(IInspectable*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel)(IInspectable*, TrustLevel*);
} IInspectableVtbl;

typedef struct _IInspectable
{
	IInspectableVtbl* lpVtbl;
} IInspectable;

typedef struct _IDelegate IDelegate;

typedef struct _IDelegateVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IDelegate*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IDelegate*);
	ULONG(STDMETHODCALLTYPE* Release)(IDelegate*);

	// IDelegate
	void(STDMETHODCALLTYPE* Invoke)(IDelegate*);
} IDelegateVtbl;

typedef struct _IDelegate
{
	IDelegateVtbl* lpVtbl;
} IDelegate;

typedef struct _ITypedEventHandler ITypedEventHandler;

typedef struct _ITypedEventHandlerVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(ITypedEventHandler*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(ITypedEventHandler*);
	ULONG(STDMETHODCALLTYPE* Release)(ITypedEventHandler*);

	// ITypedEventHandler
	void(STDMETHODCALLTYPE* Invoke)(ITypedEventHandler*, PVOID TSender, PVOID TResult);
} ITypedEventHandlerVtbl;

typedef struct _ITypedEventHandler
{
	ITypedEventHandlerVtbl* lpVtbl;
} ITypedEventHandler;

typedef struct _IEventHandlerEntry IEventHandlerEntry;

typedef struct _IEventHandlerEntry
{
	IEventHandlerEntry* prev;
	EventRegistrationToken token;
	ITypedEventHandler* handler;
} IEventHandlerEntry;

typedef struct _IActivationFactory IActivationFactory;

typedef struct _IActivationFactoryVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IActivationFactory*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IActivationFactory*);
	ULONG(STDMETHODCALLTYPE* Release)(IActivationFactory*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids)(IActivationFactory*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName)(IActivationFactory*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel)(IActivationFactory*, TrustLevel*);

	// IActivationFactory
	HRESULT(STDMETHODCALLTYPE* ActivateInstance)(IActivationFactory*, IInspectable**);
} IActivationFactoryVtbl;

typedef struct _IActivationFactory
{
	IActivationFactoryVtbl* lpVtbl;
} IActivationFactory;

extern IActivationFactory CActivationFactory;

typedef struct _IVectorView_HSTRING IVectorView_HSTRING;

typedef struct _IVectorView_HSTRINGVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IVectorView_HSTRING*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IVectorView_HSTRING*);
	ULONG(STDMETHODCALLTYPE* Release)(IVectorView_HSTRING*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids)(IVectorView_HSTRING*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName)(IVectorView_HSTRING*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel)(IVectorView_HSTRING*, TrustLevel*);

	// IVectorView_HSTRING
	HRESULT(STDMETHODCALLTYPE* GetAt)(IVectorView_HSTRING*, ULONG, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Size)(IVectorView_HSTRING*, PULONG);
	HRESULT(STDMETHODCALLTYPE* IndexOf)(IVectorView_HSTRING*, HSTRING, PULONG, PBOOLEAN);
	HRESULT(STDMETHODCALLTYPE* GetMany)(IVectorView_HSTRING*, ULONG, ULONG, HSTRING*, PULONG);
} IVectorView_HSTRINGVtbl;

typedef struct _IVectorView_HSTRING
{
	IVectorView_HSTRINGVtbl* lpVtbl;
	LONG					RefCount;
	ULONG					NumberOfHstrings;
	HSTRING* HstringArray;
} IVectorView_HSTRING;

//
// WINRT: Windows.System.UserProfile.GlobalizationPreferences
//

typedef struct _IGlobalizationPreferencesStatics IGlobalizationPreferencesStatics;

typedef struct _IGlobalizationPreferencesStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IGlobalizationPreferencesStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IGlobalizationPreferencesStatics*);
	ULONG(STDMETHODCALLTYPE* Release)(IGlobalizationPreferencesStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids)(IGlobalizationPreferencesStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName)(IGlobalizationPreferencesStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel)(IGlobalizationPreferencesStatics*, TrustLevel*);

	// IGlobalizationPreferencesStatics
	HRESULT(STDMETHODCALLTYPE* get_Calendars)(IGlobalizationPreferencesStatics*, IVectorView_HSTRING**);
	HRESULT(STDMETHODCALLTYPE* get_Clocks)(IGlobalizationPreferencesStatics*, IVectorView_HSTRING**);
	HRESULT(STDMETHODCALLTYPE* get_Currencies)(IGlobalizationPreferencesStatics*, IVectorView_HSTRING**);
	HRESULT(STDMETHODCALLTYPE* get_Languages)(IGlobalizationPreferencesStatics*, IVectorView_HSTRING**);
	HRESULT(STDMETHODCALLTYPE* get_HomeGeographicRegion)(IGlobalizationPreferencesStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_WeekStartsOn)(IGlobalizationPreferencesStatics*, DayOfWeek*);
} IGlobalizationPreferencesStaticsVtbl;

typedef struct _IGlobalizationPreferencesStatics
{
	IGlobalizationPreferencesStaticsVtbl* lpVtbl;
} IGlobalizationPreferencesStatics;

extern IGlobalizationPreferencesStatics CGlobalizationPreferencesStatics;

//
// WINRT: Windows.UI.ViewManagement.UIViewSettings
//

typedef struct _IUIViewSettingsInterop IUIViewSettingsInterop;

typedef struct _IUIViewSettingsInteropVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUIViewSettingsInterop*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUIViewSettingsInterop*);
	ULONG(STDMETHODCALLTYPE* Release) (IUIViewSettingsInterop*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUIViewSettingsInterop*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUIViewSettingsInterop*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUIViewSettingsInterop*, TrustLevel*);

	// IUIViewSettingsInterop
	HRESULT(STDMETHODCALLTYPE* GetForWindow) (IUIViewSettingsInterop*, HWND, REFIID, PPVOID);
} IUIViewSettingsInteropVtbl;

typedef struct _IUIViewSettingsInterop
{
	IUIViewSettingsInteropVtbl* lpVtbl;
} IUIViewSettingsInterop;

typedef enum _UserInteractionMode
{
	UserInteractionMode_Mouse,
	UserInteractionMode_Touch
} UserInteractionMode;

typedef struct _IUIViewSettings IUIViewSettings;

typedef struct _IUIViewSettingsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUIViewSettings*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUIViewSettings*);
	ULONG(STDMETHODCALLTYPE* Release) (IUIViewSettings*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUIViewSettings*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUIViewSettings*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUIViewSettings*, TrustLevel*);

	// IUIViewSettings
	HRESULT(STDMETHODCALLTYPE* get_UserInteractionMode) (IUIViewSettings*, UserInteractionMode*);
} IUIViewSettingsVtbl;

typedef struct _IUIViewSettings
{
	IUIViewSettingsVtbl* lpVtbl;
} IUIViewSettings;

extern IUIViewSettingsInterop CUIViewSettingsInterop;
extern IUIViewSettings CUIViewSettings;

//
// WINRT: Windows.UI.ViewManagement.UISettings
//

typedef enum _UIElementType
{
	UIElementType_ActiveCaption = 0,
	UIElementType_Background = 1,
	UIElementType_ButtonFace = 2,
	UIElementType_ButtonText = 3,
	UIElementType_CaptionText = 4,
	UIElementType_GrayText = 5,
	UIElementType_Highlight = 6,
	UIElementType_HighlightText = 7,
	UIElementType_Hotlight = 8,
	UIElementType_InactiveCaption = 9,
	UIElementType_InactiveCaptionText = 10,
	UIElementType_Window = 11,
	UIElementType_WindowText = 12,
	UIElementType_AccentColor = 1000,
	UIElementType_TextHigh = 1001,
	UIElementType_TextMedium = 1002,
	UIElementType_TextLow = 1003,
	UIElementType_TextContrastWithHigh = 1004,
	UIElementType_NonTextHigh = 1005,
	UIElementType_NonTextMediumHigh = 1006,
	UIElementType_NonTextMedium = 1007,
	UIElementType_NonTextMediumLow = 1008,
	UIElementType_NonTextLow = 1009,
	UIElementType_PageBackground = 1010,
	UIElementType_PopupBackground = 1011,
	UIElementType_OverlayOutsidePopup = 1012,
} UIElementType;

typedef enum _UIHandPreference
{
	HandPreference_LeftHanded,
	HandPreference_RightHanded
} UIHandPreference;

typedef struct _UISettingsSize
{
	FLOAT	Width;
	FLOAT	Height;
} UISettingsSize;

typedef struct _UIColor
{
	BYTE A;
	BYTE R;
	BYTE G;
	BYTE B;
} UIColor;

typedef struct _IUISettings IUISettings;

typedef struct _IUISettingsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUISettings*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUISettings*);
	ULONG(STDMETHODCALLTYPE* Release) (IUISettings*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUISettings*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUISettings*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUISettings*, TrustLevel*);

	// IUISettings
	HRESULT(STDMETHODCALLTYPE* get_HandPreference) (IUISettings*, UIHandPreference*);
	HRESULT(STDMETHODCALLTYPE* get_CursorSize) (IUISettings*, UISettingsSize*);
	HRESULT(STDMETHODCALLTYPE* get_ScrollBarSize) (IUISettings*, UISettingsSize*);
	HRESULT(STDMETHODCALLTYPE* get_ScrollBarArrowSize) (IUISettings*, UISettingsSize*);
	HRESULT(STDMETHODCALLTYPE* get_ScrollBarThumbBoxSize) (IUISettings*, UISettingsSize*);
	HRESULT(STDMETHODCALLTYPE* get_MessageDuration) (IUISettings*, PULONG);
	HRESULT(STDMETHODCALLTYPE* get_AnimationsEnabled) (IUISettings*, PBOOLEAN);
	HRESULT(STDMETHODCALLTYPE* get_CaretBrowsingEnabled) (IUISettings*, PBOOLEAN);
	HRESULT(STDMETHODCALLTYPE* get_CaretBlinkRate) (IUISettings*, PULONG);
	HRESULT(STDMETHODCALLTYPE* get_CaretWidth) (IUISettings*, PULONG);
	HRESULT(STDMETHODCALLTYPE* get_DoubleClickTime) (IUISettings*, PULONG);
	HRESULT(STDMETHODCALLTYPE* get_MouseHoverTime) (IUISettings*, PULONG);
	HRESULT(STDMETHODCALLTYPE* UIElementColor) (IUISettings*, UIElementType, UIColor*);
} IUISettingsVtbl;

typedef struct _IUISettings
{
	IUISettingsVtbl* lpVtbl;
} IUISettings;

extern IUISettings CUISettings;

typedef enum _UIColorType
{
	UIColorType_Background = 0,
	UIColorType_Foreground = 1,
	UIColorType_AccentDark3 = 2,
	UIColorType_AccentDark2 = 3,
	UIColorType_AccentDark1 = 4,
	UIColorType_Accent = 5,
	UIColorType_AccentLight1 = 6,
	UIColorType_AccentLight2 = 7,
	UIColorType_AccentLight3 = 8,
	UIColorType_Complement = 9,
} UIColorType;

typedef struct _IUISettings2 IUISettings2;

typedef struct _IUISettings2Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUISettings2*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUISettings2*);
	ULONG(STDMETHODCALLTYPE* Release) (IUISettings2*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUISettings2*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUISettings2*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUISettings2*, TrustLevel*);

	// IUISettings2
	HRESULT(STDMETHODCALLTYPE* get_TextScaleFactor) (IUISettings2*, DOUBLE*);
	HRESULT(STDMETHODCALLTYPE* add_TextScaleFactorChanged) (IUISettings2*, PVOID, PPVOID);
	HRESULT(STDMETHODCALLTYPE* remove_TextScaleFactorChanged) (IUISettings2*, PVOID);
} IUISettings2Vtbl;

typedef struct _IUISettings2
{
	IUISettings2Vtbl* lpVtbl;
} IUISettings2;

extern IUISettings2 CUISettings2;


typedef struct _IUISettings3 IUISettings3;

typedef struct _IUISettings3Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUISettings3*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUISettings3*);
	ULONG(STDMETHODCALLTYPE* Release) (IUISettings3*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUISettings3*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUISettings3*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUISettings3*, TrustLevel*);

	// IUISettings3
	HRESULT(STDMETHODCALLTYPE* GetColorValue) (IUISettings3*, UIColorType, UIColor*);
	HRESULT(STDMETHODCALLTYPE* add_ColorValuesChanged) (IUISettings3*, PVOID, PPVOID);
	HRESULT(STDMETHODCALLTYPE* remove_ColorValuesChanged) (IUISettings3*, PVOID);
} IUISettings3Vtbl;

typedef struct _IUISettings3
{
	IUISettings3Vtbl* lpVtbl;
} IUISettings3;

extern IUISettings3 CUISettings3;

typedef struct _IUISettings4 IUISettings4;

typedef struct _IUISettings4Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUISettings4*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUISettings4*);
	ULONG(STDMETHODCALLTYPE* Release) (IUISettings4*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUISettings4*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUISettings4*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUISettings4*, TrustLevel*);

	// IUISettings4
	HRESULT(STDMETHODCALLTYPE* get_AdvancedEffectsEnabled) (IUISettings4*, PBOOLEAN);
	HRESULT(STDMETHODCALLTYPE* add_AdvancedEffectsEnabledChanged) (IUISettings4*, PVOID, PPVOID);
	HRESULT(STDMETHODCALLTYPE* remove_AdvancedEffectsEnabledChanged) (IUISettings4*, PVOID);
} IUISettings4Vtbl;

typedef struct _IUISettings4
{
	IUISettings4Vtbl* lpVtbl;
} IUISettings4;

extern IUISettings4 CUISettings4;

typedef struct _IUISettings5 IUISettings5;

typedef struct _IUISettings5Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUISettings5*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUISettings5*);
	ULONG(STDMETHODCALLTYPE* Release) (IUISettings5*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUISettings5*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUISettings5*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUISettings5*, TrustLevel*);

	// IUISettings5
	HRESULT(STDMETHODCALLTYPE* get_AutoHideScrollBars) (IUISettings5*, PBOOLEAN);
	HRESULT(STDMETHODCALLTYPE* add_AutoHideScrollBarsChanged) (IUISettings5*, PVOID, PPVOID);
	HRESULT(STDMETHODCALLTYPE* remove_AutoHideScrollBarsChanged) (IUISettings5*, PVOID);
} IUISettings5Vtbl;

typedef struct _IUISettings5
{
	IUISettings5Vtbl* lpVtbl;
} IUISettings5;

extern IUISettings5 CUISettings5;

typedef struct _IUISettings6 IUISettings6;

typedef struct _IUISettings6Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUISettings6*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUISettings6*);
	ULONG(STDMETHODCALLTYPE* Release) (IUISettings6*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUISettings6*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUISettings6*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUISettings6*, TrustLevel*);

	// IUISettings6
	HRESULT(STDMETHODCALLTYPE* add_AnimationsEnabledChanged) (IUISettings6*, PVOID, PPVOID);
	HRESULT(STDMETHODCALLTYPE* remove_AnimationsEnabledChanged) (IUISettings6*, PVOID);
	HRESULT(STDMETHODCALLTYPE* add_MessageDurationChanged) (IUISettings6*, PVOID, PPVOID);
	HRESULT(STDMETHODCALLTYPE* remove_MessageDurationChanged) (IUISettings6*, PVOID);
} IUISettings6Vtbl;

typedef struct _IUISettings6
{
	IUISettings6Vtbl* lpVtbl;
} IUISettings6;

extern IUISettings6 CUISettings6;


typedef struct _IRestrictedErrorInfo IRestrictedErrorInfo;

typedef struct _IRestrictedErrorInfoVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IRestrictedErrorInfo*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IRestrictedErrorInfo*);
	ULONG(STDMETHODCALLTYPE* Release)(IRestrictedErrorInfo*);

	// IRestrictedErrorInfo
	HRESULT(STDMETHODCALLTYPE* GetErrorDetails)(IRestrictedErrorInfo*, BSTR*, HRESULT*, BSTR*, BSTR*);
	HRESULT(STDMETHODCALLTYPE* GetReference)(IRestrictedErrorInfo*, BSTR*);
} IRestrictedErrorInfoVtbl;

typedef struct _IRestrictedErrorInfo
{
	IRestrictedErrorInfoVtbl* lpVtbl;
} IRestrictedErrorInfo;

typedef struct _IUriRuntimeClass IUriRuntimeClass;

typedef struct _IUriRuntimeClassVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUriRuntimeClass*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUriRuntimeClass*);
	ULONG(STDMETHODCALLTYPE* Release) (IUriRuntimeClass*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUriRuntimeClass*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUriRuntimeClass*, TrustLevel*);

	// IUriRuntimeClass
	HRESULT(STDMETHODCALLTYPE* get_AbsoluteUri) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_DisplayUri) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Domain) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Extension) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Fragment) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Host) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Password) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Path) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Query) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_QueryParsed) (IUriRuntimeClass*, IUnknown**); // actually IWwwFormUrlDecoderRuntimeClass **
	HRESULT(STDMETHODCALLTYPE* get_RawUri) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_SchemeName) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_UserName) (IUriRuntimeClass*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Port) (IUriRuntimeClass*, PULONG);
	HRESULT(STDMETHODCALLTYPE* get_Suspicious) (IUriRuntimeClass*, PBOOLEAN);
	HRESULT(STDMETHODCALLTYPE* Equals) (IUriRuntimeClass*, IUriRuntimeClass*, PBOOLEAN);
	HRESULT(STDMETHODCALLTYPE* CombineUri) (IUriRuntimeClass*, HSTRING, IUriRuntimeClass**);
} IUriRuntimeClassVtbl;

typedef struct _IUriRuntimeClass
{
	IUriRuntimeClassVtbl* lpVtbl;
	LONG					RefCount;
	IUri* Uri;
} IUriRuntimeClass;

typedef struct _IUriRuntimeClassFactory IUriRuntimeClassFactory;

typedef struct _IUriRuntimeClassFactoryVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IUriRuntimeClassFactory*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IUriRuntimeClassFactory*);
	ULONG(STDMETHODCALLTYPE* Release) (IUriRuntimeClassFactory*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IUriRuntimeClassFactory*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IUriRuntimeClassFactory*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IUriRuntimeClassFactory*, TrustLevel*);

	// IUriRuntimeClassFactory
	HRESULT(STDMETHODCALLTYPE* CreateUri) (IUriRuntimeClassFactory*, HSTRING, IUriRuntimeClass**);
	HRESULT(STDMETHODCALLTYPE* CreateWithRelativeUri) (IUriRuntimeClassFactory*, HSTRING, HSTRING, IUriRuntimeClass**);
} IUriRuntimeClassFactoryVtbl;

typedef struct _IUriRuntimeClassFactory
{
	IUriRuntimeClassFactoryVtbl* lpVtbl;
} IUriRuntimeClassFactory;

extern IUriRuntimeClassFactory CUriRuntimeClassFactory;

typedef struct _ILauncherStatics ILauncherStatics;

typedef struct _ILauncherStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (ILauncherStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (ILauncherStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (ILauncherStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (ILauncherStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (ILauncherStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (ILauncherStatics*, TrustLevel*);

	// ILauncherStatics
	HRESULT(STDMETHODCALLTYPE* LaunchFileAsync) (ILauncherStatics*, IUnknown*, IAsyncOperation**);
	HRESULT(STDMETHODCALLTYPE* LaunchFileWithOptionsAsync) (ILauncherStatics*, IUnknown*, IUnknown*, IAsyncOperation**);
	HRESULT(STDMETHODCALLTYPE* LaunchUriAsync) (ILauncherStatics*, IUriRuntimeClass*, IAsyncOperation**);
	HRESULT(STDMETHODCALLTYPE* LaunchUriWithOptionsAsync) (ILauncherStatics*, IUriRuntimeClass*, IUnknown*, IAsyncOperation**);
} ILauncherStaticsVtbl;

typedef struct _ILauncherStatics
{
	ILauncherStaticsVtbl* lpVtbl;
} ILauncherStatics;

extern ILauncherStatics CLauncherStatics;

//
// Network : Windows.Networking.Connectivity.NetworkInformation
//

typedef struct _INetworkInformationStatics INetworkInformationStatics;

typedef struct _INetworkInformationStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (INetworkInformationStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (INetworkInformationStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (INetworkInformationStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (INetworkInformationStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (INetworkInformationStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (INetworkInformationStatics*, TrustLevel*);

	// INetworkInformationStatics
	HRESULT(STDMETHODCALLTYPE* GetConnectionProfiles) (INetworkInformationStatics*, void**);
	HRESULT(STDMETHODCALLTYPE* GetInternetConnectionProfile) (INetworkInformationStatics*, void**);
	HRESULT(STDMETHODCALLTYPE* GetLanIdentifiers) (INetworkInformationStatics*, void**);
	HRESULT(STDMETHODCALLTYPE* GetHostNames) (INetworkInformationStatics*, void**);
	HRESULT(STDMETHODCALLTYPE* GetProxyConfigurationAsync) (INetworkInformationStatics*, void*, IUnknown**);
	HRESULT(STDMETHODCALLTYPE* GetSortedEndpointPairs) (INetworkInformationStatics*, void*, UINT32, void**);
	HRESULT(STDMETHODCALLTYPE* add_NetworkStatusChanged) (INetworkInformationStatics*, ITypedEventHandler*, EventRegistrationToken*);
	HRESULT(STDMETHODCALLTYPE* remove_NetworkStatusChanged) (INetworkInformationStatics*, EventRegistrationToken);
} INetworkInformationStaticsVtbl;

typedef struct _INetworkInformationStatics
{
	INetworkInformationStaticsVtbl* lpVtbl;
} INetworkInformationStatics;

extern INetworkInformationStatics CNetworkInformationStatics;

// Network : Windows.Networking.Connectivity.ConnectionProfile

typedef enum NetworkConnectivityLevel
{
	NetworkConnectivityLevel_None = 0,
	NetworkConnectivityLevel_LocalAccess = 1,
	NetworkConnectivityLevel_ConstrainedInternetAccess = 2,
	NetworkConnectivityLevel_InternetAccess = 3,
} NetworkConnectivityLevel;

typedef struct _IConnectionProfile IConnectionProfile;

typedef struct _IConnectionProfileVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IConnectionProfile*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IConnectionProfile*);
	ULONG(STDMETHODCALLTYPE* Release) (IConnectionProfile*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IConnectionProfile*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IConnectionProfile*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IConnectionProfile*, TrustLevel*);

	// IConnectionProfile
	HRESULT(STDMETHODCALLTYPE* get_ProfileName) (IConnectionProfile*, void**);
	HRESULT(STDMETHODCALLTYPE* GetNetworkConnectivityLevel) (IConnectionProfile*, NetworkConnectivityLevel*);
	HRESULT(STDMETHODCALLTYPE* GetNetworkNames) (IConnectionProfile*, void**);
	HRESULT(STDMETHODCALLTYPE* GetConnectionCost) (IConnectionProfile*, void**);
	HRESULT(STDMETHODCALLTYPE* GetDataPlanStatus) (IConnectionProfile*, void**);
	HRESULT(STDMETHODCALLTYPE* GetLocalUsage) (IConnectionProfile*, UINT64, UINT64, void**);
	HRESULT(STDMETHODCALLTYPE* GetLocalUsagePerRoamingStates) (IConnectionProfile*, UINT64, UINT64, UINT32, void**);
	HRESULT(STDMETHODCALLTYPE* get_NetworkSecuritySettings) (IConnectionProfile*, void**);

} IConnectionProfileVtbl;

typedef struct _IConnectionProfile2Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IConnectionProfile*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IConnectionProfile*);
	ULONG(STDMETHODCALLTYPE* Release) (IConnectionProfile*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IConnectionProfile*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IConnectionProfile*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IConnectionProfile*, TrustLevel*);

	// IConnectionProfile2
	HRESULT(STDMETHODCALLTYPE* get_IsWwanConnectionProfile) (IConnectionProfile*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* get_IsWlanConnectionProfile) (IConnectionProfile*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* get_WwanConnectionProfileDetails) (IConnectionProfile*, void**);
	HRESULT(STDMETHODCALLTYPE* get_WlanConnectionProfileDetails) (IConnectionProfile*, void**);
	HRESULT(STDMETHODCALLTYPE* get_ServiceProviderGuid) (IConnectionProfile*, void**);
	HRESULT(STDMETHODCALLTYPE* GetSignalBars) (IConnectionProfile*, void**);
	HRESULT(STDMETHODCALLTYPE* GetDomainConnectivityLevel) (IConnectionProfile*, UINT32*);
	HRESULT(STDMETHODCALLTYPE* GetNetworkUsageAsync) (IConnectionProfile*, UINT64, UINT64, UINT32, void**);
	HRESULT(STDMETHODCALLTYPE* GetConnectivityIntervalsAsync) (IConnectionProfile*, UINT64, UINT64, void**);

} IConnectionProfile2Vtbl;

typedef struct _IConnectionProfile3Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IConnectionProfile*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IConnectionProfile*);
	ULONG(STDMETHODCALLTYPE* Release) (IConnectionProfile*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IConnectionProfile*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IConnectionProfile*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IConnectionProfile*, TrustLevel*);

	// IConnectionProfile3
	HRESULT(STDMETHODCALLTYPE* GetAttributedNetworkUsageAsync) (IConnectionProfile*, UINT64, UINT64, void**);

} IConnectionProfile3Vtbl;

typedef struct _IConnectionProfile4Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IConnectionProfile*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IConnectionProfile*);
	ULONG(STDMETHODCALLTYPE* Release) (IConnectionProfile*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IConnectionProfile*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IConnectionProfile*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IConnectionProfile*, TrustLevel*);

	// IConnectionProfile4
	HRESULT(STDMETHODCALLTYPE* GetProviderNetworkUsageAsync) (IConnectionProfile*, UINT64, UINT64, void**);

} IConnectionProfile4Vtbl;

typedef struct _IConnectionProfile5Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IConnectionProfile*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IConnectionProfile*);
	ULONG(STDMETHODCALLTYPE* Release) (IConnectionProfile*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IConnectionProfile*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IConnectionProfile*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IConnectionProfile*, TrustLevel*);

	// IConnectionProfile5
	HRESULT(STDMETHODCALLTYPE* get_CanDelete) (IConnectionProfile*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* TryDeleteAsync) (IConnectionProfile*, void**);

} IConnectionProfile5Vtbl;

typedef struct _IConnectionProfile6Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IConnectionProfile*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IConnectionProfile*);
	ULONG(STDMETHODCALLTYPE* Release) (IConnectionProfile*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IConnectionProfile*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IConnectionProfile*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IConnectionProfile*, TrustLevel*);

	// IConnectionProfile6
	HRESULT(STDMETHODCALLTYPE* IsDomainAuthenticatedBy) (IConnectionProfile*, BOOL*);

} IConnectionProfile6Vtbl;

typedef struct _IConnectionProfile
{
	IConnectionProfileVtbl* lpVtbl;
	IConnectionProfile* Parent;

	struct IConnectionProfile2
	{
		IConnectionProfile2Vtbl* lpVtbl;
		IConnectionProfile* Parent;
	} cConnectionProfile2;

	struct IConnectionProfile3
	{
		IConnectionProfile3Vtbl* lpVtbl;
		IConnectionProfile* Parent;
	} cConnectionProfile3;

	struct IConnectionProfile4
	{
		IConnectionProfile4Vtbl* lpVtbl;
		IConnectionProfile* Parent;
	} cConnectionProfile4;

	struct IConnectionProfile5
	{
		IConnectionProfile5Vtbl* lpVtbl;
		IConnectionProfile* Parent;
	} cConnectionProfile5;

	struct IConnectionProfile6
	{
		IConnectionProfile6Vtbl* lpVtbl;
		IConnectionProfile* Parent;
	} cConnectionProfile6;

	IUnknown* manager;
	LONG					RefCount;
} IConnectionProfile;


// Network : Windows.Networking.Connectivity.ConnectionCost

typedef enum _NetworkCostType
{
	NetworkCostType_Unknown = 0,
	NetworkCostType_Unrestricted = 1,
	NetworkCostType_Fixed = 2,
	NetworkCostType_Variable = 3,
} NetworkCostType;

typedef struct _IConnectionCost IConnectionCost;

typedef struct _IConnectionCostVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IConnectionCost*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IConnectionCost*);
	ULONG(STDMETHODCALLTYPE* Release) (IConnectionCost*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IConnectionCost*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IConnectionCost*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IConnectionCost*, TrustLevel*);

	// IConnectionCost
	HRESULT(STDMETHODCALLTYPE* get_NetworkCostType) (IConnectionCost*, NetworkCostType*);
	HRESULT(STDMETHODCALLTYPE* get_Roaming) (IConnectionCost*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* get_OverDataLimit) (IConnectionCost*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* get_ApproachingDataLimit) (IConnectionCost*, BOOL*);

} IConnectionCostVtbl;

typedef struct _IConnectionCost2Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IConnectionCost*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IConnectionCost*);
	ULONG(STDMETHODCALLTYPE* Release) (IConnectionCost*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IConnectionCost*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IConnectionCost*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IConnectionCost*, TrustLevel*);

	// IConnectionCost2
	HRESULT(STDMETHODCALLTYPE* get_BackgroundDataUsageRestricted) (IConnectionCost*, BOOL*);

} IConnectionCost2Vtbl;

typedef struct _IConnectionCost
{
	IConnectionCostVtbl* lpVtbl;
	IConnectionCost* Parent;

	struct IConnectionCost2
	{
		IConnectionCost2Vtbl* lpVtbl;
		IConnectionCost* Parent;
	} cConnectionCost2;

	LONG					RefCount;
} IConnectionCost;


// Foundation : Windows.Foundation.IAsyncAction

typedef enum AsyncStatus
{
	AsyncStatus_Started = 0,
	AsyncStatus_Completed,
	AsyncStatus_Canceled,
	AsyncStatus_Error,
} AsyncStatus;

typedef struct _IAsyncActionCompletedHandler IAsyncActionCompletedHandler;

typedef struct _IAsyncAction IAsyncAction;

typedef struct _IAsyncActionVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IAsyncAction*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IAsyncAction*);
	ULONG(STDMETHODCALLTYPE* Release) (IAsyncAction*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IAsyncAction*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IAsyncAction*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IAsyncAction*, TrustLevel*);

	// IAsyncAction
	HRESULT(STDMETHODCALLTYPE* put_Completed) (IAsyncAction*, IAsyncActionCompletedHandler*);
	HRESULT(STDMETHODCALLTYPE* get_Completed) (IAsyncAction*, IAsyncActionCompletedHandler**);
	HRESULT(STDMETHODCALLTYPE* GetResults) (IAsyncAction*);

} IAsyncActionVtbl;

typedef struct _IAsyncAction
{
	IAsyncActionVtbl*			   lpVtbl;
	LONG						   RefCount;
	HRESULT						   Result;
	IAsyncActionCompletedHandler*  callback;
} IAsyncAction;

typedef struct _IAsyncActionCompletedHandlerVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IAsyncActionCompletedHandler*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IAsyncActionCompletedHandler*);
	ULONG(STDMETHODCALLTYPE* Release)(IAsyncActionCompletedHandler*);

	// IAsyncActionCompletedHandler
	void(STDMETHODCALLTYPE* Invoke)(IAsyncActionCompletedHandler*, IAsyncAction*, AsyncStatus);
} IAsyncActionCompletedHandlerVtbl;

typedef struct _IAsyncActionCompletedHandler
{
	IAsyncActionCompletedHandlerVtbl* lpVtbl;
} IAsyncActionCompletedHandler;

// Foundation : Windows.Foundation.IAsyncInfo

typedef struct _IAsyncInfo IAsyncInfo;

typedef struct _IAsyncInfoVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IAsyncInfo*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IAsyncInfo*);
	ULONG(STDMETHODCALLTYPE* Release) (IAsyncInfo*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IAsyncInfo*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IAsyncInfo*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IAsyncInfo*, TrustLevel*);

	// IAsyncInfo
	HRESULT(STDMETHODCALLTYPE* get_Id) (IAsyncInfo*, UINT*);
	HRESULT(STDMETHODCALLTYPE* get_Status) (IAsyncInfo*, UINT*);
	HRESULT(STDMETHODCALLTYPE* get_ErrorCode) (IAsyncInfo*, HRESULT*);
	HRESULT(STDMETHODCALLTYPE* Cancel) (IAsyncInfo*);
	HRESULT(STDMETHODCALLTYPE* Close) (IAsyncInfo*);

} IAsyncInfoVtbl;

typedef struct _IAsyncInfo
{
	IAsyncInfoVtbl* lpVtbl;
	CRITICAL_SECTION		cs;
	AsyncStatus				status;
	LONG			RefCount;
} IAsyncInfo;

// System : Windows.System.IDispatcherQueue

typedef struct _IDispatcherQueue IDispatcherQueue;

typedef struct _IDispatcherQueueVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IDispatcherQueue*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IDispatcherQueue*);
	ULONG(STDMETHODCALLTYPE* Release) (IDispatcherQueue*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IDispatcherQueue*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IDispatcherQueue*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IDispatcherQueue*, TrustLevel*);

	// IDispatcherQueue
	HRESULT(STDMETHODCALLTYPE* CreateTimer) (IDispatcherQueue*, PPVOID);
	HRESULT(STDMETHODCALLTYPE* TryEnqueue) (IDispatcherQueue*, IDelegate*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* TryEnqueueWithPriority) (IDispatcherQueue*, INT32, IDelegate*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* add_ShutdownStarting) (IDispatcherQueue*, ITypedEventHandler*, EventRegistrationToken*);
	HRESULT(STDMETHODCALLTYPE* remove_ShutdownStarting) (IDispatcherQueue*, EventRegistrationToken);
	HRESULT(STDMETHODCALLTYPE* add_ShutdownCompleted) (IDispatcherQueue*, ITypedEventHandler*, EventRegistrationToken*);
	HRESULT(STDMETHODCALLTYPE* remove_ShutdownCompleted) (IDispatcherQueue*, EventRegistrationToken);

} IDispatcherQueueVtbl;

typedef struct _IDispatcherQueue2Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IDispatcherQueue*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IDispatcherQueue*);
	ULONG(STDMETHODCALLTYPE* Release) (IDispatcherQueue*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IDispatcherQueue*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IDispatcherQueue*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IDispatcherQueue*, TrustLevel*);

	// IDispatcherQueue2
	HRESULT(STDMETHODCALLTYPE* get_HasThreadAccess) (IDispatcherQueue*, BOOL*);

} IDispatcherQueue2Vtbl;

typedef struct _IDispatcherQueue
{
	IDispatcherQueueVtbl* lpVtbl;
	IDispatcherQueue* Parent;

	struct IDispatcherQueue2
	{
		IDispatcherQueue2Vtbl* lpVtbl;
		IDispatcherQueue* Parent;
	} cDispatcherQueue2;

	LONG				  RefCount;
	DWORD				  dwThreadId;
	HANDLE				  hThread;
	HHOOK				  Hook;
	IDispatcherQueue*	  pPrev;

	IEventHandlerEntry* shutdownStart;
	IEventHandlerEntry* shutdownCompleted;
} IDispatcherQueue;

extern IDispatcherQueueVtbl CDispatcherQueueVtbl;
extern IDispatcherQueue2Vtbl CDispatcherQueue2Vtbl;

// System : Windows.System.IDispatcherQueueTimer

typedef struct _IDispatcherQueueTimer IDispatcherQueueTimer;

typedef struct _IDispatcherQueueTimerVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IDispatcherQueueTimer*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IDispatcherQueueTimer*);
	ULONG(STDMETHODCALLTYPE* Release) (IDispatcherQueueTimer*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IDispatcherQueueTimer*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IDispatcherQueueTimer*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IDispatcherQueueTimer*, TrustLevel*);

	// IDispatcherQueueTimer
	HRESULT(STDMETHODCALLTYPE* get_Interval) (IDispatcherQueueTimer*, INT64*);
	HRESULT(STDMETHODCALLTYPE* put_Interval) (IDispatcherQueueTimer*, INT64);
	HRESULT(STDMETHODCALLTYPE* get_IsRunning) (IDispatcherQueueTimer*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* get_IsRepeating) (IDispatcherQueueTimer*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* put_IsRepeating) (IDispatcherQueueTimer*, BOOL);
	HRESULT(STDMETHODCALLTYPE* Start) (IDispatcherQueueTimer*);
	HRESULT(STDMETHODCALLTYPE* Stop) (IDispatcherQueueTimer*);
	HRESULT(STDMETHODCALLTYPE* add_Tick) (IDispatcherQueueTimer*, ITypedEventHandler*, EventRegistrationToken*);
	HRESULT(STDMETHODCALLTYPE* remove_Tick) (IDispatcherQueueTimer*, EventRegistrationToken);

} IDispatcherQueueTimerVtbl;

typedef struct _IDispatcherQueueTimer
{
	IDispatcherQueueTimerVtbl* lpVtbl;
	LONG RefCount;

	IDispatcherQueue* queue;
	HANDLE hTimer;
	BOOL IsRunning;
	BOOL IsRepeating;
	ULONG Interval;

	IEventHandlerEntry* tick;
} IDispatcherQueueTimer;

extern IDispatcherQueueTimerVtbl CDispatcherQueueTimerVtbl;

// System : Windows.System.IDispatcherQueueController

typedef struct _IDispatcherQueueController IDispatcherQueueController;

typedef struct _IDispatcherQueueControllerVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IDispatcherQueueController*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IDispatcherQueueController*);
	ULONG(STDMETHODCALLTYPE* Release) (IDispatcherQueueController*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IDispatcherQueueController*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IDispatcherQueueController*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IDispatcherQueueController*, TrustLevel*);

	// IDispatcherQueueController
	HRESULT(STDMETHODCALLTYPE* get_DispatcherQueue) (IDispatcherQueueController*, IDispatcherQueue**);
	HRESULT(STDMETHODCALLTYPE* ShutdownQueueAsync) (IDispatcherQueueController*, IAsyncAction**);

} IDispatcherQueueControllerVtbl;

typedef struct _IDispatcherQueueController
{
	IDispatcherQueueControllerVtbl* lpVtbl;
	LONG							RefCount;
	IDispatcherQueue* queue;
} IDispatcherQueueController;

extern IDispatcherQueueControllerVtbl CDispatcherQueueControllerVtbl;

// System : Windows.System.IDispatcherQueueStatics

typedef struct _IDispatcherQueueStatics IDispatcherQueueStatics;

typedef struct _IDispatcherQueueStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IDispatcherQueueStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IDispatcherQueueStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (IDispatcherQueueStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IDispatcherQueueStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IDispatcherQueueStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IDispatcherQueueStatics*, TrustLevel*);

	// IDispatcherQueueStatics
	HRESULT(STDMETHODCALLTYPE* GetForCurrentThread) (IDispatcherQueueStatics*, PVOID);

} IDispatcherQueueStaticsVtbl;

typedef struct _IDispatcherQueueStatics
{
	IDispatcherQueueStaticsVtbl* lpVtbl;
} IDispatcherQueueStatics;

extern IDispatcherQueueStatics CDispatcherQueueStatics;

// System : Windows.System.IDispatcherQueueControllerStatics

typedef struct _IDispatcherQueueControllerStatics IDispatcherQueueControllerStatics;

typedef struct _IDispatcherQueueControllerStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IDispatcherQueueControllerStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IDispatcherQueueControllerStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (IDispatcherQueueControllerStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IDispatcherQueueControllerStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IDispatcherQueueControllerStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IDispatcherQueueControllerStatics*, TrustLevel*);

	// IDispatcherQueueControllerStatics
	HRESULT(STDMETHODCALLTYPE* CreateOnDedicatedThread) (IDispatcherQueueControllerStatics*, IDispatcherQueueController**);

} IDispatcherQueueControllerStaticsVtbl;

typedef struct _IDispatcherQueueControllerStatics
{
	IDispatcherQueueControllerStaticsVtbl* lpVtbl;
} IDispatcherQueueControllerStatics;

extern IDispatcherQueueControllerStatics CDispatcherQueueControllerStatics;

// Gaming : Windows.Gaming.UI.IGameBarStatics

typedef struct _IGameBarStatics IGameBarStatics;

typedef struct _IGameBarStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IGameBarStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IGameBarStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (IGameBarStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IGameBarStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IGameBarStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IGameBarStatics*, TrustLevel*);

	// IGameBarStatics
	HRESULT(STDMETHODCALLTYPE* add_VisibilityChanged) (IGameBarStatics*, ITypedEventHandler*, EventRegistrationToken*);
	HRESULT(STDMETHODCALLTYPE* remove_VisibilityChanged) (IGameBarStatics*, EventRegistrationToken);
	HRESULT(STDMETHODCALLTYPE* add_IsInputRedirectedChanged) (IGameBarStatics*, ITypedEventHandler*, EventRegistrationToken*);
	HRESULT(STDMETHODCALLTYPE* remove_IsInputRedirectedChanged) (IGameBarStatics*, EventRegistrationToken);
	HRESULT(STDMETHODCALLTYPE* get_Visible) (IGameBarStatics*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* get_IsInputRedirected) (IGameBarStatics*, BOOL*);

} IGameBarStaticsVtbl;

typedef struct _IGameBarStatics
{
	IGameBarStaticsVtbl* lpVtbl;
} IGameBarStatics;

extern IGameBarStatics CGameBarStatics;

// UI : Windows.UI.ViewManagement.IAccessibilitySettings

typedef struct _IAccessibilitySettings IAccessibilitySettings;

typedef struct _IAccessibilitySettingsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IAccessibilitySettings*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IAccessibilitySettings*);
	ULONG(STDMETHODCALLTYPE* Release) (IAccessibilitySettings*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IAccessibilitySettings*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IAccessibilitySettings*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IAccessibilitySettings*, TrustLevel*);

	// IAccessibilitySettings
	HRESULT(STDMETHODCALLTYPE* get_HighContrast)(IAccessibilitySettings*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* get_HighContrastScheme)(IAccessibilitySettings*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* add_HighContrastChanged)(IAccessibilitySettings*, ITypedEventHandler*, EventRegistrationToken*);
	HRESULT(STDMETHODCALLTYPE* remove_HighContrastChanged)(IAccessibilitySettings*, EventRegistrationToken);

} IAccessibilitySettingsVtbl;

typedef struct _IAccessibilitySettings
{
	IAccessibilitySettingsVtbl* lpVtbl;
} IAccessibilitySettings;

extern IAccessibilitySettings CAccessibilitySettings;

// UI : Windows.UI.ViewManagement.IInputPane

typedef struct _IInputPane IInputPane;

typedef struct _IInputPaneVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IInputPane*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IInputPane*);
	ULONG(STDMETHODCALLTYPE* Release) (IInputPane*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IInputPane*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IInputPane*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IInputPane*, TrustLevel*);

	// IInputPane
	HRESULT(STDMETHODCALLTYPE* add_Showing)(IInputPane*, ITypedEventHandler*, EventRegistrationToken*);
	HRESULT(STDMETHODCALLTYPE* remove_Showing)(IInputPane*, EventRegistrationToken);
	HRESULT(STDMETHODCALLTYPE* add_Hiding)(IInputPane*, ITypedEventHandler*, EventRegistrationToken*);
	HRESULT(STDMETHODCALLTYPE* remove_Hiding)(IInputPane*, EventRegistrationToken);
	HRESULT(STDMETHODCALLTYPE* get_OccludedRect)(IInputPane*, RECT*);

} IInputPaneVtbl;

typedef struct _IInputPane2Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IInputPane*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IInputPane*);
	ULONG(STDMETHODCALLTYPE* Release) (IInputPane*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IInputPane*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IInputPane*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IInputPane*, TrustLevel*);

	// IInputPane2
	HRESULT(STDMETHODCALLTYPE* TryShow) (IInputPane*, BOOL*);
	HRESULT(STDMETHODCALLTYPE* TryHide) (IInputPane*, BOOL*);

} IInputPane2Vtbl;

typedef struct _IInputPane
{
	IInputPaneVtbl* lpVtbl;
	IInputPane* Parent;

	struct IInputPane2
	{
		IInputPane2Vtbl* lpVtbl;
		IInputPane* Parent;
	} cInputPane2;

	ULONG RefCount;
} IInputPane;

// UI : Windows.UI.ViewManagement.IInputPaneStatics

typedef struct _IInputPaneStatics IInputPaneStatics;

typedef struct _IInputPaneStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IInputPaneStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IInputPaneStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (IInputPaneStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IInputPaneStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IInputPaneStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IInputPaneStatics*, TrustLevel*);

	// IInputPaneStatics
	HRESULT(STDMETHODCALLTYPE* GetForCurrentView)(IInputPaneStatics*, IInputPane**);

} IInputPaneStaticsVtbl;

typedef struct _IInputPaneStatics
{
	IInputPaneStaticsVtbl* lpVtbl;
} IInputPaneStatics;

extern IInputPaneStatics CInputPaneStatics;

// UI : Windows.UI.ViewManagement.IInputPaneStatics2

typedef IInspectable IUIContext;
typedef struct _IInputPaneStatics2 IInputPaneStatics2;

typedef struct _IInputPaneStatics2Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IInputPaneStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IInputPaneStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (IInputPaneStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IInputPaneStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IInputPaneStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IInputPaneStatics*, TrustLevel*);

	// IInputPaneStatics2
	HRESULT(STDMETHODCALLTYPE* GetForUIContext)(IInputPaneStatics2*, IUIContext*, IInputPane**);

} IInputPaneStatics2Vtbl;

typedef struct _IInputPaneStatics2
{
	IInputPaneStatics2Vtbl* lpVtbl;
} IInputPaneStatics2;

extern IInputPaneStatics2 CInputPaneStatics2;

typedef struct _IInputPaneInterop IInputPaneInterop;

typedef struct _IInputPaneInteropVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IInputPaneStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IInputPaneStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (IInputPaneStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IInputPaneStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IInputPaneStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IInputPaneStatics*, TrustLevel*);

	// IInputPaneInterop
	HRESULT(STDMETHODCALLTYPE* GetForWindow) (IInputPaneInterop*, HWND, REFIID, PPVOID);

} IInputPaneInteropVtbl;

typedef struct _IInputPaneInterop
{
	IInputPaneInteropVtbl* lpVtbl;
} IInputPaneInterop;

extern IInputPaneInterop CInputPaneInterop;

// UI : Windows.UI.IColorHelperStatics

typedef struct _IColorHelperStatics IColorHelperStatics;

typedef struct _IColorHelperStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IColorHelperStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IColorHelperStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (IColorHelperStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IColorHelperStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IColorHelperStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IColorHelperStatics*, TrustLevel*);

	// IColorHelperStatics
	HRESULT(STDMETHODCALLTYPE* FromArgb)(IColorHelperStatics*, BYTE A, BYTE R, BYTE G, BYTE B, UIColor*);

} IColorHelperStaticsVtbl;

typedef struct _IColorHelperStatics
{
	IColorHelperStaticsVtbl* lpVtbl;
} IColorHelperStatics;

extern IColorHelperStatics CColorHelperStatics;

// UI : Windows.UI.IColorHelperStatics2

typedef struct _IColorHelperStatics2 IColorHelperStatics2;

typedef struct _IColorHelperStatics2Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IColorHelperStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IColorHelperStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (IColorHelperStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IColorHelperStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IColorHelperStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IColorHelperStatics*, TrustLevel*);

	// IColorHelperStatics2
	HRESULT(STDMETHODCALLTYPE* ToDisplayName)(IColorHelperStatics2*, UIColor, HSTRING*);

} IColorHelperStatics2Vtbl;

typedef struct _IColorHelperStatics2
{
	IColorHelperStatics2Vtbl* lpVtbl;
} IColorHelperStatics2;

extern IColorHelperStatics2 CColorHelperStatics2;

// System : Windows.System.Threading.ThreadpoolStatics

typedef struct _IWorkItemHandler IWorkItemHandler;

typedef struct _IWorkItemHandlerVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IWorkItemHandler*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IWorkItemHandler*);
	ULONG(STDMETHODCALLTYPE* Release)(IWorkItemHandler*);

	// IWorkItemHandler
	HRESULT(STDMETHODCALLTYPE* Invoke)(IWorkItemHandler*, IAsyncAction*);
} IWorkItemHandlerVtbl;

typedef struct _IWorkItemHandler
{
	IWorkItemHandlerVtbl* lpVtbl;
} IWorkItemHandler;

typedef struct _IThreadpoolStatics IThreadpoolStatics;

typedef struct _IThreadpoolStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface) (IThreadpoolStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef) (IThreadpoolStatics*);
	ULONG(STDMETHODCALLTYPE* Release) (IThreadpoolStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IThreadpoolStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IThreadpoolStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IThreadpoolStatics*, TrustLevel*);

	// IThreadpoolStatics
	HRESULT(STDMETHODCALLTYPE* RunAsync)(IThreadpoolStatics*, IWorkItemHandler*, IAsyncAction**);
	HRESULT(STDMETHODCALLTYPE* RunWithPriorityAsync)(IThreadpoolStatics*, IWorkItemHandler*, WorkItemPriority, IAsyncAction**);
	HRESULT(STDMETHODCALLTYPE* RunWithPriorityAndOptionsAsync)(IThreadpoolStatics*, IWorkItemHandler*, WorkItemPriority,
															   WorkItemOptions, IAsyncAction**);

} IThreadpoolStaticsVtbl;

typedef struct _IThreadpoolStatics
{
	IThreadpoolStaticsVtbl* lpVtbl;
} IThreadpoolStatics;

extern IThreadpoolStatics CThreadpoolStatics;

// Globalization : Windows.Globalization.ILanguage

typedef enum LanguageLayoutDirection
{
	LanguageLayoutDirection_Ltr = 0,
	LanguageLayoutDirection_Rtl = 1,
	LanguageLayoutDirection_TtbLtr = 2,
	LanguageLayoutDirection_TtbRtl = 3
} LanguageLayoutDirection;

typedef struct _ILanguage ILanguage;

typedef struct _ILanguageVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(ILanguage*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(ILanguage*);
	ULONG(STDMETHODCALLTYPE* Release)(ILanguage*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (ILanguage*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (ILanguage*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (ILanguage*, TrustLevel*);

	// ILanguage
	HRESULT(STDMETHODCALLTYPE* get_LanguageTag)(ILanguage*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_DisplayName)(ILanguage*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_NativeName)(ILanguage*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Script)(ILanguage*, HSTRING*);
} ILanguageVtbl;

typedef struct _ILanguage2Vtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(ILanguage*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(ILanguage*);
	ULONG(STDMETHODCALLTYPE* Release)(ILanguage*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (ILanguage*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (ILanguage*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (ILanguage*, TrustLevel*);

	// ILanguage2
	HRESULT(STDMETHODCALLTYPE* get_LayoutDirection)(ILanguage*, LanguageLayoutDirection*);
} ILanguage2Vtbl;

typedef struct _ILanguage
{
	ILanguageVtbl* lpVtbl;
	ILanguage* Parent;

	struct ILanguage2
	{
		ILanguage2Vtbl* lpVtbl;
		ILanguage* Parent;
	} cLanguage2;

	WCHAR Name[LOCALE_NAME_MAX_LENGTH];
	ULONG RefCount;
} ILanguage;

// Globalization : Windows.Globalization.ILanguageFactory

typedef struct _ILanguageFactory ILanguageFactory;

typedef struct _ILanguageFactoryVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(ILanguageFactory*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(ILanguageFactory*);
	ULONG(STDMETHODCALLTYPE* Release)(ILanguageFactory*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (ILanguageFactory*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (ILanguageFactory*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (ILanguageFactory*, TrustLevel*);

	// ILanguageFactory
	HRESULT(STDMETHODCALLTYPE* CreateLanguage)(ILanguageFactory*, HSTRING, ILanguage**);
} ILanguageFactoryVtbl;

typedef struct _ILanguageFactory
{
	ILanguageFactoryVtbl* lpVtbl;
} ILanguageFactory;

extern ILanguageFactory CLanguageFactory;

// Globalization : Windows.Globalization.IApplicationLanguagesStatics

typedef struct _IApplicationLanguagesStatics IApplicationLanguagesStatics;

typedef struct _IApplicationLanguagesStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IApplicationLanguagesStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IApplicationLanguagesStatics*);
	ULONG(STDMETHODCALLTYPE* Release)(IApplicationLanguagesStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IApplicationLanguagesStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IApplicationLanguagesStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IApplicationLanguagesStatics*, TrustLevel*);

	// IApplicationLanguagesStatics
	HRESULT(STDMETHODCALLTYPE* get_PrimaryLanguageOverride)(IApplicationLanguagesStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* put_PrimaryLanguageOverride)(IApplicationLanguagesStatics*, HSTRING);
	HRESULT(STDMETHODCALLTYPE* get_Languages)(IApplicationLanguagesStatics*, IVectorView_HSTRING**);
	HRESULT(STDMETHODCALLTYPE* get_ManifestLanguages)(IApplicationLanguagesStatics*, IVectorView_HSTRING**);
} IApplicationLanguagesStaticsVtbl;

typedef struct _IApplicationLanguagesStatics
{
	IApplicationLanguagesStaticsVtbl* lpVtbl;
} IApplicationLanguagesStatics;

extern IApplicationLanguagesStatics CApplicationLanguagesStatics;

// SysInfo : Windows.System.Profile.SystemManufacturers.ISmbiosInformationStatics

typedef struct _ISmbiosInformationStatics ISmbiosInformationStatics;

typedef struct _ISmbiosInformationStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(ISmbiosInformationStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(ISmbiosInformationStatics*);
	ULONG(STDMETHODCALLTYPE* Release)(ISmbiosInformationStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (ISmbiosInformationStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (ISmbiosInformationStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (ISmbiosInformationStatics*, TrustLevel*);

	// ISmbiosInformationStatics
	HRESULT(STDMETHODCALLTYPE* get_SerialNumber)(ISmbiosInformationStatics*, HSTRING*);
} ISmbiosInformationStaticsVtbl;

typedef struct _ISmbiosInformationStatics
{
	ISmbiosInformationStaticsVtbl* lpVtbl;
} ISmbiosInformationStatics;

extern ISmbiosInformationStatics CSmbiosInformationStatics;

// Networking : Windows.Networking.IHostName

typedef enum HostNameType
{
	HostNameType_DomainName = 0,
	HostNameType_Ipv4 = 1,
	HostNameType_Ipv6 = 2,
	HostNameType_Bluetooth = 3
} HostNameType;

typedef struct _IHostName IHostName;

typedef struct _IHostNameVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IHostName*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IHostName*);
	ULONG(STDMETHODCALLTYPE* Release)(IHostName*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IHostName*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IHostName*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IHostName*, TrustLevel*);

	// IHostName
	HRESULT(STDMETHODCALLTYPE* get_IPInformation)(IHostName*, IUnknown**); // IIPInformation
	HRESULT(STDMETHODCALLTYPE* get_RawName)(IHostName*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_DisplayName)(IHostName*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_CanonicalName)(IHostName*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* get_Type)(IHostName*, HostNameType*);
	HRESULT(STDMETHODCALLTYPE* IsEqual)(IHostName*, IHostName*, BOOL*);
} IHostNameVtbl;

typedef struct _IHostName
{
	IHostNameVtbl* lpVtbl;
	HSTRING RawName;
	HSTRING CanonicalName;
	HSTRING DisplayName;
	HostNameType Type;
	ULONG RefCount;
} IHostName;

// Networking : Windows.Networking.IHostNameStatics

typedef struct _IHostNameStatics IHostNameStatics;

typedef struct _IHostNameStaticsVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IHostNameStatics*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IHostNameStatics*);
	ULONG(STDMETHODCALLTYPE* Release)(IHostNameStatics*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IHostNameStatics*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IHostNameStatics*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IHostNameStatics*, TrustLevel*);

	// IHostNameStatics
	HRESULT(STDMETHODCALLTYPE* Compare)(IHostNameStatics*, HSTRING, HSTRING, INT*);
} IHostNameStaticsVtbl;

typedef struct _IHostNameStatics
{
	IHostNameStaticsVtbl* lpVtbl;
} IHostNameStatics;

extern IHostNameStatics CHostNameStatics;

// Networking : Windows.Networking.IHostNameFactory

typedef struct _IHostNameFactory IHostNameFactory;

typedef struct _IHostNameFactoryVtbl
{
	// IUnknown
	HRESULT(STDMETHODCALLTYPE* QueryInterface)(IHostNameFactory*, REFIID, PPVOID);
	ULONG(STDMETHODCALLTYPE* AddRef)(IHostNameFactory*);
	ULONG(STDMETHODCALLTYPE* Release)(IHostNameFactory*);

	// IInspectable
	HRESULT(STDMETHODCALLTYPE* GetIids) (IHostNameFactory*, PULONG, IID**);
	HRESULT(STDMETHODCALLTYPE* GetRuntimeClassName) (IHostNameFactory*, HSTRING*);
	HRESULT(STDMETHODCALLTYPE* GetTrustLevel) (IHostNameFactory*, TrustLevel*);

	// IHostNameFactory
	HRESULT(STDMETHODCALLTYPE* CreateHostName)(IHostNameFactory*, HSTRING, IHostName**);
} IHostNameFactoryVtbl;

typedef struct _IHostNameFactory
{
	IHostNameFactoryVtbl* lpVtbl;
} IHostNameFactory;

extern IHostNameFactory CHostNameFactory;


//
// roinit.c
//

KXCOMAPI HRESULT WINAPI RoInitialize(
	IN	RO_INIT_TYPE	InitType);

KXCOMAPI VOID WINAPI RoUninitialize(
	VOID);

//
// rofactry.c
//

HRESULT WINAPI RoGetActivationFactory(
	IN	HSTRING	ActivatableClassId,
	IN	REFIID	RefIID,
	OUT	PPVOID	Factory);

HRESULT WINAPI RoActivateInstance(
	IN	HSTRING			ActivatableClassId,
	OUT	IInspectable** Instance);

//
// roerror.c
//

BOOL WINAPI RoOriginateError(
	IN	HRESULT	Result,
	IN	HSTRING	Message);

BOOL WINAPI RoOriginateErrorW(
	IN	HRESULT	Result,
	IN	ULONG	Length,
	IN	PCWSTR	Message);

KXCOMAPI BOOL WINAPI RoOriginateLanguageException(
	IN	HRESULT		Result,
	IN	HSTRING		Message OPTIONAL,
	IN	IUnknown* LanguageException);

HRESULT WINAPI GetRestrictedErrorInfo(
	OUT	IUnknown** RestrictedErrorInfo);

//
// roapi.c
//

KXCOMAPI HRESULT WINAPI RoGetAgileReference(
	IN	ULONG		Options,
	IN	REFIID		RefIID,
	IN	IUnknown* pUnknown,
	OUT	IUnknown** AgileReference);

//
// mta.c
//

KXCOMAPI HRESULT WINAPI CoIncrementMTAUsage(
	OUT	PCO_MTA_USAGE_COOKIE	Cookie);

KXCOMAPI HRESULT WINAPI CoDecrementMTAUsage(
	IN	CO_MTA_USAGE_COOKIE		Cookie);

//
// winrt.c
//

KXCOMAPI ULONG WINAPI WindowsGetStringLen(
	IN	HSTRING	String);

KXCOMAPI PCWSTR WINAPI WindowsGetStringRawBuffer(
	IN	HSTRING	String,
	OUT	PULONG	Length OPTIONAL);

KXCOMAPI HRESULT WINAPI WindowsCreateString(
	IN	PCNZWCH			SourceString,
	IN	ULONG			SourceStringCch,
	OUT	HSTRING* String);

KXCOMAPI HRESULT WINAPI WindowsCreateStringReference(
	IN	PCWSTR			SourceString,
	IN	ULONG			SourceStringCch,
	OUT	HSTRING_HEADER* StringHeader,
	OUT	HSTRING* String);

KXCOMAPI HRESULT WINAPI WindowsDuplicateString(
	IN	HSTRING	OriginalString,
	OUT	HSTRING* DuplicatedString);

KXCOMAPI HRESULT WINAPI WindowsDeleteString(
	IN	HSTRING	String);

KXCOMAPI BOOL WINAPI WindowsIsStringEmpty(
	IN	HSTRING	String);

KXCOMAPI HRESULT WINAPI WindowsStringHasEmbeddedNull(
	IN	HSTRING	String,
	OUT	PBOOL	HasEmbeddedNull);

KXCOMAPI HRESULT WINAPI WindowsCompareStringOrdinal(
	IN	HSTRING	String1,
	IN	HSTRING	String2,
	OUT	PINT	ComparisonResult);

KXCOMAPI HRESULT WINAPI WindowsSubstring(
	IN	HSTRING	String,
	IN	ULONG	StartIndex,
	OUT	HSTRING* NewString);

KXCOMAPI HRESULT WINAPI WindowsSubstringWithSpecifiedLength(
	IN	HSTRING	OriginalString,
	IN	ULONG	StartIndex,
	IN	ULONG	SubstringLength,
	OUT	HSTRING* NewString);

KXCOMAPI HRESULT WINAPI WindowsConcatString(
	IN	HSTRING	String1,
	IN	HSTRING	String2,
	OUT	HSTRING* NewString);

KXCOMAPI HRESULT WINAPI WindowsPreallocateStringBuffer(
	IN	ULONG			Length,
	OUT	PPWSTR			CharacterBuffer,
	OUT	PHSTRING_BUFFER	BufferHandle);

KXCOMAPI HRESULT WINAPI WindowsDeleteStringBuffer(
	IN	HSTRING_BUFFER	BufferHandle);

KXCOMAPI HRESULT WINAPI WindowsPromoteStringBuffer(
	IN	HSTRING_BUFFER	BufferHandle,
	OUT	HSTRING* NewString);

//
// firefoxpatch.c
//

KXCOMAPI HRESULT WINAPI Ext_CoCreateInstance(
	IN	REFCLSID	RefCLSID,
	IN	LPUNKNOWN	OuterUnknown,
	IN	ULONG		ClassContext,
	IN	REFIID		RefIID,
	OUT	PPVOID		Instance);

KXCOMAPI HRESULT WINAPI Ext_CoCreateInstanceEx(
	IN		REFCLSID		RefCLSID,
	IN		LPUNKNOWN		OuterUnknown,
	IN		ULONG			ClassContext,
	IN		COSERVERINFO* ServerInfo,
	IN		ULONG			NumberOfInterfaces,
	IN OUT	MULTI_QI* Interfaces);

KXCOMAPI HRESULT WINAPI Ext_CoGetClassObject(
	IN	REFCLSID		RefCLSID,
	IN	ULONG			ClassContext,
	IN	COSERVERINFO* ServerInfo	OPTIONAL,
	IN	REFIID			RefIID,
	OUT	PPVOID			ClassObject);

typedef enum _OLE_TLS_FLAGS
{
	OLETLS_LOCALTID = 0x01,		// This TID is in the current process.
	OLETLS_UUIDINITIALIZED = 0x02,		// This Logical thread is init'd.
	OLETLS_INTHREADDETACH = 0x04,		// This is in thread detach. Needed
												// due to NT's special thread detach
												// rules.
												OLETLS_CHANNELTHREADINITIALZED = 0x08,		// This channel has been init'd
												OLETLS_WOWTHREAD = 0x10,		// This thread is a 16-bit WOW thread.
												OLETLS_THREADUNINITIALIZING = 0x20,		// This thread is in CoUninitialize.
												OLETLS_DISABLE_OLE1DDE = 0x40,		// This thread can't use a DDE window.
												OLETLS_APARTMENTTHREADED = 0x80,		// This is an STA apartment thread
												OLETLS_MULTITHREADED = 0x100,	// This is an MTA apartment thread
												OLETLS_IMPERSONATING = 0x200,	// This thread is impersonating
												OLETLS_DISABLE_EVENTLOGGER = 0x400,	// Prevent recursion in event logger
												OLETLS_INNEUTRALAPT = 0x800,	// This thread is in the NTA
												OLETLS_DISPATCHTHREAD = 0x1000,	// This is a dispatch thread
												OLETLS_HOSTTHREAD = 0x2000,	// This is a host thread
												OLETLS_ALLOWCOINIT = 0x4000,	// This thread allows inits
												OLETLS_PENDINGUNINIT = 0x8000,	// This thread has pending uninit
												OLETLS_FIRSTMTAINIT = 0x10000,	// First thread to attempt an MTA init
												OLETLS_FIRSTNTAINIT = 0x20000,	// First thread to attempt an NTA init
												OLETLS_APTINITIALIZING = 0x40000	// Apartment Object is initializing
} TYPEDEF_TYPE_NAME(OLE_TLS_FLAGS);

typedef struct _SWindowData
{
	HWND				Window;
	ULONG				FirstMessage;
	ULONG				LastMessage;
} TYPEDEF_TYPE_NAME(SWindowData);

typedef struct _CAptCallCtrl
{
	IMessageFilter* MessageFilter;
	BOOL				InMessageFilter;
	PVOID				TopCML;			// original datatype: CCliModalLoop
	SWindowData			WindowData[2];
} TYPEDEF_TYPE_NAME(CAptCallCtrl);

// NtCurrentTeb()->ReservedForOle points to one of these structures.
// See tagSOleTlsData in oletls.h (nt5src) for descriptions of values.
// The full definition of this structure can be found in the public symbol
// files for ole32.dll (this one is incomplete, I've replaced most of the
// pointers with PVOIDs and there is stuff missing from the end)
typedef struct _SOleTlsData
{
	PVOID			pvThreadBase;
	PVOID			pSmAllocator;
	ULONG			dwApartmentID;
	ULONG			dwFlags; // OLETLS_*
	LONG			TlsMapIndex;
	PVOID* ppTlsSlot;
	ULONG			cComInits;
	ULONG			cOleInits;
	ULONG			cCalls;
	PVOID			pCallInfo;
	PVOID			pFreeAsyncCall;
	PVOID			pFreeClientCall;
	PVOID			pObjServer;
	ULONG			dwTIDCaller;
	PVOID			pCurrentCtx;
	PVOID			pEmptyCtx;
	PVOID			pNativeCtx;
	ULONGLONG		ContextId; // This is 64-bit even on the 32-bit version of ole32.dll
	PVOID			pNativeApt;
	IUnknown* pCallContext;
	PVOID			pCtxCall;
	PVOID			pPS;
	PVOID			pvPendingCallsFront;
	PVOID			pvPendingCallsBack;
	CAptCallCtrl* pCallCtrl; // Initialized by CAptCallCtrl::CAptCallCtrl in ole32.dll
	PVOID			pTopSCS;
	IMessageFilter* pMsgFilter;
	HWND			hwndSTA;
	LONG			cORPCNestingLevel;
	ULONG			cDebugData;
	GUID			LogicalThreadId;
	PVOID			hThread;
	PVOID			hRevert;
	IUnknown* pAsyncRelease;
	HWND			hwndDdeServer;
	HWND			hwndDdeClient;
	ULONG			cServeDdeObjects;
	PVOID			pSTALSvrsFront;
	HWND			hwndClip;
	IDataObject* pDataObjClip;
	ULONG			dwClipSeqNum;
	ULONG			fIsClipWrapper;
	IUnknown* punkState;
	ULONG			cCallCancellation;
	ULONG			cAsyncSends;
	PVOID			pAsyncCallList;
	PVOID			pSurrogateList;
	PVOID			pRWLockTlsEntry;
	//...
} TYPEDEF_TYPE_NAME(SOleTlsData);