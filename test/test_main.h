//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_MAIN_H_
#define LINUX_NET_TEST_MAIN_H_
//---------------------------------------------------------------------------
#include <unordered_map>
#include <memory>
//---------------------------------------------------------------------------
namespace net
{

namespace test
{

class TestBase;

class TestMain
{
public:
    TestMain();
    ~TestMain();

    void StartTest();

private:
    typedef std::unordered_map<std::string, std::shared_ptr<TestBase>> TestObj;
    TestObj test_obj_list_;
};

}//namespace test

}//namespace net
//---------------------------------------------------------------------------
#endif// LINUX_NET_TEST_MAIN_H_
