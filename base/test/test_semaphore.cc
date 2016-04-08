//---------------------------------------------------------------------------
#include "test_semaphore.h"
#include "../semaphore.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestSemaphore::DoTest()
{
//    if(false == Test_Unnamed()) return false;
    if(false == Test_Named())   return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestSemaphore::Test_Unnamed()
{
    //illegal
    {
    base::Semaphore sema;
    sema.Uninitalize();

    //sema.Post();
    //MY_ASSERT(Semaphore::WAIT_ERROR == sema.Wait())
    //MY_ASSERT(Semaphore::WAIT_ERROR == sema.Wait(1));
    //MY_ASSERT(Semaphore::WAIT_ERROR == sema.TryWait());
    }

    {
    Semaphore sema;
    sema.Initialize();
    sema.Post();
    MY_ASSERT(Semaphore::WAIT_SUCESS == sema.Wait());
    MY_ASSERT(Semaphore::WAIT_TIMEOUT == sema.Wait(3));
    MY_ASSERT(Semaphore::WAIT_BUSY == sema.TryWait());
    sema.Uninitalize();
    }

    {
    sem_unnamed_.Initialize();
    sem_unnamed_.Initialize();
    thread_unnamed1_.Start();
    thread_unnamed2_.Start();
    thread_unnamed1_.Join();
    thread_unnamed2_.Join();
    sem_unnamed_.Uninitalize();
    sem_unnamed_.Uninitalize();
    std::cout << "post wait success" << std::endl;
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestSemaphore::Test_Named()
{
    //illegal
    {
    base::Semaphore sema("/my test", 1);
    sema.Uninitalize();

    //sema.Post();
    //MY_ASSERT(Semaphore::WAIT_ERROR == sema.Wait())
    //MY_ASSERT(Semaphore::WAIT_ERROR == sema.Wait(1));
    //MY_ASSERT(Semaphore::WAIT_ERROR == sema.TryWait());

    }

    {
    sem_named1_.Initialize();
    sem_named2_.Initialize();
    sem_named1_.Post();
    MY_ASSERT(Semaphore::WAIT_SUCESS == sem_named1_.Wait());
    MY_ASSERT(Semaphore::WAIT_TIMEOUT == sem_named1_.Wait(3));
    MY_ASSERT(Semaphore::WAIT_BUSY == sem_named1_.TryWait());
    thread_named1_.Start();
    thread_named2_.Start();
    thread_named1_.Join();
    thread_named2_.Join();
    sem_named1_.Uninitalize();
    sem_named2_.Uninitalize();
    std::cout << "post wait success" << std::endl;
    }
    return true;
}
//---------------------------------------------------------------------------
void TestSemaphore::OnThreadUnmamed1()
{
    for(int i=0; i<5; i++)
    {
        sem_unnamed_.Wait();
        sleep(1);
        std::cout << "wait success" << std::endl;
    }

    for(int i=0; i<5; i++)
    {
        sem_unnamed_.Wait(10);
        sleep(1);
        std::cout << "time wait success" << std::endl;
    }

    assert(Semaphore::WAIT_BUSY == sem_unnamed_.TryWait());
    return;
}
//---------------------------------------------------------------------------
void TestSemaphore::OnThreadUnmamed2()
{
    for(int i=0; i<10; i++)
    {
        sem_unnamed_.Post();
        sleep(1);
        std::cout << "post success" << std::endl;
    }

    return;
}
//---------------------------------------------------------------------------
void TestSemaphore::OnThreadNamed1()
{
    for(int i=0; i<5; i++)
    {
        sem_named1_.Wait();
        sleep(1);
        std::cout << "wait success" << std::endl;
    }

    for(int i=0; i<5; i++)
    {
        sem_named1_.Wait(10);
        sleep(1);
        std::cout << "time wait success" << std::endl;
    }

    assert(Semaphore::WAIT_BUSY == sem_named1_.TryWait());
    return;
}
//---------------------------------------------------------------------------
void TestSemaphore::OnThreadNamed2()
{
    for(int i=0; i<10; i++)
    {
        sem_named2_.Post();
        sleep(1);
        std::cout << "post success" << std::endl;
    }

    return;
}
//---------------------------------------------------------------------------
