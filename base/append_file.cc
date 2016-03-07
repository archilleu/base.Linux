#include "append_file.h"
#include "function.h"

namespace base
{

bool AppendFile::Open(const std::string& file_path)
{
    if(file_path.empty())   return false;
    if(0 != fp_)            return true;

    struct stat file_info;
    if(0 == stat(file_path.c_str(), &file_info))
        init_size_ = file_info.st_size;

    fp_ = ::fopen(file_path.c_str(), "ae+");
    if(0 == fp_)
        return false;
    ::setbuffer(fp_, buffer_.data(), buffer_.size());

    path_ = PathParent(file_path);
    name_ = PathName(file_path);
    return true;
}

void AppendFile::Close()
{
    if(0 != fp_)
    {
        ::fclose(fp_);
        fp_ = 0;
    }

    return;
}

bool AppendFile::Append(const char* dat, size_t len)
{
    assert(fp_);
    assert(dat);

    //写磁盘一般不会写失败,除非硬盘真不够或者账号可以使用的空间已满
    size_t offset = 0;
    while(len)
    {
        size_t wlen = ::fwrite_unlocked(dat+offset, 1, len, fp_);
        if(0 == wlen)
        {
            char buf[128];
            fprintf(stderr, "AppendFile::Append() failed: %s\n", strerror_r(ferror(fp_), buf, 128));
            return false;
        }

        offset  += wlen;
        len     -= wlen;
    }
    
    return true;
}

void AppendFile::Flush()
{
    ::fflush_unlocked(fp_);
    return;
}

size_t AppendFile::Size()
{
    return (init_size_ + ::ftell(fp_));
}

}//namespace base
