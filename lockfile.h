//stdint.h
#include <stdint.h>
//windows.h
#include <windows.h>
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
//ntifs.h
typedef NTSTATUS(NTAPI*PRTL_HEAP_COMMIT_ROUTINE)(PVOID,PVOID *,PSIZE_T);
typedef struct{
    ULONG Length;
    SIZE_T SegmentReserve;
    SIZE_T SegmentCommit;
    SIZE_T DeCommitFreeBlockThreshold;
    SIZE_T DeCommitTotalFreeThreshold;
    SIZE_T MaximumAllocationSize;
    SIZE_T VirtualMemoryThreshold;
    SIZE_T InitialCommit;
    SIZE_T InitialReserve;
    PRTL_HEAP_COMMIT_ROUTINE CommitRoutine;
    SIZE_T Reserved[2];
}RTL_HEAP_PARAMETERS,*PRTL_HEAP_PARAMETERS;
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
typedef ULONG(NTAPI*PRtlGetProcessHeaps)(ULONG,PVOID *);
typedef PVOID(NTAPI*PRtlCreateHeap)(ULONG,PVOID,SIZE_T,SIZE_T,PVOID,PRTL_HEAP_PARAMETERS);
typedef PVOID(NTAPI*PRtlAllocateHeap)(PVOID,ULONG,SIZE_T);
typedef PVOID(NTAPI*PRtlReAllocateHeap)(PVOID,ULONG,PVOID,SIZE_T);
typedef LOGICAL(NTAPI*PRtlFreeHeap)(PVOID,ULONG,PVOID);
typedef PVOID(NTAPI*PRtlDestroyHeap)(PVOID);
typedef HANDLE(WINAPI*PGetStdHandle)(DWORD);
typedef BOOL(WINAPI*PSetConsoleMode)(HANDLE,DWORD);
typedef BOOL(WINAPI*PSetConsoleTitleA)(LPCTSTR);
typedef BOOL(WINAPI*PFreeLibrary)(HMODULE);
typedef BOOL(WINAPI*PReadConsoleA)(HANDLE,LPVOID,DWORD,LPDWORD,LPVOID);
typedef BOOL(WINAPI*PWriteConsoleW)(HANDLE,LPVOID,DWORD,LPDWORD,LPVOID);
typedef DWORD(WINAPI*PGetFullPathNameW)(LPCWSTR,DWORD,LPWSTR,LPWSTR *);
typedef NTSTATUS(NTAPI*PNtClose)(HANDLE);
typedef NTSTATUS(NTAPI*PNtReadFile)(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,PVOID,ULONG,PLARGE_INTEGER,PULONG);
typedef NTSTATUS(NTAPI*PNtOpenFile)(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,ULONG,ULONG);
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
    //创建的堆
    PVOID Heap;
    //状态码
    NTSTATUS Status;
    //函数
    PRtlReAllocateHeap RtlReAllocateHeap;
    PWriteConsoleW WriteConsoleW;
    PNtReadFile NtReadFile;
    PNtOpenFile NtOpenFile;
}TempDataStruct;
//简写
#define file (*hand)
#define path (data->DosDevices)
#define ustr (data->Path)
#define plen (ustr.Length)
#define heap (data->Heap)
#define obja (data->ObjectAttributes)
#define iosb (data->IOStatusBlock)
#define stdo (data->Stdout)
#define buff (data->Buffer)
#define stts (data->Status)
#define RtlReAllocateHeap (data->RtlReAllocateHeap)
#define WriteConsoleW (data->WriteConsoleW)
#define NtReadFile (data->NtReadFile)
#define NtOpenFile (data->NtOpenFile)