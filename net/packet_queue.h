//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_PACKET_QUEUE_H_
#define BASE_LINUX_NET_PACKET_QUEUE_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "datagram_packet.h"
//---------------------------------------------------------------------------
namespace net
{

class PacketQueue
{
public:
    PacketQueue();
    ~PacketQueue();

    void            Push(DatagramPacket&& pkt);
    DatagramPacket  Pop ();

    void Wakeup();

private:
    std::queue<DatagramPacket>  pkt_queue_;
    std::mutex                  mutex_;
    std::condition_variable     cond_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_PACKET_QUEUE_H_
