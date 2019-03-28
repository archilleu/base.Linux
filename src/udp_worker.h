//---------------------------------------------------------------------------
#ifndef NET_UDP_WORKER_H_
#define NET_UDP_WORKER_H_
//---------------------------------------------------------------------------
#include <vector>
#include "../thirdpart/base/include/thread.h"
//---------------------------------------------------------------------------
namespace net
{

class PacketQueue;
class DatagramPacket;

class UDPWorkder
{
public:
    using CallbackRcvPacket = std::function<void (const DatagramPacket& pkt)>;

    UDPWorkder(PacketQueue* pkt_queue);
    ~UDPWorkder();

    void set_callback_rcv_pkt(const CallbackRcvPacket& callback)    { callback_rcv_pkt_ = callback; }

    bool Start  (int thread_nums=1);
    void Stop   ();
    
private:
    void OnThreadProcessPacket();

private:
    bool                        running_;
    PacketQueue*                pkt_queue_;
    CallbackRcvPacket           callback_rcv_pkt_;
    std::vector<base::Thread>   thread_list_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_UDP_WORKER_H_
