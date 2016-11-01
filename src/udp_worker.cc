//---------------------------------------------------------------------------
#include "udp_worker.h"
#include "net_log.h"
#include "packet_queue.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
UDPWorkder::UDPWorkder(PacketQueue* pkt_queue)
:   pkt_queue_(pkt_queue)
{
    assert(0 != pkt_queue_);
    NetLogger_info("udp worker ctor");
    return;
}
//---------------------------------------------------------------------------
UDPWorkder::~UDPWorkder()
{
    NetLogger_info("udp worker dtor");
    return;
}
//---------------------------------------------------------------------------
bool UDPWorkder::Start(int thread_nums)
{
    assert(0 < thread_nums);
    NetLogger_info("udp worker starting with thread nums:%d", thread_nums);

    for(int i=0; i<thread_nums; i++)
    {
        thread_list_.push_back(base::Thread(std::bind(&UDPWorkder::OnThreadProcessPacket, this), "UDPWorkder"));
    }
    running_ = true;
    for(int i=0; i<thread_nums; i++)
    {
        if(false == thread_list_[i].Start())
        {
            NetLogger_off("thread:%d start failed", i);
            assert(0);
            return false;
        }
    }

    NetLogger_info("udp worker started");
    return true;
}
//---------------------------------------------------------------------------
void UDPWorkder::Stop()
{
    if(false == running_)
        return;

    NetLogger_info("udp worker stopping");

    running_ = false;
    pkt_queue_->Wakeup();
    for(size_t i=0; i<thread_list_.size(); i++)
    {
        thread_list_[i].Join();
    }

    NetLogger_info("udp worker stoped");
    return;
}
//---------------------------------------------------------------------------
void UDPWorkder::OnThreadProcessPacket()
{
    while(running_)
    {
        DatagramPacket pkt = pkt_queue_->Pop();//等待收到数包
        if(false == running_)   //worker stop
            return;

        if(0 == pkt.effective())
            return;

        callback_rcv_pkt_(std::move(pkt));
    }

    return;
}
//---------------------------------------------------------------------------

}//namespace net
