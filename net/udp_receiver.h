//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_UDP_RECEIVER_H_
#define BASE_LINUX_NET_UDP_RECEIVER_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class InetAddress;
class EventLoop;
class DatagramSocket;
class Channel;
class PacketQueue;

class UDPReceiver
{
public:
    //当接收到错误消息时,对象本身已经不可信,需要由调用者重新生成该对象
    typedef std::function<void (void)>  CallbackError;

    UDPReceiver(EventLoop* owner_loop, DatagramSocket* sock, PacketQueue* pkt_queue);
    ~UDPReceiver();

    void set_callback_error(const CallbackError& callback)  { callback_error_ = callback; }
    void Start();
    void Stop();

    void set_pkt_size(int size)  { pkt_size_ = size; }

private:
    void HandleRead (base::Timestamp rcv_time);
    void HandleError();

private:
    EventLoop*                  owner_loop_;
    PacketQueue*                pkt_queue_;
    int                         pkt_size_;
    DatagramSocket*             socket_;
    std::shared_ptr<Channel>    channel_;

    CallbackError   callback_error_;
};

}//namespace net
#endif// BASE_LINUX_NET_UDP_RECEIVER_H_
