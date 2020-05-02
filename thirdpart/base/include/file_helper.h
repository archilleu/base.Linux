//---------------------------------------------------------------------------
#ifndef BASE_FILE_HELPER_H_
#define BASE_FILE_HELPER_H_
//---------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <fstream>

#include "memory_block.h"
//---------------------------------------------------------------------------
namespace base
{

class FileHelper
{
public:
    FileHelper();
    ~FileHelper();
    FileHelper(const FileHelper&) =delete;
    FileHelper operator=(const FileHelper&) =delete;

    void Open(const std::string& file_path, bool truncate=false);
    void Flush(bool sync=false);
    void Close();

    void Write(const void* dat, size_t len);
    void Write(const std::string& dat);
    void Write(int8_t dat);
    void Write(int16_t dat);
    void Write(int32_t dat);
    void Write(int64_t dat);

    size_t Read(void* dat, size_t len);
    std::string ReadString(size_t len);
    int8_t ReadInt8();
    int16_t ReadInt16();
    int32_t ReadInt32();
    int64_t ReadInt64();

    std::string PeekString(size_t len);
    int8_t PeekInt8();
    int16_t PeekInt16();
    int32_t PeekInt32();
    int64_t PeekInt64();

    //如果调用了write，需要调用flush才能获取正确的结果
    size_t Size();

    const std::string& path() { return path_; }

private:
    FILE* fp_;
    std::string path_;
};

}//namespace base
//---------------------------------------------------------------------------
#endif //BASE_FILE_HELPER_H_
