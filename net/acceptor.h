//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_ACCEPTOR_H_
#define BASE_LINUX_NET_ACCEPTOR_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "../base/timestamp.h"
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
    typedef std::function<void (int , const InetAddress&, base::Timestamp)>  CallbackNewConnection;

    Acceptor(EventLoop* owner_loop, const InetAddress& inet_listet);
    ~Acceptor();

    void set_callback_new_connection(const CallbackNewConnection& callback) { callback_new_connection_ = callback; }

    bool Listen();

private:
    int AcceptConnection(InetAddress* inet_peer);

    void HandleRead(base::Timestamp rcv_time);

private:
    EventLoop*                  owner_loop_;
    std::shared_ptr<Socket>     listen_sock_;
    std::shared_ptr<Channel>    channel_listen_;

    CallbackNewConnection callback_new_connection_;

protected:
    DISALLOW_COPY_AND_ASSIGN(Acceptor);
};

}
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_ACCEPTOR_H_
