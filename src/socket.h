//---------------------------------------------------------------------------
#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_
//---------------------------------------------------------------------------
#include <cassert>
#include <unistd.h>
#include "inet_address.h"
#include "base/include/noncopyable.h"
//---------------------------------------------------------------------------
/*
 * Socket 内部维护的描述符由外部创建,内部销毁
 */
//---------------------------------------------------------------------------
struct tcp_info;

namespace net
{

class Socket : public base::Noncopyable
{
public:
    Socket(int sockfd);
    Socket(Socket&& other)
    :   fd_(std::move(other.fd_))
    {
        other.fd_ = -1;
    }
    ~Socket();

    int fd() const { return fd_; };

    tcp_info GetTCPInfo() const;
    std::string GetTCPInfoString() const;

    void ShutDownWrite();

    void Bind(const InetAddress& inet_addr);

    void SetReuseAddress();
    void SetReusePort();

    void SetNodelay();

    void SetKeepAlive(int interval);

    void SetIPV6Only();
    
    void SetTimeoutRecv(int timeoutS);
    void SetTimeoutSend(int timeoutS);

    void SetSendBufferSize(int size);
    void SetRecvBufferSize(int size);

    int GetSendBufferSize();
    int GetRecvBufferSize();

    InetAddress GetLocalAddress();
    InetAddress GetPeerAddress();

    bool IsSelfConnect();

public:
    static InetAddress GetLocalAddress(int sockfd);
    static InetAddress GetPeerAddress(int sockfd);

    //total detect disconnect time=interval*2 secs
    static void SetKeepAlive(int sockfd, int interval);
    
    static int GetSocketError(int sockfd);

private:
   int fd_;
};

}
//---------------------------------------------------------------------------
#endif// NET_SOCKET_H_
