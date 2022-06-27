//这是一个试图不使用crt,尽可能隐式调用更底层函数(但不运行在内核态)的测试项目
#include "lockfile.h"
void main(void){
    //在销毁堆之后还要用的变量
    PVOID proc;
    HANDLE stdi,*hand;
    HMODULE krnl,ntdl;
    FARPROC func;
    TempDataStruct *data;
    uintmax_t flen;
    //必要的初始化步骤,失败直接退出
    if(!(
        //加载kernel32.dll和ntdll.dll用于获取函数指针
        (krnl=LoadLibraryA("kernel32"))&&
        (ntdl=LoadLibraryA("ntdll"))&&
        //初始化内存分配,将临时的变量放在新创建的堆,解析完成后等待用户关闭的时候销毁堆,正常free内存占用不会完全释放,这保证了等待控制台输入(即已经锁定文件等待用户关闭程序时)的低内存占用
        (func=GetProcAddress(ntdl,"RtlGetProcessHeaps"))&&
        (((PRtlGetProcessHeaps)func)(1,&proc))&&
        (flen=(uintmax_t)GetProcAddress(ntdl,"RtlAllocateHeap"))&&
        (hand=(HANDLE *)(((PRtlAllocateHeap)flen)(proc,HEAP_NO_SERIALIZE,sizeof(PVOID))))&&
        (func=GetProcAddress(ntdl,"RtlCreateHeap"))&&
        (*((PVOID *)hand)=((PRtlCreateHeap)func)(HEAP_GROWABLE|HEAP_NO_SERIALIZE,0,0,0,0,0))&&
        (data=(TempDataStruct *)(((PRtlAllocateHeap)flen)(*((PVOID *)hand),HEAP_NO_SERIALIZE,sizeof(TempDataStruct))))&&
        //加载函数
        (RtlReAllocateHeap=(PRtlReAllocateHeap)GetProcAddress(ntdl,"RtlReAllocateHeap"))&&
        (WriteConsoleW=(PWriteConsoleW)GetProcAddress(krnl,"WriteConsoleW"))&&
        (NtReadFile=(PNtReadFile)GetProcAddress(ntdl,"NtReadFile"))&&
        (NtOpenFile=(PNtOpenFile)GetProcAddress(ntdl,"NtOpenFile"))&&
        //获取stdio(必须是控制台)句柄
        (func=GetProcAddress(krnl,"GetStdHandle"))&&
        ((stdi=((PGetStdHandle)func)(STD_INPUT_HANDLE))!=INVALID_HANDLE_VALUE)&&
        ((stdo=((PGetStdHandle)func)(STD_OUTPUT_HANDLE))!=INVALID_HANDLE_VALUE)&&
        //设置io模式(保证可读的情况下高性能,无输入缓冲(按任意键继续))
        (func=GetProcAddress(krnl,"SetConsoleMode"))&&
        (((PSetConsoleMode)func)(stdi,0))&&
        (((PSetConsoleMode)func)(stdo,ENABLE_PROCESSED_OUTPUT|ENABLE_WRAP_AT_EOL_OUTPUT))
    )){return;}
    //将创建的堆的句柄复制到临时区
    heap=*((PVOID *)hand);
    //填充ObjectAttributes,之后不再改变,几个规则:不区分大小写;
    obja.Length=sizeof(OBJECT_ATTRIBUTES);
    obja.RootDirectory=0;
    obja.ObjectName=&ustr;
    obja.Attributes=OBJ_CASE_INSENSITIVE;
    obja.SecurityDescriptor=0;
    obja.SecurityQualityOfService=0;
    //填充Path,之后除了Length不再改变
    ustr.MaximumLength=65535;
    ustr.Buffer=(PWSTR)&path;
    //标题=LockFile
    if(func=GetProcAddress(krnl,"SetConsoleTitleA")){
        ((PSetConsoleTitleA)func)("LockFile");
    }
    //BuildBy hg56th56gd6g
    WriteConsoleW(stdo,L"BuildBy hg56th56gd6g\n\n",22,0,0);
    //NtOpenFile/NtCreateFile的对象名称是"\DosDevices\[FilePath(c:\Windows\...)]"或"\Device\[设备名,例如HarddiskVolume1]\[FilePath(Windows\...)]"
    path[0]='\\';path[1]=0;path[2]='D';path[3]=0;path[4]='o';path[5]=0;path[6]='s';path[7]=0;path[8]='D';path[9]=0;path[10]='e';path[11]=0;path[12]='v';path[13]=0;path[14]='i';path[15]=0;path[16]='c';path[17]=0;path[18]='e';path[19]=0;path[20]='s';path[21]=0;path[22]='\\';path[23]=0;
    if(!(
        //获取完整路径
        (func=GetProcAddress(krnl,"GetFullPathNameW"))&&
        (plen=24+(2*(((PGetFullPathNameW)func)(L"file.list",32755,(LPWSTR)&buff,0))))&&
        //打开file.list,几个规则:只读数据(不能读属性,扩展属性等);独占文件;顺序操作;同步操作;文件不是目录
        (NtOpenFile(&file,FILE_READ_DATA|SYNCHRONIZE,&obja,&iosb,0,FILE_SEQUENTIAL_ONLY|FILE_SYNCHRONOUS_IO_NONALERT|FILE_NON_DIRECTORY_FILE)==STATUS_SUCCESS)&&
        (iosb.Status==STATUS_SUCCESS)&&
        (iosb.Information==FILE_OPENED)
    )){
        //打开失败
        flen=0;
        WriteConsoleW(stdo,L"OpenErr",7,0,0);
        goto ends;
    }
    flen=1;
    //解析,结构为{[uint8][uint16路径长度(0-65511)][路径(绝对路径,unicode)]}*
    for(;;){
        plen=0;
        //读3字节,判断端序并设置plen
        stts=NtReadFile(file,0,0,0,&iosb,&buff,3,0,0);
        //读完了
        if(
            (stts==STATUS_END_OF_FILE)||
            (iosb.Status==STATUS_END_OF_FILE)
        ){
            WriteConsoleW(stdo,L"\nok",3,0,0);
            goto ends;
        }
        //读到了
        if(
            (stts==STATUS_SUCCESS)&&
            (iosb.Status==STATUS_SUCCESS)
        ){
            //判断是否读到3字节
            if(iosb.Information==3){
                //前面的uint8是位掩码,0b[路径长度是否大端序][][][][][][][]
                if((buff[0])&((uint8_t)0b10000000)){
                    plen=(buff[1]<<8)+buff[2];
                }else{
                    plen=buff[1]+(buff[2]<<8);
                }
                //检查plen并读plen字节
                if(
                    (plen<65512)&&
                    (NtReadFile(file,0,0,0,&iosb,&buff,plen,0,0)==STATUS_SUCCESS)&&
                    (iosb.Status==STATUS_SUCCESS)&&
                    (iosb.Information==plen)
                ){
                    //扩展句柄数组
                    if(hand=(HANDLE *)RtlReAllocateHeap(proc,HEAP_NO_SERIALIZE,hand,sizeof(HANDLE)*flen+1)){
                        //打印路径并把plen加上L"\\DosDevices\\"的长度
                        WriteConsoleW(stdo,&buff,plen>>1,0,0);
                        plen+=24;
                        //打开路径,几个规则:拥有所有权限(如果没有权限,即使没有设置分享权限,也不能独占);独占文件;
                        if(
                            (NtOpenFile(&(hand[flen]),FILE_ALL_ACCESS,&obja,&iosb,0,0)==STATUS_SUCCESS)&&
                            (iosb.Status==STATUS_SUCCESS)&&
                            (iosb.Information==FILE_OPENED)
                        ){
                            //打开成功
                            flen++;
                            WriteConsoleW(stdo,L"|T\n",3,0,0);
                            continue;
                        }
                        //打开失败
                        WriteConsoleW(stdo,L"|F\n",3,0,0);
                        continue;
                    }
                    //扩展句柄列表失败
                    WriteConsoleW(stdo,L"\nMemErr",7,0,0);
                    goto ends;
                }
            }
        }
        //读file.list失败或格式错误
        WriteConsoleW(stdo,L"\nReadErr",8,0,0);
        goto ends;
    }
    //PressAnyKeyToExit
    ends:
    WriteConsoleW(stdo,L"\nPressAnyKeyToExit",18,0,0);
    //销毁储存临时变量的堆
    if(func=GetProcAddress(ntdl,"RtlDestroyHeap")){
        ((PRtlDestroyHeap)func)(heap);
    }
    //按任意键继续
    if(func=GetProcAddress(krnl,"ReadConsoleA")){
        ((PReadConsoleA)func)(stdi,(LPVOID)&data,1,(LPDWORD)&data,0);
    }
    //关闭file.list和已打开(锁定)的文件
    if(func=GetProcAddress(ntdl,"NtClose")){
        while(flen--){
            ((PNtClose)func)(hand[flen]);
        }
    }
    //释放储存句柄的数组
    if(func=GetProcAddress(ntdl,"RtlFreeHeap")){
        ((PRtlFreeHeap)func)(proc,HEAP_NO_SERIALIZE,hand);
    }
    //卸载kernel32和ntdll
    if(func=GetProcAddress(krnl,"FreeLibrary")){
        ((PFreeLibrary)func)(krnl);
        ((PFreeLibrary)func)(ntdl);
    }
}