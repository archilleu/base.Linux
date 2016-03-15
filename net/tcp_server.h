//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_TCP_SERVER_H_
#define BASE_LINUX_NET_TCP_SERVER_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "callback.h"
#include "../base/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class InetAddress;
class Acceptor;
class EventLoopThreadPool;

class TCPServer
{
public:
    TCPServer(EventLoop* owner_loop, InetAddress& listen_addr);
    ~TCPServer();

    void set_callback_connection    (const CallbackConnection& callback)    { callback_connection_      = callback; }
    void set_callback_disconnection (const CallbackDisconnection& callback) { callback_disconnection_   = callback; }
    void set_callback_read          (const CallbackRead& callback)          { callback_read_            = callback; }

    void set_event_loop_nums(int nums);

private:
    void OnNewConnection(int clientfd, const InetAddress& client_addr, base::Timestamp accept_time);

    void OnConnectionDestroy        (const TCPConnectionPtr& connection_ptr);
    void OnConnectionDestroyInLoop  (const TCPConnectionPtr& connection_ptr);

private:
    CallbackConnection      callback_connection_;
    CallbackDisconnection   callback_disconnection_;
    CallbackRead            callback_read_;

    EventLoop*                  owner_loop_;
    std::string                 name_;
    size_t                      next_connect_id_;
    std::shared_ptr<Acceptor>   acceptor_;

    std::map<std::string, TCPConnectionPtr> tcp_name_connection_map_;
    std::shared_ptr<EventLoopThreadPool>    loop_thread_pool_;
};

}
#endif //BASE_LINUX_NET_TCP_SERVER_H_
