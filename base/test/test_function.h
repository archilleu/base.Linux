#ifndef BASE_LINUX_BASE_TEST_TEST_FUNCTION_BLOCK_H_
#define BASE_LINUX_BASE_TEST_TEST_FUNCTION_BLOCK_H_

#include "test_base.h"

namespace base
{

namespace test
{

class TestFunction : public TestBase
{
public:
    TestFunction()
    {
    }
    virtual ~TestFunction()
    {
    }

    virtual bool DoTest();

private:
    bool Test_String();
    bool Test_BinString();
    bool Test_BinChar();
    bool Test_Path();
    bool Test_Document();
};

}//namespace test

}//namespace base

#endif //BASE_LINUX_BASE_TEST_TEST_FUNCTION_BLOCK_H_
