//---------------------------------------------------------------------------
#ifndef BASE_LINUX_BASE_TEST_TEST_LOG_FILE_H_
#define BASE_LINUX_BASE_TEST_TEST_LOG_FILE_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace base
{

namespace test
{

class TestLogFile : public TestBase
{
public:
    TestLogFile()
    {
    }
    virtual ~TestLogFile()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Illegal();
    bool Test_Size();
    bool Test_Date();
};

}//namespace test

}//namespace base
//---------------------------------------------------------------------------
#endif// BASE_LINUX_BASE_TEST_TEST_LOG_FILE_H_
