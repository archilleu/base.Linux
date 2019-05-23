//---------------------------------------------------------------------------
#ifndef BASE_TEST_TEST_MAIN_H_
#define BASE_TEST_TEST_MAIN_H_
//---------------------------------------------------------------------------
#include <map>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <functional>
#include <set>
#include <mutex>
#include <cstring>
//---------------------------------------------------------------------------
#define TEST_ASSERT(EXPRESSION) \
                {if(true != (EXPRESSION)) {     \
                printf("test error, in function: %s, line: %d\n", __func__, __LINE__);  \
                abort();    \
                }}
//---------------------------------------------------------------------------
namespace base
{

namespace test
{

void TestTitle()
{
    std::cout << "===========================================================>" << std::endl;
    std::cout << "验证没有bug后请使用 查看是否有内存问题: valgrind --tool=memcheck --show-reachable=yes --leak-check=full ./test" << std::endl;
    std::cout << "lsof -p pid 验证句柄" << std::endl;
    std::cout << "===========================================================>" << std::endl;
}

}//namespace test

}//namespace base
//---------------------------------------------------------------------------
#endif// BASE_TEST_TEST_MAIN_H_
