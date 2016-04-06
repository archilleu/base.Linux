//---------------------------------------------------------------------------
#include "value.h"
#include "json_writer.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{
//---------------------------------------------------------------------------
const Value Value::kValueNull = Value(TYPE_NULL);
//---------------------------------------------------------------------------
Value::Value()
:   type_(TYPE_NULL),
    array_(0),
    pairs_(0)
{
    val_ = "null";
    return;
}
//---------------------------------------------------------------------------
Value::Value(ValueType val_type)
:   type_(val_type),
    array_(0),
    pairs_(0)
{
    if(type_ == TYPE_OBJECT)
        pairs_= new JsonPair;

    if(type_ == TYPE_ARRAY)
        array_ = new JsonArray;

    val_ = "null";
    return;
}
//---------------------------------------------------------------------------
Value::Value(const std::string& value)
:   type_(TYPE_STRING),
    array_(0),
    pairs_(0)
{
    val_ = value;
    return;
}
//---------------------------------------------------------------------------
Value::Value(std::string&& value)
:   type_(TYPE_STRING),
    array_(0),
    pairs_(0)
{
    val_ = std::move(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(const char* value)
:   type_(TYPE_STRING),
    array_(0),
    pairs_(0)
{
    val_ = value;
    return;
}
//---------------------------------------------------------------------------
Value::Value(int value)
:   type_(TYPE_INT),
    array_(0),
    pairs_(0)
{
    set_int(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(int64_t value)
:   type_(TYPE_INT),
    array_(0),
    pairs_(0)
{
    set_int(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(uint64_t value)
:   type_(TYPE_UINT),
    array_(0),
    pairs_(0)
{
    set_uint(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(double value)
:   type_(TYPE_REAL),
    array_(0),
    pairs_(0)
{
    set_double(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(bool value)
:   type_(TYPE_BOOLEAN),
    array_(0),
    pairs_(0)
{
    set_boolean(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(const Value& other)
{
    *this = other;
    return;
}
//---------------------------------------------------------------------------
Value::Value(Value&& other)
{
    *this = std::move(other);
    return;
}
//---------------------------------------------------------------------------
Value& Value::operator=(const Value& other)
{
    if(this == &other)
        return *this;

    type_   = other.type_;
    val_    = other.val_;
    array_  = 0;
    pairs_  = 0;

    switch(type_)
    {
        case TYPE_OBJECT:
            pairs_ = new JsonPair;
            *pairs_= *(other.pairs_);
            break;

        case TYPE_ARRAY:
            array_  = new JsonArray;
            *array_ = *(other.array_);
            break;

        case TYPE_KEY:
        case TYPE_STRING:
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_REAL:
        case TYPE_BOOLEAN:
        case TYPE_NULL:
            break;

        default:
            assert(0);
    }

    return *this;
}
//---------------------------------------------------------------------------
Value& Value::operator=(Value&& other)
{
    type_       = other.type_;
    val_        = std::move(other.val_);
    other.val_  = "null";
    array_      = 0;
    pairs_      = 0;

    switch(type_)
    {
        case TYPE_OBJECT:
            pairs_        = other.pairs_;
            other.pairs_  = 0;
            break;

        case TYPE_ARRAY:
            array_      = other.array_;
            other.array_= 0;
            break;

        case TYPE_KEY:
        case TYPE_STRING:
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_REAL:
        case TYPE_BOOLEAN:
        case TYPE_NULL:
            break;

        default:
            assert(0);
    }

    other.type_ = TYPE_NULL;
    return *this;
}
//---------------------------------------------------------------------------
Value::~Value()
{
    if(TYPE_NULL == type_)
    {
        assert(0 == array_);
        assert(0 == pairs_);
        assert("null" == val_);
        return;
    }

    switch(type_)
    {
        case TYPE_OBJECT:
            delete pairs_;
            break;

        case TYPE_ARRAY:
            delete array_;
            break;
        
        case TYPE_KEY:
        case TYPE_STRING:
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_REAL:
        case TYPE_BOOLEAN:
        case TYPE_NULL:
            break;

        default:
            assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
void Value::set_type(ValueType type_val)
{
    assert(TYPE_NULL == type_);

    type_ = type_val;

    if(type_ == TYPE_OBJECT)
        pairs_= new JsonPair;

    if(type_ == TYPE_ARRAY)
        array_ = new JsonArray;

    return;
}
//---------------------------------------------------------------------------
void Value::PairAdd(const std::string& key, const Value& value)
{
    PairAdd(key.c_str(), value);
    return;
}
//---------------------------------------------------------------------------
void Value::PairAdd(std::string&& key, const Value& value)
{
    if(0 == pairs_)
    {
        assert(0);
        return;
    }

    (*pairs_)[std::move(key)] = value;
    return;
}
//---------------------------------------------------------------------------
void Value::PairAdd(const char* key, const Value& value)
{
    if(0 == pairs_)
    {
        assert(0);
        return;
    }

    (*pairs_)[key] = value;
    return;
}
//---------------------------------------------------------------------------
void Value::PairAdd(const std::string& key, Value&& value)
{
    PairAdd(key.c_str(), std::move(value));
    return;
}
//---------------------------------------------------------------------------
void Value::PairAdd(std::string&& key, Value&& value)
{
    if(0 == pairs_)
    {
        assert(0);
        return;
    }

    (*pairs_)[std::move(key)] = std::move(value);
    return;
}
//---------------------------------------------------------------------------
void Value::PairAdd(const char* key, Value&& value)
{
    if(0 == pairs_)
    {
        assert(0);
        return;
    }

    (*pairs_)[key] = std::move(value);
    return;
}
//---------------------------------------------------------------------------
bool Value::PairDel(const std::string& key)
{
    if(0 == pairs_)
    {
        assert(0);
        return false;
    }

    size_t nums = pairs_->erase(key);
    return (1 <= nums);
}
//---------------------------------------------------------------------------
bool Value::PairDel(const char* key)
{
    if(0 == pairs_)
    {
        assert(0);
        return false;
    }

    size_t nums = pairs_->erase(key);
    return (1 <= nums);
}
//---------------------------------------------------------------------------
bool Value::PairGet(const std::string& key, Value* value)
{
    return PairGet(key.c_str(), value);
}
//---------------------------------------------------------------------------
bool Value::PairGet(const char* key, Value* value)
{
    if(0 == pairs_)
    {
        assert(0);
        return false;
    }

    auto iter = pairs_->find(key);
    if(pairs_->end() == iter)
        return false;

    *value = iter->second;
    return true;
}
//---------------------------------------------------------------------------
void Value::ArrayResize(size_t size)
{
    if(0 == array_)
    {
        assert(0);
        return;
    }

    array_->resize(size);
    return;
}
//---------------------------------------------------------------------------
void Value::ArraySet(size_t index, const Value& value)
{
    if(0 == array_)
    {
        assert(0);
        return;
    }

    array_->at(index) = value;
    return;
}
//---------------------------------------------------------------------------
void Value::ArraySet(size_t index, const Value&& value)
{
    if(0 == array_)
    {
        assert(0);
        return;
    }

    array_->at(index) = std::move(value);
    return;
}
//---------------------------------------------------------------------------
Value& Value::ArrayGet(size_t index)
{
    if(0 == array_)
    {
        assert(0);
    }

    return array_->at(index);
}
//---------------------------------------------------------------------------
const Value& Value::ArrayGet(size_t index) const
{
    if(0 == array_)
    {
        assert(0);
        return kValueNull;
    }

    return array_->at(index); 
}
//---------------------------------------------------------------------------
void Value::ArrayAdd(const Value& value)
{
    if(0 == array_)
    {
        assert(0);
        return;
    }

    array_->push_back(value);
    return;
}
//---------------------------------------------------------------------------
void Value::ArrayAdd(Value&& value)
{
    if(0 == array_)
    {
        assert(0);
        return;
    }

    array_->push_back(std::move(value));
    return;
}
//---------------------------------------------------------------------------
void Value::ArrayZero(size_t index)
{
    if(0 == array_)
    {
        assert(0);
        return;
    }

    array_->at(index) = kValueNull;
    return;
}
//---------------------------------------------------------------------------
//Value& Value::operator[](const char* key)
//{
//    assert(TYPE_OBJECT == type_);
//    
//    Value& value = (*pairs_)[key];
//    return value;
//}
//---------------------------------------------------------------------------
Value& Value::operator[](const std::string& key)
{
    assert(TYPE_OBJECT == type_);
    
    Value& value = (*pairs_)[key];
    return value;
}
//---------------------------------------------------------------------------
Value& Value::operator[](size_t index)
{
    assert(TYPE_ARRAY == type_);

    return (*array_)[index];
}
//---------------------------------------------------------------------------
const Value& Value::operator[](size_t index) const
//---------------------------------------------------------------------------
{
    assert(TYPE_ARRAY == type_);

    return (*array_)[index];
}
//---------------------------------------------------------------------------
std::string Value::ToString(bool format)
{
    return JsonWriter::ToString(*this, format);
}
//---------------------------------------------------------------------------
}//namespace json

}//namespace base
