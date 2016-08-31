//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_SOCKET_H_
#define BASE_LINUX_NET_SOCKET_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
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

protected:
    DISALLOW_COPY_AND_ASSIGN(Socket);

};

}
//---------------------------------------------------------------------------
#endif// BASE_LINUX_NET_SOCKET_H_
