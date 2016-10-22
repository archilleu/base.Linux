//---------------------------------------------------------------------------
#ifndef NET_CONNECTOR_H_
#define NET_CONNECTOR_H_
//---------------------------------------------------------------------------
#include <memory>
#include "inet_address.h"
#include "timer_task_id.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop;
class Channel;

class Connector : public std::enable_shared_from_this<Connector>
{
public:
    using CallbackNewConnection = std::function<void (int sockfd)>;

    Connector(EventLoop* loop, const InetAddress& svr);
    Connector(const Connector&) =delete;
    Connector& operator=(const Connector&) =delete;
    ~Connector();

    void set_callbakc_new_connection_(CallbackNewConnection&& callback)    { callback_new_connection_ = std::move(callback); }

    void Start();   //thread safe
    void Restart(); //must call in current loop thread
    void Stop();    //thread safe

    const InetAddress& svr_addr() const { return svr_addr_; }

private:
    void StartInLoop();
    void StopInLoop();

    void Connect();
    void Connecting(int sockfd);

    void HandleWrite();
    void HandleError();

    void Retry(int fd);
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
    TimerTaskId timer_task_id_;

private:
    static const int kMaxRetryDelay = 30 * 1000;
    static const int kInitRetryDelay= 1000;
};

}//net
#endif //NET_CONNECTOR_H_
