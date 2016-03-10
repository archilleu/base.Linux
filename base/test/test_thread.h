//---------------------------------------------------------------------------
#ifndef LINUX_BASE_TEST_TEST_THREAD_H_
#define LINUX_BASE_TEST_TEST_THREAD_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../thread.h"
//---------------------------------------------------------------------------
namespace base
{

namespace test
{

class TestThread : public TestBase
{
public:
    TestThread()
    :   thread1_(std::bind(&TestThread::Thread_Func1, this)),
        thread2_(std::bind(&TestThread::Thread_Func1, this))
    {
    }
    ~TestThread()
    {}

    virtual bool DoTest();

private:
    bool Test_None();
    bool Test_ParamNone();
    bool Test_Param1();
    bool Test_ParamClass();

private:
    void Thread_Func1();
    
    uint64_t count_;
    std::mutex mutex_;

    Thread thread1_;
    Thread thread2_;
};

}// namespace test

}// namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_TEST_TEST_THREAD_H_
