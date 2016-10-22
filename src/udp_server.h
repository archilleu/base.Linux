//---------------------------------------------------------------------------
#ifndef NET_UDP_SERVER_H_
#define NET_UDP_SERVER_H_
//---------------------------------------------------------------------------
#include "callback.h"
#include "inet_address.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class UDPReceiver;
class UDPWorkder;
class PacketQueue;
class DatagramSocket;

class UDPServer
{
public:
    UDPServer(const InetAddress& rcv_addr);
    ~UDPServer();

    void set_callback_rcv_pkt(const CallbackRcvPacket& callback);

    void Start  (int thread_nums);
    void Stop   ();

    void Send(const DatagramPacket& pkt);
    void Send(const char* dat, size_t len, const InetAddress& to);

private:
    void OnReceiverError();

private:
    std::shared_ptr<DatagramSocket> socket_;
    std::shared_ptr<EventLoop>      loop_;
    std::shared_ptr<PacketQueue>    pkt_queue_;
    std::shared_ptr<UDPReceiver>    receiver_;
    std::shared_ptr<UDPWorkder>     worker_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_UDP_SERVER_H_
