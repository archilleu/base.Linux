//---------------------------------------------------------------------------
#include "test_json.h"
#include "../json/token_reader.h"
#include "../json/json_reader.h"
#include "../json/value.h"
#include "../memory_block.h"
#include "../json/json_writer.h"
#include "../json/value.h"
#include "../function.h"
#include <dirent.h>
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
using namespace base::json;
//---------------------------------------------------------------------------
bool TestJson::DoTest()
{
    //if(false == Test_Value_Base())      return false;
    //if(false == Test_Value_Obj())       return false;
    //if(false == Test_Value_Array())     return false;
    //if(false == Test_CharReader())      return false;
    //if(false == Test_TokenReader())     return false;
    //if(false == Test_Json_KV())         return false;
    //if(false == Test_Json_Array())      return false;
    //if(false == Test_Json_Object())     return false;
    if(false == Test_Json_ArrayObject())return false;
    if(false == Test_json_Format())     return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Value_Base()
{
    {
    //基本操作
    Value value_none;
    value_none.set_type(Value::TYPE_KEY);
    const char* c_str = "key";
    value_none.set_str(c_str);
    MY_ASSERT(value_none.val() == std::string(c_str));

    std::string s_str = "key";
    value_none.set_str(s_str);
    MY_ASSERT(value_none.val() == s_str);
    value_none.set_str(std::move(s_str));
    MY_ASSERT(value_none.val() == std::string(c_str));

    Value value(Value::TYPE_KEY);
    MY_ASSERT(Value::TYPE_KEY == value.type());
    value.set_str(c_str);
    MY_ASSERT(value.val() == std::string(c_str));

    std::string str = "key";
    value.set_str(str);
    MY_ASSERT(value.val() == str);
    value.set_str(std::move(str));
    MY_ASSERT(value.val() == std::string(c_str));

    //复制构造
    Value val_copy(value);
    MY_ASSERT(value.val() == val_copy.val());
    
    //=
    Value val_assg= value;
    MY_ASSERT(value.val() == val_assg.val());

    //move
    Value val_move(std::move(val_copy));
    MY_ASSERT(value.val() == val_move.val());

    //move==
    Value val_move_assg = std::move(val_assg);
    MY_ASSERT(value.val() == val_move_assg.val());
    }

    //string
    {
    //基本操作
    Value value_none;
    value_none.set_type(Value::TYPE_STRING);
    const char* c_str = "string";
    value_none.set_str(c_str);
    MY_ASSERT(value_none.val() == std::string(c_str));

    std::string s_str = "string";
    value_none.set_str(s_str);
    MY_ASSERT(value_none.val() == s_str);
    value_none.set_str(std::move(s_str));
    MY_ASSERT(value_none.val() == std::string(c_str));

    Value value(Value::TYPE_STRING);
    MY_ASSERT(Value::TYPE_STRING == value.type());
    value.set_str(c_str);
    MY_ASSERT(value.val() == std::string(c_str));

    std::string str = "string";
    value.set_str(str);
    MY_ASSERT(value.val() == str);
    value.set_str(std::move(str));
    MY_ASSERT(value.val() == std::string(c_str));

    //复制构造
    Value val_copy(value);
    MY_ASSERT(value.val() == val_copy.val());
    
    //=
    Value val_assg= value;
    MY_ASSERT(value.val() == val_assg.val());

    //move
    Value val_move(std::move(val_copy));
    MY_ASSERT(value.val() == val_move.val());

    //move==
    Value val_move_assg = std::move(val_assg);
    MY_ASSERT(value.val() == val_move_assg.val());
    }

    //int
    {
    //基本操作
    Value value_none;
    value_none.set_type(Value::TYPE_INT);
    int64_t int64 = 0x7FFFFFFFFFFFFFFF;
    value_none.set_int(int64);
    MY_ASSERT(value_none.val() == base::CombineString("%" PRId64 "", int64));
    MY_ASSERT(value_none.get_int() == int64);

    Value value(Value::TYPE_INT);
    MY_ASSERT(Value::TYPE_INT == value.type());
    value.set_int(int64);
    MY_ASSERT(value.val() == base::CombineString("%" PRId64 "", int64));
    MY_ASSERT(value.get_int() == int64);

    //复制构造
    Value val_copy(value);
    MY_ASSERT(value.val() == val_copy.val());
    
    //=
    Value val_assg= value;
    MY_ASSERT(value.val() == val_assg.val());

    //move
    Value val_move(std::move(val_copy));
    MY_ASSERT(value.val() == val_move.val());

    //move==
    Value val_move_assg = std::move(val_assg);
    MY_ASSERT(value.val() == val_move_assg.val());
    }

    //uint
    {
    //基本操作
    Value value_none;
    value_none.set_type(Value::TYPE_UINT);
    uint64_t uint64 = 0xFFFFFFFFFFFFFFFF;
    value_none.set_uint(uint64);
    MY_ASSERT(value_none.val() == base::CombineString("%" PRIu64 "", uint64));
    MY_ASSERT(value_none.get_uint() == uint64);

    Value value(Value::TYPE_UINT);
    MY_ASSERT(Value::TYPE_UINT == value.type());
    value.set_uint(uint64);
    MY_ASSERT(value.val() == base::CombineString("%" PRIu64 "", uint64));
    MY_ASSERT(value.get_uint() == uint64);

    //复制构造
    Value val_copy(value);
    MY_ASSERT(value.val() == val_copy.val());
    
    //=
    Value val_assg= value;
    MY_ASSERT(value.val() == val_assg.val());

    //move
    Value val_move(std::move(val_copy));
    MY_ASSERT(value.val() == val_move.val());

    //move==
    Value val_move_assg = std::move(val_assg);
    MY_ASSERT(value.val() == val_move_assg.val());
    }

    //real
    {
    //基本操作
    Value value_none;
    value_none.set_type(Value::TYPE_REAL);
    double real = 12345678.123456;
    value_none.set_double(real);
    MY_ASSERT(value_none.val() == base::CombineString("%f", real));
    MY_ASSERT(value_none.get_double() == real);

    Value value(Value::TYPE_REAL);
    MY_ASSERT(Value::TYPE_REAL == value.type());
    value.set_double(real);
    MY_ASSERT(value.val() == base::CombineString("%f", real));
    MY_ASSERT(value.get_double() == real);

    //复制构造
    Value val_copy(value);
    MY_ASSERT(value.val() == val_copy.val());
    
    //=
    Value val_assg= value;
    MY_ASSERT(value.val() == val_assg.val());

    //move
    Value val_move(std::move(val_copy));
    MY_ASSERT(value.val() == val_move.val());

    //move==
    Value val_move_assg = std::move(val_assg);
    MY_ASSERT(value.val() == val_move_assg.val());
    }

    //boolean true
    {
    //基本操作
    Value value_none;
    value_none.set_type(Value::TYPE_BOOLEAN);
    bool boolean = true;
    value_none.set_boolean(boolean);
    MY_ASSERT(value_none.val() == "true");
    MY_ASSERT(value_none.get_boolean() == boolean);

    Value value(Value::TYPE_BOOLEAN);
    MY_ASSERT(Value::TYPE_BOOLEAN == value.type());
    value.set_boolean(boolean);
    MY_ASSERT(value.val() == "true");
    MY_ASSERT(value.get_boolean() == boolean);

    //复制构造
    Value val_copy(value);
    MY_ASSERT(value.val() == val_copy.val());
    
    //=
    Value val_assg= value;
    MY_ASSERT(value.val() == val_assg.val());

    //move
    Value val_move(std::move(val_copy));
    MY_ASSERT(value.val() == val_move.val());

    //move==
    Value val_move_assg = std::move(val_assg);
    MY_ASSERT(value.val() == val_move_assg.val());
    }

    //boolean false
    {
    //基本操作
    Value value_none;
    value_none.set_type(Value::TYPE_BOOLEAN);
    bool boolean = false;
    value_none.set_boolean(boolean);
    MY_ASSERT(value_none.val() == "false");
    MY_ASSERT(value_none.get_boolean() == boolean);

    Value value(Value::TYPE_BOOLEAN);
    MY_ASSERT(Value::TYPE_BOOLEAN == value.type());
    value.set_boolean(boolean);
    MY_ASSERT(value.val() == "false");
    MY_ASSERT(value.get_boolean() == boolean);

    //复制构造
    Value val_copy(value);
    MY_ASSERT(value.val() == val_copy.val());
    
    //=
    Value val_assg= value;
    MY_ASSERT(value.val() == val_assg.val());

    //move
    Value val_move(std::move(val_copy));
    MY_ASSERT(value.val() == val_move.val());

    //move==
    Value val_move_assg = std::move(val_assg);
    MY_ASSERT(value.val() == val_move_assg.val());
    }

    //null
    {
    //基本操作
    Value value_none;
    value_none.set_type(Value::TYPE_NULL);
    MY_ASSERT(value_none.val() == "null");

    Value value(Value::TYPE_NULL);
    MY_ASSERT(value.val() == "null");

    //复制构造
    Value val_copy(value);
    MY_ASSERT(value.val() == val_copy.val());
    
    //=
    Value val_assg= value;
    MY_ASSERT(value.val() == val_assg.val());

    //move
    Value val_move(std::move(val_copy));
    MY_ASSERT(value.val() == val_move.val());

    //move==
    Value val_move_assg = std::move(val_assg);
    MY_ASSERT(value.val() == val_move_assg.val());
    }

    //初始即构造
    {
    std::string v_str = "str";
    Value v_str1("str");
    Value v_str2(std::string("str"));
    Value v_str3(std::move(v_str));
    MY_ASSERT(Value::TYPE_STRING == v_str1.type());
    MY_ASSERT(Value::TYPE_STRING == v_str2.type());
    MY_ASSERT(Value::TYPE_STRING == v_str3.type());
    MY_ASSERT("str" == v_str1.val());
    MY_ASSERT("str" == v_str2.val());
    MY_ASSERT("str" == v_str3.val());

    int64_t int64 = 1;
    Value v_int(int64);
    MY_ASSERT(Value::TYPE_INT == v_int.type());
    MY_ASSERT(1 == v_int.get_int());

    uint64_t uint64 = 1;
    Value v_uint(uint64);
    MY_ASSERT(Value::TYPE_UINT == v_uint.type());
    MY_ASSERT(1 == v_uint.get_uint());

    bool boolean = true;
    Value v_boolean(boolean);
    MY_ASSERT(Value::TYPE_BOOLEAN == v_boolean.type());
    MY_ASSERT(true == v_boolean.get_boolean());

    double real = 1;
    Value v_real(real);
    MY_ASSERT(Value::TYPE_REAL == v_real.type());

    }
    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Value_Obj()
{
    //object
    Value value_none;
    value_none.set_type(Value::TYPE_OBJECT);
    MY_ASSERT(value_none.type() == Value::TYPE_OBJECT);
    MY_ASSERT(value_none.val() == "null");

    Value value(Value::TYPE_OBJECT);
    MY_ASSERT(value.type() == Value::TYPE_OBJECT);
    MY_ASSERT(value.val() == "null");

    //初始状态
    MY_ASSERT(0 == value.PairSize());
    MY_ASSERT(false == value.PairDel("none"));
    MY_ASSERT(false == value.PairDel(std::string("none")));

    Value v;
    MY_ASSERT(false == value.PairGet("none", &v));
    MY_ASSERT(false == value.PairGet(std::string("none"), &v));
    size_t count = 0;
    for(auto iter=value.PairIterBegin(); value.PairIterEnd()!=iter; ++iter)
        count ++;
    MY_ASSERT(0 == count);

    //添加pair
    {
    Value v1(Value::TYPE_INT);
    v1.set_int(-1);
    Value v2(Value::TYPE_UINT);
    v2.set_uint(1);
    Value v3(Value::TYPE_REAL);
    v3.set_double(1.1);
    Value v4(Value::TYPE_BOOLEAN);
    v4.set_boolean(true);
    Value v5(Value::TYPE_BOOLEAN);
    v5.set_boolean(false);
    Value v6(Value::TYPE_NULL);
    
    value.PairAdd("key1", std::move(v1));
    value.PairAdd(std::string("key2"), std::move(v2));
    std::string key3 = "key3";
    value.PairAdd(std::move(key3), std::move(v3));
    value.PairAdd("key4", std::move(v4));
    value.PairAdd(std::string("key5"), std::move(v5));
    std::string key6 = "key6";
    value.PairAdd(std::move(key6), std::move(v6));
    }

    size_t size = value.PairSize();
    MY_ASSERT(6 == size);
    
    //检查添加的元素
    {
    Value v1;
    MY_ASSERT(true == value.PairGet("key1", &v1));
    MY_ASSERT(v1.get_int() == -1);

    Value v2;
    MY_ASSERT(true == value.PairGet(std::string("key2"), &v2));
    MY_ASSERT(v2.get_uint() == 1);

    Value v3;
    MY_ASSERT(true == value.PairGet("key3", &v3));
    MY_ASSERT((v3.get_double() - 1.1)<0.01);

    Value v4;
    MY_ASSERT(true == value.PairGet("key4", &v4));
    MY_ASSERT(v4.get_boolean() == true);

    Value v5;
    MY_ASSERT(true == value.PairGet("key5", &v5));
    MY_ASSERT(v5.get_boolean() == false);

    Value v6;
    MY_ASSERT(true == value.PairGet("key6", &v6));
    MY_ASSERT(v6.type() == Value::TYPE_NULL);

    for(auto iter=value.PairIterBegin(); value.PairIterEnd()!=iter; ++iter)
    {
        std::cout << "key:" << iter->first << " value:" << iter->second.val() << std::endl;
    }

    //添加同样key值的会被覆盖
    Value v1_cover(Value::TYPE_INT);
    v1_cover.set_int(2);
    value.PairAdd("key1", std::move(v1_cover));
    Value v1_cover_get;
    MY_ASSERT(true ==  value.PairGet("key1", &v1_cover_get));
    MY_ASSERT(v1_cover_get.get_int()==2);

    //删除key值
    MY_ASSERT(value.PairDel("key1"));
    MY_ASSERT(value.PairDel("key2"));
    MY_ASSERT(value.PairDel("key3"));
    MY_ASSERT(value.PairDel("key4"));
    MY_ASSERT(value.PairDel("key5"));
    MY_ASSERT(value.PairDel("key6"));

    MY_ASSERT(0 == value.PairSize());
    for(auto iter=value.PairIterBegin(); value.PairIterEnd()!=iter; ++iter)
    {
        //没有元素
        assert(0);
    }
    }

    //拷贝构造等等
    {
    Value val(Value::TYPE_OBJECT);
    Value v1(Value::TYPE_OBJECT);
    value.PairAdd("key", std::move(v1));

    Value val_copy(val);
    Value val_assg = value;
    Value val_move_copy(std::move(val_copy));
    Value val_move_asg = std::move(val_assg);
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Value_Array()
{
    //array
    Value value_none;
    value_none.set_type(Value::TYPE_ARRAY);
    MY_ASSERT(value_none.type() == Value::TYPE_ARRAY);
    MY_ASSERT(value_none.val() == "null");

    Value value(Value::TYPE_ARRAY);
    MY_ASSERT(value.type() == Value::TYPE_ARRAY);
    MY_ASSERT(value.val() == "null");

    //初始状态
    MY_ASSERT(0 == value.ArraySize());
    for(auto iter=value.ArrayIterBegin(); value.ArrayIterEnd()!=iter; ++iter)
        assert(0);

    //添加value
    {
    Value v1(Value::TYPE_INT);
    v1.set_int(-1);
    Value v2(Value::TYPE_UINT);
    v2.set_uint(1);
    Value v3(Value::TYPE_REAL);
    v3.set_double(1.1);
    Value v4(Value::TYPE_BOOLEAN);
    v4.set_boolean(true);
    Value v5(Value::TYPE_BOOLEAN);
    v5.set_boolean(false);
    Value v6(Value::TYPE_NULL);
    
    value.ArrayAdd(std::move(v1));
    value.ArrayAdd(std::move(v2));
    value.ArrayAdd(std::move(v3));
    value.ArrayAdd(std::move(v4));
    value.ArrayAdd(std::move(v5));
    value.ArrayAdd(std::move(v6));
    }

    size_t size = value.ArraySize();
    MY_ASSERT(6 == size);
    
    //检查添加的元素
    {
    Value v1 = value.ArrayGet(0);
    MY_ASSERT(v1.get_int() == -1);

    Value v2 = value.ArrayGet(1);
    MY_ASSERT(v2.get_uint() == 1);

    Value v3 = value.ArrayGet(2);
    MY_ASSERT((v3.get_double() - 1.1)<0.01);

    Value v4 = value.ArrayGet(3);
    MY_ASSERT(v4.get_boolean() == true);

    Value v5 = value.ArrayGet(4);
    MY_ASSERT(v5.get_boolean() == false);

    Value v6 = value.ArrayGet(5);
    MY_ASSERT(v6.type() == Value::TYPE_NULL);
    }

    for(auto iter=value.ArrayIterBegin(); value.ArrayIterEnd()!=iter; ++iter)
    {
        std::cout <<  "value:" << iter->val() << std::endl;
    }

    //添加同样key值的会被覆盖
    Value v1_cover(Value::TYPE_INT);
    v1_cover.set_int(2);
    value.ArraySet(0, std::move(v1_cover));
    Value v1_cover_get = value.ArrayGet(0);
    MY_ASSERT(v1_cover_get.get_int()==2);

    //删除值
    for(size_t i=0; i<value.ArraySize(); i++)
    {
        value.ArrayZero(i);
        MY_ASSERT(value.ArrayGet(i).val() == "null");
    }

    //重置大小
    value.ArrayResize(0);
    value.ArrayResize(10);
    
    for(size_t i=0; i<10; i++)
    {
        Value v(Value::TYPE_INT);
        v.set_int(i);
        value.ArraySet(i, std::move(v));
    }

    MY_ASSERT(10 == value.ArraySize());

    for(size_t i=0; i<value.ArraySize(); i++)
    {
        MY_ASSERT(static_cast<int64_t>(i) == value.ArrayGet(i).get_int());
    }

    //拷贝构造等等
    {
    Value val(Value::TYPE_ARRAY);
    Value v1(Value::TYPE_ARRAY);
    value.ArrayAdd(std::move(v1));

    Value val_copy(val);
    Value val_assg = value;
    Value val_move_copy(std::move(val_copy));
    Value val_move_asg = std::move(val_assg);
    }

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
        (void)c;
    }

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
    json::TokenReader reader3;
    json::TokenReader reader4;
    std::string str         = " normal\"";
    std::string str_spe     = "\\b\\f\\n\\r\\t \\\\ `1~!@#$%^&*()_+-={':[,]}|;.</>? / & \\/ \\\" \"";
    std::string str_err1    = "a";
    std::string str_err3    = "a";
    reader1.set_dat(MemoryBlock(str.data(), str.length()));
    reader2.set_dat(MemoryBlock(str_err1.data(), str_err1.length()));
    reader3.set_dat(MemoryBlock(str_spe.data(), str_spe.length()));
    reader4.set_dat(MemoryBlock(str_err3.data(), str_err3.length()));

    std::string key;
    bool err_code = reader1.ReadString(key);
    MY_ASSERT(true == err_code);
    MY_ASSERT(key == " normal");

    key.clear();
    err_code = reader3.ReadString(key);
    MY_ASSERT(true == err_code);
    std::cout << "---" << std::endl;
    std::cout << key <<std::endl;
    std::cout << "---" << std::endl;
    //MY_ASSERT(key == str_spe);

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
    
    std::string num;
    json::Value::ValueType num_type;
    bool err_code = reader1.ReadNumber(num, num_type);
    MY_ASSERT(true == err_code);
    MY_ASSERT(json::Value::TYPE_UINT == num_type);
    MY_ASSERT(n1 == num);

    err_code = reader2.ReadNumber(num, num_type);
    MY_ASSERT(true == err_code);
    MY_ASSERT(json::Value::TYPE_INT == num_type);
    MY_ASSERT(n2 == num);

    err_code = reader3.ReadNumber(num, num_type);
    MY_ASSERT(true == err_code);
    MY_ASSERT(json::Value::TYPE_REAL == num_type);
    MY_ASSERT(n3 == num);

    err_code = reader4.ReadNumber(num, num_type);
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
bool TestJson::Test_Json_KV()
{
    //null kv
    {
    json::JsonReader reader;
    std::string str = "{}";

    std::string j_str;
    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //string
    {
    json::JsonReader reader;
    std::string str = "{\"key1\":\"value2\",\"key2\":\"value2\",\"key3\":\"value3\"}";

    std::string j_str;
    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //number
    {
    json::JsonReader reader;
    std::string str = "{\"key1\":-1,\"key2\":123,\"key3\":1245.5678}";
    
    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);
    
    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //boolean
    {
    json::JsonReader reader;
    std::string str = "{\"key1\":true,\"key2\":false}";
    
    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);
    
    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //null
    {
    json::JsonReader reader;
    std::string str = "{\"key1\":null}";
    
    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);
    
    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Json_Array()
{
    //nll
    {
    json::JsonReader reader;
    std::string str = "[]";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //杂七杂八
    {
    json::JsonReader reader;
    std::string str = "[1,\"value2\",null,true,false,1.1,-1]";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    
    }

    //string
    {
    json::JsonReader reader;
    std::string str = "{\"key\":[\"value1\",\"value2\"]}";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //number
    {
    json::JsonReader reader;
    std::string str = "{\"key\":[1,-1,1234.5678]}";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //boolean
    {
    json::JsonReader reader;
    std::string str = "{\"key\":[true,false]}";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //null
    {
    json::JsonReader reader;
    std::string str = "{\"key\":[null,null,null]}";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Json_Object()
{
    //string
    {
    json::JsonReader reader;
    std::string str = "{\"key\":{\"objkey1\":\"obj_value1\",\"objkey2\":\"obj_value2\"}}";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //nmber
    {
    json::JsonReader reader;
    std::string str = "{\"key\":{\"objkey1\":1,\"objkey2\":-1,\"objkey3\":1234.5678}}";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //boolean
    {
    json::JsonReader reader;
    std::string str = "{\"key\":{\"objkey1\":true,\"objkey2\":false}}";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    //null
    {
    json::JsonReader reader;
    std::string str = "{\"key\":{\"objkey1\":null,\"objkey2\":null}}";

    json::Value root;
    bool err_code = reader.Parse(str, &root);
    MY_ASSERT(true == err_code);

    std::string j_str = json::JsonWriter::ToString(root);
    assert(j_str == str);
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_Json_ArrayObject()
{
    //测试一个文件夹下的数据
    
    DIR* dir = opendir("./test_file");
    if(0 == dir)
        return false;
    
    std::list<std::string> pass_file_list;
    std::list<std::string> fail_file_list;
    for(struct dirent* ent=readdir(dir); 0!=ent; ent=readdir(dir))
    {
        if(DT_REG & ent->d_type)
        {
            if('f' == ent->d_name[0])
                fail_file_list.push_back(ent->d_name);
            else if('p' == ent->d_name[0])
                pass_file_list.push_back(ent->d_name);
        }
    }
    closedir(dir);

    //错误的文件
    for(auto i=fail_file_list.begin(); i!=fail_file_list.end(); ++i)
    {
        json::Value         root;
        json::JsonReader    reader;
        bool err_code = reader.ParseFile("./test_file/"+*i, &root);
        MY_ASSERT(false == err_code);
    }

    //正确的文件
    for(auto i=pass_file_list.begin(); i!=pass_file_list.end(); ++i)
    {
        json::Value         root;
        json::JsonReader    reader;
        bool err_code = reader.ParseFile("./test_file/"+*i, &root);
        MY_ASSERT(true == err_code);

        std::string str = json::JsonWriter::ToString(root, true);
        std::string save_file = "./test_file/"+std::string("b.")+*i;
        FileDelete(save_file);
        SaveFile(save_file, str.data(), str.length());
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestJson::Test_json_Format()
{
    std::string         file    = "./test_file/tostring.json";
    json::Value         root;
    json::JsonReader    reader;
    bool err_code = reader.ParseFile(file, &root);
    if(false == err_code)
        return false;

    std::string format_str = json::JsonWriter::ToString(root, true);
    std::cout << format_str << std::endl; 
    return true;
}
//---------------------------------------------------------------------------
