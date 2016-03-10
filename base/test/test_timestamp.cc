//---------------------------------------------------------------------------
#include "test_timestamp.h"
#include "../timestamp.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestTimestamp::DoTest()
{
    std::cout << Timestamp::Zero().Datatime(true)<< std::endl;
    Timestamp   now     = Timestamp::Now();
    uint64_t    seconds = now.Microseconds();

    std::string time1   = now.Time();
    std::string time2   = now.Data();
    std::string time3   = now.Datatime();
    std::string time4   = now.Datatime(true);
    std::string total   = time1 + " "+ time2 + " " + time3 + " " + time4;

    std::cout << time1 << std::endl;
    std::cout << time2 << std::endl;
    std::cout << time3 << std::endl;
    std::cout << time4 << std::endl;
    std::cout << total << std::endl;

    Timestamp t1(seconds);
    assert(time1 == t1.Time());
    assert(time2 == t1.Data());
    assert(time3 == t1.Datatime());
    assert(time4 == t1.Datatime(true));

    MY_ASSERT(time1 == t1.Time());
    MY_ASSERT(time2 == t1.Data());
    MY_ASSERT(time3 == t1.Datatime());
    MY_ASSERT(time4 == t1.Datatime(true));

    t1.AddTime(5);
    std::string time_add = t1.Datatime(true);
    std::cout << "add time:" <<  time_add << std::endl;

    MY_ASSERT(now < t1);
    MY_ASSERT(now != t1);
    MY_ASSERT(t1 > now);

    t1.ReduceTime(5);
    MY_ASSERT(now == t1);

    return true;
}
//---------------------------------------------------------------------------
