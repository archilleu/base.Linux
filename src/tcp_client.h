//---------------------------------------------------------------------------
#ifndef NET_TCP_CLIENT_H_
#define NET_TCP_CLIENT_H_
//---------------------------------------------------------------------------
#include <mutex>
#include "inet_address.h"
#include "callback.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class Connector;

class TCPClient
{
public:
    TCPClient(EventLoop* loop, const InetAddress& svr, const std::string& name);
    ~TCPClient();

    void set_callback_connection    (CallbackConnection&& callback)     { callback_connection_ = callback; }
    void set_callback_read          (CallbackRead&& callback)           { callback_read_ = callback; }
    void set_callback_write_complete(CallbackWriteComplete&& callback)  { callback_write_complete_ = callback; }

    void Connect();
    void Disconnect();
    void Stop();

    void EnableRetry() { retry_ = true; }

    //程序退出的时候,所有持有该连接的对象都必须释首先放该对象
    TCPConnPtr connection()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return connection_;
    }

    EventLoop* loop() const { return loop_; }

    const std::string& name() const { return name_; }

private:
    void NewConnection(short sockfd);
    void RemoveConnection(const TCPConnPtr& conn_ptr);

private:
    EventLoop*  loop_;
    bool        retry_;
    bool        connect_;
    int         next_conn_id_;
    std::string name_;

    std::mutex                  mutex_;     //保护connection_
    TCPConnPtr                  connection_;
    std::shared_ptr<Connector>  connector_;
    //Before calling shared_from_this() your class needs to be stored in a shared_ptr

    CallbackConnection      callback_connection_;
    CallbackRead            callback_read_;
    CallbackWriteComplete   callback_write_complete_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TCP_CLIENT_H_
