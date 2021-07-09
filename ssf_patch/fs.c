// Filesystem Related Operations.

#include "global.h"
#include "sptd.h"
#include "fs.h"



// Hardcoded FS Patch Offset Constants
#define DEVICE_PATH "\\\\.\\"

#define SCSI_PASSTHROUGH_DIRECT 0x4D014
static HANDLE scsi_handle = NULL;
#define SCSI_HANDLE 0xB00BD001
static is_init_removable=NULL;
// CreateFileA Hook
HANDLE __stdcall rfx_CreateFileA(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile){
    OutputDebugStr("CreateFileA");
    OutputDebugStr(lpFileName);

    if(strstr(lpFileName,DEVICE_PATH)!= 0){
        OutputDebugStr("Optical Device Open Called.");
        return SCSI_HANDLE;
    }
    return CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
}

BOOL __stdcall rfx_CloseHandle(HANDLE hObject){
    if(hObject == SCSI_HANDLE){
        OutputDebugStr("Closing Virtual SPTD Handle");
        return TRUE;
    }
    CloseHandle(hObject);
}

BOOL __stdcall rfx_DeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped){
    if(hDevice == SCSI_HANDLE && dwIoControlCode == SCSI_PASSTHROUGH_DIRECT){
        //OutputDebugStr("IOCTL to DISC");
        return sptd_ioctl(hDevice,dwIoControlCode,lpInBuffer,nInBufferSize,lpOutBuffer,nOutBufferSize,lpBytesReturned,lpOverlapped);
    }
    return DeviceIoControl(hDevice,dwIoControlCode,lpInBuffer,nInBufferSize,lpOutBuffer,nOutBufferSize,lpBytesReturned,lpOverlapped);
}

// Return removable once, and that's it.
unsigned int __stdcall rfx_GetDriveTypeA(LPCSTR lpRootPathName){
    if(is_init_removable == NULL){
        is_init_removable = 1;
        return 5;
    }else{
        return 3;
    }

}

// Startup Routine
void patch_fs(){
    // Save root patch
    // Various file hooks
    Hook_IAT_Name("KERNEL32.dll","CreateFileA",&rfx_CreateFileA);
    Hook_IAT_Name("KERNEL32.dll","DeviceIoControl",&rfx_DeviceIoControl);
    Hook_IAT_Name("KERNEL32.dll","GetDriveTypeA",&rfx_GetDriveTypeA);
    Hook_IAT_Name("KERNEL32.dll","CloseHandle",&rfx_CloseHandle);
}