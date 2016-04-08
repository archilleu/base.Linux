//---------------------------------------------------------------------------
#ifndef LINUX_BASE_TEST_TEST_SEMAPHORE_H_
#define LINUX_BASE_TEST_TEST_SEMAPHORE_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../semaphore.h"
#include "../thread.h"
//---------------------------------------------------------------------------
namespace base
{

namespace test
{

class TestSemaphore : public TestBase
{
public:
    TestSemaphore()
    :   sem_unnamed_(0),
        sem_named1_("/test_name", 0),
        sem_named2_("/test_name", 0),
        thread_unnamed1_(std::bind(&TestSemaphore::OnThreadUnmamed1, this)),
        thread_unnamed2_(std::bind(&TestSemaphore::OnThreadUnmamed2, this)),
        thread_named1_(std::bind(&TestSemaphore::OnThreadNamed1, this)),
        thread_named2_(std::bind(&TestSemaphore::OnThreadNamed2, this))
    {
        return;
    }
    virtual ~TestSemaphore()
    {
        return;
    }

    virtual bool DoTest();

public:
    bool Test_Unnamed();
    bool Test_Named();


private:
    void OnThreadUnmamed1();
    void OnThreadUnmamed2();

    void OnThreadNamed1();
    void OnThreadNamed2();

private:
    Semaphore sem_unnamed_;
    Semaphore sem_named1_;
    Semaphore sem_named2_;

    Thread thread_unnamed1_;
    Thread thread_unnamed2_;

    Thread thread_named1_;
    Thread thread_named2_;
};

}//namespace test

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_TEST_TEST_SEMAPHORE_H_
