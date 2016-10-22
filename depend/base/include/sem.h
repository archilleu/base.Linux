//---------------------------------------------------------------------------
#ifndef BASE_SEMAPHORE_H_
#define BASE_SEMAPHORE_H_
//---------------------------------------------------------------------------
#include <semaphore.h>
#include <string>
//---------------------------------------------------------------------------
//使用POSIX信号量
//---------------------------------------------------------------------------
namespace base
{

class Semaphore
{
public:
    Semaphore(int value=0);                                             //匿名信号量,用于进程内,进程持续
    Semaphore(const std::string& name, int value=0/*<SEM_VALUE_MAX*/);  //命名信号量,用于进程间,内核持续, 名字格式为/somename, 大小NAME_MAX-4;
    ~Semaphore();
    Semaphore(const Semaphore&) =delete;
    Semaphore& operator=(const Semaphore&) =delete;

    enum
    {
        WAIT_ERROR=1,   //信号量等待错误
        WAIT_SUCESS,    //信号量等待成功
        WAIT_BUSY,      //信号量空
        WAIT_TIMEOUT    //信号量等待超时
    };

    bool Initialize();  //初始化信号量
    void Uninitalize(); //反初始化信号量,如果是匿名信号量,确保反初始化的时候没有其它线程在等待信号量,否则未定义

    void    Post();

    int     Wait    (int timeoutS=-1/*-1==infiniti*/);
    int     TryWait ();

private:
    bool        available_; //信号量是否可用
    int         value_;     //信号量初始值
    std::string name_;      //命名信号量的名字
    sem_t*      sem_;       //信号量描述符
};

}//namespace base
//---------------------------------------------------------------------------
#endif //BASE_SEMAPHORE_H_
