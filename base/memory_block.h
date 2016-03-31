//---------------------------------------------------------------------------
#ifndef LINUX_BASE_MEMORYBLOCK_H_
#define LINUX_BASE_MEMORYBLOCK_H_
//---------------------------------------------------------------------------
#include "share_inc.h"
//---------------------------------------------------------------------------
namespace base
{

class MemoryBlock
{
public:
    explicit MemoryBlock(size_t size=0);
    MemoryBlock(const char* dat, size_t size);
    MemoryBlock(const MemoryBlock& other);
    MemoryBlock& operator=(const MemoryBlock& other);
    MemoryBlock(MemoryBlock&& other);
    MemoryBlock& operator=(MemoryBlock&& other);
    ~MemoryBlock();

    void Resize(size_t size);
    void Fill(char c);

    size_t      len() const { return len_; }
    char*       dat()       { return dat_; }
    const char* dat() const { return dat_; }

    char    operator[] (size_t index) const { return dat_[index]; }
    char&   operator[] (size_t index)       { return dat_[index]; }

private:
    friend bool operator==  (const MemoryBlock& left, const MemoryBlock& right);
    friend bool operator!=  (const MemoryBlock& left, const MemoryBlock& right);
    friend bool operator>   (const MemoryBlock& left, const MemoryBlock& right);
    friend bool operator<   (const MemoryBlock& left, const MemoryBlock& right);

private:
    size_t  len_;
    char*   dat_;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline bool operator==(const MemoryBlock& left, const MemoryBlock& right)
{
    return  (
            (left.len_ != right.len_)   ? false
                                        :
                                          (0 == memcmp(left.dat_, right.dat_, left.len_))
            );
}
//---------------------------------------------------------------------------
inline bool operator!=(const MemoryBlock& left, const MemoryBlock& right)
{
    return !(left==right);
}
//---------------------------------------------------------------------------
inline bool operator>(const MemoryBlock& left, const MemoryBlock& right)
{
    return  (
            (left.len_ > right.len_)    ?   true
                                        :
                                            (left.len_ != right.len_    ? false
                                                                        : (memcmp(left.dat_, right.dat_, left.len_)>0)
                                            )
            );
}
//---------------------------------------------------------------------------
inline bool operator<(const MemoryBlock& left, const MemoryBlock& right)
{
    return !(left > right);
}
//---------------------------------------------------------------------------
}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_MEMORYBLOCK_H_
