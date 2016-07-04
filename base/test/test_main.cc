//---------------------------------------------------------------------------
#include "test_main.h"
#include "test_base.h"
#include "test_memory_block.h"
#include "test_timestamp.h"
#include "test_function.h"
#include "test_thread.h"
#include "test_append_file.h"
#include "test_log_file.h"
#include "test_mysql_kit.h"
#include "test_mysql_pool.h"
#include "test_md5.h"
//#include "test_semaphore.h"
#include "test_computer_info.h"
#include "test_thread_local.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
TestMain::TestMain()
{
#define TEST_ADD(TypeName)   test_obj_list_[#TypeName]=std::shared_ptr<TestBase>(dynamic_cast<TestBase*>(new TypeName))

    //TEST_ADD(TestMemoryBlock);
    //TEST_ADD(TestTimestamp);
    //TEST_ADD(TestFunction);
    TEST_ADD(TestThread);
    //TEST_ADD(TestAppendFile);
    //TEST_ADD(TestLogFile);
    //TEST_ADD(TestMysqlKit);
    //TEST_ADD(TestMysqlPool);
    //TEST_ADD(TestJson);
    //TEST_ADD(TestMD5);
    //TEST_ADD(TestSemaphore);
    //TEST_ADD(TestComputerInfo);
    //TEST_ADD(TestThreadLocal);

    
#undef TEST_ADD
}
//---------------------------------------------------------------------------
TestMain::~TestMain()
{
    test_obj_list_.clear();
}

void TestMain::StartTest()
{
    std::cout << "验证没有bug后请使用 查看是否有内存问题: valgrind --tool=memcheck --leak-check=full ./test" << std::endl;
    std::cout << "lsof -p pid 验证句柄" << std::endl;

    for(auto iter=test_obj_list_.begin(); test_obj_list_.end()!=iter; ++iter)
    {
        std::cout <<"----------------------------------------------------------->"<< std::endl;
        std::cout << "test:" << iter->first << " start..." << std::endl;
        bool result = iter->second->DoTest();
        //todo count time
        std::cout << "test:" << iter->first << " end, result:" << result << std::endl;
    }
}
//---------------------------------------------------------------------------
