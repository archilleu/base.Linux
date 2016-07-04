//---------------------------------------------------------------------------
#include "sem.h"
//---------------------------------------------------------------------------
namespace base
{

//---------------------------------------------------------------------------
Semaphore::Semaphore(int value)
:   available_(false),
    value_(value),
    sem_(0)
{
    return;
}
//---------------------------------------------------------------------------
Semaphore::Semaphore(const std::string& name, int value)
:   available_(false),
    value_(value),
    name_(name),
    sem_(0)
{
    return; 
}
//---------------------------------------------------------------------------
Semaphore::~Semaphore()
{
    Uninitalize();
}
//---------------------------------------------------------------------------
bool Semaphore::Initialize()
{
    //匿名
    if(true == name_.empty())
    {
        sem_ = new sem_t;
        if(-1 == sem_init(sem_, 0, value_))
        {
            assert(0);
            perror("sem_init");
            delete sem_;
            return false;
        }
    }
    else
    {
        sem_ = sem_open(name_.c_str(), O_CREAT, S_IRWXU|S_IRWXG, value_);
        if(SEM_FAILED == sem_)
        {
            perror("sem_open");
            assert(0);
            return false;
        }
    }

    available_ = true;
    return true;
}
//---------------------------------------------------------------------------
void Semaphore::Uninitalize()
{
    if(false == available_)
        return;

    if(true == name_.empty())
    {
        sem_destroy(sem_);//如果其它进程在等待信号量中,则结果不可预料
        delete sem_;
    }
    else
    {
        sem_close(sem_);
        sem_unlink(name_.c_str());
    }

    available_ = false;
    return;
}
//---------------------------------------------------------------------------
void Semaphore::Post()
{
    int err_code = sem_post(sem_);
    if(0 != err_code)
    {
        perror("sem post:");
        assert(0);
        return;
    }

    return;
}
//---------------------------------------------------------------------------
int Semaphore::Wait(int timeoutS)
{
    //无限等待
    if(-1 == timeoutS)
    {
        do
        {
            if(0 == sem_wait(sem_))
                return WAIT_SUCESS;

        }while(EINTR==errno);//如果被信号打断则重启等待
    }
    else
    {
        struct timespec ts;
        if(-1 == clock_gettime(CLOCK_REALTIME, &ts))
            return WAIT_ERROR; 
        ts.tv_sec += timeoutS;

        do
        {
            int err_code = sem_timedwait(sem_, &ts);
            if(0 == err_code) 
                return WAIT_SUCESS;

            if(ETIMEDOUT == errno)
                return WAIT_TIMEOUT;

        }while(EINTR==errno);//如果被信号打断则重启等待
    }

    return WAIT_ERROR;
}
//---------------------------------------------------------------------------
int Semaphore::TryWait()
{
    int err_code = sem_trywait(sem_);
    if(-1 == err_code) 
    {
        if(EAGAIN == errno)
            return WAIT_BUSY;
    
        return WAIT_ERROR;
    }

    return WAIT_SUCESS;
}
//---------------------------------------------------------------------------
       
}//namespace base
