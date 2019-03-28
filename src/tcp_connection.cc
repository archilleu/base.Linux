//---------------------------------------------------------------------------
#include "tcp_connection.h"
#include "event_loop.h"
#include "inet_address.h"
#include "acceptor.h"
#include "net_logger.h"
#include "event_loop_thread_pool.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
TCPConnection::TCPConnection(EventLoop* ownerloop, std::string&& tcpname, Socket&& socket,
        InetAddress&& localaddr, InetAddress&& peeraddr)
:   owner_loop_(ownerloop),
    name_(tcpname),
    local_addr_(localaddr),
    peer_addr_(peeraddr),
    state_(CONNECTING),
    requests_(0),
    socket_(std::move(socket)),
    channel_(owner_loop_, socket_.fd()),
    overstock_size_(0)
{
    NetLogger_trace("ctor-->name:%s, fd:%d, localaddr:%s, peeraddr:%s",
            name_.c_str(), socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    assert(0 != owner_loop_);
    assert(0 < socket_.fd());
    return;
}
//---------------------------------------------------------------------------
TCPConnection::TCPConnection(EventLoop* owner_loop, std::string&& name, Socket&& socket,
        InetAddress&& local_addr, InetAddress&& peer_addr, const base::any& config_data)
:   TCPConnection(owner_loop, std::move(name), std::move(socket), std::move(local_addr), std::move(peer_addr))
{
    config_data_ = config_data;
    return;
}
//---------------------------------------------------------------------------
TCPConnection::~TCPConnection()
{
    NetLogger_trace("dtor-->name:%s, fd:%d, localaddr:%s, peeraddr:%s",
            name_.c_str(), socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    assert(DISCONNECTED == state_);
    owner_loop_ = 0;
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::Initialize()
{
    assert(CONNECTING == state_);

    channel_.set_read_cb(std::bind(&TCPConnection::HandleRead, this, std::placeholders::_1));
    channel_.set_write_cb(std::bind(&TCPConnection::HandleWrite, this));
    channel_.set_error_cb(std::bind(&TCPConnection::HandleError, this));
    channel_.set_close_cb(std::bind(&TCPConnection::HandleClose, this));

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::Send(const char* dat, size_t len)
{
    if(CONNECTED == state_)
    {
        //如果在本线程调用,则直接发送
        if(true == owner_loop_->IsInLoopThread())
        {
            _Send(dat, len);
            return;
        }

        //不在本线程调用,则排入本线程发送队列,该conn在EventLoop的DoPendingTask处理之前，
        //可能被conn.reset()导致conn被析构，所以不能用this指针
        owner_loop_->QueueInLoop(std::bind(&TCPConnection::SendInLoop,
                    shared_from_this()/*this*/, net::MemoryBlock(dat, dat+len)));
    }
    else
    {
        //TCPConnection可能在其他线程强制关闭，这个时候发送请求忽略
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::Send(net::MemoryBlock&& dat)
{
    if(CONNECTED == state_)
    {
        //如果在本线程调用,则直接发送
        if(true == owner_loop_->IsInLoopThread())
        {
            _Send(dat.data(), dat.size());
            return;
        }

        //不在本线程调用,则排入本线程发送队列,该conn在EventLoop的DoPendingTask处理之前，
        //可能被conn.reset()导致conn被析构，所以不能用this指针
        owner_loop_->QueueInLoop(std::bind(&TCPConnection::SendInLoop,
                    shared_from_this()/*this*/, std::move(dat)));
    }
    else
    {
        //TCPConnection可能在其他线程强制关闭，这个时候发送请求忽略
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ShutdownWirte()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(),
            socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    if(CONNECTED == state_)
    {
        owner_loop_->QueueInLoop(std::bind(&TCPConnection::ShutdownWriteInLoop, shared_from_this()));
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ForceClose()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(),
            socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    if(CONNECTED == state_)
    {
        owner_loop_->QueueInLoop(std::bind(&TCPConnection::ForceCloseInLoop, shared_from_this()));
    }

    return;
}
//---------------------------------------------------------------------------
std::string TCPConnection::GetTCPInfo() const
{
    if(CONNECTED != state_)
        return "";

    return socket_.GetTCPInfoString();
}
//---------------------------------------------------------------------------
void TCPConnection::ConnectionEstablished()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(),
            socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    owner_loop_->AssertInLoopThread();

    state_ = CONNECTED;
    channel_.Tie(shared_from_this());
    channel_.EnableReading();
    
    if(connection_cb_)
        connection_cb_(shared_from_this());

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ConnectionDestroy()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(),
            socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());
    owner_loop_->AssertInLoopThread();

    state_ = DISCONNECTED;
    channel_.Remove();
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::_Send(const char* dat, size_t len)
{
    owner_loop_->AssertInLoopThread();

    if(CONNECTED == state_)
    {
        ssize_t remain = _SendMostPossible(dat, len);
        if(-1 == remain)    //发送出错不需要特别处理，因为下一步会关闭连接
            return;

        if(0 != remain)
        {
            //放入缓存
            _SendDatQueueInBuffer(dat+(len-remain), remain);
            return;
        }
        
        //发送完成
        if(write_complete_cb_)
        {
            owner_loop_->QueueInLoop(std::bind(write_complete_cb_, shared_from_this()));
        }
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::SendInLoop(const net::MemoryBlock& dat)
{
    _Send(dat.data(), dat.size());
    return;
}
//---------------------------------------------------------------------------
ssize_t TCPConnection::_SendMostPossible(const char* dat, size_t len)
{
    //如果没有关注写事件,意味着写缓存为空,则可以直接发送数据
    ssize_t wlen = 0;
    if(false == channel_.IsWriting())
    {
        assert(0 == buffer_output_.ReadableBytes());

        wlen = ::send(channel_.fd(), dat, len, 0) ;
        if(0 > wlen)
        {
            //发送出错,关闭连接
            //当epoll返回时， 某个channel 收到 ERR 等关闭连接事件，但是该channel在执行前，
            //在它前面的channel调用该channel所属的tcp_conn发送数据，此时会产生ECONNRESET错误
            //if((EAGAIN!=errno) && (EWOULDBLOCK!=errno) && (ECONNRESET!=errno) && (EPIPE!=errno))
            {
                NetLogger_debug("send failed, errno:%d, msg:%s, name:%s, fd:%d, localaddr:%s, peeraddr:%s",
                        errno, OSError(errno), name_.c_str(), socket_.fd(), 
                        local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

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
void TCPConnection::_SendDatQueueInBuffer(const char* dat, size_t remain)
{
    //如果之前已经有缓存没有发送完成,或者这次没有发送完成,需要缓存数据等待下一次发送
    
    //高发送水位警示
    size_t wait_send_size = buffer_output_.ReadableBytes() + remain;
    if((overstock_size_<=wait_send_size) && (high_water_mark_cb_))
        owner_loop_->QueueInLoop(std::bind(high_water_mark_cb_,
                    shared_from_this(), wait_send_size));
    
    //添加未完成发送的数据到缓存
    buffer_output_.Append(dat, remain);
    if(false == channel_.IsWriting())
        channel_.EnableWriting();

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ShutdownWriteInLoop()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(),
            socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    owner_loop_->AssertInLoopThread();

    if(false == channel_.IsWriting())
        socket_.ShutDownWrite();

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::ForceCloseInLoop()
{
    NetLogger_info("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(),
            socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    owner_loop_->AssertInLoopThread();

    //假如是在QueueInLoop中调用的,如果conn在此之前已经接收到close,conn就已经被销毁,
    //所以状态可能为DISCONNECTE
    if((DISCONNECTING==state_) || (DISCONNECTED==state_))
        return;

    HandleClose();
    return;
}
//---------------------------------------------------------------------------
void TCPConnection::HandleRead(uint64_t rcv_time)
{
    owner_loop_->AssertInLoopThread();
    assert(CONNECTED == state_);

    int err_no = 0;
    int rlen = buffer_input_.ReadFd(socket_.fd(), &err_no);
    if(0 < rlen)
    {
        if(read_cb_)
        {
            read_cb_(shared_from_this(), buffer_input_, rcv_time);
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
            , err_no, OSError(err_no), name_.c_str(), socket_.fd(),
            local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::HandleWrite()
{
    owner_loop_->AssertInLoopThread();
    assert(CONNECTED == state_);

    size_t readable_len = buffer_output_.ReadableBytes();
    ssize_t wlen = ::send(socket_.fd(), buffer_output_.Peek(), readable_len, 0);
    if(0 < wlen)
    {
        buffer_output_.Retrieve(wlen);
        if(readable_len == static_cast<size_t>(wlen))
        {
            channel_.DisableWriting();

            //发送完成回调
            if(write_complete_cb_)
                owner_loop_->QueueInLoop(std::bind(write_complete_cb_, shared_from_this()));
        }
    
        return;
    }

    if((EAGAIN!=errno) || (EWOULDBLOCK!=errno))
        return;

    NetLogger_error("write failed, errno:%d, msg:%s, name:%s, fd:%d, localaddr:%s, peeraddr:%s"
            , errno, OSError(errno), name_.c_str(), socket_.fd(),
            local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::HandleError()
{
    NetLogger_error("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(),
            socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    owner_loop_->AssertInLoopThread();

    return;
}
//---------------------------------------------------------------------------
void TCPConnection::HandleClose()
{
    NetLogger_trace("name:%s, fd:%d, localaddr:%s, peeraddr:%s", name_.c_str(),
            socket_.fd(), local_addr_.IpPort().c_str(), peer_addr_.IpPort().c_str());

    owner_loop_->AssertInLoopThread();

    state_ = DISCONNECTING;
    /*
    EPOLLERR
        Error condition happened on the associated file descriptor. epoll_wait(2) will 
        always wait for this event; it is not necessary to set it in events.
    EPOLLHUP
        Hang up happened on the associated file descriptor.epoll_wait(2) will always 
        wait for this event; it is not necessary to set it in events.
    */
    channel_.DisableAll();
    
    TCPConnectionPtr guard(shared_from_this());

    //通知下线,回调给用户,原则上接到这个指令后,上层使用者不应该在对该连接操作
    if(disconnection_cb_)
    {
        disconnection_cb_(guard);
    }

    //TCPServer开始销毁链接
    remove_cb_(guard);

    return;
}
//---------------------------------------------------------------------------

}//namespace net
