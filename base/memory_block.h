#ifndef BASE_LINUX_BASE_MEMORYBLOCK_H_
#define BASE_LINUX_BASE_MEMORYBLOCK_H_

#include "share_inc.h"

namespace base
{

class MemoryBlock
{
public:
    explicit MemoryBlock(size_t size=0);
    MemoryBlock(const MemoryBlock& other);
    MemoryBlock& operator=(const MemoryBlock& other);
    MemoryBlock(MemoryBlock&& other);
    MemoryBlock& operator=(MemoryBlock&& other);
    ~MemoryBlock();

    void Fill(char c);

    char*       dat()       { return dat_; }
    const char* dat() const { return dat_; }
    size_t      len()       { return len_; }

    char operator[] (size_t index) { return dat_[index]; }

private:
    friend bool operator==  (const MemoryBlock& left, const MemoryBlock& right);
    friend bool operator!=  (const MemoryBlock& left, const MemoryBlock& right);
    friend bool operator>   (const MemoryBlock& left, const MemoryBlock& right);
    friend bool operator<   (const MemoryBlock& left, const MemoryBlock& right);

private:
    size_t  len_;
    char*   dat_;
};

inline bool operator==(const MemoryBlock& left, const MemoryBlock& right)
{
    return  (
            (left.len_ != right.len_)   ? false
                                        :
                                          (0 == memcmp(left.dat_, right.dat_, left.len_))
            );
}

inline bool operator!=(const MemoryBlock& left, const MemoryBlock& right)
{
    return !(left==right);
}

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

inline bool operator<(const MemoryBlock& left, const MemoryBlock& right)
{
    return !(left > right);
}

}
#endif //BASE_LINUX_BASE_MEMORYBLOCK_H_
