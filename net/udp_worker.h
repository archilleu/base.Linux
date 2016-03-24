//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_UDP_WORKER_H_
#define BASE_LINUX_NET_UDP_WORKER_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/thread.h"
//---------------------------------------------------------------------------
namespace net
{

class PacketQueue;
class DatagramPacket;

class UDPWorkder
{
public:
    typedef std::function<void (const DatagramPacket& pkt)> CallbackRcvPacket;

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
#endif //BASE_LINUX_NET_UDP_WORKER_H_
