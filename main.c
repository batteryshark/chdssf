/*
 * Loader Stub to inject our SPTD Emulation code into SSF.
 */
#include <stdio.h>
#include <Windows.h>

static char inject_dll_path[4096];
static char full_exe_path[4096];

void createShellcode(int ret, int str, unsigned char** shellcode, int* shellcodeSize)
{
    unsigned char* retChar = (unsigned char*) &ret;
    unsigned char* strChar = (unsigned char*) &str;
    int api = (int) GetProcAddress(LoadLibraryA("kernel32.dll"), "LoadLibraryA");
    unsigned char* apiChar = (unsigned char*) &api;
    unsigned char sc[] = {
            // Push ret
            0x68, retChar[0], retChar[1], retChar[2], retChar[3],
            // Push all flags
            0x9C,
            // Push all register
            0x60,
            // Push 0x66666666 (later we convert it to the string of injected dll)
            0x68, strChar[0], strChar[1], strChar[2], strChar[3],
            // Mov eax, 0x66666666 (later we convert it to LoadLibrary adress)
            0xB8, apiChar[0], apiChar[1], apiChar[2], apiChar[3],
            // Call eax
            0xFF, 0xD0,
            // Pop all register
            0x61,
            // Pop all flags
            0x9D,
            // Ret
            0xC3
    };

    *shellcodeSize = 22;
    *shellcode = (unsigned char*) malloc(22);
    memcpy(*shellcode, sc, 22);
}

int main(int argc, char* argv[]) {
    unsigned char* shellcode;
    int shellcodeLen;

    LPVOID remote_dllStringPtr;
    LPVOID remote_shellcodePtr;

    CONTEXT ctx;

    // Get the Full DLL Path for our injection.
    GetFullPathName("ssf_patch.dll",4096,inject_dll_path,NULL);
    printf("%s\n",inject_dll_path);
    // Create Process SUSPENDED
    PROCESS_INFORMATION pi;
    STARTUPINFOA Startup;
    LPSTR cmd_args = GetCommandLineA();
    ZeroMemory(&Startup, sizeof(Startup));
    ZeroMemory(&pi, sizeof(pi));
    GetFullPathName("SSF.exe",4096,full_exe_path,NULL);
    CreateProcessA(full_exe_path,cmd_args, NULL, NULL, NULL, CREATE_SUSPENDED, NULL, NULL, &Startup, &pi);

    // Initialize DLLs Already Present
    //ResumeThread(pi.hThread);
    //Sleep(10);
    //SuspendThread(pi.hThread);
    remote_dllStringPtr = VirtualAllocEx(pi.hProcess, NULL, strlen(inject_dll_path)+1, MEM_COMMIT, PAGE_READWRITE);
    printf("DLL Adress: %X\n", remote_dllStringPtr);

    printf("Get EIP\n");
    ctx.ContextFlags = CONTEXT_CONTROL;
    GetThreadContext(pi.hThread, &ctx);
    printf("EIP: %X\n", ctx.Eip);

    printf("Build Shellcode\n");
    createShellcode(ctx.Eip, (int) remote_dllStringPtr, &shellcode, &shellcodeLen);

    printf ("Created Shellcode: \n");
    for(int i=0; i<shellcodeLen; i++)
        printf ("%X ", shellcode[i]);
    printf("\n");

    // Allocate Memory for Shellcode
    printf("Allocating Remote Memory For Shellcode\n");
    remote_shellcodePtr = VirtualAllocEx(pi.hProcess, NULL, shellcodeLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    printf("Shellcode Adress: %X\n", remote_shellcodePtr);

    printf("Write DLL Path To Remote Process\n");
    WriteProcessMemory(pi.hProcess, remote_dllStringPtr, inject_dll_path, strlen(inject_dll_path)+1, NULL);

    printf("Write Shellcode To Remote Process\n");
    WriteProcessMemory(pi.hProcess, remote_shellcodePtr, shellcode, shellcodeLen, NULL);

    // Set EIP To Shellcode
    printf("Set EIP\n");

    ctx.Eip = (DWORD)remote_shellcodePtr;
    ctx.ContextFlags = CONTEXT_CONTROL;
    SetThreadContext(pi.hThread, &ctx);

    printf("Run The Shellcode\n");
    ResumeThread(pi.hThread);

    printf("Wait Till Code Was Executed\n");
    Sleep(8000); // Might want to turn this down... 8 seconds is a lot.

    printf("Free Remote Resources\n");
    VirtualFreeEx(pi.hProcess, remote_dllStringPtr, strlen(inject_dll_path)+1, MEM_DECOMMIT);
    VirtualFreeEx(pi.hProcess, remote_shellcodePtr, shellcodeLen, MEM_DECOMMIT);

    return 0;
}