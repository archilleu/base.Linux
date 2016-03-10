//---------------------------------------------------------------------------
#ifndef BASE_LINUX_NET_EVENT_LOOP_H_
#define BASE_LINUX_NET_EVENT_LOOP_H_
//---------------------------------------------------------------------------
#include "../base/share_inc.h"
//---------------------------------------------------------------------------
namespace net
{

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void Quit();

    void AssertInLoopThread();
    bool IsInLoopThread();

private:
    void AbortNotInLoopThread();

private:
    bool        looping_;
    uint32_t    tid_;
    const char* tname_;
protected:
    DISALLOW_COPY_AND_ASSIGN(EventLoop);
};

}//namespace net
//---------------------------------------------------------------------------
#endif //BASE_LINUX_NET_EVENT_LOOP_H_

