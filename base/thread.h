//---------------------------------------------------------------------------
#ifndef LINUX_BASE_THREAD_H_
#define LINUX_BASE_THREAD_H_
//---------------------------------------------------------------------------
#include "share_inc.h"
#include <atomic>
//---------------------------------------------------------------------------
namespace base
{
//---------------------------------------------------------------------------
//每个线程都需要自己的副本
namespace CurrentThread
{
    extern __thread int         t_cache_tid;
    extern __thread char        t_cache_tid_str[32];
    extern __thread const char* t_thread_name;

    void CacheTid();

    inline int tid()
    {
        if(__builtin_expect(0==t_cache_tid, 0))
            CacheTid();

        return t_cache_tid;
    }

    inline const char* tid_str()
    {
        return t_cache_tid_str;
    }

    inline const char* thread_name()
    {
        return t_thread_name;
    }

    bool IsMainThread();

}//namespace CurrentThread
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class Thread
{
public:
    typedef std::function<void (void)> ThreadFunc;

    Thread(ThreadFunc&& thread_func, const std::string& thread_name=std::string());
    Thread(Thread&& other);
    Thread(const Thread&) =delete;
    Thread& operator=(const Thread&) =delete;
    Thread& operator=(Thread&&) =delete;
    ~Thread();

    bool Start();
    void Join();

    int                 tid() const     { return tid_; }
    const std::string&  name() const    { return name_; }

private:
    void OnThreadFunc();

    void SetThreadName();

private:
    int         tid_;
    std::string name_;

    bool joined_;
    bool started_;

    std::thread thread_;
    ThreadFunc  thread_func_;

private:
    static std::atomic<int> thread_num_;
};

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_THREAD_H_
