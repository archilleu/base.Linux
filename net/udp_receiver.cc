//---------------------------------------------------------------------------
#include "udp_receiver.h"
#include "inet_address.h"
#include "datagram_socket.h"
#include "channel.h"
#include "packet_queue.h"
#include "net_log.h"
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
    SystemLog_Info("UDPReceiver ctor");

    if(0 > socket_->fd())
    {
        char buffer[128];
        SystemLog_Error("svr socket create filed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));
        assert(0);
        return;
    }

    SystemLog_Debug("rcv buf:%d snd buf:%d", socket_->GetRcvBufferSize(), socket_->GetSndBufferSize());

    socket_->SetReuseAddress();
    socket_->SetSndBufferSize(base::UNIT_KB*256);
    socket_->SetRcvBufferSize(base::UNIT_KB*256);

    channel_->set_callback_read(std::bind(&UDPReceiver::HandleRead, this, std::placeholders::_1));
    channel_->set_callback_error(std::bind(&UDPReceiver::HandleError, this));

    return;
}
//---------------------------------------------------------------------------
UDPReceiver::~UDPReceiver()
{
    SystemLog_Info("UDPReceiver dtor");

    if(!channel_)
        return;

    channel_->DisableAll();
    channel_->Remove();

    return;
}
//---------------------------------------------------------------------------
void UDPReceiver::Start()
{
    SystemLog_Info("Start...");
    channel_->ReadEnable();

    return;
}
//---------------------------------------------------------------------------
void UDPReceiver::Stop()
{
    SystemLog_Info("Stop...");
    channel_->DisableAll();

    //停止读,不停止写,可以继续处理完成的包
    socket_->Shutdown(true, false);

    return;
}
//---------------------------------------------------------------------------
void UDPReceiver::HandleRead(base::Timestamp)
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
    char buffer[128];
    SystemLog_Error("receive failed, errno:%d, msg:%s", errno, strerror_r(errno, buffer, sizeof(buffer)));

    channel_->DisableAll();
    channel_->Remove();

    if(callback_error_)
        callback_error_();

    return;
}
//---------------------------------------------------------------------------
}//namespace net
