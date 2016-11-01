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
TCPConn::TCPConn(EventLoop* ownerloop, const std::string& tcpname, int fd, const InetAddress& localaddr, const InetAddress& peeraddr)
:   owner_loop_(ownerloop),
    name_(tcpname),
    local_addr_(localaddr),
    peer_addr_(peeraddr),
    state_(CONNECTING),
    socket_(new Socket(fd)),
    channel_(new Channel(owner_loop_, fd)),
    overstock_size_(0)
{
    NetLogger_trace("ctor-->name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), fd, local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    assert(0 != owner_loop_);
    assert(0 < fd);
    return;
}
//---------------------------------------------------------------------------
TCPConn::~TCPConn()
{
    NetLogger_trace("dtor-->name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    assert(DISCONNECTED == state_);
    owner_loop_ = 0;
    return;
}
//---------------------------------------------------------------------------
void TCPConn::Initialize()
{
    assert(CONNECTING == state_);

    channel_->set_callback_read(std::bind(&TCPConn::HandleRead, this, std::placeholders::_1));
    channel_->set_callback_write(std::bind(&TCPConn::HandleWrite, this));
    channel_->set_callback_error(std::bind(&TCPConn::HandleError, this));
    channel_->set_callback_close(std::bind(&TCPConn::HandleClose, this));

    return;
}
//---------------------------------------------------------------------------
void TCPConn::Send(const char* dat, size_t len)
{
    if(CONNECTED == state_)
    {
        //如果在本线程调用,则直接发送
        if(true == owner_loop_->IsInLoopThread())
        {
            SendInLoopB(dat, len);
            return;
        }

        //不在线程调用,则排入本线程发送队列
        //该conn在EventLoop的DoPendingTask处理之前，可能被conn.reset()导致conn被析构，所以不能用this
        owner_loop_->QueueInLoop(std::bind(&TCPConn::SendInLoopA, shared_from_this()/*this*/, net::MemoryBlock(dat, dat+len)));

        return;
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConn::Send(const net::MemoryBlock& dat)
{
    if(CONNECTED == state_)
    {
        //如果在本线程调用,则直接发送
        if(true == owner_loop_->IsInLoopThread())
        {
            SendInLoopA(dat);
            return;
        }

        //不在线程调用,则排入本线程发送队列
        //该conn在EventLoop的DoPendingTask处理之前，可能被conn.reset()导致conn被析构，所以不能用this
        owner_loop_->QueueInLoop(std::bind(&TCPConn::SendInLoopA, shared_from_this()/*this*/, dat));
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConn::ShutdownWirte()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    if(CONNECTED == state_)
    {
        owner_loop_->QueueInLoop(std::bind(&TCPConn::ShutdownWriteInLoop, shared_from_this()));
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConn::ForceClose()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    if(CONNECTED == state_)
    {
        owner_loop_->QueueInLoop(std::bind(&TCPConn::ForceCloseInLoop, shared_from_this()));
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConn::ConnectionEstablished()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    owner_loop_->AssertInLoopThread();

    int expected = CONNECTING;
    if(!state_.compare_exchange_strong(expected, CONNECTED))
        assert(0);

    channel_->Tie(shared_from_this());
    channel_->ReadEnable();
    
    if(callback_connection_)
        callback_connection_(shared_from_this());

    return;
}
//---------------------------------------------------------------------------
void TCPConn::ConnectionDestroy()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    owner_loop_->AssertInLoopThread();

    int expected = DISCONNECTING;
    if(!state_.compare_exchange_strong(expected, DISCONNECTED))
        assert(0);

    channel_->Remove();
    return;
}
//---------------------------------------------------------------------------
std::string TCPConn::GetTCPInfo() const
{
    if(CONNECTED != state_)
        return "";

    return socket_->GetTCPInfoString();
}
//---------------------------------------------------------------------------
void TCPConn::SendInLoopA(net::MemoryBlock dat)
{
    SendInLoopB(dat.data(), dat.size());
    return;
}
//---------------------------------------------------------------------------
void TCPConn::SendInLoopB(const char* dat, size_t len)
{
    owner_loop_->AssertInLoopThread();

    if(CONNECTED == state_)
    {
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
    }

    return;
}
//---------------------------------------------------------------------------
ssize_t TCPConn::_SendMostPossible(const char* dat, size_t len)
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
            //当Poll返回时， 某个channel 收到 ERR 等关闭连接事件，但是该channel在执行前，
            //在它前面的channel调用该channel所属的tcp_conn发送数据，此时会产生ECONNRESET错误
            //if((EAGAIN!=errno) && (EWOULDBLOCK!=errno) && (ECONNRESET!=errno) && (EPIPE!=errno))
            {
                NetLogger_info("send failed, errno:%d, msg:%s, name:%s, fd:%d, localaddr:%s, peeraddr:%s"
                        , errno, OSError(errno), name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

                //不需要HandleClose，原因如上
                //HandleClose();
            }

            return -1;
        }
    }

    size_t remain = len - wlen;
    return remain;
}
//---------------------------------------------------------------------------
void TCPConn::_SendDatQueueInBuffer(const char* dat, size_t remain)
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
void TCPConn::ShutdownWriteInLoop()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    owner_loop_->AssertInLoopThread();

    if(false == channel_->IsWriting())
        socket_->ShutDownWrite();

    return;
}
//---------------------------------------------------------------------------
void TCPConn::ForceCloseInLoop()
{
    NetLogger_info("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    owner_loop_->AssertInLoopThread();

    //假如是在QueueInLoop中调用的,如果conn在此之前已经接收到close,conn就已经被销毁,所以状态可能为DISCONNECTE
    if((DISCONNECTING==state_) || (DISCONNECTED==state_))
        return;

    HandleClose();
    return;
}
//---------------------------------------------------------------------------
void TCPConn::HandleRead(uint64_t rcv_time)
{
    owner_loop_->AssertInLoopThread();
    //assert(CONNECTED == state_);

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

    NetLogger_error("read failed, errno:%d, msg:%s, name:%s, fd:%d, localaddr:%s, peeraddr:%s"
            , errno, OSError(errno), name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    return;
}
//---------------------------------------------------------------------------
void TCPConn::HandleWrite()
{
    owner_loop_->AssertInLoopThread();
    //assert(CONNECTED == state_);

    size_t  readable_len= buffer_output_.ReadableBytes();
    ssize_t wlen        = ::send(socket_->fd(), buffer_output_.Peek(), readable_len, 0);
    if(0 < wlen)
    {
        buffer_output_.Retrieve(wlen);
        if(readable_len == static_cast<size_t>(wlen))
        {
            assert(0 == buffer_output_.ReadableBytes());
            channel_->WriteDisable();

            //发送完成回调
            if(callback_write_complete_)
                owner_loop_->QueueInLoop(std::bind(callback_write_complete_, shared_from_this()));

            if(DISCONNECTING == state_)
                ShutdownWriteInLoop();
        }
    
        return;
    }

    if((EAGAIN!=errno) || (EWOULDBLOCK!=errno))
        return;

    NetLogger_error("write failed, errno:%d, msg:%s, name:%s, fd:%d, localaddr:%s, peeraddr:%s"
            , errno, OSError(errno), name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    return;
}
//---------------------------------------------------------------------------
void TCPConn::HandleError()
{
    NetLogger_error("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    owner_loop_->AssertInLoopThread();

    return;
}
//---------------------------------------------------------------------------
void TCPConn::HandleClose()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(), socket_->fd(), local_addr_.IPPort().c_str(), peer_addr_.IPPort().c_str());

    owner_loop_->AssertInLoopThread();

    int expected = CONNECTED;
    if(!state_.compare_exchange_strong(expected, DISCONNECTING))
        assert(0);

    /*
    EPOLLERR
        Error condition happened on the associated file descriptor.  epoll_wait(2) will always wait for this event; it is not necessary to set it in events.
    EPOLLHUP
        Hang up happened on the associated file descriptor.  epoll_wait(2) will always wait for this event; it is not necessary to set it in events.
    */
    channel_->DisableAll();
    
    TCPConnPtr guard(shared_from_this());

    //通知下线,回调给用户,原则上接到这个指令后,上层使用者不应该在对该连接操作
    if(callback_disconnection_)
    {
        callback_disconnection_(guard);
    }

    //内部开始销毁链接
    callback_remove_(guard);

    return;
}
//---------------------------------------------------------------------------

}//namespace net
