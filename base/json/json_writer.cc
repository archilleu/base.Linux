//---------------------------------------------------------------------------
#include "json_writer.h"
#include "value.h"
#include "../function.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{
//---------------------------------------------------------------------------
std::string JsonWriter::ToString(const Value& value)
{
    std::string str;
    ToString(value, str); 

    return str;
}
//---------------------------------------------------------------------------
void JsonWriter::ToString(const Value& value, std::string& str)
{
    int type = value.type();
    if(Value::TYPE_OBJECT == type)
    {
        ObjectToString(value, str);
    }

    if(Value::TYPE_ARRAY == type)
    {
        ArrayToString(value, str);
    }

    return;
}
//---------------------------------------------------------------------------
void JsonWriter::ObjectToString(const Value& value, std::string& str)
{
    str.push_back('{');

    for(auto iter=value.PairIterBegin(); iter!=value.PairIterEnd(); ++iter)
    {
        //添加key
        str.push_back('\"');
            str += iter->first;
        str += "\":";

        //添加value
        switch(iter->second.type())
        {
            case Value::TYPE_OBJECT:
            case Value::TYPE_ARRAY:
                ToString(iter->second, str);
                break;

            case Value::TYPE_STRING:
                str.push_back('\"');
                    str += iter->second.val();
                str.push_back('\"');
                break;

            case Value::TYPE_INT:
            case Value::TYPE_UINT:
            case Value::TYPE_REAL:
            case Value::TYPE_BOOLEAN:
            case Value::TYPE_NULL:
            case Value::TYPE_KEY:
                str += iter->second.val();
                break;

            default:
                assert(0);
         }

        str.push_back(',');
    }
    if(',' == str.back())
        str.pop_back();

    str.push_back('}');

    return;
}
//---------------------------------------------------------------------------
void JsonWriter::ArrayToString(const Value& value, std::string& str)
{
    str.push_back('[');

    for(auto iter=value.ArrayIterBegin(); value.ArrayIterEnd()!=iter; ++iter)
    {
        //添加value
        switch(iter->type())
        {
            case Value::TYPE_OBJECT:
            case Value::TYPE_ARRAY:
                ToString(*iter, str);
                break;

            case Value::TYPE_STRING:
                str.push_back('\"');
                    str += iter->val();
                str.push_back('\"');
                break;

            case Value::TYPE_INT:
            case Value::TYPE_UINT:
            case Value::TYPE_REAL:
            case Value::TYPE_BOOLEAN:
            case Value::TYPE_NULL:
            case Value::TYPE_KEY:
                str += iter->val();
                break;

            default:
                assert(0);
        }
        
        str.push_back(',');
    }
    if(',' == str.back())
        str.pop_back();

    str.push_back(']');

    return;
}
//---------------------------------------------------------------------------
void JsonWriter::NumberToString(const Value& value, std::string& str)
{
}
//---------------------------------------------------------------------------
void JsonWriter::BooleanToString(const Value& value, std::string& str)
{
}
//---------------------------------------------------------------------------
void JsonWriter::NullToString(const Value& value, std::string& str)
{
}
//---------------------------------------------------------------------------
}//namespace json

}//namespace base
