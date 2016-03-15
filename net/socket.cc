//---------------------------------------------------------------------------
#include "socket.h"
#include "net_log.h"
#include <sys/socket.h>
#include <netinet/tcp.h>
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
Socket::Socket(int sockfd)
:   fd_(sockfd)
{
    assert(0 < fd_);
    return;
}
//---------------------------------------------------------------------------
Socket::~Socket()
{
    if(0 < fd_)
        ::close(fd_);

    return;
}
//---------------------------------------------------------------------------
int Socket::fd()
{
    return fd_;
}
//---------------------------------------------------------------------------
void Socket::ShutDown()
{
    ::shutdown(fd_, SHUT_RDWR);
    return;
}
//---------------------------------------------------------------------------
bool Socket::Bind(const InetAddress& inet_addr)
{
    if(0 > ::bind(fd_, reinterpret_cast<const sockaddr*>(&inet_addr.address()), sizeof(struct sockaddr_in)))
    {
        char buffer[128];
        SystemLog_Error("bind failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void Socket::SetReuseAddress()
{
    int reuse = 1;
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)))
    {
        char buffer[128];
        SystemLog_Error("setsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetNodelay()
{
    ///设置监听套接字选项，accept自动继承以下套接字选项
    ///SO_DEBUG,SO_DONTROUTE,SO_KEEPALIVE,SO_LINGER,SO_OOBINLINE,SO_SNDBUF,SO_RCVBUF,SO_RCVLOWAT,SO_SNDLOWAT,TCP_NODELAY
    int nodelay = 1;
    if(0 > setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)))
    {
        char buffer[128];
        SystemLog_Error("setsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }
    
    return;
}
//---------------------------------------------------------------------------
void Socket::SetTimeoutRecv(int timeoutS)
{
    struct timeval timeout;
    timeout.tv_sec  = timeoutS;
    timeout.tv_usec = 0;
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
    {
        char buffer[128];
        SystemLog_Error("setsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetTimeoutSend(int timeoutS)
{
    struct timeval timeout;
    timeout.tv_sec  = timeoutS;
    timeout.tv_usec = 0;
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)))
    {
        char buffer[128];
        SystemLog_Error("setsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetSendBufferSize(int size)
{
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&size), sizeof(int)))
    {
        char buffer[128];
        SystemLog_Error("setsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetRecvBufferSize(int size)
{
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&size), sizeof(int)))
    {
        char buffer[128];
        SystemLog_Error("setsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
int Socket::GetSendBufferSize()
{
    int         val = 0;
    socklen_t   len = sizeof(val);
    if(0 > getsockopt(fd_, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&val), &len))
    {
        char buffer[128];
        SystemLog_Error("getsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return val;
}
//---------------------------------------------------------------------------
int Socket::GetRecvBufferSize()
{
    int         val = 0;
    socklen_t   len = sizeof(val);
    if(0 > getsockopt(fd_, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&val), &len))
    {
        char buffer[128];
        SystemLog_Error("getsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return val;
}
//---------------------------------------------------------------------------
InetAddress Socket::GetLocalAddress()
{
    struct sockaddr_in  local_address;
    socklen_t           len = static_cast<socklen_t>(sizeof(local_address));
    if(0 > ::getsockname(fd_, reinterpret_cast<sockaddr*>(&local_address), &len))
    {
        char buffer[128];
        SystemLog_Error("getsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return local_address;
}
//---------------------------------------------------------------------------
InetAddress Socket::GetPeerAddress()
{
    struct sockaddr_in  peer_address;
    socklen_t           len = static_cast<socklen_t>(sizeof(peer_address));
    if(0 > ::getpeername(fd_, reinterpret_cast<sockaddr*>(&peer_address), &len))
    {
        char buffer[128];
        SystemLog_Error("getsockopt failed errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return peer_address;
}
//---------------------------------------------------------------------------
bool Socket::IsSelfConnect()
{
    if(GetLocalAddress() == GetPeerAddress())
        return true;

    return false;
}
//---------------------------------------------------------------------------
}//namespace net
