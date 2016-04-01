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
    return _ToString(value); 
}
//---------------------------------------------------------------------------
std::string JsonWriter::_ToString(const Value& value)
{
    std::string str;
    int type = value.type();
    if(Value::TYPE_OBJECT == type)
    {
        str.push_back('{');

        for(auto iter=value.PairIterBegin(); iter!=value.PairIterEnd(); ++iter)
        {
            //添加key
            str.push_back('\"');
                str += iter->first;
            str.push_back('\"');

            //添加value
            switch(iter->second.type())
            {
                case Value::TYPE_OBJECT:
                case Value::TYPE_ARRAY:
                    _ToString(iter->second);
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

                default:
                    assert(0);
             }

            str.push_back(',');
        }
        if(',' == str.back())
            str.pop_back();

        str.push_back('}');

        return str;
    }

    if(Value::TYPE_ARRAY == type)
    {
        str.push_back('[');

        for(auto iter=value.ArrayIterBegin(); value.ArrayIterEnd()!=iter; ++iter)
        {
            //添加value
            switch(iter->type())
            {
                case Value::TYPE_OBJECT:
                case Value::TYPE_ARRAY:
                    _ToString(*iter);
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

                default:
                    assert(0);
            }
            
            str.push_back(',');
        }
        if(',' == str.back())
            str.pop_back();

        str.push_back(']');

        return str;
    }

    assert(0);
    return str;
}
//---------------------------------------------------------------------------
}//namespace json

}//namespace base
