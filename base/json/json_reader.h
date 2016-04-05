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

    //解析字符串,以\0作为结束符
    bool Parse(const std::string& str, Value* root);
    bool Parse(const char* str, Value* root);
    
    //解析内存数据,可以不用\0为结束符
    bool Parse(MemoryBlock&& dat, Value* root);

    //解析JSON格式文件
    bool ParseFile(const std::string& path, Value* root);
    bool ParseFile(const char* path, Value* root);

private:
    bool _Parse(Value* root);

    //当前是否有对应的状态
    bool HasStatus(int status)  { return (cur_status_ & status); }

private:
    bool CaseStatusObjectBegin  (std::stack<Value>& parse_stack);
    bool CaseStatusObjectKey    (std::stack<Value>& parse_stack);
    bool CaseStatusObjectValue  (std::stack<Value>& parse_stack, int type);
    bool CaseStatusObjectEnd    (std::stack<Value>& parse_stack);
    bool CaseStatusArrayBegin   (std::stack<Value>& parse_stack);
    bool CaseStatusArrayValue   (std::stack<Value>& parse_stack, int type);
    bool CaseStatusArrayEnd     (std::stack<Value>& parse_stack);
    bool CaseStatusSepColon     (std::stack<Value>& parse_stack);
    bool CaseStatusSepComma     (std::stack<Value>& parse_stack);
    bool CaseStatusDocumentEnd  (std::stack<Value>& parse_stack);

private:
    TokenReader token_reader_;
    int         cur_status_;    //当前解析状态,即下一个期待的字符类型

private:
    //解析过程中的状态(期待的下一个字符类型)
    static const int kEXP_STATUS_OBJECT_BEGIN = 0x0001;     //期待对象
    static const int kEXP_STATUS_OBJECT_KEY   = 0x0002;     //期待对象key值
    static const int kEXP_STATUS_OBJECT_VALUE = 0x0004;     //期待对象Value值
    static const int kEXP_STATUS_OBJECT_END   = 0x0008;     //期待对象结束
    static const int kEXP_STATUS_ARRAY_BEGIN  = 0x0010;     //期待数组开始
    static const int kEXP_STATUS_ARRAY_VALUE  = 0x0020;     //期待数组Value
    static const int kEXP_STATUS_ARRAY_END    = 0x0040;     //期待数组结束
    static const int kEXP_STATUS_SEP_COLON    = 0x0080;     //期待:
    static const int kEXP_STATUS_SEP_COMMA    = 0x0100;     //期待,
    static const int kEXP_STATUS_DOCUMENT_END = 0x0200;     //期待JSON解析结束

protected:
    DISALLOW_COPY_AND_ASSIGN(JsonReader);
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_JSON_READER_H_
