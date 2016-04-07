//---------------------------------------------------------------------------
#ifndef LINUX_BASE_TEST_TEST_BASE_H_
#define LINUX_BASE_TEST_TEST_BASE_H_
//---------------------------------------------------------------------------
#include "../share_inc.h"
//---------------------------------------------------------------------------
#define MY_ASSERT(EXPRESSION) {if(true != (EXPRESSION)) { assert(0); return false;}}
//---------------------------------------------------------------------------
namespace base
{

namespace test
{

class TestBase
{
public:
    TestBase()
    {
    }

    virtual ~TestBase()
    {
    }

    virtual bool DoTest() =0;
};

}// namespace test

}//namespace base
//---------------------------------------------------------------------------
#endif// LINUX_BASE_TEST_TEST_BASE_H_

