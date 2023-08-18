# Explorer Hooker
This software can hide files from File Explorer!

# How does it work?
This DLL hooks The Windows Shell (explorer.exe) Native API functions responsible for retrieving files.
When Injected with any simple DLL injector, it uses the IAT hooking technique on File Explorer and all of its DLLs to redirect every single call.
The overridden function modifies the FILE_ID_BOTH_DIR_INFORMATION struct, which acts as a linked list in which every node represents a file, so we can remove any file we don't want to show.