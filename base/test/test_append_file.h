#ifndef BASE_LINUX_BASE_TEST_TEST_APPEND_FILE_H_
#define BASE_LINUX_BASE_TEST_TEST_APPEND_FILE_H_

#include "test_base.h"

namespace base
{

namespace test
{

class TestAppendFile : public TestBase
{
public:
    TestAppendFile()
    {
    }
    virtual ~TestAppendFile()
    {
    }

    virtual bool DoTest();
};

}//namespace test

}//namespace test

#endif //BASE_LINUX_BASE_TEST_TEST_APPEND_FILE_H_

