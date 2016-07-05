//---------------------------------------------------------------------------
#include "thread.h"
#include "sys/syscall.h"
//---------------------------------------------------------------------------
namespace base
{
//---------------------------------------------------------------------------
namespace CurrentThread
{

__thread int            t_cache_tid = 0;
__thread char           t_cache_tid_str[32];
__thread const char*    t_thread_name = "unknow";

void CacheTid()
{
    t_cache_tid = static_cast<int>(::syscall(SYS_gettid));
    snprintf(t_cache_tid_str, sizeof(t_cache_tid_str), "%5d", t_cache_tid);

    return;
}

bool IsMainThread()
{
    return (tid() == ::getpid());//系统主线程id等于进程Id
}

}//namespace CurrentThread
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace 
{

//防止fork导致线程局部存储成员带到新进程中
void AfterFork()
{
    CurrentThread::t_cache_tid  = 0;
    CurrentThread::t_thread_name= "main";
    CurrentThread::tid();

    return;
}
//---------------------------------------------------------------------------
class MainThreadInitialize
{
public:
    MainThreadInitialize()
    {
        CurrentThread::t_thread_name = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &AfterFork);
    }
};MainThreadInitialize init;

}//namespace detail
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
std::atomic<int> Thread::thread_num_;
//---------------------------------------------------------------------------
Thread::Thread(ThreadFunc&& thread_func, const std::string& thread_name)
:   tid_(0),
    name_(thread_name),
    joined_(false),
    started_(false),
    thread_func_(std::move(thread_func))
{
    SetThreadName();
    assert(thread_func_);
    return;
}
//---------------------------------------------------------------------------
Thread::Thread(Thread&& other)
:   tid_(0),
    name_(std::move(other.name_)),
    joined_(false),
    started_(false),
    thread_(std::move(other.thread_)),
    thread_func_(std::move(other.thread_func_))
{
    assert(thread_func_);
    return;
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
    assert(!started_);

    try
    {
        started_= true;
        thread_ = std::thread(std::bind(&Thread::OnThreadFunc, this));
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
    return;
}
//---------------------------------------------------------------------------
void Thread::OnThreadFunc()
{
    tid_                        = CurrentThread::tid();
    CurrentThread::t_thread_name= name_.c_str();

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

    return;
}
//---------------------------------------------------------------------------
void Thread::SetThreadName()
{
    int num = thread_num_++;

    char buf[32];
    snprintf(buf, sizeof(buf), "(no:%d)", num);
    name_ += buf;
}
//---------------------------------------------------------------------------
}//namespace base
