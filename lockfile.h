//other
#define PROCESS_MODE_BACKGROUND_BEGIN ((DWORD)0x00100000)
#define THREAD_MODE_BACKGROUND_BEGIN  ((DWORD)0x00010000)
//stdint.h
#include <stdint.h>
//windows.h
#include <windows.h>
//memoryapi.h
WINBASEAPI WINBOOL WINAPI SetProcessWorkingSetSizeEx (HANDLE hProcess, SIZE_T dwMinimumWorkingSetSize, SIZE_T dwMaximumWorkingSetSize, DWORD Flags);
//winnt.h
#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1FF)
//ntdef.h
#define FILE_SEQUENTIAL_ONLY              0x00000004
#define FILE_SYNCHRONOUS_IO_NONALERT      0x00000020
#define FILE_NON_DIRECTORY_FILE           0x00000040
#define OBJ_CASE_INSENSITIVE    0x00000040
typedef LONG NTSTATUS;
typedef ULONG LOGICAL;
typedef struct{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
}UNICODE_STRING,*PUNICODE_STRING;
typedef struct{
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
}OBJECT_ATTRIBUTES,*POBJECT_ATTRIBUTES;
//ntstatus.h
#define STATUS_SUCCESS ((NTSTATUS)0x00000000)
#define STATUS_END_OF_FILE ((NTSTATUS)0xC0000011)
//wdm.h
#define FILE_OPENED 0x00000001
typedef struct{
    union{
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
}IO_STATUS_BLOCK,*PIO_STATUS_BLOCK;
typedef VOID(NTAPI*PIO_APC_ROUTINE)(PVOID,PIO_STATUS_BLOCK,ULONG);
//函数
typedef NTSTATUS(NTAPI*PNtReadFile)(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,PVOID,ULONG,PLARGE_INTEGER,PULONG);
typedef NTSTATUS(NTAPI*PNtOpenFile)(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,ULONG,ULONG);
typedef NTSTATUS(NTAPI*PNtClose)(HANDLE);
//data(临时变量)的结构
typedef struct{
    //储存路径
    uint8_t DosDevices[24];
    uint8_t Buffer[65511];
    //用于readfile和openfile
    IO_STATUS_BLOCK IOStatusBlock;
    //用于openfile
    OBJECT_ATTRIBUTES ObjectAttributes;
    //ObjectAttributes的ObjectName(文件路径)
    UNICODE_STRING Path;
    //stdout
    HANDLE Stdout;
    //file.list句柄
    HANDLE FileHandle;
    //临时句柄
    HANDLE TempHandle;
    //状态码
    NTSTATUS Status;
    //函数
    PNtReadFile NtReadFile;
    PNtOpenFile NtOpenFile;
    PNtClose NtClose;
}TempDataStruct;
//简写
#define file (data->FileHandle)
#define hand (data->TempHandle)
#define none (*((DWORD *)&hand))
#define ntdl (*((HMODULE *)&hand))
#define path (data->DosDevices)
#define ustr (data->Path)
#define plen (ustr.Length)
#define obja (data->ObjectAttributes)
#define iosb (data->IOStatusBlock)
#define stdo (data->Stdout)
#define buff (data->Buffer)
#define stts (data->Status)
#define NtReadFile (data->NtReadFile)
#define NtOpenFile (data->NtOpenFile)
#define NtClose (data->NtClose)