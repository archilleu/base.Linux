//---------------------------------------------------------------------------
#ifndef LINUX_BASE_TEST_TEST_THREAD_LOCAL_H_
#define LINUX_BASE_TEST_TEST_THREAD_LOCAL_H_
//---------------------------------------------------------------------------
#include "test_base.h"
#include "../thread.h"
#include "../thread_local.h"
//---------------------------------------------------------------------------
namespace base
{

namespace test
{

class TestThreadLocal : public TestBase
{
public:
    TestThreadLocal()
    :   thread1_(std::bind(&TestThreadLocal::OnThread1, this)),
        thread2_(std::bind(&TestThreadLocal::OnThread2, this))
    {
    }
    virtual ~TestThreadLocal()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Normal();

private:
    void OnThread1();
    void OnThread2();

private:
    Thread thread1_;
    Thread thread2_;

    std::set<std::string> set1_;
    std::set<std::string> set2_;
    ThreadLocal<std::set<std::string>> tls_;
};

}//namespace test

}//namespace base
//---------------------------------------------------------------------------
#endif//LINUX_BASE_TEST_TEST_THREAD_LOCAL_H_
