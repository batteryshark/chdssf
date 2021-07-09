// Global Structures and Helpers

#include "global.h"

// Generic replacement functions to return a specified value.
int patch_return_zero(){return 0;}
int patch_return_one(){return 1;}

// [--Memory Patch Functions--]

// Patch sz number of bytes as NOP.
void patch_memory_nop(void* addr,unsigned int sz){
    unsigned int OldProtect = 0;
    VirtualProtect(addr, sz, PAGE_EXECUTE_READWRITE, &OldProtect);
    memset(addr,0x90,sz);
}

// Patch memory with an arbitrary buffer.
void patch_memory_buffer(void* addr, unsigned char* buffer, unsigned int sz){
    unsigned int OldProtect = 0;
    VirtualProtect(addr, sz, PAGE_EXECUTE_READWRITE, &OldProtect);
    memcpy(addr,buffer,sz);
}

// Patch memory with an arbitrary string.
void patch_memory_str(void* addr, char* str){
    unsigned int OldProtect = 0;
    unsigned int sz = strlen(str)+1;
    VirtualProtect(addr, sz, PAGE_EXECUTE_READWRITE, &OldProtect);
    strcpy(addr,str);
}

// Patch memory to add a jump to an arbitrary address.
void detour_jmp(void* new_addr,void* addr){
    // Calculate Jump Distance.
    unsigned int OldProtect = 0;
    int jmp_distance = (new_addr - (DWORD)addr) - 5;
    // Unprotect Memory to Write.
    VirtualProtect((void*)addr,4096, PAGE_EXECUTE_READWRITE,&OldProtect);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)(addr + 1)) = jmp_distance;
}

// Patch memory to add a call to an arbitrary address.
void detour_call(void* new_addr,void* addr){
    // Calculate Jump Distance.
    unsigned int OldProtect = 0;
    int jmp_distance = (new_addr - (DWORD)addr) - 5;
    // Unprotect Memory to Write.
    VirtualProtect((void*)addr,4096, PAGE_EXECUTE_READWRITE,&OldProtect);
    *((unsigned char*)(addr)) = 0xE8;
    *((int*)(addr + 1)) = jmp_distance;
}

// Patch memory to in-line replace a call to an IAT function without replacing the whole function.
void detour_function_ds(void * new_adr, void* addr){
    unsigned int OldProtect = 0;
    int call = (int)(new_adr);
    VirtualProtect(addr,4096, PAGE_EXECUTE_READWRITE, &OldProtect);
    *((unsigned char*)(addr)) = 0xFF;
    *((unsigned char*)(addr+1)) = 0x15;
    *((int*)(addr + 2)) = call;
}

// Patch an IAT entry to replace any calls to it with a new function by ordinal.
BOOL Hook_IAT_Ordinal(char* dll_name,char*func_name,unsigned int ordinal,DWORD replacement_function_ptr){
    IMAGE_DOS_HEADER *pDOSHeader;
    IMAGE_NT_HEADERS *pNTHeader;
    IMAGE_IMPORT_DESCRIPTOR *ImportDirectory;
    DWORD *OriginalFirstThunk;
    DWORD *FirstThunk;
    DWORD *address;
    DWORD *func_address;
    char *module_name="";
    DWORD overwrite;
    char *name;
    ULONG_PTR Cur_Ordinal=NULL;
    HANDLE hHandle;
    DWORD oldProtect;
    DWORD PEHeaderOffset;
    int i=0;

    hHandle = GetModuleHandle(NULL);

    if(hHandle == NULL){
        OutputDebugStr("there was an error in retrieving the handle");
        exit(0);
    }

    pDOSHeader = (IMAGE_DOS_HEADER *) hHandle;

    PEHeaderOffset = (DWORD) pDOSHeader->e_lfanew;

    pNTHeader = (IMAGE_NT_HEADERS *) ((DWORD) hHandle + PEHeaderOffset);

    ImportDirectory = (IMAGE_IMPORT_DESCRIPTOR *) ((DWORD) pNTHeader->OptionalHeader.DataDirectory[1].VirtualAddress + (DWORD) hHandle);
    module_name = (char *)(ImportDirectory->Name + (DWORD) hHandle);

    while(strcmp(module_name, dll_name) != 0){
        ImportDirectory++;
        module_name = (char *)(ImportDirectory->Name + (DWORD) hHandle);
    }


    OriginalFirstThunk = (DWORD *)((DWORD) ImportDirectory->OriginalFirstThunk + (DWORD) hHandle);
    FirstThunk = (DWORD *)((DWORD) ImportDirectory->FirstThunk + (DWORD) hHandle);

    while(*(OriginalFirstThunk+i) != 0x00000000){
        Cur_Ordinal = *(OriginalFirstThunk+i);
        Cur_Ordinal &= ~0x80000000;
        if(Cur_Ordinal == ordinal) {
            address=OriginalFirstThunk+i;
            break;
        }
        i++;
    }


    func_address = FirstThunk - OriginalFirstThunk + address;

    VirtualProtect(func_address, 0x4, 0x40, &oldProtect);
    overwrite = (DWORD) replacement_function_ptr;
    WriteProcessMemory(0xffffffff, func_address, &overwrite, 0x4, NULL);
    VirtualProtect(func_address, 0x4, 0x20, &oldProtect);
    return 0;

}

// Patch an IAT entry to replace any calls to it with a new function by name.
BOOL Hook_IAT_Name (char* dll_name, char* func_name,DWORD replacement_function_ptr){
    IMAGE_DOS_HEADER *pDOSHeader;
    IMAGE_NT_HEADERS *pNTHeader;
    IMAGE_IMPORT_DESCRIPTOR *ImportDirectory;
    DWORD *OriginalFirstThunk;
    DWORD *FirstThunk;
    DWORD *address;
    DWORD *func_address;
    char *module_name="";
    DWORD overwrite;
    char *name;
    HANDLE hHandle;
    DWORD oldProtect;
    DWORD PEHeaderOffset;
    int i=0;

    hHandle = GetModuleHandle(NULL);

    if(hHandle == NULL){
        OutputDebugStr("there was an error in retrieving the handle");
        exit(0);
    }

    pDOSHeader = (IMAGE_DOS_HEADER *) hHandle;

    PEHeaderOffset = (DWORD) pDOSHeader->e_lfanew;

    pNTHeader = (IMAGE_NT_HEADERS *) ((DWORD) hHandle + PEHeaderOffset);

    ImportDirectory = (IMAGE_IMPORT_DESCRIPTOR *) ((DWORD) pNTHeader->OptionalHeader.DataDirectory[1].VirtualAddress + (DWORD) hHandle);
    module_name = (char *)(ImportDirectory->Name + (DWORD) hHandle);

    while(strcmp(module_name, dll_name) != 0){
        ImportDirectory++;
        module_name = (char *)(ImportDirectory->Name + (DWORD) hHandle);
    }


    OriginalFirstThunk = (DWORD *)((DWORD) ImportDirectory->OriginalFirstThunk + (DWORD) hHandle);
    FirstThunk = (DWORD *)((DWORD) ImportDirectory->FirstThunk + (DWORD) hHandle);


    while(*(OriginalFirstThunk+i) != 0x00000000){
        name = (char *) (*(OriginalFirstThunk+i) + (DWORD) hHandle + 0x2);
        if(strcmp(name, func_name) == 0)
        {
            address=OriginalFirstThunk+i;
            break;
        }
        i++;
    }

    func_address = FirstThunk - OriginalFirstThunk + address;
    VirtualProtect(func_address, 0x4, 0x40, &oldProtect);
    overwrite = (DWORD) replacement_function_ptr;
    WriteProcessMemory(0xffffffff, func_address, &overwrite, 0x4, NULL);
    VirtualProtect(func_address, 0x4, 0x20, &oldProtect);
    return 0;
}
