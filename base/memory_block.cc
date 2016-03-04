#include "memory_block.h"

using namespace base;

MemoryBlock::MemoryBlock(size_t size)
:   len_(size),
    dat_(0)
{
    if(0 < len_)
        dat_ = new char[len_];

    return;
}

MemoryBlock::MemoryBlock(const MemoryBlock& other)
:   len_(other.len_),
    dat_(0)
{
    if(0 < len_)
    {
        dat_ = new char[len_];
        memcpy(dat_, other.dat_, other.len_);
    }

    return;
}

MemoryBlock& MemoryBlock::operator=(const MemoryBlock& other)
{
    if(this != &other)
    {
        if(0 != len_)
            delete[] dat_;

        len_ = other.len_;
        dat_ = 0;
        if(0 < len_)
        {
            dat_ = new char[len_];
            memcpy(dat_, other.dat_, len_);
        }
    }

    return *this;
}

MemoryBlock::MemoryBlock(MemoryBlock&& other)
{
    dat_ = other.dat_;
    len_ = other.len_;

    other.dat_ = 0;
    other.len_ = 0;

    return;
}

MemoryBlock& MemoryBlock::operator=(MemoryBlock&& other)
{
    if(this != &other)
    {
        if(0 != len_)
            delete[] dat_;

        dat_ = other.dat_;
        len_ = other.len_;

        other.dat_ = 0;
        other.len_ = 0;
    }

    return *this;
}

MemoryBlock::~MemoryBlock()
{
    if(0 != dat_)
        delete[] dat_;

    return;
}

void MemoryBlock::Fill(char c)
{
    if(0 != len_)
        memset(dat_, c, len_);

    return;
}

