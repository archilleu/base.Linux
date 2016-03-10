//---------------------------------------------------------------------------
#ifndef LINUX_BASE_TEST_TEST_TIMESTAMP_H_
#define LINUX_BASE_TEST_TEST_TIMESTAMP_H_
//---------------------------------------------------------------------------
#include "test_base.h"
//---------------------------------------------------------------------------
namespace base
{

namespace test
{

class TestTimestamp : public TestBase
{
public:
    TestTimestamp()
    {
    }
    virtual ~TestTimestamp()
    {
    }

    virtual bool DoTest();
};

}//namespace test

}//namespace base
//---------------------------------------------------------------------------
#endif// BASE_LINUX_BASE_TEST_TEST_TIMESTAMP_H_
