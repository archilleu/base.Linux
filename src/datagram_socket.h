//---------------------------------------------------------------------------
#ifndef NET_DATAGRAM_SOCKET_H_
#define NET_DATAGRAM_SOCKET_H_
//---------------------------------------------------------------------------
#include <memory>
#include "datagram_packet.h"
//---------------------------------------------------------------------------
namespace net
{

class Socket;

class DatagramSocket
{
public:
    DatagramSocket();
    DatagramSocket(short port);
    DatagramSocket(const InetAddress& addr);
    ~DatagramSocket();

    int fd();

    void Bind(const InetAddress& inet_addr);
    bool IsBind(){ return is_bind_; }

    //如果需要Bind, 则需要在connect之前Bind
    bool Connect(const InetAddress& server_addr);
    bool Disconnect();
    bool IsConnect() { return connected_; }

    const InetAddress& local_addr() { return local_addr_; }
    const InetAddress& server_addr(){ return server_addr_; }

    void SetRcvBufferSize(int size);
    void SetSndBufferSize(int size);

    int GetRcvBufferSize();
    int GetSndBufferSize();

    void SetBroadcast(bool enable);
    bool IsBroadcast();

    void SetReuseAddress();

    DatagramPacket  Receive(int len);
    void            Receive(DatagramPacket& pkt);

    int Send(const DatagramPacket& pkt);
    int Send(const char* dat, size_t len, const InetAddress& to=InetAddress::INVALID_ADDR);

    void Shutdown(bool read=true, bool write=false);

private:
    int SocketCeate();

private:
    InetAddress local_addr_;
    InetAddress server_addr_;
    bool        connected_;
    bool        is_bind_;

    std::shared_ptr<Socket>  sockfd_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_DATAGRAM_SOCKET_H_
