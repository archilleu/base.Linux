//---------------------------------------------------------------------------
#ifndef LINUX_BASE_JSON_CHAR_READER_H_
#define LINUX_BASE_JSON_CHAR_READER_H_
//---------------------------------------------------------------------------
#include "../share_inc.h"
#include "../memory_block.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{

class CharReader
{
public:
    CharReader()
    :   pos_(0)
    {
    }
    ~CharReader()
    {
    }

    void set_dat(MemoryBlock&& dat) { dat_ = std::move(dat); }

    bool HasMore()  { return pos_ <= dat_.len(); }

    char Peek()
    {
        assert(true == HasMore());
        
        return dat_[pos_];
    }

    char Next()
    {
        assert(true == HasMore());

        char c = dat_[pos_];
        ++pos_;
        return c;
    }

    std::string Next(size_t size)
    {
        assert((pos_+size) <= dat_.len());

        std::string val(dat_.dat()+pos_, size);
        pos_ += size;
        return val; //ROV
    }

private:
    MemoryBlock dat_;
    size_t      pos_;
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_CHAR_READER_H_
