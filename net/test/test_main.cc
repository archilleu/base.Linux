//---------------------------------------------------------------------------
#include "test_main.h"
#include "test_base.h"
#include "test_event_loop.h"
#include "test_event_loop_thread.h"
#include "test_inet_address.h"
#include "test_socket.h"
#include "test_acceptor.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
TestMain::TestMain()
{
#define TEST_ADD(TypeName)   test_obj_list_[#TypeName]=std::shared_ptr<TestBase>(dynamic_cast<TestBase*>(new TypeName))

    //TEST_ADD(TestEventLoop);
    //TEST_ADD(TestEventLoopThread);
    //TEST_ADD(TestInetAddress);
    //TEST_ADD(TestSocket);
    TEST_ADD(TestAcceptor);

#undef TEST_ADD
}
//---------------------------------------------------------------------------
TestMain::~TestMain()
{
    test_obj_list_.clear();
}
//---------------------------------------------------------------------------
void TestMain::StartTest()
{
    std::cout << "验证没有bug后请使用 查看是否有内存问题: valgrind --tool=memcheck --leak-check=full ./test" << std::endl;

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
