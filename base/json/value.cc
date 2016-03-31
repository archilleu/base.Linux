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
Value::Value(ValueType val_type)
:   type_(val_type),
    array_(0),
    pairs_(0)
{
    value_.u_int = 0;

    if(type_ == TYPE_OBJECT)
        pairs_= new JsonPair;

    if(type_ == TYPE_ARRAY)
        array_ = new JsonArray;

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

    type_       = other.type_;
    array_      = 0;
    pairs_      = 0;
    value_.u_int= 0;

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

        case TYPE_STRING:
            if(0 == other.value_.u_str)
                break;

            value_.u_str = new char[strlen(other.value_.u_str)+1];
            strcpy(value_.u_str, other.value_.u_str);
            break;

        case TYPE_KEY:
            if(0 == other.value_.u_key)
                break;

            value_.u_key = new char[strlen(other.value_.u_key)+1];
            strcpy(value_.u_key, other.value_.u_key);
            break;
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_REAL:
        case TYPE_BOOLEAN:
        case TYPE_NULL:
            value_ = other.value_;
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
    array_      = 0;
    pairs_      = 0;
    value_.u_int= 0;

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

        case TYPE_STRING:
            value_.u_str        = other.value_.u_str;
            other.value_.u_str  = 0;
            break;

        case TYPE_KEY:
            value_.u_key        = other.value_.u_key;
            other.value_.u_key  = 0;
            break;

        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_REAL:
        case TYPE_BOOLEAN:
        case TYPE_NULL:
            value_ = other.value_;
            break;

        default:
            assert(0);
    }

    other.type_         = TYPE_NULL;
    other.value_.u_int  = 0;
    return *this;
}
//---------------------------------------------------------------------------
Value::~Value()
{
    if(TYPE_NULL == type_)
    {
        assert(0 == value_.u_str);
        assert(0 == value_.u_key);
        assert(0 == array_);
        assert(0 == pairs_);
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

        case TYPE_STRING:
            delete value_.u_str;
            break;

        case TYPE_KEY:
            delete value_.u_key;
            break;

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
void Value::set_str(const char* value)
{
    if(0 != value_.u_str)
        delete value_.u_str;

    value_.u_str = new char[strlen(value)+1];
    strcpy(value_.u_str, value);

    return;
}
//---------------------------------------------------------------------------
void Value::set_str(const std::string& value)
{
    set_str(value.c_str());
    return;
}
//---------------------------------------------------------------------------
void Value::set_key(const char* key)
{
    if(0 != value_.u_key)
        delete value_.u_key;

    value_.u_key = new char[strlen(key)+1];
    strcpy(value_.u_key, key);

    return;
}
//---------------------------------------------------------------------------
void Value::set_key(const std::string& key)
{
    set_key(key.c_str());
    return;
}
//---------------------------------------------------------------------------
bool Value::PairAdd(const std::string& key, const Value& value)
{
    if(0 == pairs_)
    {
        assert(0);
        return false;
    }

    auto pair = pairs_->insert(std::make_pair(key, value));
    return pair.second;
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
