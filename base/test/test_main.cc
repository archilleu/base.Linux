#include "test_main.h"
#include "test_base.h"
#include "test_memory_block.h"
#include "test_timestamp.h"

using namespace base;
using namespace base::test;

TestMain::TestMain()
{
#define TEST_ADD(TypeName)   test_obj_list_[#TypeName]=std::shared_ptr<TestBase>(dynamic_cast<TestBase*>(new TypeName))

    TEST_ADD(TestMemoryBlock);
    TEST_ADD(TestTimestamp);

#undef TEST_ADD
}

TestMain::~TestMain()
{
    test_obj_list_.clear();
}

void TestMain::StartTest()
{
    for(auto iter=test_obj_list_.begin(); test_obj_list_.end()!=iter; ++iter)
    {
        std::cout <<"----------------------------------------------------------->"<< std::endl;
        std::cout << "test:" << iter->first << " start..." << std::endl;
        bool result = iter->second->DoTest();
        //todo count time
        std::cout << "test:" << iter->first << " end, result:" << result << std::endl;
    }
}
