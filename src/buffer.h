//---------------------------------------------------------------------------
#ifndef NET_BUFFER_H_
#define NET_BUFFER_H_
//---------------------------------------------------------------------------
#include <vector>
#include <string>
#include <cassert>
#include <cstring>
//---------------------------------------------------------------------------
namespace net
{

class Buffer
{
public:
    Buffer()
    :   read_index_(0),
        write_index_(0),
        buffer_(kInitialSize)
    {
    }
    ~Buffer()
    {
    }

    size_t ReadableBytes() const
    {
        return write_index_ - read_index_;
    }

    const char* Peek() const
    {
        return Begin() + read_index_;
    }

    int64_t PeekInt64() const
    {
        assert(sizeof(int64_t) <= ReadableBytes());

        int64_t value = 0;
        ::memcpy(&value, Peek(), sizeof(value));
        return be64toh(value);
    }

    int32_t PeekInt32() const
    {
        assert(sizeof(int32_t) <= ReadableBytes());

        int32_t value = 0;
        ::memcpy(&value, Peek(), sizeof(value));
        return be32toh(value);
    }

    int16_t PeekInt16() const
    {
        assert(sizeof(int16_t) <= ReadableBytes());

        int16_t value = 0;
        ::memcpy(&value, Peek(), sizeof(value));
        return be16toh(value);
    }

    int8_t PeekInt8() const
    {
        assert(sizeof(int8_t) <= ReadableBytes());

        int8_t value = 0;
        ::memcpy(&value, Peek(), sizeof(value));
        return value;
    }

    void Retrieve(size_t len)
    {
        assert(len <= ReadableBytes());

        if(len < ReadableBytes())
        {
            //没有检索完
            read_index_ += len;
            if(read_index_ == write_index_)
            {
                read_index_ = 0;
                write_index_= 0;
            }
        }
        else
        {
            //检索全部
            RetrieveAll();
        }

        return;
    }

    void RetrieveInt64()    { Retrieve(sizeof(int64_t)); }
    void RetrieveInt32()    { Retrieve(sizeof(int32_t)); }
    void RetrieveInt16()    { Retrieve(sizeof(int16_t)); }
    void RetrieveInt8()     { Retrieve(sizeof(int8_t)); }
    void RetrieveAll()
    {
        read_index_ = 0;
        write_index_= 0;
    }

    void AppendInt64(int64_t value)
    {
        int64_t val = htobe64(value);
        Append(&val, sizeof(val));
        return;
    }

    void AppendInt32(int32_t value)
    {
        int32_t val = htobe32(value);
        Append(&val, sizeof(val));
        return;
    }

    void AppendInt16(int16_t value)
    {
        int16_t val = htobe16(value);
        Append(&val, sizeof(val));
        return;
    }

    void AppendInt8(int8_t value)
    {
        Append(&value, sizeof(value));
        return;
    }

    void Append(const std::string& dat)
    {
        Append(dat.data(), dat.size());

        return;
    }

    void Append(const void* dat, size_t len)
    {
        EnsureWritableBytes(len);

        memcpy(BeginWrite(), dat, len);
        HasWriteBytes(len);

        return;
    }

    int64_t ReadInt64()
    {
        int64_t value = PeekInt64();
        RetrieveInt64();
        return value;
    }

    int32_t ReadInt32()
    {
        int32_t value = PeekInt32();
        RetrieveInt32();
        return value;
    }

    int16_t ReadInt16()
    {
        int16_t value = PeekInt16();
        RetrieveInt16();
        return value;
    }

    int8_t ReadInt8()
    {
        int8_t value = PeekInt8();
        RetrieveInt8();
        return value;
    }

    int ReadFd(int fd, int* saved_errno);

private:
    char* Begin()
    {
        return buffer_.data();
    }

    const char* Begin() const
    {
        return buffer_.data();
    }

    char* BeginWrite()
    {
        return Begin() + write_index_;
    }
    
    const char* BeginWrite() const
    {
        return Begin() + write_index_;
    }

    size_t WritableBytes()
    {
        return buffer_.size() - write_index_;
    }

    void HasWriteBytes(size_t len)
    {
        assert(len <= WritableBytes());

        write_index_ += len;
        return;
    }

    //可写内存不足时,分配内存
    void MakeSpace(size_t len)
    {
        //如果已经读过的数据和可写的内存总和都不能够满足要写写入的大小,则重新申请足够大的内存
        if((WritableBytes() + read_index_) < len)
        {
            assert((write_index_+len) > buffer_.size());

            buffer_.resize(write_index_ + len);
            return;
        }

        //否则移动已经读过的数据腾出可写内存
        size_t readable = ReadableBytes();
//      memory areas must not overlap
//      memcpy(Begin(), Begin() + read_index_, readable);
        std::copy_backward(Begin()+read_index_, Begin()+write_index_, Begin()+readable);
        read_index_ = 0;
        write_index_= readable;

        assert(readable == ReadableBytes());
    }

    void EnsureWritableBytes(size_t len)
    {
        if(len > WritableBytes())
            MakeSpace(len);

        assert(len <= WritableBytes());
    }

private:
    size_t              read_index_;
    size_t              write_index_;
    std::vector<char>   buffer_;

    static const size_t   kInitialSize = 1024;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_BUFFER_H_
