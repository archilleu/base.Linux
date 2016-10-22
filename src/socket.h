//---------------------------------------------------------------------------
#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_
//---------------------------------------------------------------------------
#include <cassert>
#include <unistd.h>
#include "inet_address.h"
//---------------------------------------------------------------------------
/*
 * Socket 内部维护的描述符不是由自己创建的,但是由自己销毁
 */
//---------------------------------------------------------------------------
struct tcp_info;

namespace net
{

class Socket
{
public:
    Socket(int sockfd)
    :   fd_(sockfd)
    {
        assert(0 < fd_);
        return;
    }

    ~Socket()
    {
        if(0 < fd_)
            ::close(fd_);
    }
    Socket(const Socket&) =delete;
    Socket& operator=(const Socket&) =delete;

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

    static void SetKeepAlive(int sockfd, int interval); //total detect disconnect time=interval*2 secs
    
    static int GetSocketError(int sockfd);

private:
   int fd_;

};

}
//---------------------------------------------------------------------------
#endif// NET_SOCKET_H_
