//---------------------------------------------------------------------------
#ifndef LINUX_BASE_APPEND_FILE_H_
#define LINUX_BASE_APPEND_FILE_H_
//---------------------------------------------------------------------------
#include "share_inc.h"
//---------------------------------------------------------------------------
namespace base
{

class AppendFile
{
public:
    AppendFile()
    :   fp_(0),
        init_size_(0)
    {
    }
    ~AppendFile()
    {
        Close();
    }
    AppendFile(const AppendFile&) =delete;
    AppendFile(const AppendFile&&) =delete;
    AppendFile& operator=(const AppendFile&) =delete;
    AppendFile& operator=(const AppendFile&&) =delete;

    bool Open   (const std::string& path);
    void Close  ();

    bool Append (const char* dat, size_t len);
    void Flush  ();

    size_t              Size();
    const std::string&  path();
    const std::string&  name();
    
private:
    std::string path_;
    std::string name_;

    FILE*                           fp_;
    size_t                          init_size_;
    std::array<char, UNIT_KB*64>    buffer_;
};

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_APPEND_FILE_H_
