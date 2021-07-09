//
// Created by batte on 4/29/2017.
//

#ifndef STORM_PATCH_GLOBALS_H
#define STORM_PATCH_GLOBALS_H

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Hardcoded Serial Offset Constants
#define COM1_GUNIO_HANDLE 0xB00BC001
#define COM2_JVS_HANDLE 0xB00BC003

int init_global(void);
int patch_return_zero(void);
int patch_return_one(void);
void patch_memory_nop(void* addr,unsigned int sz);
void patch_memory_buffer(void* addr, unsigned char* buffer, unsigned int sz);
void detour_jmp(void* new_addr,void* addr);
void detour_call(void* new_addr,void* addr);
void detour_function_ds(void * new_adr, void* addr);
void patch_memory_str(void* addr,char* str);
BOOL Hook_IAT_Name(char* dll_name, char* func_name,DWORD replacement_function_ptr);
BOOL Hook_IAT_Ordinal(char* dll_name,char*func_name,unsigned int ordinal,DWORD replacement_function_ptr);

#endif //STORM_PATCH_GLOBALS_H
