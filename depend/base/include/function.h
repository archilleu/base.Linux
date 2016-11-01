//---------------------------------------------------------------------------
#ifndef BASE_FUNCTION_H_
#define BASE_FUNCTION_H_
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
namespace base
{

class MemoryBlock;

//---------------------------------------------------------------------------
//组装字符串，和printf类似
std::string CombineString(const char* format, ...);
//---------------------------------------------------------------------------
//二进制数据转换为字符串(1byte<==>2byte)
std::string BinToString(const unsigned char* buffer, size_t len);
MemoryBlock StringToBin(const std::string& buffer);
MemoryBlock StringToBin(const unsigned char* buffer, size_t len);
//---------------------------------------------------------------------------
//二进制数据转换为等值的字符(1byte<==>1byte)
std::string BinToChars(const unsigned char* buffer, size_t len);
MemoryBlock CharsToBin(const std::string& buffer);
MemoryBlock CharsToBin(const char* buffer);
//---------------------------------------------------------------------------
//获取程序运行的路径
std::string RunPathFolder   ();
std::string RunPathFileName (const std::string& name);
std::string RunPathFileName (const char* name);
//---------------------------------------------------------------------------
std::string PathParent  (const std::string& path);
std::string PathParent  (const char* path);
std::string PathName    (const std::string& path);
std::string PathName    (const char* path);
//---------------------------------------------------------------------------
//文件夹操作
bool FolderCreate   (const std::string& path, bool recursive);
bool FolderDelete   (const std::string& path);
bool FolderExist    (const std::string& path);
bool FolderExist    (const char* path);
//---------------------------------------------------------------------------
//文件操作
bool FileDelete (const std::string& pathname);
bool FileDelete (const char* pathname);
bool FileExist  (const std::string& pathname);
bool FileExist  (const char* pathname);

bool LoadFile   (const std::string& path, MemoryBlock* result);
bool LoadFile   (const char* path, MemoryBlock* result);
bool SaveFile   (const std::string& path, const char* dat, size_t len);
bool SaveFile   (const char* path, const char* dat, size_t len);
//---------------------------------------------------------------------------
//文档（文件和文件夹）
bool DocumentExist  (const std::string& pathname);
bool DocumentExist  (const char* pathname);
//---------------------------------------------------------------------------

}//namespace base
//---------------------------------------------------------------------------
#endif// BASE_FUNCTION_H_
