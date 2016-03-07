#ifndef BASE_LINUX_BASE_THREAD_H_
#define BASE_LINUX_BASE_THREAD_H_

#include "share_inc.h"

namespace base
{

//因为每个线程都需要自己的副本,所以这里用namespace替代类
namespace CurrentThread
{
    extern __thread uint32_t    t_cache_tid;
    extern __thread const char* t_thread_name;

    void CacheTid();

    inline uint32_t Tid()
    {
        if(0 == t_cache_tid)
            CacheTid();

        return t_cache_tid;
    }

    inline const char* ThreadName()
    {
        return t_thread_name;
    }

}//namespace CurrentThread


class Thread
{
public:
    typedef std::function<void (void)> ThreadFunc;

    //warning: thread_func inner copy use std::move
    Thread(const ThreadFunc& thread_func, const std::string& thread_name="unkown thread");
    Thread(const Thread&& other);
    Thread(const Thread&) =delete;
    Thread& operator=(const Thread&) =delete;
    Thread& operator=(const Thread&&) =delete;
    ~Thread();

    bool Start();
    void Join();

    uint32_t            tid()   { return tid_; }
    const std::string&  name()  { return name_; }

private:
    void OnThreadFunc();

private:
    uint32_t    tid_;
    std::string name_;

    bool joined_;
    bool started_;

    mutable std::thread thread_;
    ThreadFunc thread_func_;
};

}//namespace base

#endif //BASE_LINUX_BASE_THREAD_H_
