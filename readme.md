# Explorer Hooker
This DLL hooks The Windows Shell (explorer.exe) Native API functions responsible for retrieving files.
When Injected with any simple DLL injector, it uses the IAT hooking technique on File Explorer and all of its DLLs to redirect every single call.
On the override function, it modifies the FILE_ID_BOTH_DIR_INFORMATION struct, which acts as a linked list, so we can remove any node we don't want File Explorer to show.
