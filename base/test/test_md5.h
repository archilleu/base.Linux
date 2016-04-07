//---------------------------------------------------------------------------
#ifndef LINUX_BASE_TEST_TEST_MD5_H_
#define LINUX_BASE_TEST_TEST_MD5_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace base
{

namespace test
{

class TestMD5 : public TestBase
{
public:
    TestMD5()
    {
    }
    virtual ~TestMD5()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Normal();
    bool Test_File();
};

}//name test

}//name base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_TEST_TEST_MD5_H_
