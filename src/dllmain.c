#include "pch.h"
#include "types.h"
#include "hookers.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD64  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		SavePrehookAddresses();
		HookAllModules(GetCurrentProcessId());
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}