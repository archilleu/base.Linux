//---------------------------------------------------------------------------
#include "buffer.h"
#include <sys/uio.h>
#include "net_log.h"
//---------------------------------------------------------------------------
__thread char t_extra_buf[1024*32];
//---------------------------------------------------------------------------
namespace net
{

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
    else
    {
        //如果可写的大小不足,则需要重新申请空间或者移动空间
        write_index_ = buffer_.size();
        Append(t_extra_buf, rlen - writable);
    }

    return static_cast<int>(rlen);
}

//---------------------------------------------------------------------------
}//namespace net
