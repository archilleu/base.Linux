#include "socket.h"
#include "net_logger.h"
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
Socket::~Socket()
{
    if(0 < fd_)
        ::close(fd_);
}
//---------------------------------------------------------------------------
tcp_info Socket::GetTCPInfo() const
{
    tcp_info info;
    bzero(&info, sizeof(info));
    socklen_t len = sizeof(info);
    ::getsockopt(fd_, SOL_TCP, TCP_INFO, &info, &len);

    return info;
}
//---------------------------------------------------------------------------
std::string Socket::GetTCPInfoString() const
{
    tcp_info info = GetTCPInfo();
    char buf[256];
    snprintf(buf, sizeof(buf), 
            "unrecovered=%u "
            "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
            "lost=%u retrans=%u rtt=%u rttvar=%u "
            "sshthresh=%u cwnd=%u total_retrans=%u",
            info.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
            info.tcpi_rto,          // Retransmit timeout in usec
            info.tcpi_ato,          // Predicted tick of soft clock in usec
            info.tcpi_snd_mss,
            info.tcpi_rcv_mss,
            info.tcpi_lost,         // Lost packets
            info.tcpi_retrans,      // Retransmitted packets out
            info.tcpi_rtt,          // Smoothed round trip time in usec
            info.tcpi_rttvar,       // Medium deviation
            info.tcpi_snd_ssthresh,
            info.tcpi_snd_cwnd,
            info.tcpi_total_retrans);  // Total retransmits for entire connection

    return buf;
}
//---------------------------------------------------------------------------
void Socket::ShutDownWrite()
{
    ::shutdown(fd_, SHUT_WR);
    return;
}
//---------------------------------------------------------------------------
void Socket::Bind(const InetAddress& inet_addr)
{
    if(0 > ::bind(fd_, reinterpret_cast<const sockaddr*>(&inet_addr.address()), sizeof(inet_addr.address())))
    {
        NetLogger_error("bind failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetReuseAddress()
{
    int reuse = 1;
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetReusePort()
{
    //http://xiaorui.cc/2015/12/02/%E4%BD%BF%E7%94%A8socket-so_reuseport%E6%8F%90%E9%AB%98%E6%9C%8D%E5%8A%A1%E7%AB%AF%E6%80%A7%E8%83%BD/
    
    int reuse = 1;
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(int)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetNodelay()
{
    ///设置监听套接字选项，accept自动继承以下套接字选项
    ///SO_DEBUG,SO_DONTROUTE,SO_KEEPALIVE,SO_LINGER,SO_OOBINLINE,
    //SO_SNDBUF,SO_RCVBUF,SO_RCVLOWAT,SO_SNDLOWAT,TCP_NODELAY
    int nodelay = 1;
    if(0 > setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }
    
    return;
}
//---------------------------------------------------------------------------
void Socket::SetKeepAlive(int interval)
{
    Socket::SetKeepAlive(fd_, interval);
}
//---------------------------------------------------------------------------
void Socket::SetIPV6Only()
{
    int yes = 1;
    if(0 > setsockopt(fd_, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(yes)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }
    
    return;
}
//---------------------------------------------------------------------------
void Socket::SetTimeoutRecv(int timeoutS)
{
    struct timeval timeout;
    timeout.tv_sec = timeoutS;
    timeout.tv_usec = 0;
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetTimeoutSend(int timeoutS)
{
    struct timeval timeout;
    timeout.tv_sec = timeoutS;
    timeout.tv_usec = 0;
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetSendBufferSize(int size)
{
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&size), sizeof(int)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Socket::SetRecvBufferSize(int size)
{
    if(0 > setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&size), sizeof(int)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
int Socket::GetSendBufferSize()
{
    int val = 0;
    socklen_t len = sizeof(val);
    if(0 > getsockopt(fd_, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&val), &len))
    {
        NetLogger_warn("getsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return val;
}
//---------------------------------------------------------------------------
int Socket::GetRecvBufferSize()
{
    int val = 0;
    socklen_t len = sizeof(val);
    if(0 > getsockopt(fd_, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&val), &len))
    {
        NetLogger_warn("getsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return val;
}
//---------------------------------------------------------------------------
InetAddress Socket::GetLocalAddress()
{
    return Socket::GetLocalAddress(fd_);
}
//---------------------------------------------------------------------------
InetAddress Socket::GetPeerAddress()
{
    return Socket::GetPeerAddress(fd_);
}
//---------------------------------------------------------------------------
bool Socket::IsSelfConnect()
{
    if(GetLocalAddress() != GetPeerAddress())
        return false;

    return true;
}
//---------------------------------------------------------------------------
InetAddress Socket::GetLocalAddress(int sockfd)
{
    struct sockaddr_storage  local_address;
    socklen_t len = static_cast<socklen_t>(sizeof(local_address));
    if(0 > ::getsockname(sockfd, reinterpret_cast<sockaddr*>(&local_address), &len))
    {
        NetLogger_warn("getsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return local_address;
}
//---------------------------------------------------------------------------
InetAddress Socket::GetPeerAddress(int sockfd)
{
    struct sockaddr_storage peer_address;
    socklen_t len = static_cast<socklen_t>(sizeof(peer_address));
    if(0 > ::getpeername(sockfd, reinterpret_cast<sockaddr*>(&peer_address), &len))
    {
        NetLogger_warn("getsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
    }

    return peer_address;
}
//---------------------------------------------------------------------------
void Socket::SetKeepAlive(int sockfd, int interval)
{
    int val = 1;
    if(0 > setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
        return;
    }

    //send first probe after val interval.
    val = interval;
    if(0 > setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
        return;
    }

    /* Send next probes after the specified interval.
    * Note that we set the delay as interval / 3,
    * as we send three probes before detecting an error
    * (see the next setsockopt call). */ 
    val = interval/3;
    if(0 > setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
        return;
    }

    /* Consider the socket in error state after three we send three ACK
     * probes without getting a reply. */ 
    val = 3;
    if(0 > setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)))
    {
        NetLogger_warn("setsockopt failed errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
        return;
    }

    return;
}
//---------------------------------------------------------------------------
int Socket::GetSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);
    if(0 > ::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen))
    {
        NetLogger_warn("getsocket failed, errno:%d, msg:%s", errno, OSError(errno));
        return errno;
    }
    
    return optval;
}
//---------------------------------------------------------------------------

}//namespace net
