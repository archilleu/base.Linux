//---------------------------------------------------------------------------
#include "connector.h"
#include "event_loop.h"
#include "channel.h"
#include "net_log.h"
#include "socket.h"
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
const int Connector::kMaxRetryDelay;
//---------------------------------------------------------------------------
Connector::Connector(EventLoop* loop, const InetAddress& svr)
:   running_(false),
    loop_(loop),
    svr_addr_(svr),
    states_(DISCONNECTED),
    retry_delay_(kInitRetryDelay)
{
    SystemLog_Debug("Connector ctor");    
    return;
}
//---------------------------------------------------------------------------
Connector::~Connector()
{
    SystemLog_Debug("Connector dtor");    
    assert(!channel_);

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
    if(CONNECTED == states_)
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
    int sockfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0);
    if(0 > sockfd)
    {
        SystemLog_Error("Create socket failed, errno:%d, msg:%s", errno, StrError(errno)); 
        Retry();
        return;
    }

    int err_code= ::connect(sockfd, reinterpret_cast<const sockaddr*>(&(svr_addr_.address())), sizeof(sockaddr));
    int err_no  = (0==err_code) ? 0 : errno;
    switch(err_no)
    {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            Connecting(sockfd);
            break;

        default:
            SystemLog_Error("connect failed, errno:%d, msg:%s", err_no, StrError(err_no));
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
    channel_->set_callback_write(std::bind(&Connector::HandleWrite, this));
    channel_->set_callback_error(std::bind(&Connector::HandleError, this));
    channel_->set_callback_close(std::bind(&Connector::HandleError, this));

    channel_->WriteEnable();
    return;
}
//---------------------------------------------------------------------------
void Connector::HandleWrite()
{
    assert(CONNECTINTG == states_);

    int sockfd = RemoveAndResetChannel();

    //检查连接情况
    int err_code = Socket::GetSocketError(sockfd);
    if(0 < err_code)//大于0表示出错
    {
        SystemLog_Error("connect failed, errno:%d, msg:%s", err_code, StrError(err_code));
        return;
    }

    //是否自连接 
    if(Socket::GetLocalAddress(sockfd) == Socket::GetPeerAddress(sockfd))
    {
        SystemLog_Warning("self connection, retry");
        ::close(sockfd);
        Retry();
    }

    //连接成功,通知上层
    states_ = CONNECTED;
    if(running_)
        callback_new_connection_(sockfd);//此时sockfd交予上层控制释放

    return;
}
//---------------------------------------------------------------------------
void Connector::HandleError()
{
    SystemLog_Warning("connect error");

    if(CONNECTINTG == states_)
    {
        int sockfd  = RemoveAndResetChannel();
        int err_code= Socket::GetSocketError(sockfd);
        SystemLog_Error("connect error, errno:%d, msg:%s", err_code, StrError(err_code));
        ::close(sockfd);
        Retry();
    }

    return;
}
//---------------------------------------------------------------------------
void Connector::Retry()
{
    states_ = DISCONNECTED;
    
    if(!running_) 
        return;

    SystemLog_Info("Connector retry connect to %s in seconds %d",  svr_addr_.IPPort().c_str(), retry_delay_/1000);
    loop_->RunAfter(retry_delay_/1000, std::bind(&Connector::StartInLoop, shared_from_this()));
    retry_delay_ = std::min(retry_delay_*2, kMaxRetryDelay);
    return;
}
//---------------------------------------------------------------------------
int Connector::RemoveAndResetChannel()
{
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
