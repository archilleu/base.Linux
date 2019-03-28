//---------------------------------------------------------------------------
#ifndef NET_TIMER_ID_H_
#define NET_TIMER_ID_H_
//---------------------------------------------------------------------------
#include <cstdint>
//---------------------------------------------------------------------------
namespace net
{

class Timer;

class TimerId
{
public:
    TimerId()
    :   timer_(0),
        sequence_(0)
    {
    }
    TimerId(Timer* timer, uint64_t sequence)
    :   timer_(timer),
        sequence_(sequence)
    {
    }

    Timer* timer() const  { return timer_; }
    uint64_t sequence() const   { return sequence_; }
    
private:
    Timer* timer_;
    uint64_t sequence_;
};


}//namespace net
//---------------------------------------------------------------------------
#endif //NET_TIMER_ID_
