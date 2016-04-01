//---------------------------------------------------------------------------
#include "value.h"
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
            val_ = other.val_;
            break;

        default:
            assert(0);
    }

    return *this;
}
//---------------------------------------------------------------------------
Value& Value::operator=(Value&& other)
{
    type_   = other.type_;
    array_  = 0;
    pairs_  = 0;

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
            val_ = std::move(other.val_);
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
bool Value::PairAdd(const std::string& key, Value&& value)
{
    return PairAdd(key.c_str(), std::move(value));
}
//---------------------------------------------------------------------------
bool Value::PairAdd(const char* key, Value&& value)
{
    if(0 == pairs_)
    {
        assert(0);
        return false;
    }

    auto pair = pairs_->insert(std::make_pair(key, std::move(value)));
    return pair.second;
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
void Value::ArrayReserve(size_t size)
{
    if(0 == array_)
    {
        assert(0);
        return;
    }

    array_->reserve(size);
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

    array_->at(index) = value;
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
}//namespace json

}//namespace base
