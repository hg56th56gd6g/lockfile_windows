# lockfile.exe;lockfile_upx.exe

## 说明

锁定现有的文件,目录,设备,卷

("c:\","c:\TestDir","c:\TestDir\TestFile.txt";这种的都可以)

from"[https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntopenfile](https://)":opens an existing file, directory, device, or volume

## 使用

直接打开即可

# file.list

## 说明

用于让lockfile.exe读取要锁定的路径列表

## 格式

{[uint8位掩码] [uint16路径长度] [路径(utf16编码,但没有2字节头部,即windows unicode编码)]}*

## 位掩码

0b [后面的uint16是否大端序] [保留] [保留] [保留] [保留] [保留] [保留] [保留]

# IOfile.list.py

## 说明

用于从file.list读(r),写(w),添加(a)路径

## 使用

python IOfile.list.py [模式(r/w/a)] [路径]*
