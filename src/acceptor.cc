//---------------------------------------------------------------------------
#include <fcntl.h>
#include "acceptor.h"
#include "event_loop.h"
#include "socket.h"
#include "inet_address.h"
#include "channel.h"
#include "net_log.h"
#include "poll.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
Acceptor::Acceptor(EventLoop* owner_loop, const InetAddress& inet_listen)
:   owner_loop_(owner_loop),
    idle_fd_(::open("/dev/null", O_RDONLY|O_CLOEXEC))
{
    SystemLog_Info("Acceptor ctor");
    if(0 > idle_fd_)
    {
        SystemLog_Error("open idle_fd failed, errno:%d, msg:%s", errno, StrError(errno));
        abort();
    }

    if(true == inet_listen.IsIPV4())
    {
        listen_sock_ = std::make_shared<Socket>(::socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0));
        if(0 > listen_sock_->fd())
        {
            SystemLog_Error("listen sock create failed, errno:%d, msg:%s", errno, StrError(errno));
            abort();
        }
    }
    else
    {
        listen_sock_ = std::make_shared<Socket>(::socket(AF_INET6, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0));
        if(0 > listen_sock_->fd())
        {
            SystemLog_Error("listen sock create failed, errno:%d, msg:%s", errno, StrError(errno));
            abort();
        }
        listen_sock_->SetIPV6Only();
    }
    channel_listen_ = std::make_shared<Channel>(owner_loop, listen_sock_->fd());

    listen_sock_->SetReuseAddress();
    listen_sock_->Bind(inet_listen);
    channel_listen_->set_callback_read(std::bind(&Acceptor::HandleRead, this, std::placeholders::_1));

    return;
}
//---------------------------------------------------------------------------
Acceptor::~Acceptor()
{
    SystemLog_Info("Acceptor dtor");

    if(!channel_listen_)
        return;

    channel_listen_->DisableAll();
    channel_listen_->Remove();
    ::close(idle_fd_);
    
    return;
}
//---------------------------------------------------------------------------
void Acceptor::Listen()
{
    SystemLog_Info("Acceptor listen");
    owner_loop_->AssertInLoopThread();

    if(0 > ::listen(channel_listen_->fd(), SOMAXCONN))
    {
        SystemLog_Error("listen sock failed, errno:%d, msg:%s", errno, StrError(errno));
        abort();
    }

    channel_listen_->ReadEnable();
    return;
}
//---------------------------------------------------------------------------
int Acceptor::AcceptConnection(InetAddress* inet_peer)
{
    sockaddr_storage client_addr;
    socklen_t len = sizeof(client_addr);
    int clientfd= ::accept4(channel_listen_->fd(), reinterpret_cast<sockaddr*>(&client_addr), &len , SOCK_NONBLOCK|SOCK_CLOEXEC);
    if(0 > clientfd)
    {
        if(EAGAIN == errno)
            return clientfd;

        SystemLog_Error("accept client failed, errno:%d, msg:%s", errno, StrError(errno));
        return -1;
    }

    inet_peer->set_address(client_addr);
    return clientfd;
}
//---------------------------------------------------------------------------
void Acceptor::HandleRead(uint64_t rcv_time)
{
    for(;;)
    {
        InetAddress peer_addr;
        int clientfd = AcceptConnection(&peer_addr);
        if(0 < clientfd)
        {
            if(false == CheckConnection(clientfd))
            {
                SystemLog_Warning("bad connection");
                ::close(clientfd);
                continue;
            }

            //处理连接,如果不处理,则关闭
            if(callback_new_connection_)
            {
                Socket::SetKeepAlive(clientfd, 30);
                callback_new_connection_(clientfd, peer_addr, rcv_time);
            }
            else
                ::close(clientfd);
        }
        else
        {
            if(EAGAIN == errno)
                break;

            if(EMFILE == errno)
            {
                ::close(idle_fd_);
                idle_fd_ = AcceptConnection(&peer_addr);
                ::close(idle_fd_);
                idle_fd_ = ::open("/dev/null", O_RDONLY|O_CLOEXEC);
            }

            SystemLog_Error("AcceptConnection failed");
            assert(0);
            return;
        }
    }

    return;
}
//---------------------------------------------------------------------------
bool Acceptor::CheckConnection(int fd)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLOUT;
    pfd.revents = 0;

    int num = poll(&pfd, 1, 0);
    if(1 == num)
    {
        if(POLLOUT & pfd.revents)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
}//namespace net
