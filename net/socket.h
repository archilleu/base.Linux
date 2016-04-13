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
namespace net
{

class Socket
{
public:
    Socket(int fd);
    ~Socket();

    int fd();

    void ShutDown();

    bool Bind(const InetAddress& inet_addr);

    void SetReuseAddress();
    void SetNodelay();
    
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
    
    static int GetSocketError(int sockfd);

private:
   int fd_;

protected:
    DISALLOW_COPY_AND_ASSIGN(Socket);
};

}
//---------------------------------------------------------------------------
#endif// BASE_LINUX_NET_SOCKET_H_
