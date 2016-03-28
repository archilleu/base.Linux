//---------------------------------------------------------------------------
#ifndef LINUX_BASE_TEST_TEST_MYSQL_POOL_H_
#define LINUX_BASE_TEST_TEST_MYSQL_POOL_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace base
{

namespace test
{

class TestMysqlPool : public TestBase
{
public:
    TestMysqlPool()
    {
    }
    virtual ~TestMysqlPool()
    {
    }

    virtual bool DoTest();

private:
    bool Test_Normal();
};

}//namespace test

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_TEST_TEST_MYSQL_POOL_H_
