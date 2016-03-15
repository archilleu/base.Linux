//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_TCP_CONNECTION_H_
#define BASE_LINUX_NET_TCP_CONNECTION_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/memory_block.h"
#include "callback.h"
#include "inet_address.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class InetAddress;
class Socket;
class Channel;

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
public:
    typedef std::function<void (const TCPConnectionPtr&)> CalllbackDestroy;

    TCPConnection(EventLoop* owner_loop, const std::string& name, int fd, const InetAddress& local_addr, const InetAddress& peer_addr);
    ~TCPConnection();

    void Initialize();

    const std::string name();
    const InetAddress& local_addr();
    const InetAddress& peer_addr();
    EventLoop* owner_loop();

    void set_callback_connection(const CallbackConnection& callback);
    void set_callback_disconnection(const CallbackDisconnection& callback);
    void set_callback_destroy(const CalllbackDestroy& callback);
    void set_callback_read(const CallbackRead& callback);
    void set_callback_write_complete(const CallbackWriteComplete& callback);
    void set_callback_high_water_mark(const CallbackWriteHighWaterMark& callback);

    //发送数据,线程安全
    void Send(const char* dat, size_t len);
    void Send(const base::MemoryBlock& dat);
    void Send(const base::MemoryBlock&& dat);

    //关闭链接
    void Shutdown();

    //强制断线
    void ForceClose();

    //连接就绪和销毁
    void ConnectionEstablished();
    void ConnectionDestroy();

private:

private:
    EventLoop*                  owner_loop_;
    std::string                 name_;
    InetAddress                 local_addr_;
    InetAddress                 peer_addr_;
    std::shared_ptr<Socket>     socket_;
    std::shared_ptr<Channel>    channel_;

    CallbackConnection          callback_connection_;
    CallbackDisconnection       callback_disconnection_;
    CalllbackDestroy            callback_destroy_;
    CallbackRead                callback_read_;
    CallbackWriteComplete       callback_write_complete_;
    CallbackWriteHighWaterMark  callbakc_higt_water_mark_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_TCP_CONNECTION_H_
