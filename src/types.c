#include "pch.h"
#include "types.h"

// link with static lib that has the compiled functions  
#pragma comment(lib,"ntdll.lib")

NtQueryDirectoryFileEx_T NtQueryDirectoryFileEx_static;
NtQueryDirectoryFile_T NtQueryDirectoryFile_static;

// save the addresses of the functions before hooking explorer's ntdll.dll
void SavePrehookAddresses()
{
	NtQueryDirectoryFileEx_static = (NtQueryDirectoryFileEx_T)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryDirectoryFileEx");
	NtQueryDirectoryFile_static = (NtQueryDirectoryFile_T)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryDirectoryFile");
}