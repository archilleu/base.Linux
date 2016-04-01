//---------------------------------------------------------------------------
#include "test_json.h"
#include "../json/token_reader.h"
#include "../json/json_reader.h"
#include "../json/value.h"
#include "../memory_block.h"
#include "../json/json_writer.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestJson::DoTest()
{
    if(false == Test_Value())       return false;
    if(false == Test_CharReader())  return false;
    if(false == Test_TokenReader()) return false;
    if(false == Test_Json())        return false;
    if(false == Test_Normal())      return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Value()
{
    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_CharReader()
{
    json::CharReader char_reader;
    std::string str =   "{               \
                            \"a\":\"b\"     \
                         }";
    MemoryBlock mb(str.data(), str.length());
    char_reader.set_dat(std::move(mb));

    for(char c=char_reader.Peek(); true==char_reader.HasMore(); c=char_reader.Next())
    {
        std::cout << c;
    }
    std::cout << std::endl;

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_TokenReader()
{
    enum
    {
        T_DOCUMENT_END = 1,   // 分析结束
        T_OBJECT_BEGIN,       // obj开始
        T_OBJECT_END,         // obj结束
        T_ARRAY_BEGIN,        // array开始
        T_ARRAY_END,          // array结束
        T_SEP_COLON,          // 分隔符 :
        T_SEP_COMMA,          // 分隔符 ,
        T_STRING,             // 字符串
        T_BOOLEAN,            // 布尔值
        T_NUMBER,             // 数值
        T_NUL,                // 空
        T_INVALID             // 非法的字符
    };
    std::string end         = "";
    std::string obj_begin   = "{";
    std::string obj_end     = "}";
    std::string array_begin = "[";
    std::string array_end   = "]";
    std::string colon       = ":";
    std::string comma       = ",";
    std::string t_str       = "\"";
    std::string t_boolean   = "t";
    std::string t_num1      = "-";
    std::string t_num2      = "1";
    std::string t_null      = "n";

    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(end.data(), end.size()));
    MY_ASSERT(json::TokenReader::TokenType::DOCUMENT_END == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(obj_begin.data(), obj_begin.size()));
    MY_ASSERT(json::TokenReader::TokenType::OBJECT_BEGIN == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(obj_end.data(), obj_end.size()));
    MY_ASSERT(json::TokenReader::TokenType::OBJECT_END == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(array_begin.data(), array_begin.size()));
    MY_ASSERT(json::TokenReader::TokenType::ARRAY_BEGIN == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(array_end.data(), array_end.size()));
    MY_ASSERT(json::TokenReader::TokenType::ARRAY_END == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(colon.data(), colon.size()));
    MY_ASSERT(json::TokenReader::TokenType::SEP_COLON == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(comma.data(), comma.size()));
    MY_ASSERT(json::TokenReader::TokenType::SEP_COMMA == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(t_str.data(), t_str.size()));
    MY_ASSERT(json::TokenReader::TokenType::STRING == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(t_boolean.data(), t_boolean.size()));
    MY_ASSERT(json::TokenReader::TokenType::BOOLEAN == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(t_num1.data(), t_num1.size()));
    MY_ASSERT(json::TokenReader::TokenType::NUMBER == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(t_num2.data(), t_num2.size()));
    MY_ASSERT(json::TokenReader::TokenType::NUMBER == reader.ReadNextToken());
    }
    
    {
    json::TokenReader reader;
    reader.set_dat(MemoryBlock(t_null.data(), t_null.size()));
    MY_ASSERT(json::TokenReader::TokenType::NUL== reader.ReadNextToken());
    }

    {
    json::TokenReader reader1;
    json::TokenReader reader2;
    json::TokenReader reader4;
    std::string str         = "normal\"";
    std::string str_err1    = "a";
    std::string str_err3    = "a";
    reader1.set_dat(MemoryBlock(str.data(), str.length()));
    reader2.set_dat(MemoryBlock(str_err1.data(), str_err1.length()));
    reader4.set_dat(MemoryBlock(str_err3.data(), str_err3.length()));

    std::string key;
    bool err_code = reader1.ReadString(key);
    MY_ASSERT(true == err_code);
    MY_ASSERT(key == "normal");

    err_code = reader2.ReadString(key);
    MY_ASSERT(false == err_code);

    err_code = reader4.ReadString(key);
    MY_ASSERT(false == err_code);
    }

    {
    json::TokenReader reader1;
    json::TokenReader reader2;
    json::TokenReader reader3;
    json::TokenReader reader4;
    json::TokenReader reader5;
    std::string n1 = "1234";
    std::string n2 = "-1234";
    std::string n3 = "1234.5678";
    std::string n4 = "a1234";
    std::string n5 = "1a234";
    reader1.set_dat(MemoryBlock(n1.data(), n1.length()));
    reader2.set_dat(MemoryBlock(n2.data(), n2.length()));
    reader3.set_dat(MemoryBlock(n3.data(), n3.length()));
    reader4.set_dat(MemoryBlock(n4.data(), n4.length()));
    reader5.set_dat(MemoryBlock(n5.data(), n5.length()));
    
    json::TokenReader::Number num;
    bool err_code = reader1.ReadNumber(num);
    MY_ASSERT(true == err_code);
    MY_ASSERT(json::TokenReader::Number::TYPE_UINT == num.type_);
    MY_ASSERT(std::stoull(n1) == num.value_.u_uint);

    err_code = reader2.ReadNumber(num);
    MY_ASSERT(true == err_code);
    MY_ASSERT(json::TokenReader::Number::TYPE_INT == num.type_);
    MY_ASSERT(std::stoll(n2) == num.value_.u_int);

    err_code = reader3.ReadNumber(num);
    MY_ASSERT(true == err_code);
    MY_ASSERT(json::TokenReader::Number::TYPE_DOUBLE == num.type_);
    MY_ASSERT(std::stod(n3) == num.value_.u_double);

    err_code = reader4.ReadNumber(num);
    MY_ASSERT(false == err_code);

    // err_code = reader5.ReadNumber(num);
    //MY_ASSERT(false == err_code);
    }

    {
    json::TokenReader reader1;
    json::TokenReader reader2;
    json::TokenReader reader3;
    json::TokenReader reader4;
    std::string t1 = "true";
    std::string t2 = "false";
    std::string t3 = "taaa";
    std::string t4 = "faaa";
    reader1.set_dat(MemoryBlock(t1.data(), t1.length()));
    reader2.set_dat(MemoryBlock(t2.data(), t2.length()));
    reader3.set_dat(MemoryBlock(t3.data(), t3.length()));
    reader4.set_dat(MemoryBlock(t4.data(), t4.length()));

    bool result;
    bool err_code = reader1.ReadBoolean(result);
    MY_ASSERT(err_code == true);
    MY_ASSERT(result == true);

    err_code = reader2.ReadBoolean(result);
    MY_ASSERT(err_code == true);
    MY_ASSERT(result == false);

    err_code = reader3.ReadBoolean(result);
    MY_ASSERT(err_code == false);

    err_code = reader4.ReadBoolean(result);
    MY_ASSERT(err_code == false);
    }

    {
    json::TokenReader reader1;
    json::TokenReader reader2;
    json::TokenReader reader3;
    std::string n1 = "null";
    std::string n2 = "naul";
    std::string n3 = "nul";
    reader1.set_dat(MemoryBlock(n1.data(), n1.length()));
    reader2.set_dat(MemoryBlock(n2.data(), n2.length()));
    reader3.set_dat(MemoryBlock(n3.data(), n3.length()));

    bool err_code= reader1.ReadNull();
    MY_ASSERT(true == err_code);

    err_code= reader2.ReadNull();
    MY_ASSERT(false == err_code);

    err_code= reader3.ReadNull();
    MY_ASSERT(false == err_code);
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Json()
{
    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Normal()
{
    json::JsonReader reader;
    std::string str = "{\"key\":\"value\"}";

    json::Value* root = 0;
    bool err_code = reader.Parse(str, root);
    MY_ASSERT(true == err_code);

    //json::JsonWriter::ToString(

    return true;
}
//---------------------------------------------------------------------------
