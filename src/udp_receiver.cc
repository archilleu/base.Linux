//---------------------------------------------------------------------------
#include "udp_receiver.h"
#include "inet_address.h"
#include "datagram_socket.h"
#include "channel.h"
#include "packet_queue.h"
#include "net_logger.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
UDPReceiver::UDPReceiver(EventLoop* owner_loop, DatagramSocket* sock, PacketQueue* pkt_queue)
:   owner_loop_(owner_loop),
    pkt_queue_(pkt_queue),
    pkt_size_(1024),
    socket_(sock),
    channel_(new Channel(owner_loop, socket_->fd()))
{
    NetLogger_info("UDPReceiver ctor");

    if(0 > socket_->fd())
    {
        NetLogger_off("svr socket create filed, errno:%d, msg:%s", errno, OSError(errno));
        assert(0);
        return;
    }

    NetLogger_info("rcv buf:%d snd buf:%d", socket_->GetRcvBufferSize(), socket_->GetSndBufferSize());

    socket_->SetReuseAddress();
    socket_->SetSndBufferSize(1024*256);
    socket_->SetRcvBufferSize(1024*256);

    channel_->set_read_cb(std::bind(&UDPReceiver::HandleRead, this, std::placeholders::_1));
    channel_->set_error_cb(std::bind(&UDPReceiver::HandleError, this));

    return;
}
//---------------------------------------------------------------------------
UDPReceiver::~UDPReceiver()
{
    NetLogger_info("UDPReceiver dtor");

    if(!channel_)
        return;

    channel_->DisableAll();
    channel_->Remove();

    return;
}
//---------------------------------------------------------------------------
void UDPReceiver::Start()
{
    NetLogger_info("Start...");
    channel_->EnableReading();

    return;
}
//---------------------------------------------------------------------------
void UDPReceiver::Stop()
{
    NetLogger_info("Stop...");
    channel_->DisableAll();

    //停止读,不停止写,可以继续处理完成的包
    socket_->Shutdown(true, false);

    return;
}
//---------------------------------------------------------------------------
void UDPReceiver::HandleRead(uint64_t)
{
    DatagramPacket pkt = socket_->Receive(pkt_size_);
    if(0 == pkt.effective())
    {
        //读到0表示fd已经close了
        return;
    }

    pkt_queue_->Push(std::move(pkt));
    return;
}
//---------------------------------------------------------------------------
void UDPReceiver::HandleError()
{
    NetLogger_error("receive failed, errno:%d, msg:%s", errno, OSError(errno));

    channel_->DisableAll();
    channel_->Remove();

    if(callback_error_)
        callback_error_();

    return;
}
//---------------------------------------------------------------------------

}//namespace net
