//---------------------------------------------------------------------------
#include "acceptor.h"
#include "socket.h"
#include "inet_address.h"
#include "channel.h"
#include "net_log.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
Acceptor::Acceptor(EventLoop* owner_loop, const InetAddress& inet_listen)
:   listen_sock_(new Socket(::socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0))),
    channel_listen_(new Channel(owner_loop, listen_sock_->fd()))
{
    SystemLog_Info("Acceptor ctor");

    if(0 > listen_sock_->fd())
    {
        SystemLog_Error("listen sock create failed, errno:%d, msg:%s", errno, StrError(errno));
        assert(0);
        return;
    }

    listen_sock_->SetReuseAddress();
    listen_sock_->SetNodelay();
    if(false == listen_sock_->Bind(inet_listen))
    {
        SystemLog_Error("bind sock create failed, errno:%d, msg:%s", errno, StrError(errno));
        assert(0);
        return;
    }

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
    
    return;
}
//---------------------------------------------------------------------------
bool Acceptor::Listen()
{
    SystemLog_Info("Acceptor listen");

    if(0 > ::listen(channel_listen_->fd(), SOMAXCONN))
    {
        SystemLog_Error("listen sock failed, errno:%d, msg:%s", errno, StrError(errno));
        assert(0);
        return false;
    }

    channel_listen_->ReadEnable();
    return true;
}
//---------------------------------------------------------------------------
int Acceptor::AcceptConnection(InetAddress* inet_peer)
{
    sockaddr_in client_addr;
    socklen_t   len     = sizeof(sockaddr_in);
    int         clientfd= ::accept4(channel_listen_->fd(), reinterpret_cast<sockaddr*>(&client_addr), &len , SOCK_NONBLOCK|SOCK_CLOEXEC);
    if(0 > clientfd)
    {
        SystemLog_Error("accept client failed, errno:%d, msg:%s", errno, StrError(errno));
        assert(0);
        return -1;
    }

    inet_peer->set_address(client_addr);
    return clientfd;
}
//---------------------------------------------------------------------------
void Acceptor::HandleRead(base::Timestamp rcv_time)
{
    InetAddress peer_addr;
    int         clientfd = AcceptConnection(&peer_addr);
    if(0 > clientfd)
    {
        SystemLog_Error("AcceptConnection failed");
        assert(0);
        return;
    }

    //处理连接,如果不处理,则关闭
    if(callback_new_connection_)
    {
        callback_new_connection_(clientfd, peer_addr, rcv_time);
    }
    else
    {
        ::close(clientfd);
    }

    return;
}
//---------------------------------------------------------------------------
}//namespace net
