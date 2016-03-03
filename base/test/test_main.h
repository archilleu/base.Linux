#ifndef BASE_LINUX_BASE_TEST_TEST_MAIN_H_
#define BASE_LINUX_BASE_TEST_TEST_MAIN_H_

#include "../share_inc.h"

namespace base
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

}//namespace base

#endif// BASE_LINUX_BASE_TEST_TEST_MAIN_H_
