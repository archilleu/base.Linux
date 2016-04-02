//---------------------------------------------------------------------------
#ifndef LINUX_BASE_JSON_VALUE_H_
#define LINUX_BASE_JSON_VALUE_H_
//---------------------------------------------------------------------------
#include "../share_inc.h"
#include <inttypes.h>
//---------------------------------------------------------------------------
namespace base
{

namespace json
{

class Value
{
public:
    //Value的类型
    enum ValueType
    {
        TYPE_KEY = 1,
        TYPE_OBJECT,    //对象类型
        TYPE_ARRAY,     //数组类型
        TYPE_STRING,    //空对象
        TYPE_INT,       //有符号整形
        TYPE_UINT,      //无符号整形
        TYPE_REAL,      //浮点型
        TYPE_NUMBER,    //数字类型统称
        TYPE_BOOLEAN,   //布伦
        TYPE_NULL,      //空
    };

    //类型定义
    typedef std::vector<Value>              JsonArray;
    typedef std::map<std::string, Value>    JsonPair;
    typedef JsonArray::const_iterator       JsonArrayIter;
    typedef JsonPair::const_iterator        JsonPairIter;

public:
    Value();
    Value(ValueType type);
    Value(const Value& other);
    Value(Value&& other);
    Value& operator=(const Value& other);
    Value& operator=(Value&& other);
    ~Value();
    
public:
    ValueType type() const
    { return type_; }

    void set_type(ValueType type);

    //设置值
    void set_int(int64_t value)
    {
        assert(TYPE_INT == type_); 
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%" PRId64 "", value);
        val_ = buffer;
    }

    void set_uint(uint64_t value)
    {
        assert(TYPE_UINT == type_); 
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%" PRIu64 "", value);
        val_ = buffer;
    }

    void set_boolean(bool value)
    {
        assert(TYPE_BOOLEAN == type_); 
        value ? val_ = "true": val_ = "false";
    }

    void set_double(double value)
    {
        assert(TYPE_INT == type_); 
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%f", value);
        val_ = buffer;
    }

    //注意:仅供json_reader调用
    void set_number(const std::string& number, ValueType num_type)
    {
       val_ = number;
       type_= num_type;
    }

    //注意:仅供json_reader调用
    void set_number(const char* number, ValueType num_type)
    {
       val_ = number;
       type_= num_type;
    }

    //注意:仅供json_reader调用
    void set_number(std::string&& number, ValueType num_type)
    {
       val_ = std::move(number);
       type_= num_type;
    }

    void set_str(const char* str)
    {
        if((TYPE_STRING!=type_) && (TYPE_KEY!=type_))
        {
            assert(0); 
        }
        val_ = str;
    }

    void set_str(const std::string& str)
    {
        if((TYPE_STRING!=type_) && (TYPE_KEY!=type_))
        {
            assert(0);
        }
        val_ = str;
    }

    void set_str(std::string&& str)
    {
        if((TYPE_STRING!=type_) && (TYPE_KEY!=type_))
        {
            assert(0);
        }
        val_ = str;
    }

    //获取值
    std::string&        val()       { return val_; }
    const std::string&  val() const { return val_; } 

    bool    PairAdd (const std::string& key, Value&& value);
    bool    PairAdd (std::string&& key, Value&& value);
    bool    PairAdd (const char* key, Value&& value);
    bool    PairDel (const std::string& key);
    bool    PairDel (const char* key);
    bool    PairGet (const std::string& key, Value* value);
    bool    PairGet (const char* key, Value* value);
    size_t  PairSize()
    { if(0 == pairs_) return 0;  return pairs_->size(); }

    JsonPairIter PairIterBegin  () const    { return pairs_->begin(); }
    JsonPairIter PairIterEnd    () const    { return pairs_->end(); }
    
    void            ArrayReserve    (size_t size);
    void            ArraySet        (size_t index, const Value& value);
    void            ArraySet        (size_t index, const Value&& value);
    Value&          ArrayGet        (size_t index);
    const Value&    ArrayGet        (size_t index) const;
    void            ArrayAdd        (const Value& value);
    void            ArrayAdd        (Value&& value);
    void            ArrayZero       (size_t index);
    size_t          ArraySize       ()
    { if(0 == array_) return 0; return array_->size(); }

    JsonArrayIter ArrayIterBegin()  const   { return array_->begin(); }
    JsonArrayIter ArrayIterEnd()    const   { return array_->end(); }

private:
    ValueType                       type_;  //类型
    std::string                     val_;   //值
    std::vector<Value>*             array_; //数组
    std::map<std::string, Value>*   pairs_; //对象

private:
    const static Value kValueNull;
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_VALUE_H_
