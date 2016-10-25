//---------------------------------------------------------------------------
#ifndef NET_TCP_CONNECTION_H_
#define NET_TCP_CONNECTION_H_
//---------------------------------------------------------------------------
#include <atomic>
#include "callback.h"
#include "inet_address.h"
#include "buffer.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class InetAddress;
class Socket;
class Channel;

class TCPConn : public std::enable_shared_from_this<TCPConn>
{
public:
    using CalllbackDestroy = std::function<void (const TCPConnPtr&)>;

    TCPConn(EventLoop* owner_loop, const std::string& name, int fd, const InetAddress& local_addr, const InetAddress& peer_addr);
    TCPConn(const TCPConn&) =delete;
    TCPConn& operator=(const TCPConn&) =delete;
    ~TCPConn();

    //各种回调
    //注意:connection 回调不能在回调里面发送数据
    void set_callback_connection        (const CallbackConnection& callback)        { callback_connection_      = callback; }
    void set_callback_disconnection     (const CallbackDisconnection& callback)     { callback_disconnection_   = callback; }
    void set_callback_read              (const CallbackRead& callback)              { callback_read_            = callback; }
    void set_callback_write_complete    (const CallbackWriteComplete& callback)     { callback_write_complete_  = callback; }
    void set_callback_high_water_mark   (const CallbackWriteHighWaterMark& callback, size_t overstock_size)
    {
        callback_high_water_mark_   = callback; 
        overstock_size_             = overstock_size;
    }

    //for TCPServer use
    void set_callback_remove (const CallbackRemove& callback) { callback_remove_ = callback; }

    //初始化
    void Initialize();

    //发送数据,线程安全
    void Send(const char* dat, size_t len);
    void Send(const net::MemoryBlock& dat);

    //关闭链接
    void ShutdownWirte();

    //强制断线
    void ForceClose();

    //连接就绪
    void ConnectionEstablished();
    void ConnectionDestroy();

    //自定义附加数据
    std::shared_ptr<void> any_;

    const std::string   name()          { return name_; }
    const InetAddress&  local_addr()    { return local_addr_; }
    const InetAddress&  peer_addr()     { return peer_addr_; }

    EventLoop*                      owner_loop()    { return owner_loop_; }
    const std::shared_ptr<Socket>   socket()        { return socket_; }

    std::string GetTCPInfo() const;

private:
    //如果上面的Send调用不在本io线程中调用,则转换到本线程发送数据,达到线程安全的目的
    void    SendInLoopA             (net::MemoryBlock dat);
    void    SendInLoopB             (const char* dat, size_t len);
    ssize_t _SendMostPossible       (const char* dat, size_t len);      //尽可能的发送数据
    void    _SendDatQueueInBuffer   (const char* dat, size_t remain);   //未完成发送的数据放入缓存

    //关闭和断开连接都需要在本线程做
    void ShutdownWriteInLoop();
    void ForceCloseInLoop();

    //事件处理
    void HandleRead(uint64_t rcv_time);
    void HandleWrite();
    void HandleError();
    void HandleClose();

private:
    EventLoop*  owner_loop_;
    std::string name_;
    InetAddress local_addr_;
    InetAddress peer_addr_;
    enum
    {
        DISCONNECTED=1,
        CONNECTING,
        CONNECTED,
        DISCONNECTING
    };
    std::atomic<int> state_;

    Buffer buffer_input_;
    Buffer buffer_output_;

    std::shared_ptr<Socket>     socket_;
    std::shared_ptr<Channel>    channel_;

    CallbackConnection          callback_connection_;
    CallbackDisconnection       callback_disconnection_;
    CallbackRead                callback_read_;
    CallbackWriteComplete       callback_write_complete_;
    CallbackWriteHighWaterMark  callback_high_water_mark_;
    CallbackRemove              callback_remove_;
    size_t                      overstock_size_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TCP_CONNECTION_H_
