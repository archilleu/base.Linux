//---------------------------------------------------------------------------
#ifndef BASE_APPEND_FILE_H_
#define BASE_APPEND_FILE_H_
//---------------------------------------------------------------------------
#include <string>
#include <array>
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
    AppendFile& operator=(const AppendFile&) =delete;

    bool Open   (const std::string& path);
    void Close  ();

    bool Append (const char* dat, size_t len);
    void Flush  ();

    size_t              Size();
    const std::string&  path();

private:
    std::string path_;

    FILE* fp_;
    size_t init_size_;
    std::array<char, 1024*64> buffer_;
};

}//namespace base
//---------------------------------------------------------------------------
#endif //BASE_APPEND_FILE_H_
