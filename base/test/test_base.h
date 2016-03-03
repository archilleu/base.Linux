#ifndef BASE_LINUX_BASE_TEST_TEST_BASE_H_
#define BASE_LINUX_BASE_TEST_TEST_BASE_H_

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

#endif// BASE_LINUX_BASE_TEST_TEST_BASE_H_

