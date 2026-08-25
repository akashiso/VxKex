#include "buildcfg.h"
#include <KexComm.h>

EXTERN PKEX_PROCESS_DATA KexData;

#if defined(KEX_TARGET_TYPE_EXE) || defined(KEX_TARGET_TYPE_DLL)
#  if defined(KEX_ARCH_X64)
#    pragma comment(lib, "dwrite_x64.lib")
#    pragma comment(lib, "dwrw10_x64.lib")
#  elif defined(KEX_ARCH_X86)
#    pragma comment(lib, "dwrite_x86.lib")
#    pragma comment(lib, "dwrw10_x86.lib")
#  endif
#endif

const IID IID_IDWriteFactory;
const IID IID_IDWriteFactory3;

typedef enum _DWRITE_FACTORY_TYPE {
    DWRITE_FACTORY_TYPE_SHARED,
    DWRITE_FACTORY_TYPE_ISOLATED
} TYPEDEF_TYPE_NAME(DWRITE_FACTORY_TYPE);

DECLSPEC_IMPORT HRESULT WINAPI DWriteCreateFactory(
	IN	DWRITE_FACTORY_TYPE	factoryType,
	IN	REFIID              iid,
	OUT	IUnknown            **factory);

DECLSPEC_IMPORT HRESULT WINAPI DWriteCoreCreateFactory(
	IN	DWRITE_FACTORY_TYPE	factoryType,
	IN	REFIID              iid,
	OUT	IUnknown            **factory);

KXDWAPI HRESULT WINAPI Ext_DWriteCreateFactory(
	IN	DWRITE_FACTORY_TYPE	factoryType,
	IN	REFIID              iid,
	OUT	IUnknown            **factory);

void PatchDWriteFontFaceByDWriteFactory(IUnknown* dwfact);