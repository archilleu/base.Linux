//---------------------------------------------------------------------------
#ifndef LINUX_NET_TEST_BASE_H_
#define LINUX_NET_TEST_BASE_H_
//---------------------------------------------------------------------------
#include <string>
#include <set>
#include <map>
#include <queue>
#include <unordered_map>
#include <cassert>
#include <cstring>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <iostream>
#define MY_ASSERT(EXPRESSION) {if(true != (EXPRESSION)) { assert(0); return false;}}
//---------------------------------------------------------------------------
namespace net
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

}//namespace net
//---------------------------------------------------------------------------
#endif// LINUX_NET_TEST_BASE_H_

