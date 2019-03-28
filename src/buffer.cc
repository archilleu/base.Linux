//---------------------------------------------------------------------------
#include "buffer.h"
#include <sys/uio.h>
#include "net_logger.h"
//---------------------------------------------------------------------------
__thread char t_extra_buf[1024*32];
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
const char Buffer::kCRLF[] = "\r\n";
//---------------------------------------------------------------------------
int Buffer::ReadFd(int fd, int* saved_errno)
{
    struct iovec vec[2];
    size_t writable = WritableBytes();
    vec[0].iov_base = Begin() + write_index_;
    vec[0].iov_len  = writable;
    vec[1].iov_base = t_extra_buf;
    vec[1].iov_len  = sizeof(t_extra_buf);

    ssize_t rlen = ::readv(fd, vec, 2);
    if(0 > rlen)
    {
        *saved_errno = errno;
        NetLogger_error("readv failed, errno:%d, msg:%s", errno, OSError(errno));
    }
    else if(static_cast<size_t>(rlen) <= writable)
    {
        write_index_ += rlen;
    }
    /*
    //不采用ET模式，因为在写的时候如果未写完需要再次调用Channel EnableWriting，并不高效
    else if(static_cast<size_t>(rlen) == writable)
    {
        //因为采用的是ET模式，所以如果读入数据等于缓冲区大小，需要再次读取确保数据
        //读取完毕
        write_index_ += rlen;
        return ReadFd(fd, saved_errno);
    }
    */
    else
    {
        //如果可写的大小不足,则需要重新申请空间或者移动空间
        write_index_ = buffer_.size();
        Append(t_extra_buf, rlen - writable);
    }

    return static_cast<int>(rlen);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
}//namespace net
