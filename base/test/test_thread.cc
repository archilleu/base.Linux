//---------------------------------------------------------------------------
#include "test_thread.h"
//---------------------------------------------------------------------------
namespace base
{

namespace test
{
//---------------------------------------------------------------------------
void ThreadFunc_None()
{
    std::cout << "tid:" << CurrentThread::tid() << std::endl;
    std::cout << "tid str:" << CurrentThread::tid_str() << std::endl;
    std::cout << "thread name::" << CurrentThread::tname() << std::endl;
}
//---------------------------------------------------------------------------
void Thread_Func_ParanNone()
{
    sleep(5);
}
//---------------------------------------------------------------------------
void Thread_Func_ParamTow(int num, int* count)
{
    for(int i=0; i<10; i++)
    {
        num++;
        (*count)++;
    }

    return;
}
//---------------------------------------------------------------------------
bool TestThread::TestThread::DoTest()
{
    if(false == Test_None())        return false;
    if(false == Test_ParamNone())   return false;
    if(false == Test_Param1())      return false;
    if(false == Test_ParamClass())  return false;
    if(false == Test_100())         return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestThread::Test_None()
{
    Thread t1(ThreadFunc_None);
    MY_ASSERT(t1.Start());

    std::cout << "fname:" << t1.name() << std::endl;
    std::cout << "fid:" << t1.tid() << std::endl;;

    t1.Join();
    return true;
}
//---------------------------------------------------------------------------
bool TestThread::Test_ParamNone()
{
    Thread t1(ThreadFunc_None);
    Thread t2(ThreadFunc_None);
    Thread t3(ThreadFunc_None);
    t1.Start();
    t2.Start();
    MY_ASSERT(t3.Start());
    t3.Join();

    return true;
}
//---------------------------------------------------------------------------
bool TestThread::Test_Param1()
{
    int num     = 0;
    int count   = 0;

    Thread t(std::bind(Thread_Func_ParamTow, num, &count));
    MY_ASSERT(t.Start());
    t.Join();

    MY_ASSERT(0 == num);
    MY_ASSERT(10 == count);

    return true;
}
//---------------------------------------------------------------------------
bool TestThread::Test_ParamClass()
{
    count_ = 0;  

    MY_ASSERT(thread1_.Start());
    MY_ASSERT(thread2_.Start());

    fprintf(stderr, "test log time, please wait");
    thread1_.Join();
    thread2_.Join();

    MY_ASSERT(UNIT_GB == count_);
    MY_ASSERT(UNIT_GB == acount_);
    return true;
}
//---------------------------------------------------------------------------
bool TestThread::Test_100()
{
    for(int i=0; i<100; i++)
    {
        Thread t(ThreadFunc_None);
        t.Start();
        t.Join();
    }

    return true;
}
//---------------------------------------------------------------------------
void TestThread::Thread_Func1()
{
    for(uint64_t i=0; i<UNIT_GB/2; i++)
    {
        acount_++;
        std::lock_guard<std::mutex> lock(mutex_);
        count_++;
    }

    return;
}
//---------------------------------------------------------------------------
}//namespace test

}//namespace base
