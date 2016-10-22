//---------------------------------------------------------------------------
#include "packet_queue.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
PacketQueue::PacketQueue()
{
}
//---------------------------------------------------------------------------
PacketQueue::~PacketQueue()
{
}
//---------------------------------------------------------------------------
void PacketQueue::Push(DatagramPacket&& pkt)
{
    {
    std::unique_lock<std::mutex> lock(mutex_);
    pkt_queue_.push(std::move(pkt));
    }
    cond_.notify_one();

    return;
}
//---------------------------------------------------------------------------
DatagramPacket PacketQueue::Pop()
{
    DatagramPacket pkt;

    {
    std::unique_lock<std::mutex> lock(mutex_);
    if(pkt_queue_.empty())
        cond_.wait(lock);

    if(pkt_queue_.empty())
        return pkt;

    pkt = pkt_queue_.front();
    pkt_queue_.pop();
    }

    return pkt;
}
//---------------------------------------------------------------------------
void PacketQueue::Wakeup()
{
    cond_.notify_one();
    return;
}
//---------------------------------------------------------------------------

}//namespace net
