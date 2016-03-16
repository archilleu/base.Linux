//---------------------------------------------------------------------------
#include "test_buffer.h"
#include "../buffer.h"
#include "../../base/share_inc.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../base/function.h"
#include "../../base/append_file.h"
#include "../../base/timestamp.h"
//---------------------------------------------------------------------------
using namespace net;
using namespace net::test;
//---------------------------------------------------------------------------
bool TestBuffer::DoTest()
{
    if(false == Test_Illegal()) return false;
    if(false == Test_Normal())  return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestBuffer::Test_Illegal()
{
    Buffer net_buf;

    int err_no;
    MY_ASSERT(0 > net_buf.ReadFd(9527, &err_no));
    
//    net_buf.PeekInt8();
//    net_buf.ReadInt8();

    return true;
}
//---------------------------------------------------------------------------
bool TestBuffer::Test_Normal()
{
    Buffer buffer;

    assert(0 == buffer.ReadableBytes());

    int64_t value_int64 = static_cast<int64_t>(-1);
    int32_t value_int32 = static_cast<int32_t>(-1);
    int16_t value_int16 = static_cast<int16_t>(-1);
    int8_t  value_int8  = static_cast<int8_t>(-1);

    {
    //append 接口
    buffer.AppendInt64(value_int64);
    buffer.AppendInt32(value_int32);
    buffer.AppendInt16(value_int16);
    buffer.AppendInt8(value_int8);

    const char* str = "haha";
    buffer.Append(str, strlen(str));

    assert((sizeof(int64_t) + sizeof(int32_t) + sizeof(int16_t) + sizeof(int8_t) + strlen(str)) == buffer.ReadableBytes());

    //peek接口
    assert(value_int64 == buffer.PeekInt64());
    buffer.RetrieveInt64();

    assert(value_int32 == buffer.PeekInt32());
    buffer.RetrieveInt32();

    assert(value_int16 == buffer.PeekInt16());
    buffer.RetrieveInt16();

    assert(value_int8 == buffer.PeekInt8());
    buffer.RetrieveInt8();

    assert(0 == memcmp(str, buffer.Peek(), strlen(str)));
    assert(0 == strcmp(str, buffer.Peek()));
    buffer.Retrieve(strlen(str));
    assert(0 == buffer.ReadableBytes());
    }

    {
    buffer.AppendInt64(value_int64);
    buffer.AppendInt32(value_int32);
    buffer.AppendInt16(value_int16);
    buffer.AppendInt8(value_int8);

    const char* str = "haha";
    buffer.Append(str, strlen(str));

    assert((sizeof(int64_t) + sizeof(int32_t) + sizeof(int16_t) + sizeof(int8_t)+ strlen(str)) == buffer.ReadableBytes());

    assert(value_int64 == buffer.ReadInt64());
    assert(value_int32 == buffer.ReadInt32());
    assert(value_int16 == buffer.ReadInt16());
    assert(value_int8 == buffer.ReadInt8());

    assert(0 == memcmp(str, buffer.Peek(), strlen(str)));
    buffer.Retrieve(strlen(str));
    assert(0 == buffer.ReadableBytes());
    }

    //添加分配内存 
    {
    std::vector<int> list_int;
    for(int i=0; i<1024; i++)
        list_int.push_back(rand());

    Buffer tcp_buf;
    for(size_t i=0; i<list_int.size(); i++)
    {
        tcp_buf.AppendInt32(list_int[i]);
    }

    assert((list_int.size()*sizeof(int)) == tcp_buf.ReadableBytes());

    //检查添加的数据
    for(size_t i=0; i<list_int.size(); i++)
    {
        assert(list_int[i] == tcp_buf.ReadInt32()); 
    }
    assert(0 == tcp_buf.ReadableBytes());
    }

    {
    Buffer tcp_buf;
    //buffer移动
    for(size_t i=0; i<1024; i++)
    {
        tcp_buf.AppendInt8('a');
    }
    assert(1024 == tcp_buf.ReadableBytes());

    for(size_t i=0; i<1024; i++)
    {
        tcp_buf.ReadInt8();
        tcp_buf.AppendInt8('b');
    }

    for(size_t i=0; i<1024; i++)
    {
        assert('b' == tcp_buf.ReadInt8());
    }

    }

    //{
    //Buffer tcp_buf;

    //std::string path = base::RunPathFileName("test");
    //int fd = ::open(path.c_str(), O_RDONLY);
    //if(0 > fd)
    //  return true;

    //int err_no = 0;
    //for(int len=tcp_buf.ReadFd(fd, &err_no); 0<len; len=tcp_buf.ReadFd(fd, &err_no))
    //{
    //    printf("read size:%d\n", len); 
    //}
    //close(fd);

    //base::AppendFile file;
    //assert(true == file.Open(base::RunPathFileName(base::Timestamp::Now().Datetime())));
    //file.Append(tcp_buf.Peek(), tcp_buf.ReadableBytes());
    //file.Flush();
    //file.Close();
    //}

    return true;
}
//---------------------------------------------------------------------------
