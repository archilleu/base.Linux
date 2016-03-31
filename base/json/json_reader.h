//---------------------------------------------------------------------------
#ifndef LINUX_BASE_JSON_JSON_READER_H_
#define LINUX_BASE_JSON_JSON_READER_H_
//---------------------------------------------------------------------------
#include "../share_inc.h"
#include "../memory_block.h"
#include "token_reader.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{

class Value;

class JsonReader
{
public:
    JsonReader();
    ~JsonReader();

    bool Parse(const std::string& str, Value* root);
    bool Parse(const char* str, Value* root);
    bool Parse(MemoryBlock&& dat, Value* root);

    bool ParseFile(const std::string& path, Value* root);
    bool ParseFile(const char* path, Value* root);

private:
    bool _Parse(Value* root);

    bool HasStatus(int status)  { return (cur_status_ & status); }

private:
    TokenReader token_reader_;
    int         cur_status_;

private:
    //解析过程中的状态
    static const int kSTATUS_DOCUMENT_END = 0x0000;     //期待JSON解析结束
    static const int kSTATUS_OBJECT_BEGIN = 0x0001;     //期待对象
    static const int kSTATUS_OBJECT_KEY   = 0x0002;     //期待对象key值
    static const int kSTATUS_OBJECT_VALUE = 0x0004;     //期待对象Value值
    static const int kSTATUS_OBJECT_END   = 0x0008;     //期待对象结束
    static const int kSTATUS_ARRAY_BEGIN  = 0x0010;     //期待数组开始
    static const int kSTATUS_ARRAY_VALUE  = 0x0020;     //期待数组Value
    static const int kSTATUS_ARRAY_END    = 0x0040;     //期待数组结束
    static const int kSTATUS_SEP_COLON    = 0x0080;     //期待:
    static const int kSTATUS_SEP_COMMA    = 0x0100;     //期待,
    static const int kSTATUS_SINGLE_VALUE = 0x0200;     //期待单个值

protected:
    DISALLOW_COPY_AND_ASSIGN(JsonReader);
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_JSON_READER_H_
