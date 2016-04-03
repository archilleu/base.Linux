//---------------------------------------------------------------------------
#include "function.h"
#include <fcntl.h>
//---------------------------------------------------------------------------
namespace base
{
//---------------------------------------------------------------------------
//组装字符串，和printf类似
std::string CombineString(const char* format, ...)
{
    if(0 == format)
        return "";

    va_list args;
    char*   buffer = 0;
    va_start(args, format);
        int err_code = vasprintf(&buffer, format, args);
    va_end(args);
    if(-1 == err_code)
    {
        free(buffer);
        return "";
    }
    
    std::string result = buffer;
    free(buffer);

    return result; //RVO
}
//---------------------------------------------------------------------------
//二进制数据转换为字符串(1byte<==>2byte)
std::string BinToString(const unsigned char* buffer, size_t len)
{
     char           bin[3];
     std::string    result;
     for(size_t i=0; i<len; i++)
     {
         snprintf(bin, 3, "%02X", buffer[i]);
         result.append(bin);
     }

     return result;//RVO
}
//---------------------------------------------------------------------------
MemoryBlock StringToBin(const std::string& buffer)
{
    return StringToBin(reinterpret_cast<const unsigned char*>(buffer.c_str()), buffer.size());
}
//---------------------------------------------------------------------------
MemoryBlock StringToBin(const unsigned char* buffer, size_t len)
{
    static const unsigned char char_bin[256] =
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    if(0 != len%2)
        return MemoryBlock();

    MemoryBlock mb(len/2);
    for(size_t i=0; i<mb.len(); i++)
    {
        unsigned char ch = char_bin[buffer[i*2]];
        unsigned char cl = char_bin[buffer[i*2 + 1]];
        mb[i] = static_cast<char>(ch*16 + cl);
    }

    return mb;//RVO
}
//---------------------------------------------------------------------------
//二进制数据转换为等值的字符(1byte<==>1byte)
std::string BinToChars(const unsigned char* buffer, size_t len)
{
    std::string str;
    for(size_t i=0; i<len; i++)
    {
        str.push_back(buffer[i]);
    }

    return str;//RVO
}
//---------------------------------------------------------------------------
MemoryBlock CharsToBin(const std::string& buffer)
{
    return CharsToBin(buffer.c_str());
}
//---------------------------------------------------------------------------
MemoryBlock CharsToBin(const char* buffer)
{
    MemoryBlock mb(strlen(buffer));
    for(size_t i=0; i<mb.len(); i++)
        mb[i] = buffer[i];

    return mb;//RVO
}
//---------------------------------------------------------------------------
//获取程序运行的路径
std::string RunPathFolder()
{
    char path[PATH_MAX] = {0};
    ssize_t rlen = readlink("/proc/self/exe", path, PATH_MAX);
    if(-1 == rlen)
        return "";

    //include program's name
    //path[path_len] = '\0';
    for(ssize_t i=rlen; 0<=i; i--)
    {
        if('/' == path[i])
        {
            path[i] ='\0';
            break;
        }
    }

    //in root folder
    if('\0' == path[0])
    {
        path[0] = '/';
        path[1] = '\0';
    }

    return path;
}
//---------------------------------------------------------------------------
std::string RunPathFileName(const std::string& name)
{
    return RunPathFileName(name.c_str());
}
//---------------------------------------------------------------------------
std::string RunPathFileName(const char* name)
{
    std::string path = RunPathFolder();
    if(path.empty())
        return "";

    if(('/'==path[0]) && ('\0'==path[1]))
        return path+name;

    return path + "/" + name;
}
//---------------------------------------------------------------------------
std::string PathParent(const std::string& path)
{
    return PathParent(path.c_str());
}
//---------------------------------------------------------------------------
std::string PathParent(const char* path)
{
    if(0 == path)       return "";
    if('/' != path[0])  return "";
    if('\0' == path[1]) return "/";

    size_t len = strlen(path);
    if('/' == path[len-1])
        len -= 1;

    while(len)
    {
        if('/' == path[len-1])
            break;

        len--;
    }

    if(1 == len)
        return "/";

    return std::string(path, len-1);
}
//---------------------------------------------------------------------------
std::string PathName(const std::string& path)
{
    return PathName(path.c_str());
}
//---------------------------------------------------------------------------
std::string PathName(const char* path)
{
    if(0 == path)       return "";
    if('/' != path[0])  return "";
    if('\0' == path[1]) return "";

    size_t len = strlen(path);
    if('/' == path[len-1])
        len -= 1;

    while(len)
    {
        if('/' == path[len-1])
            break;

        len--;
    }

    if(1 == len)
        return "";

    return std::string(path+len);
}
//---------------------------------------------------------------------------
//文件夹操作
bool FolderCreate(const std::string& path, bool recursive)
{
    if(path.empty())
        return false;

    if(false == recursive)
    {
        if(-1 == mkdir(path.c_str(), 0770))
        {
            if(EEXIST != errno)
                return false;
        }

        return true;
    }

    for(size_t pos=path.find("/", 0); std::string::npos!=pos; pos=path.find('/', pos+1))
    {
        if(0 == pos)    //find path is "/"
            continue;

        std::string sub_path(path.c_str(), pos);
        if(-1 == mkdir(sub_path.c_str(), 0770))
        {
            if(EEXIST == errno)
                continue;

            return false;
        }
    }

    if(-1 == mkdir(path.c_str(), 0770))
    {
        if(EEXIST != errno)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool FolderDelete(const std::string& path, bool recursive)
{
    if(path.empty())
        return false;

    /*todo recursive = true
     */
    recursive = false;
    
    if(false == recursive)
    {
        if(-1 == rmdir(path.c_str()))
            return false;

        return true;
    }

    //清空文件和文件夹
    for(size_t pos=path.find("/", 0); std::string::npos!=pos; pos=path.find('/', pos+1))
    {
        if(0 == pos)    //find path is "/"
            continue;

        std::string sub_path(path, pos);
        if(-1 == rmdir(sub_path.c_str()))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool FolderExist(const std::string& path)
{
    return FolderExist(path.c_str());
}
//---------------------------------------------------------------------------
bool FolderExist(const char* path)
{
    if(0 == path)
        return false;

    struct stat stat_info;
    if(-1 == stat(path, &stat_info))
        return false;

    return S_ISDIR(stat_info.st_mode);
}
//---------------------------------------------------------------------------
//文件操作
bool FileDelete(const std::string& pathname)
{
    return FileDelete(pathname.c_str());
}
//---------------------------------------------------------------------------
bool FileDelete(const char* pathname)
{
    if(0 == pathname)
        return false;

    return (0==unlink(pathname));
}
bool FileExist(const std::string& pathname)
{
    return FileExist(pathname.c_str());
}
//---------------------------------------------------------------------------
bool FileExist(const char* pathname)
{
    if(0 == pathname)
        return false;

    struct stat stat_info;
    if(-1 == stat(pathname, &stat_info))
        return false;

    return S_ISREG(stat_info.st_mode);
}
//---------------------------------------------------------------------------
bool LoadFile(const std::string& path, MemoryBlock* result)
{
    return LoadFile(path.c_str(), result);
}
//---------------------------------------------------------------------------
bool LoadFile(const char* path, MemoryBlock* result)
{
    int fd = open(path, O_RDONLY);
    if(0 > fd)
        return false;

    //获取文件大小
    struct stat file_info;
    int err_code = fstat(fd, &file_info);
    if(0 > err_code)
    {
        close(fd);
        return false;
    }
    result->Resize(file_info.st_size);

    size_t offset   = 0;
    size_t size     = file_info.st_size;
    for(;0<size;)
    {
        ssize_t rlen = read(fd, result->dat()+offset, UNIT_KB);
        if(0 > rlen)
        {
            close(fd);
            return false;
        }

        offset  += rlen;
        size    -= rlen;
    }

    close(fd);
    return true;
}
//---------------------------------------------------------------------------
bool SaveFile(const std::string& path, const char* dat, size_t len)
{
    return SaveFile(path.c_str(), dat, len);
}
//---------------------------------------------------------------------------
bool SaveFile(const char* path, const char* dat, size_t len)
{
    int fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 777);
    if(0 == fd)
        return false;

    size_t offset = 0;
    for(;0<len;)
    {
        ssize_t wlen = write(fd, dat+offset, len);
        if(0 > wlen)
        {
            close(fd);
            unlink(path);
            return false;
        }

        offset  += wlen;
        len     -= wlen;
    }

    close(fd);
    return true;
}
//---------------------------------------------------------------------------
//文档
bool DocumentExist(const std::string& pathname)
{
    return DocumentExist(pathname.c_str());
}
//---------------------------------------------------------------------------
bool DocumentExist(const char* pathname)
{
    if(0 == pathname)
        return false;

    struct stat stat_info;
    if(-1 == stat(pathname, &stat_info))
        return false;

    return true;
}
//---------------------------------------------------------------------------
}//namespace base
