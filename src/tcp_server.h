//---------------------------------------------------------------------------
#ifndef NET_TCP_SERVER_H_
#define NET_TCP_SERVER_H_
//---------------------------------------------------------------------------
#include <vector>
#include "callback.h"
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
    TCPServer(EventLoop* owner_loop, const std::vector<InetAddress>& listen_addr);
    TCPServer(EventLoop* owner_loop, short port);
    TCPServer(const TCPServer&) =delete;
    TCPServer& operator=(const TCPServer&) =delete;
    ~TCPServer();

    void set_callback_connection        (CallbackConnection&& callback)                     { callback_connection_   = std::move(callback); }
    void set_callback_disconnection     (CallbackDisconnection&& callback)                  { callback_disconnection_= std::move(callback); }
    void set_callback_read              (CallbackRead&& callback)                           { callback_read_         = std::move(callback); }
    void set_callback_write_complete    (CallbackWriteComplete&& callback)                  { callback_write_complete_ = std::move(callback); }
    void set_callback_high_water_mark   (CallbackWriteHighWaterMark&& callback, size_t mark)
    {
        callback_high_water_mark_ = std::move(callback);
        mark_ = mark;
    }

    void set_event_loop_nums(int nums);

    void Start();
    void Stop();

    //for debug
    void DumpConnection();

private:
    void OnNewConnection(int clientfd, const InetAddress& client_addr, uint64_t accept_time);

    void OnConnectionRemove        (const TCPConnPtr& conn_ptr);
    void OnConnectionRemoveInLoop  (const TCPConnPtr& conn_ptr);

    void ConnAddList(const TCPConnPtr& conn_ptr);
    void ConnDelList(const TCPConnPtr& conn_ptr);

private:
    CallbackConnection          callback_connection_;
    CallbackDisconnection       callback_disconnection_;
    CallbackRead                callback_read_;
    CallbackWriteComplete       callback_write_complete_;
    CallbackWriteHighWaterMark  callback_high_water_mark_;
    size_t                      mark_;

    EventLoop* owner_loop_;
    size_t next_connect_id_;
    std::vector<std::shared_ptr<Acceptor>> acceptor_;

    std::vector<TCPConnPtr> tcp_conn_list_;            
    size_t tcp_conn_count_;

    std::shared_ptr<EventLoopThreadPool>    loop_thread_pool_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TCP_SERVER_H_
