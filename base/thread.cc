//---------------------------------------------------------------------------
#include "thread.h"
#include "sys/syscall.h"
//---------------------------------------------------------------------------
namespace base
{
//---------------------------------------------------------------------------
namespace CurrentThread
{
    __thread uint32_t       t_cache_tid;
    __thread const char*    t_thread_name = "perhabs mainthread";

    void CacheTid()
    {
        t_cache_tid = static_cast<uint32_t>(::syscall(SYS_gettid));
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Thread::Thread(const ThreadFunc& thread_func, const std::string& thread_name)
:   tid_(0),
    name_(thread_name),
    joined_(false),
    started_(false),
    thread_func_(std::move(thread_func))
{
    assert(thread_func_);
}
//---------------------------------------------------------------------------
Thread::Thread(const Thread&& other)
:   tid_(0),
    name_(std::move(other.name_)),
    joined_(false),
    started_(false),
    thread_(std::move(other.thread_)),
    thread_func_(std::move(other.thread_func_))
{
}
//---------------------------------------------------------------------------
Thread::~Thread()
{
    if((started_) && (!joined_))
    {
        thread_.detach();
    }

    return;
}
//---------------------------------------------------------------------------
bool Thread::Start()
{
    if(started_)
        return true;

    try
    {
        started_    = true;
        thread_     = std::thread(std::bind(&Thread::OnThreadFunc, this));
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
        started_ = false;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void Thread::Join()
{
    if(started_)
        thread_.join();

    joined_ = true;
}
//---------------------------------------------------------------------------
void Thread::OnThreadFunc()
{
    tid_                            = CurrentThread::Tid();
    CurrentThread::t_thread_name    = name_.c_str();

    //if(thread_func_)
    {
    try
    {
        thread_func_();
    }
    catch(std::exception& e)
    {
        std::cout << "--------------------------->" << std::endl;
        std::cout << "thread catch exception:"<< e.what() << std::endl;
        std::cout << "--------------------------->" << std::endl;
    }
    }

    return;
}
//---------------------------------------------------------------------------
}//namespace base
