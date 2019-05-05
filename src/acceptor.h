//---------------------------------------------------------------------------
#ifndef NET_ACCEPTOR_H_
#define NET_ACCEPTOR_H_
//---------------------------------------------------------------------------
#include <memory>
#include <functional>
#include "../thirdpart/base/include/noncopyable.h"
#include "../thirdpart/base/include/any.h"
//---------------------------------------------------------------------------
namespace net
{

class InetAddress;
class EventLoop;
class Channel;
class Socket;

class Acceptor : public base::Noncopyable
{
public:
    using NewConnectionCallback =
        std::function<void (Socket&& client, InetAddress&&, uint64_t)>;

    //附带额外数据回调接口,使用set_data设置的数据
    using NewConnectionDataCallback =
        std::function<void (Socket&& client, InetAddress&&, uint64_t, const base::any&)>;

    Acceptor(EventLoop* event_loop, const InetAddress& addr_listen);
    ~Acceptor();

public:
    //2个回调只能设置一个，不带额外数据的优先
    void set_new_conn_cb(const NewConnectionCallback&& cb) { new_conn_cb_ = cb; }
    void set_new_conn_data_cb(const NewConnectionDataCallback&& cb) { new_conn_data_cb_ = cb; }

    void set_data(const base::any& data) { data_ = data; }
    const base::any& data() const { return data_; }

    void Listen();

private:
    int AcceptConnection(InetAddress& addr_peer);

    void HandleRead(uint64_t rcv_time);

    bool CheckConnection(int fd);

private:
    EventLoop* event_loop_;
    std::shared_ptr<Channel> listen_channel_;
    std::shared_ptr<Socket> listen_socket_;
    NewConnectionCallback new_conn_cb_;
    NewConnectionDataCallback new_conn_data_cb_;

    //每个链接附带的数据
    base::any data_;

    //占位的fd，用于fd打开过多导致链接建立失败，关闭该链接的一个占位fd
    int idle_fd_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_ACCEPTOR_H_
