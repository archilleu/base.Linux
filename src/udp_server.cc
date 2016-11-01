//---------------------------------------------------------------------------
#include "udp_server.h"
#include "event_loop.h"
#include "udp_receiver.h"
#include "udp_worker.h"
#include "packet_queue.h"
#include "net_log.h"
#include "datagram_socket.h"
//---------------------------------------------------------------------------
namespace net
{

//---------------------------------------------------------------------------
UDPServer::UDPServer(const InetAddress& rcv_addr)
:   socket_(new DatagramSocket(rcv_addr)),
    loop_(new EventLoop),
    pkt_queue_(new PacketQueue),
    receiver_(new UDPReceiver(loop_.get(), socket_.get(), pkt_queue_.get())),
    worker_(new UDPWorkder(pkt_queue_.get()))
{
    NetLogger_info("udp server ctor");
    return;
}
//---------------------------------------------------------------------------
UDPServer::~UDPServer()
{
    NetLogger_info("udp server dtor");
    return;
}
//---------------------------------------------------------------------------
void UDPServer::set_callback_rcv_pkt(const CallbackRcvPacket& callback)
{
    worker_->set_callback_rcv_pkt(callback);
    return;
}
//---------------------------------------------------------------------------
void UDPServer::Start(int thread_nums)
{
    NetLogger_off("udp server start");

    receiver_->set_pkt_size(1500);
    receiver_->set_callback_error(std::bind(&UDPServer::OnReceiverError, this));

    worker_->Start(thread_nums);
    receiver_->Start();
    loop_->Loop();

    return;
}
//---------------------------------------------------------------------------
void UDPServer::Stop()
{
    NetLogger_info("udp server stopping");

    receiver_->Stop();
    worker_->Stop();
    loop_->Quit();


    NetLogger_info("udp server stoped");
    return;
}
//---------------------------------------------------------------------------
void UDPServer::Send(const DatagramPacket& pkt)
{
    socket_->Send(pkt);
    return;
}
//---------------------------------------------------------------------------
void UDPServer::Send(const char* dat, size_t len, const InetAddress& to)
{
    socket_->Send(dat, len, to);
    return;
}
//---------------------------------------------------------------------------
void UDPServer::OnReceiverError()
{
    NetLogger_error("reciever error, restart now!!");

    receiver_->Stop();
    receiver_.reset(new UDPReceiver(loop_.get(), socket_.get(), pkt_queue_.get()));
    receiver_->set_pkt_size(1500);
    receiver_->Start();

    NetLogger_warn("reciever restart success!");
    return;
}
//---------------------------------------------------------------------------

}//namespace net
