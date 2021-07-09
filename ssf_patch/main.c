#include "main.h"
#include "global.h"
#include "fs.h"
#include "sptd.h"

#include <stdio.h>
#include <Windows.h>


void patch_binary(){

    init_sptd();
    patch_fs();
}

// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    if (fdwReason == DLL_PROCESS_ATTACH ){ patch_binary(); }
    return TRUE;
}