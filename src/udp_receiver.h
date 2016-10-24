//---------------------------------------------------------------------------
#ifndef NET_UDP_RECEIVER_H_
#define NET_UDP_RECEIVER_H_
//---------------------------------------------------------------------------
#include <functional>
#include <memory>
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
    using CallbackError = std::function<void (void)>;

    UDPReceiver(EventLoop* owner_loop, DatagramSocket* sock, PacketQueue* pkt_queue);
    ~UDPReceiver();

    void set_callback_error(const CallbackError& callback)  { callback_error_ = callback; }
    void Start();
    void Stop();

    void set_pkt_size(int size)  { pkt_size_ = size; }

private:
    void HandleRead (uint64_t rcv_time);
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
#endif// NET_UDP_RECEIVER_H_
