//---------------------------------------------------------------------------
#ifndef NET_TIMER_H_
#define NET_TIMER_H_
//---------------------------------------------------------------------------
#include <atomic>
#include "callback.h"
#include "../thirdpart/base/include/timestamp.h"
//---------------------------------------------------------------------------
namespace net
{

class Timer
{
public:
    Timer(TimerCallback&& cb, base::Timestamp when, int intervalS)
    :   callback_(std::move(cb)),
        expiration_(when),
        interval_(intervalS),
        repeat_(intervalS>0),
        sequence_(s_numCreated_++)
    {
    }

public:
    void Run()
    {
        callback_();
    }
    void Restart(base::Timestamp now);

    base::Timestamp expiration() const  { return expiration_; }
    bool repeat() const { return repeat_; }
    int64_t sequence() const   { return sequence_; }

private:
    const TimerCallback callback_;
    base::Timestamp expiration_;
    const int interval_;
    const bool repeat_;
    const int64_t sequence_;

private:
    static std::atomic_uint64_t s_numCreated_;
};

}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TIMER_H_
