//---------------------------------------------------------------------------
#ifndef NET_TCP_CONNECTION_H_
#define NET_TCP_CONNECTION_H_
//---------------------------------------------------------------------------
#include <atomic>
#include "callback.h"
#include "inet_address.h"
#include "buffer.h"
#include "socket.h"
#include "base/include/noncopyable.h"
#include "base/include/any.h"
//---------------------------------------------------------------------------
namespace net
{

class Channel;
class EventLoop;

class TCPConnection : public base::Noncopyable,
    public std::enable_shared_from_this<TCPConnection>
{
public:
    using CalllbackDestroy = std::function<void (const TCPConnectionPtr&)>;

    TCPConnection(EventLoop* owner_loop, std::string&& name, Socket&& socket,
            InetAddress&& local_addr, InetAddress&& peer_addr);
    ~TCPConnection();

    void set_connection_cb(const ConnectionCallback& cb) { connection_cb_ = cb; } //注意:connection回调不能在回调里面发送数据
    void set_disconnection_cb(const DisconnectionCallback& cb) { disconnection_cb_ = cb; }
    void set_read_cb(const ReadCallback& cb) { read_cb_ = cb; }
    void set_write_complete_cb(const WriteCompleteCallback& cb) { write_complete_cb_ = cb; }
    void set_high_water_mark_cb(const WriteHighWaterMarkCallback& cb, size_t overstock_size)
    {
        high_water_mark_cb_ = cb;
        overstock_size_ = overstock_size;
    }

    //for TCPServer use
    void set_remove_cb(const RemoveCallback& callback) { remove_cb_ = callback; }

    //初始化,绑定事件
    void Initialize();

    //发送数据,线程安全
    void Send(const std::string& dat);
    void Send(const char* dat, size_t len);
    void Send(MemoryBlock&& dat);

    //关闭链接
    void ShutdownWirte();

    //强制断线
    void ForceClose();

    //链接附加自定义数据
    void set_data(const base::any& data) { data_ = data; }
    const base::any& data() const { return data_; }

public:
    const std::string name() const { return name_; }
    const InetAddress& local_addr() const { return local_addr_; }
    const InetAddress& peer_addr() const { return peer_addr_; }

    void AddRequests() { requests_++; }
    int requests() const { return requests_; }

    EventLoop* owner_loop() const { return owner_loop_; }
    const Socket& socket() const { return socket_; }

    std::string GetTCPInfo() const;

    void set_context(const base::any& context) { context_ = context; }
    const base::any& context() const { return context_; }
    base::any* getContext() { return &context_; }

private:
    //以下方法仅供TCPServer调用
    friend class TCPServer;
    friend class TCPClient;
    //连接就绪,这会让该连接可以收发数据
    void ConnectionEstablished();

    //连接销毁
    void ConnectionDestroy();

private:
    void _Send(const char* dat, size_t len);
    //如果上面的Send调用不在本io线程中调用,则转换到本线程发送数据,达到线程安全的目的
    void SendInLoop(const net::MemoryBlock& dat);

    ssize_t _SendMostPossible(const char* dat, size_t len);         //尽可能的发送数据
    void  _SendDatQueueInBuffer (const char* dat, size_t remain);   //未完成发送的数据放入缓存

    //关闭和断开连接都需要在本线程做
    void ShutdownWriteInLoop();
    void ForceCloseInLoop();

    //事件处理
    void HandleRead(uint64_t rcv_time);
    void HandleWrite();
    void HandleError();
    void HandleClose();

private:
    EventLoop* owner_loop_;
    std::string name_;
    InetAddress local_addr_;
    InetAddress peer_addr_;
    enum
    {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        DISCONNECTING
    };
    std::atomic<int> state_;

    //完成请求次数
    std::atomic<int> requests_;

    Buffer buffer_input_;
    Buffer buffer_output_;

    Socket socket_;
    std::shared_ptr<Channel> channel_;

    ConnectionCallback connection_cb_;
    DisconnectionCallback disconnection_cb_;
    ReadCallback read_cb_;
    WriteCompleteCallback write_complete_cb_;
    WriteHighWaterMarkCallback high_water_mark_cb_;
    RemoveCallback remove_cb_;
    size_t overstock_size_;

    //附加数据
    mutable base::any data_;

    //该连接上下文数据
    base::any context_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TCP_CONNECTION_H_
