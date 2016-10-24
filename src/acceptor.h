//---------------------------------------------------------------------------
#ifndef NET_ACCEPTOR_H_
#define NET_ACCEPTOR_H_
//---------------------------------------------------------------------------
#include <memory>
#include <functional>
#include "../depend/base/include/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class InetAddress;
class EventLoop;
class Socket;
class Channel;

class Acceptor
{
public:
    using CallbackNewConnection = std::function<void (int, const InetAddress&, uint64_t)>;

    Acceptor(EventLoop* owner_loop, const InetAddress& inet_listen);
    Acceptor(const Acceptor&) =delete;
    Acceptor& operator=(const Acceptor&) =delete;
    ~Acceptor();

    void set_callback_new_connection(const CallbackNewConnection& callback) { callback_new_connection_ = callback; }

    void Listen();

private:
    int AcceptConnection(InetAddress* inet_peer);

    void HandleRead(uint64_t rcv_time);

    bool CheckConnection(int fd);

private:
    EventLoop*                  owner_loop_;
    std::shared_ptr<Socket>     listen_sock_;
    std::shared_ptr<Channel>    channel_listen_;
    int                         idle_fd_;

    CallbackNewConnection callback_new_connection_;
};

}
//---------------------------------------------------------------------------
#endif //NET_ACCEPTOR_H_
