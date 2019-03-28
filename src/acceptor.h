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

    //附带额外的自定义数据
    using NewConnectionDataCallback =
        std::function<void (Socket&& client, InetAddress&&, uint64_t, const base::any&)>;

    Acceptor(EventLoop* event_loop, const InetAddress& addr_listen);
    ~Acceptor();

public:
    void set_new_conn_cb(const NewConnectionCallback&& cb) { new_conn_cb_ = cb; }
    void set_new_conn_data_cb(const NewConnectionDataCallback&& cb) { new_conn_data_cb_ = cb; }

    void set_config_data(const base::any& config_data) { config_data_ = config_data; }
    const base::any& config_data() const { return config_data_; }

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

    //配置文件
    base::any config_data_;

    //fd打开过多
    int idle_fd_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_ACCEPTOR_H_
