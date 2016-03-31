//---------------------------------------------------------------------------
#ifndef LINUX_BASE_JSON_VALUE_H_
#define LINUX_BASE_JSON_VALUE_H_
//---------------------------------------------------------------------------
#include "../share_inc.h"
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
        TYPE_BOOLEAN,   //布伦
        TYPE_NULL,      //空
    };

    //类型定义
    typedef std::vector<Value>              JsonArray;
    typedef std::map<std::string, Value>    JsonPair;
    typedef JsonArray::const_iterator       JsonArrayIter;
    typedef JsonPair::const_iterator        JsonPairIter;

public:
    Value(ValueType type);
    Value(const Value& other);
    Value(Value&& other);
    Value& operator=(const Value& other);
    Value& operator=(Value&& other);
    ~Value();
    
public:
    ValueType type()    { return type_; }

    void set_int    (int64_t value)     { assert(TYPE_INT       == type_); value_.u_int    = value; }
    void set_uint   (uint64_t value)    { assert(TYPE_UINT      == type_); value_.u_uint   = value; }
    void set_boolean(bool value)        { assert(TYPE_BOOLEAN   == type_); value_.u_real   = value; }
    void set_double (double value)      { assert(TYPE_REAL      == type_); value_.u_bool   = value; }

    int64_t     get_int()       { assert(TYPE_INT       == type_);  return value_.u_int; }
    uint64_t    get_uint()      { assert(TYPE_UINT      == type_);  return value_.u_uint; }
    bool        get_boolean()   { assert(TYPE_BOOLEAN   == type_);  return value_.u_real; }
    double      get_double()    { assert(TYPE_REAL      == type_);  return value_.u_bool; }
    char*       get_str()       { assert(TYPE_STRING    == type_);  return value_.u_str; }
    const char* get_str() const { assert(TYPE_STRING    == type_);  return value_.u_str; }
    char*       get_key()       { assert(TYPE_KEY       == type_);  return value_.u_key; }
    const char* get_key() const { assert(TYPE_KEY       == type_);  return value_.u_key; }

    void set_str(const char* value);
    void set_str(const std::string& value);
    void set_key(const char* key);
    void set_key(const std::string& key);

    bool    PairAdd (const std::string& key, const Value& value);
    bool    PairAdd (const char* key, Value&& value);
    bool    PairDel (const std::string& key);
    bool    PairDel (const char* key);
    bool    PairGet (const std::string& key, Value* value);
    bool    PairGet (const char* key, Value* value);
    size_t  PairSize()  { if(0 == pairs_) return 0;  return pairs_->size(); }

    JsonPairIter PairIterBegin()    { return pairs_->begin(); }
    JsonPairIter PairIterEnd()      { return pairs_->end(); }
    
    void            ArrayReserve    (size_t size);
    void            ArraySet        (size_t index, const Value& value);
    void            ArraySet        (size_t index, const Value&& value);
    Value&          ArrayGet        (size_t index);
    const Value&    ArrayGet        (size_t index) const;
    void            ArrayZero       (size_t index);
    size_t          ArraySize       ()  { if(0 == array_) return 0; return array_->size(); }

    JsonArrayIter ArrayIterBegin()  { return array_->begin(); }
    JsonArrayIter ArrayIterEnd()    { return array_->end(); }

private:
    //部分value的数据,有效内容由type_字段指出
    union
    {
        int64_t     u_int;
        uint64_t    u_uint;
        double      u_real;
        bool        u_bool;
        char*       u_str;
        char*       u_key;
    }value_;                                //基本类型
    ValueType                       type_;  //类型
    std::vector<Value>*             array_; //数组
    std::map<std::string, Value>*   pairs_; //对象

private:
    const static Value kValueNull;
};

}//namespace json

}//namespace base
//---------------------------------------------------------------------------
#endif //LINUX_BASE_JSON_VALUE_H_
