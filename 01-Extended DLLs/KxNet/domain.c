#include "buildcfg.h"
#include "kxnetp.h"

KXNETAPI void WINAPI NetFreeAadJoinInformation(
	IN OPTIONAL PDSREG_JOIN_INFO pJoinInfo)
{ }

KXNETAPI HRESULT WINAPI NetGetAadJoinInformation(
	IN	LPCWSTR		TenantId OPTIONAL,
	OUT PPVOID		JoinInfo)
{
	if (!JoinInfo) return E_INVALIDARG;
	*JoinInfo = NULL;
	return S_OK;
}