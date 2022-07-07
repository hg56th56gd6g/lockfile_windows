from struct import pack
print("\"python IOfile.list.py [mode(\"r\"/\"w\"/\"a\")] [path]*\"")
Success=0
ParamError=1
FormatError=2
def Main(Mode,FileList):
    if Mode=="r":
        with open("file.list","rb") as File:
            while True:
                Buffer=tuple(ord(Buffer) for Buffer in File.read(2))
                if not Buffer:
                    break
                if len(Buffer)!=2:
                    return FormatError
                Length=Buffer[0]+(Buffer[1]<<8)
                Buffer=File.read(Length)
                if len(Buffer)!=Length:
                    return FormatError
                FileList.append(Buffer.decode("utf16"))
        return Success
    elif Mode=="w":
        with open("file.list","wb") as File:
            for Path in FileList:
                Path=Path.decode("utf8").encode("utf16")[2::]
                File.write(pack("<H",len(Path)))
                File.write(Path)
        return Success
    elif Mode=="a":
        with open("file.list","ab") as File:
            for Path in FileList:
                Path=Path.decode("utf8").encode("utf16")[2::]
                File.write(pack("<H",len(Path)))
                File.write(Path)
        return Success
    else:
        return ParamError
if __name__=="__main__":
    from sys import argv
    if len(argv)<2:
        print("Command Line Error")
    else:
        Status=argv[1]
        argv=argv[2::]
        if Status=="r":
            FileList=[]
            Status=Main("r",FileList)
            if Status==Success:
                for Path in FileList:
                    print(Path)
                print("R_Success")
            elif Status==ParamError:
                print("R_ParamError")
            elif Status==FormatError:
                print("R_FormatError")
        elif Status=="w":
            Status=Main("w",argv)
            if Status==Success:
                for Path in argv:
                    print(Path)
                print("W_Success")
            elif Status==ParamError:
                print("W_ParamError")
            elif Status==FormatError:
                print("W_FormatError")
        elif Status=="a":
            Status=Main("a",argv)
            if Status==Success:
                for Path in argv:
                    print(Path)
                print("A_Success")
            elif Status==ParamError:
                print("A_ParamError")
            elif Status==FormatError:
                print("A_FormatError")
        else:
            print("Param_Mode Error")