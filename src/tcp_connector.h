//---------------------------------------------------------------------------
#ifndef NET_TCP_CONNECTOR_H_
#define NET_TCP_CONNECTOR_H_
//---------------------------------------------------------------------------
#include <functional>
#include "inet_address.h"
#include "channel.h"
#include "../thirdpart/base/include/noncopyable.h"
#include "timer_id.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;

class TCPConnector : public base::Noncopyable,
                     public std::enable_shared_from_this<TCPConnector>
{
public:
    using NewConnectionCallback = std::function<void (int)>;

    TCPConnector(EventLoop* event_loop, const InetAddress& server_addr);
    ~TCPConnector();

    void set_new_conn_cb(NewConnectionCallback&& cb) { new_conn_cb_ = std::move(cb); }
    const InetAddress& server_addr() { return server_addr_; }

    void Start();
    void Stop();

    //如果连接被断开，则可以重启连接
    void Restart();

private:
    void StartInLoop();
    void StopInLoop();

    void Connect(); //发起连接
    void Connecting(int fd);  //正在连接中，等待服务器ack

    void HandleWrite();
    void HandleError();

    void Retry();

    void RemoveAndResetChannel();
    void ResetChannel();

private:
    bool running_;
    EventLoop* event_loop_;
    InetAddress server_addr_;
    std::shared_ptr<Channel> channel_;
    NewConnectionCallback new_conn_cb_;

    enum
    {
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    }state_;

    int retry_delay_;
    TimerId timer_id_;
};


}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TCP_CONNNECTOR_H_

