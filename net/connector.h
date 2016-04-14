//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_CONNECTOR_H_
#define BASE_LINUX_NET_CONNECTOR_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
#include "inet_address.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class Channel;

class Connector : public std::enable_shared_from_this<Connector>
{
public:
    typedef std::function<void (int sockfd)> CallbackNewConnection;

    Connector(EventLoop* loop, const InetAddress& svr);
    ~Connector();

    void set_callbakc_new_connection_(const CallbackNewConnection& callback)    { callback_new_connection_ = callback; }

    void Start();
    void Restart();
    void Stop();

    const InetAddress& svr_addr() const { return svr_addr_; }

private:
    void StartInLoop();
    void StopInLoop();

    void Connect();
    void Connecting(int sockfd);

    void HandleWrite();
    void HandleError();

    void Retry();
    int  RemoveAndResetChannel();
    void ResetChannel();

private:
    bool                        running_;   //指示是否还在运行(重连期间可能被Stop)
    EventLoop*                  loop_;
    InetAddress                 svr_addr_;
    std::shared_ptr<Channel>    channel_;
    CallbackNewConnection       callback_new_connection_;
    enum
    {
        DISCONNECTED=1,
        CONNECTINTG,
        CONNECTED
    }states_;

    int retry_delay_;

private:
    static const int kMaxRetryDelay = 30 * 1000;
    static const int kInitRetryDelay= 1000;
};

}//net
#endif //BASE_LINUX_NET_CONNECTOR_H_
