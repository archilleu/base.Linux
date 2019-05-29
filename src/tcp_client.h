//---------------------------------------------------------------------------
#ifndef NET_TCP_CLIENT_H_
#define NET_TCP_CLIENT_H_
//---------------------------------------------------------------------------
#include <mutex>
#include <memory>
#include "inet_address.h"
#include "callback.h"
#include "tcp_connector.h"
#include "base/include/noncopyable.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;

class TCPClient : public base::Noncopyable
{
public:
    TCPClient(EventLoop* event_loop, const InetAddress& server_addr, const std::string name);
    ~TCPClient();

    void set_conn_cb(ConnectionCallback&& cb) { conn_cb_ = std::move(cb); }
    void set_read_cb(ReadCallback&& cb) { read_cb_ = std::move(cb); }
    void set_write_complete_cb(WriteCompleteCallback&& cb)
    { write_complete_cb_ = std::move(cb); }
    void set_high_water_mark_cb(WriteHighWaterMarkCallback&& cb, size_t high_water_mark)
    {
        write_highwater_mark_cb_ = std::move(cb);
        high_water_mark_ = high_water_mark;
    }

    void Connect();
    void Disconnect();

    //允许自动重连
    void EnableRetry() { retry_ = true; }

    //不建议使用，因为connection_有可能被reset
    //const TCPConnectionPtr& connection() const { return connection_; }
    TCPConnectionPtr connection()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return connection_;
    }

    EventLoop* event_loop() const { return event_loop_; }
    const std::string& name() const { return name_; }

private:
    void NewConnection(int fd);
    void RemoveConnection(const TCPConnectionPtr& conn);

private:
    EventLoop* event_loop_;
    bool retry_;
    bool connect_;
    int next_conn_id_;
    std::string name_;
    size_t high_water_mark_;

    std::mutex mutex_;
    TCPConnectionPtr connection_;
    std::shared_ptr<TCPConnector> connector_;  //用到shared_from_this,所以用shared_ptr

    ConnectionCallback conn_cb_;
    ReadCallback read_cb_;
    WriteCompleteCallback write_complete_cb_;
    WriteHighWaterMarkCallback write_highwater_mark_cb_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TCP_CLIENT_H_

