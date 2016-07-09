//---------------------------------------------------------------------------
#include "tcp_connection.h"
#include "event_loop.h"
#include "inet_address.h"
#include "acceptor.h"
#include "socket.h"
#include "channel.h"
#include "net_log.h"
#include "event_loop_thread_pool.h"
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
TCPConnection::TCPConnection(EventLoop* ownerloop, const std::string& tcpname, int fd, const InetAddress& localaddr, const InetAddress& peeraddr)
:   owner_loop_(ownerloop),
    name_(tcpname),
    local_addr_(localaddr),
    peer_addr_(peeraddr),
    connected_(false),
    socket_(new Socket(fd)),
    channel_(new Channel(owner_loop_, fd)),
    overstock_size_(0)
{
    assert(0 != owner_loop_);
    assert(0 < fd);

    SystemLog_Debug("ctor==>name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), fd, local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());
    return;
}
//---------------------------------------------------------------------------
TCPConnection::~TCPConnection()
{
    SystemLog_Debug("dtor==>name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::Initialize()
{
    channel_->set_callback_read(std::bind(&TCPConnection::HandleRead, this, std::placeholders::_1));
    channel_->set_callback_write(std::bind(&TCPConnection::HandleWrite, this));
    channel_->set_callback_error(std::bind(&TCPConnection::HandleError, this));
    channel_->set_callback_close(std::bind(&TCPConnection::HandleClose, this));

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::Send(const char* dat, size_t len)
{
    assert(true == connected_);//有可能被其它线程关闭

    //如果在本线程调用,则直接发送
    if(true == owner_loop_->IsInLoopThread())
    {
        SendInLoopB(dat, len);
        return;
    }

    //不在线程调用,则排入本线程发送队列
    owner_loop_->QueueInLoop(std::bind(&TCPConnection::SendInLoopA, this, base::MemoryBlock(dat, len)));
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::Send(const base::MemoryBlock& dat)
{
    assert(true == connected_);//有可能被其它线程关闭

    //如果在本线程调用,则直接发送
    if(true == owner_loop_->IsInLoopThread())
    {
        SendInLoopA(dat);
        return;
    }

    //不在线程调用,则排入本线程发送队列
    owner_loop_->QueueInLoop(std::bind(&TCPConnection::SendInLoopA, this, dat));
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::Shutdown()
{
    assert(true == connected_);//有可能被其它线程关闭

    SystemLog_Debug("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    //connected_ = false;
    owner_loop_->QueueInLoop(std::bind(&TCPConnection::ShutdownInLoop, shared_from_this()));
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ForceClose()
{
    assert(true == connected_);//有可能被其它线程关闭

    SystemLog_Debug("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    //connected_ = false;
    owner_loop_->QueueInLoop(std::bind(&TCPConnection::ForceCloseInLoop, shared_from_this()));
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ConnectionEstablished()
{
    owner_loop_->AssertInLoopThread();
    SystemLog_Debug("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    channel_->Tie(shared_from_this());
    channel_->ReadEnable();
    
    if(callback_connection_)
        callback_connection_(shared_from_this());

    connected_ = true;
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ConnectionDestroy()
{
    owner_loop_->AssertInLoopThread();
    SystemLog_Debug("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    connected_ = false;
    channel_->Remove();
    return;
}
//---------------------------------------------------------------------------
std::string TCPConnection::GetTCPInfo() const
{
    return socket_->GetTCPInfoString();
}
//---------------------------------------------------------------------------
void TCPConnection::SendInLoopA(const base::MemoryBlock dat)
{
    SendInLoopB(dat.dat(), dat.len());
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::SendInLoopB(const char* dat, size_t len)
{
    assert(true == connected_);
    owner_loop_->AssertInLoopThread();

    ssize_t remain = _SendMostPossible(dat, len);
    if(-1 == remain)
        return;

    if(0 != remain)
    {
        //放入缓存
        _SendDatQueueInBuffer(dat, remain);
        return;
    }
    
    //发送完成
    if(callback_write_complete_)
    {
        owner_loop_->QueueInLoop(std::bind(callback_write_complete_, shared_from_this()));
    }

    return;
}
//---------------------------------------------------------------------------
ssize_t TCPConnection::_SendMostPossible(const char* dat, size_t len)
{
    //如果没有关注写事件,意味着写缓存为空,则可以直接发送数据
    ssize_t wlen = 0;
    if(false == channel_->IsWriting())
    {
        assert(0 == buffer_output_.ReadableBytes());

        wlen = ::send(channel_->fd(), dat, len, 0) ;
        if(0 > wlen)
        {
            //发送出错,关闭连接
            if((EAGAIN!=errno) || (EWOULDBLOCK!=errno))
            {
                SystemLog_Warning("send failed, errno:%d, msg:%s", errno, StrError(errno));
                HandleClose();
                return -1;
            }
        }
    }

    size_t remain = len - wlen;
    return remain;
}
//---------------------------------------------------------------------------
void TCPConnection::_SendDatQueueInBuffer(const char* dat, size_t remain)
{
    //如果之前已经有缓存没有发送完成,或者这次没有发送完成,需要缓存数据等待下一次发送
    
    //高发送水位警示
    size_t wait_send_size = buffer_output_.ReadableBytes() + remain;
    if((overstock_size_<=wait_send_size) && (callback_high_water_mark_))
        owner_loop_->QueueInLoop(std::bind(callback_high_water_mark_, shared_from_this(), wait_send_size));
    
    //添加未完成发送的数据到缓存
    buffer_output_.Append(dat-remain, remain);
    if(false == channel_->IsWriting())
        channel_->WriteEnable();

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ShutdownInLoop()
{
    owner_loop_->AssertInLoopThread();
    SystemLog_Debug("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    if(false == channel_->IsWriting())
        socket_->ShutDownWrite();

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ForceCloseInLoop()
{
    owner_loop_->AssertInLoopThread();
    SystemLog_Debug("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    HandleClose();
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::HandleRead(base::Timestamp rcv_time)
{
    owner_loop_->AssertInLoopThread();
    assert(true == connected_);

    int err_no  = 0;
    int rlen    = buffer_input_.ReadFd(socket_->fd(), &err_no);
    if(0 < rlen)
    {
        if(callback_read_)
        {
            callback_read_(shared_from_this(), buffer_input_, rcv_time);
        }

        return;
    }

    //客户端断开连接
    if(0 == rlen)
    {
        HandleClose();
        return;
    }

    //读出错
    if((EAGAIN==err_no) || (EWOULDBLOCK==err_no))
        return;

    SystemLog_Error("read error, errno:%d, msg:%s", err_no, StrError(err_no));
    HandleError();
    assert(0);
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::HandleWrite()
{
    owner_loop_->AssertInLoopThread();
    assert(true == connected_);

    size_t  readable_len= buffer_output_.ReadableBytes();
    ssize_t wlen        = ::send(socket_->fd(), buffer_output_.Peek(), readable_len, 0);
    if(0 > wlen)
    {
        if((EAGAIN!=errno) || (EWOULDBLOCK!=errno))
        {
            SystemLog_Error("write error, errno:%d, msg:%s", errno, StrError(errno));
            HandleClose();
            assert(0);
            return;
        }
    }

    buffer_output_.Retrieve(wlen);
    if(readable_len == static_cast<size_t>(wlen))
    {
        assert(0 == buffer_output_.ReadableBytes());
        channel_->WriteDisable();

        //发送完成回调
        if(callback_write_complete_)
        {
            owner_loop_->QueueInLoop(std::bind(callback_write_complete_, shared_from_this()));
        }
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::HandleError()
{
    owner_loop_->AssertInLoopThread();
    SystemLog_Debug("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    HandleClose();
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::HandleClose()
{
    owner_loop_->AssertInLoopThread();
    SystemLog_Debug("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    connected_ = false;
    /*
    EPOLLERR
        Error condition happened on the associated file descriptor.  epoll_wait(2) will always wait for this event; it is not necessary to set it in events.
    EPOLLHUP
        Hang up happened on the associated file descriptor.  epoll_wait(2) will always wait for this event; it is not necessary to set it in events.
    */

    //而且,因为在
    //channel_->DisableAll();拯救不了不再接收事件~,原因在上
    //EventLoop中的DoPending处理函数,加入的任务和处理中的任务有一个间隔时间窗口,所以ConnectionDestroy(因为这个原因,该函数已经去掉)有可能不能在这次的poll中得到调用,导致多次触发该<HUP ERR>事件
    channel_->Remove();
    
    TCPConnectionPtr guard(shared_from_this());

    //通知下线,回调给用户,原则上接到这个指令后,上层使用者不应该在对该连接操作
    if(callback_disconnection_)
    {
        callback_disconnection_(guard);
    }

    //内部开始销毁链接
    if(callback_destroy_)
    {
        callback_destroy_(guard);
    }

    return;
}
//---------------------------------------------------------------------------
}//namespace net
