//---------------------------------------------------------------------------
#include <memory>
#include "connector.h"
#include "event_loop.h"
#include "channel.h"
#include "net_logger.h"
#include "socket.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
const int Connector::kMaxRetryDelay;
const int Connector::kInitRetryDelay;
//---------------------------------------------------------------------------
Connector::Connector(EventLoop* loop, const InetAddress& svr)
:   running_(false),
    loop_(loop),
    svr_addr_(svr),
    states_(DISCONNECTED),
    retry_delay_(kInitRetryDelay),
    timer_id_(0, 0)
{
    NetLogger_info("Connector ctor");    
    return;
}
//---------------------------------------------------------------------------
Connector::~Connector()
{
    NetLogger_info("Connector dtor");    

    return;
}
//---------------------------------------------------------------------------
void Connector::Start()
{
    running_ = true;
    loop_->RunInLoop(std::bind(&Connector::StartInLoop, this));

    return;
}
//---------------------------------------------------------------------------
void Connector::Restart()
{
    loop_->AssertInLoopThread();

    states_     = DISCONNECTED;
    retry_delay_= kInitRetryDelay;
    running_    = true;

    StartInLoop();
    return;
}
//---------------------------------------------------------------------------
void Connector::Stop()
{
    running_ = false;
    loop_->RunInLoop(std::bind(&Connector::StopInLoop, this));
    loop_->TimerCancel(timer_id_);

    return;
}
//---------------------------------------------------------------------------
void Connector::StartInLoop()
{
    loop_->AssertInLoopThread();
    assert(DISCONNECTED == states_);

    if(running_)
        Connect();

    return;
}
//---------------------------------------------------------------------------
void Connector::StopInLoop()
{
    loop_->AssertInLoopThread();
    if(CONNECTINTG == states_)
    {
        states_ = DISCONNECTED;
        int sockfd = RemoveAndResetChannel();
        ::close(sockfd);
    }

    return;
}
//---------------------------------------------------------------------------
void Connector::Connect()
{
    int sockfd  = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0);
    int err_code= ::connect(sockfd, reinterpret_cast<const sockaddr*>(&(svr_addr_.address())), sizeof(sockaddr));
    int err_no  = (0==err_code) ? 0 : errno;
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
            Retry(sockfd);

            break;
    }

    return;
}
//---------------------------------------------------------------------------
void Connector::Connecting(int sockfd)
{
    assert(DISCONNECTED == states_);
    assert(!channel_);

    states_ = CONNECTINTG;

    channel_.reset(new Channel(loop_, sockfd));
    channel_->set_write_cb(std::bind(&Connector::HandleWrite, this));
    channel_->set_error_cb(std::bind(&Connector::HandleError, this));
    channel_->set_close_cb(std::bind(&Connector::HandleError, this));

    channel_->EnableWriting();
    return;
}
//---------------------------------------------------------------------------
void Connector::HandleWrite()
{
    assert(CONNECTINTG == states_);

    //受到反馈事件，因为sock fd 不可以重用，
    //所以当前监听channel取消，链接不成功继续新的监听，
    //成功则不再监听
    int sockfd = RemoveAndResetChannel();

    //检查连接情况
    int err_code = Socket::GetSocketError(sockfd);
    if(0 < err_code)//大于0表示出错
    {
        NetLogger_warn("connect failed, errno:%d, msg:%s", err_code, OSError(err_code));

        Retry(sockfd);
        return;
    }

    //是否自连接 
    if(Socket::GetLocalAddress(sockfd) == Socket::GetPeerAddress(sockfd))
    {
        NetLogger_warn("self connection, retry");

        Retry(sockfd);
    }

    //连接成功,通知上层
    states_ = CONNECTED;
    if(running_)
        callback_new_connection_(sockfd);//此时sockfd交予上层控制释放
    else
        ::close(sockfd);

    retry_delay_ = kInitRetryDelay;
    return;
}
//---------------------------------------------------------------------------
void Connector::HandleError()
{
    NetLogger_error("connect error");

    if(CONNECTINTG == states_)
    {
        int sockfd  = RemoveAndResetChannel();
        int err_code= Socket::GetSocketError(sockfd);
        NetLogger_error("connect error, errno:%d, msg:%s", err_code, OSError(err_code));
        Retry(sockfd);
    }

    return;
}
//---------------------------------------------------------------------------
void Connector::Retry(int fd)
{
    ::close(fd);
    states_ = DISCONNECTED;

    if(running_) 
    {
        NetLogger_warn("Connector retry connect to %s in seconds %d",  svr_addr_.IpPort().c_str(), retry_delay_/1000);
        //loop_->TimerAfter(retry_delay_/1000, std::bind(&Connector::StartInLoop, shared_from_this()));
        retry_delay_ = std::min(retry_delay_*2, kMaxRetryDelay);
    }

    return;
}
//---------------------------------------------------------------------------
int Connector::RemoveAndResetChannel()
{
    channel_->DisableAll();
    channel_->Remove();
    int sockfd = channel_->fd();

    //不能直接reset channel_,因为现在还在Channel的HandleEvent方法中
    loop_->QueueInLoop(std::bind(&Connector::ResetChannel, this));
    return sockfd;
}
//---------------------------------------------------------------------------
void Connector::ResetChannel()
{
    channel_.reset();
    return;
}
//---------------------------------------------------------------------------

}//namespace net
