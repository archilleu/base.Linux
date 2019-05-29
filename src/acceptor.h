//---------------------------------------------------------------------------
#ifndef NET_ACCEPTOR_H_
#define NET_ACCEPTOR_H_
//---------------------------------------------------------------------------
#include <memory>
#include <functional>
#include "base/include/any.h"
#include "base/include/noncopyable.h"
//---------------------------------------------------------------------------
namespace net
{

class Socket;
class Channel;
class EventLoop;
class InetAddress;
class InetAddressConfig;

class Acceptor : public base::Noncopyable
{
public:
    using NewConnectionCallback =
        std::function<void (Socket&& client, InetAddress&&, uint64_t)>;

    using NewConnectionDataCallback =
        std::function<void (Socket&& client, InetAddressConfig&&, uint64_t)>;

    Acceptor(EventLoop* event_loop, const InetAddress& addr_listen);
    Acceptor(EventLoop* event_loop, const InetAddressConfig& addr_listen);
    ~Acceptor();

public:
    void set_new_conn_cb(const NewConnectionCallback&& cb) { new_conn_cb_ = cb; }
    void set_new_conn_data_cb(const NewConnectionDataCallback&& cb) { new_conn_data_cb_ = cb; }

    void set_data(const base::any& data) { data_ = data; }
    const base::any& data() const { return data_; }

    void Listen();

private:
    void Initialize(EventLoop* event_loop);

    int AcceptConnection(InetAddress& addr_peer);

    void HandleRead(uint64_t rcv_time);

    bool CheckConnection(int fd);

private:
    EventLoop* event_loop_;
    std::shared_ptr<Channel> listen_channel_;
    std::shared_ptr<Socket> listen_socket_;
    NewConnectionCallback new_conn_cb_;
    NewConnectionDataCallback new_conn_data_cb_;

    base::any data_;

    //占位的fd，用于fd打开过多导致链接建立失败，关闭该链接的一个占位fd
    int idle_fd_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_ACCEPTOR_H_
