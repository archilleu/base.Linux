//---------------------------------------------------------------------------
#include <assert.h>
#include "tcp_connector.h"
#include "event_loop.h"
#include "net_logger.h"
#include "socket.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
const int kInitRetryDelay = 1000;
const int kMaxRetryDelay = 30 * 1000;
//---------------------------------------------------------------------------
TCPConnector::TCPConnector(EventLoop* event_loop, const InetAddress& server_addr)
:   running_(false),
    event_loop_(event_loop),
    server_addr_(server_addr),
    state_(DISCONNECTED),
    retry_delay_(kInitRetryDelay),
    timer_id_(0, 0)
{
    NetLogger_trace("TCPConnector(%p) ctor", this);
    return;
}
//---------------------------------------------------------------------------
TCPConnector::~TCPConnector()
{
    NetLogger_trace("TCPConnector(%p) dtor", this);
    return;
}
//---------------------------------------------------------------------------
void TCPConnector::Start()
{
    running_ = true;
    event_loop_->RunInLoop(std::bind(&TCPConnector::StartInLoop, this));

    return;
}
//---------------------------------------------------------------------------
void TCPConnector::Stop()
{
    running_ = false;
    event_loop_->RunInLoop(std::bind(&TCPConnector::StopInLoop, this));

    return;
}
//---------------------------------------------------------------------------
void TCPConnector::Restart()
{
    event_loop_->AssertInLoopThread();

    running_ = true;
    state_ = DISCONNECTED;
    retry_delay_ = kInitRetryDelay;

    StartInLoop();
}
//---------------------------------------------------------------------------
void TCPConnector::StartInLoop()
{
    event_loop_->AssertInLoopThread();
    assert(DISCONNECTED == state_);

    if(running_)
        Connect();

    return;
}
//---------------------------------------------------------------------------
void TCPConnector::StopInLoop()
{
    event_loop_->AssertInLoopThread();

    event_loop_->TimerCancel(timer_id_);

    //如果是正在连接状态，则Channel正在等待服务器建立连接，此时需要移除侦听状态
    if(CONNECTING == state_)
    {
        state_ = DISCONNECTED;
        RemoveAndResetChannel();
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConnector::Connect()
{
    int sockfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0);
    int err_code = ::connect(sockfd,
            reinterpret_cast<const sockaddr*>(&(server_addr_.address())), sizeof(sockaddr));
    int err_no = (0==err_code) ? 0 : errno;
    switch(err_no)
    {
        case 0:
        case EINTR:
        case EINPROGRESS:
        case EISCONN:
            Connecting(sockfd);
            break;

        default:
            NetLogger_error("connect failed, errno:%d, msg:%s", err_no, OSError(err_no));
            ::close(sockfd);
            Retry();
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConnector::Connecting(int fd)
{
    assert(DISCONNECTED == state_);
    assert(!channel_);

    state_ = CONNECTING;

    channel_.reset(new Channel(event_loop_, fd));
    channel_->set_write_cb(std::bind(&TCPConnector::HandleWrite, this));
    channel_->set_error_cb(std::bind(&TCPConnector::HandleError, this));
    channel_->set_close_cb(std::bind(&TCPConnector::HandleError, this));

    //如果连接成功建立，则会出发写事件
    channel_->EnableWriting();
}
//---------------------------------------------------------------------------
void TCPConnector::HandleWrite()
{
    assert(CONNECTING == state_);

    //连接成功建立，取消监听的事件
    int sockfd = channel_->fd();
    RemoveAndResetChannel();

    //检测连接是否可用,错误码大于0表示错误
    int err_code = Socket::GetSocketError(sockfd);
    if(0 < err_code)
    {
        NetLogger_error("connect failed, errno:%d, msg:%s, reconnect", err_code, OSError(err_code));
        ::close(sockfd);
        Retry();
        return;
    }

    //检查是否自连接
    if(Socket::GetLocalAddress(sockfd) == Socket::GetPeerAddress(sockfd))
    {
        NetLogger_error("self connectioin, retry");

        ::close(sockfd);
        Retry();
        return;
    }

    //连接成功，通知回调
    state_ = CONNECTED;
    if(running_)
        new_conn_cb_(sockfd);
    else
        ::close(sockfd);
    
    return;
}
//---------------------------------------------------------------------------
void TCPConnector::HandleError()
{
    NetLogger_error("connect error");
    if(CONNECTING == state_)
    {
        int fd = channel_->fd();
        int err_code = Socket::GetSocketError(fd);
        NetLogger_error("connect error, errno:%d, msg:%s", err_code, OSError(err_code));
        RemoveAndResetChannel();

        ::close(fd);
        Retry();
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConnector::Retry()
{
    state_ = DISCONNECTED;

    if(running_)
    {
        NetLogger_info("Connector retry connectto %s in seconds %d",
                server_addr_.IpPort().c_str(), retry_delay_/1000);
        timer_id_ = event_loop_->TimerAfter(retry_delay_/1000, std::bind(&TCPConnector::StartInLoop,
                    shared_from_this()));
        retry_delay_ = std::min(retry_delay_*2, kMaxRetryDelay);
    }

    return;
}
//---------------------------------------------------------------------------
void TCPConnector::RemoveAndResetChannel()
{
    channel_->DisableAll();
    channel_->Remove();

    //不能直接reset channel_,因为现在还在Channel的HandleEvent方法中
    event_loop_->QueueInLoop(std::bind(&TCPConnector::ResetChannel, shared_from_this()));
}
//---------------------------------------------------------------------------
void TCPConnector::ResetChannel()
{
    channel_.reset();
    return;
}
//---------------------------------------------------------------------------

}//namespace net
//---------------------------------------------------------------------------
