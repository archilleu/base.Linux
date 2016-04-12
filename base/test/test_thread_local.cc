//---------------------------------------------------------------------------
#include "test_thread_local.h"
#include "../thread_local.h"
#include "../function.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestThreadLocal::DoTest()
{
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestThreadLocal::Test_Normal()
{
    set1_.insert("1");
    for(size_t i=0; i<1000*100; i++)
    {
        set1_.insert(base::CombineString("%zu", i));
        set2_.insert(base::CombineString("%zu", i+1000*100));
    }

    thread1_.Start();
    thread2_.Start();
    thread1_.Join();
    thread2_.Join();

    tls_.value().insert("aaa");
    MY_ASSERT(*(tls_.value().begin()) == "aaa");
    std::cout << *(tls_.value().begin()) << std::endl;

    return true;
}
//---------------------------------------------------------------------------
void TestThreadLocal::OnThread1()
{
    for(auto iter: set1_)
    {
        tls_.value().insert(iter);
    }

    for(auto iter: set1_)
    {
        tls_.value().erase(iter);
    }

    std::cout << "1:" << tls_.value().size() << std::endl;
    assert(0 == tls_.value().size());
    return;
}
//---------------------------------------------------------------------------
void TestThreadLocal::OnThread2()
{
    for(auto iter: set2_)
    {
        tls_.value().insert(iter);
    }

    for(auto iter: set2_)
    {
        tls_.value().erase(iter);
    }

    std::cout << "2:" << tls_.value().size() << std::endl;
    assert(0 == tls_.value().size());
    return;
}
//---------------------------------------------------------------------------
