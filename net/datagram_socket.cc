//---------------------------------------------------------------------------
#include "datagram_socket.h"
#include "socket.h"
#include "net_log.h"
//---------------------------------------------------------------------------
namespace net
{
//---------------------------------------------------------------------------
DatagramSocket::DatagramSocket()
:   connected_(false),
    is_bind_(false)
{
    sockfd_.reset(new Socket(SocketCeate()));
    if(0 > sockfd_->fd())
    {
        char buffer[128];
        SystemLog_Error("socket create failed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
DatagramSocket::DatagramSocket(short port)
:   local_addr_(port),
    connected_(false),
    is_bind_(false)
{
    sockfd_.reset(new Socket(SocketCeate()));
    if(false == Bind(local_addr_))
    {
        char buffer[128];
        SystemLog_Error("socket bind failed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    is_bind_ = true;
    return;
}
//---------------------------------------------------------------------------
DatagramSocket::DatagramSocket(const InetAddress& inet_addr)
:   connected_(false),
    is_bind_(false)
{
    sockfd_.reset(new Socket(SocketCeate()));
    if(false == Bind(inet_addr))
    {
        char buffer[128];
        SystemLog_Error("socket bind failed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
DatagramSocket::~DatagramSocket()
{
}
//---------------------------------------------------------------------------
int DatagramSocket::fd()
{
    return sockfd_->fd();
}
//---------------------------------------------------------------------------
bool DatagramSocket::Bind(const InetAddress& inet_addr)
{
    bool err_code = sockfd_->Bind(inet_addr);
    if(false == err_code)
        return false;

    local_addr_ = inet_addr;
    is_bind_    = true;
    return true;
}
//---------------------------------------------------------------------------
bool DatagramSocket::Connect(const InetAddress& addr)
{
    if(0 > ::connect(sockfd_->fd(), reinterpret_cast<const sockaddr*>(&addr.address()), sizeof(sockaddr)))
    {
        char buffer[128];
        SystemLog_Error("socket connect failed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);

        return false;
    }

    server_addr_= addr;
    connected_  = true;
    return true;
}
//---------------------------------------------------------------------------
bool DatagramSocket::Disconnect()
{
    bool err_code = Connect(InetAddress::INVALID_ADDR);
    if(true == err_code)
        connected_ = false;

    return err_code;
}
//---------------------------------------------------------------------------
void DatagramSocket::SetRcvBufferSize(int size)
{
    sockfd_->SetRecvBufferSize(size);
    return;
}
//---------------------------------------------------------------------------
void DatagramSocket::SetSndBufferSize(int size)
{
    sockfd_->SetSendBufferSize(size);
    return;
}
//---------------------------------------------------------------------------
int DatagramSocket::GetRcvBufferSize()
{
    int size = sockfd_->GetRecvBufferSize();
    return size;
}
//---------------------------------------------------------------------------
int DatagramSocket::GetSndBufferSize()
{
    int size = sockfd_->GetSendBufferSize();
    return size;
}
//---------------------------------------------------------------------------
void DatagramSocket::SetBroadcast(bool enable)
{
    int broadcast = enable;
    if(0 > setsockopt(sockfd_->fd(), SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&broadcast), sizeof(int)))
    {
        char buffer[128];
        SystemLog_Error("set broadcast failed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
bool DatagramSocket::IsBroadcast()
{
    int         broadcast   = 0;
    socklen_t   len         = sizeof(int);
    if(0 > getsockopt(sockfd_->fd(), SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&broadcast), &len))
    {
        char buffer[128];
        SystemLog_Error("set broadcast failed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
    }

    return broadcast ? true : false;
}
//---------------------------------------------------------------------------
void DatagramSocket::SetReuseAddress()
{
    sockfd_->SetReuseAddress();
    return;
}
//---------------------------------------------------------------------------
DatagramPacket DatagramSocket::Receive(int len)
{
    sockaddr_in from;
    bzero(&from, sizeof(sockaddr_in));

    DatagramPacket  pkt(len);
    socklen_t       length  = sizeof(sockaddr_in);
    ssize_t         rlen    = ::recvfrom(sockfd_->fd(), pkt.dat().dat(), len, 0, reinterpret_cast<sockaddr*>(&from), &length) ;
    if(0 < rlen)
    {
        pkt.set_effective(rlen);
        pkt.set_address(InetAddress(from));
    }

    return pkt;
}
//---------------------------------------------------------------------------
void DatagramSocket::Receive(DatagramPacket& pkt)
{
    sockaddr_in from;
    bzero(&from, sizeof(sockaddr_in));

    socklen_t   length  = sizeof(sockaddr_in);
    ssize_t     rlen    = ::recvfrom(sockfd_->fd(), pkt.dat().dat(), pkt.dat().len(), 0, reinterpret_cast<sockaddr*>(&from), &length) ;
    if(0 < rlen)
    {
        pkt.set_effective(rlen);
        pkt.set_address(InetAddress(from));
    }

    return;
}
//---------------------------------------------------------------------------
int DatagramSocket::Send(const DatagramPacket& pkt)
{
    ssize_t wlen = 0;
    if(true == connected_)
        wlen = ::send(sockfd_->fd(), pkt.dat().dat(), pkt.effective(), 0);
    else
        wlen = ::sendto(sockfd_->fd(), pkt.dat().dat(), pkt.effective(), 0, reinterpret_cast<const sockaddr*>(&pkt.address().address()), sizeof(sockaddr));

    return static_cast<int>(wlen);
}
//---------------------------------------------------------------------------
void DatagramSocket::Shutdown(bool read, bool write)
{
    if(true == read)
        ::shutdown(sockfd_->fd(), SHUT_RD);

    if(true == write)
        ::shutdown(sockfd_->fd(), SHUT_WR);
    return;
}
//---------------------------------------------------------------------------
int DatagramSocket::SocketCeate()
{
    int sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(0 > sockfd)
    {
        char buffer[128];
        SystemLog_Error("socket create failed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
    }

    return sockfd;
}
//---------------------------------------------------------------------------
}//namespace net
