//---------------------------------------------------------------------------
#include "json_reader.h"
#include "value.h"
#include "../function.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{
//---------------------------------------------------------------------------
JsonReader::JsonReader()
{
}
//---------------------------------------------------------------------------
JsonReader::~JsonReader()
{
}
//---------------------------------------------------------------------------
bool JsonReader::Parse(const std::string& str, Value* root)
{
    return Parse(str.c_str(), root);
}
//---------------------------------------------------------------------------
bool JsonReader::Parse(const char* str, Value* root)
{
    token_reader_.set_dat(MemoryBlock(str, strlen(str)));
    return _Parse(root);
}
//---------------------------------------------------------------------------
bool JsonReader::Parse(MemoryBlock&& dat, Value* root)
{
    token_reader_.set_dat(std::move(dat));
    return _Parse(root);
}
//---------------------------------------------------------------------------
bool JsonReader::ParseFile(const std::string& path, Value* root)
{
    return ParseFile(path.c_str(), root);
}
//---------------------------------------------------------------------------
bool JsonReader::ParseFile(const char* path, Value* root)
{
    MemoryBlock dat;
    if(false == base::LoadFile(path, &dat))
        return false;

    return Parse(std::move(dat), root);
}
//---------------------------------------------------------------------------
bool JsonReader::_Parse(Value* root)
{
    //解析栈
    //每当遇到"Key",{Object,[array就生成一个Value入栈
    //每当遇到Object}, array]就把Value出栈,同时如果当前栈底是key值的话,还要把key同对应的object出栈
    std::stack<Value> parse_stack;

    //初始状态
    cur_status_ = kEXP_STATUS_OBJECT_BEGIN | kEXP_STATUS_ARRAY_BEGIN;
    for(;;)
    {
        int token = token_reader_.ReadNextToken();
        switch(token)
        {
            // end
            case TokenReader::TokenType::DOCUMENT_END:
                {
                if(false == CaseStatusDocumentEnd(parse_stack))
                    return false;

                *root = parse_stack.top();
                parse_stack.pop();

                return true;
                }

            // {
            case TokenReader::TokenType::OBJECT_BEGIN:
                {
                if(false == CaseStatusObjectBegin(parse_stack))
                    return false;

                break;
                }

            // }
            case TokenReader::TokenType::OBJECT_END:
                {
                if(false == CaseStatusObjectEnd(parse_stack))
                    return false;

                break;
                }

            // [
            case TokenReader::TokenType::ARRAY_BEGIN:
                {
                if(false == CaseStatusArrayBegin(parse_stack))
                    return false;

                break;
                }

            // ]
            case TokenReader::TokenType::ARRAY_END:
                {
                if(false == CaseStatusArrayEnd(parse_stack))
                    return false;

                break;
                }

            // :
            case TokenReader::TokenType::SEP_COLON:
                {
                if(false == CaseStatusSepColon(parse_stack))
                    return false;

                break;
                }

            // ,
            case TokenReader::TokenType::SEP_COMMA:
                {
                if(false == CaseStatusSepComma(parse_stack))
                    return false;

                break;
                }

            // string
            // 这个时候可以匹配的状态有3个,需要根据上一个匹配状态来决定当前要读取的是key,还是object的值,还是array的值
            // 如果要读取的是key值,则上一次设定的期待状态是KEY,
            // 如果是object的值,则在上一次设定期待的值是object_value,
            // 如果是array的值,则上一次设定期待的值是array_value,否则是失败,返回错误
            case TokenReader::TokenType::STRING:
                {
                if(HasStatus(kEXP_STATUS_OBJECT_KEY))   // key
                {
                    if(false == CaseStatusObjectKey(parse_stack))
                        return false;

                    break;
                }

                if(HasStatus(kEXP_STATUS_OBJECT_VALUE)) // object value
                {
                    if(false == CaseStatusObjectValue(parse_stack, Value::TYPE_STRING))
                        return false;

                    break;
                }

                if(HasStatus(kEXP_STATUS_ARRAY_VALUE))  // array value
                {
                    if(false == CaseStatusArrayValue(parse_stack, Value::TYPE_STRING))
                        return false;

                    break;
                }

                return false;
                }

            //t or f
            //原因同上面的string
            case TokenReader::TokenType::BOOLEAN:
                {
                if(HasStatus(kEXP_STATUS_OBJECT_VALUE)) // obj value
                {
                    if(false == CaseStatusObjectValue(parse_stack, Value::TYPE_BOOLEAN))
                        return false;

                    break;
                }

                if(HasStatus(kEXP_STATUS_ARRAY_VALUE))  // array value
                {
                    if(false == CaseStatusArrayValue(parse_stack, Value::TYPE_BOOLEAN))
                        return false;

                    break;
                }
                
                return false;
                }

            //- or [0-9]
            //原因同上面的string
            case TokenReader::TokenType::NUMBER:
                {
                if(HasStatus(kEXP_STATUS_OBJECT_VALUE)) // obj value
                {
                    if(false == CaseStatusObjectValue(parse_stack, Value::TYPE_NUMBER))
                        return false;

                    break;
                }

                if(HasStatus(kEXP_STATUS_ARRAY_VALUE))  // array value
                {
                    if(false == CaseStatusArrayValue(parse_stack, Value::TYPE_NUMBER))
                        return false;

                    break;
                }

                return false;
                }

            //null
            //原因同上面的string
            case TokenReader::TokenType::NUL:
                {
                if(HasStatus(kEXP_STATUS_OBJECT_VALUE)) // obj value
                {
                    if(false == CaseStatusObjectValue(parse_stack, Value::TYPE_NULL))
                        return false;

                    break;
                }

                if(HasStatus(kEXP_STATUS_ARRAY_VALUE))  // array value
                {
                    if(false == CaseStatusArrayValue(parse_stack, Value::TYPE_NULL))
                        return false;

                    break;
                }

                return false;
                }

            //非法状态
            case TokenReader::TokenType::INVALID:
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusObjectBegin(std::stack<Value>& parse_stack)
{
    if(!HasStatus(kEXP_STATUS_OBJECT_BEGIN))
        return false;

    Value value(Value::TYPE_OBJECT);
    parse_stack.push(std::move(value));

    //{ -> "key" or { or }
    cur_status_ = kEXP_STATUS_OBJECT_KEY | kEXP_STATUS_OBJECT_BEGIN | kEXP_STATUS_OBJECT_END;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusObjectKey(std::stack<Value>& parse_stack)
{
    std::string key;
    bool err_code = token_reader_.ReadString(key);
    if(false == err_code)
        return false;
    
    Value value(Value::TYPE_KEY);
    value.set_str(std::move(key));
    parse_stack.push(std::move(value));

    //"key" - > :
    cur_status_ = kEXP_STATUS_SEP_COLON;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusObjectValue(std::stack<Value>& parse_stack, int type)
{
    //key:value,parse_stack.size()>=2
    if(2 > parse_stack.size())
        return false;

    //key
    if(Value::TYPE_KEY != parse_stack.top().type())
        return false;
    std::string key = parse_stack.top().val();
    parse_stack.pop();

    //value
    Value       value;
    std::string str;
    switch(type)
    {
        case Value::TYPE_STRING:
            {
            if(false == token_reader_.ReadString(str))
                return false;

            value.set_type(Value::TYPE_STRING);
            value.set_str(std::move(str));

            break;
            }

        case Value::TYPE_NUMBER:
            {
            Value::ValueType num_type;
            if(false == token_reader_.ReadNumber(str, num_type))
                return false;

            value.set_number(str, num_type);
            break;
            }

        case Value::TYPE_BOOLEAN:
            {
            bool boolean;
            if(false == token_reader_.ReadBoolean(boolean))
                return false;

            value.set_type(Value::TYPE_BOOLEAN);
            value.set_boolean(boolean);
            
            break;
            }

        case Value::TYPE_NULL:
            {
            if(false == token_reader_.ReadNull())
                return false;
            value.set_type(Value::TYPE_NULL);

            break;
            }

        default:
            assert(0);
            break;
    }

    //插入
    Value& object = parse_stack.top();
    object.PairAdd(std::move(key), std::move(value));

    //"value" -> , or }
    cur_status_ = kEXP_STATUS_SEP_COMMA | kEXP_STATUS_OBJECT_END;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusObjectEnd(std::stack<Value>& parse_stack)
{
    if(!HasStatus(kEXP_STATUS_OBJECT_END))
        return false;

    //栈底元素类型必须是object
    if(Value::TYPE_OBJECT != parse_stack.top().type())
        return false;
    
    //如果是唯一元素,则JSON解析结束
    if(1 == parse_stack.size())
    {
        cur_status_ = kEXP_STATUS_DOCUMENT_END;
        return true;
    }
    
    Value object = parse_stack.top();
    parse_stack.pop();

    //如果当前栈顶元素是key,则说明是{key1:{key2:value}}这种情况,添加{key2:value}到key1所属于的对象中
    if(Value::TYPE_KEY == parse_stack.top().type())
    {
        //此刻当前栈元素必须>=2
        if(2 > parse_stack.size())
            return false;

        std::string key = parse_stack.top().val();
        parse_stack.pop();

        parse_stack.top().PairAdd(std::move(key), std::move(object));

        //此时栈顶是个对象,期待下个元素是, or }
        cur_status_ = kEXP_STATUS_SEP_COMMA | kEXP_STATUS_OBJECT_END;
        return true;
    }

    //如果当前栈顶元素是array,
    //则说明是[{key2:value}]这种情况,添加{key2:value}到所属数组中
    if(Value::TYPE_ARRAY == parse_stack.top().type())
    {
        //此刻栈顶元素必须>1
        if(1 > parse_stack.size())
            return false;

        parse_stack.top().ArrayAdd(std::move(object));

        //此时栈顶为数组,期待下个元素是, or ]
        cur_status_ = kEXP_STATUS_SEP_COMMA | kEXP_STATUS_ARRAY_END;
        return true;
    }

    assert(0);
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusArrayBegin(std::stack<Value>& parse_stack)
{
    if(!HasStatus(kEXP_STATUS_ARRAY_BEGIN))
        return false;

    //添加array
    parse_stack.push(Value(Value::TYPE_ARRAY));

    //[ -> " or [0-9] or { or [ or ]
    cur_status_ = kEXP_STATUS_ARRAY_VALUE | kEXP_STATUS_ARRAY_BEGIN | kEXP_STATUS_ARRAY_END | kEXP_STATUS_OBJECT_BEGIN;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusArrayValue(std::stack<Value>& parse_stack, int type)
{
    //key:[array], parse_stack.size()>=3
    //if(3 > parse_stack.size())    [1,2,3]也是合法的
    //    return false;
    
    //value
    Value       value;
    std::string str;
    switch(type)
    {
        case Value::TYPE_STRING:
            {
            if(false == token_reader_.ReadString(str))
                return false;

            value.set_type(Value::TYPE_STRING);
            value.set_str(std::move(str));

            break;
            }

        case Value::TYPE_NUMBER:
            {
            Value::ValueType num_type;
            if(false == token_reader_.ReadNumber(str, num_type))
                return false;
            value.set_number(str, num_type);

            break;
            }

        case Value::TYPE_BOOLEAN:
            {
            bool boolean;
            if(false == token_reader_.ReadBoolean(boolean))
                return false;

            value.set_type(Value::TYPE_BOOLEAN);
            value.set_boolean(boolean);
            
            break;
            }

        case Value::TYPE_NULL:
            {
            if(false == token_reader_.ReadNull())
                return false;
            value.set_type(Value::TYPE_NULL);

            break;
            }

        default:
            break;
    }

    //插入
    Value& object = parse_stack.top();
    object.ArrayAdd(std::move(value));

    //"value" -> , or ]
    cur_status_ = kEXP_STATUS_SEP_COMMA | kEXP_STATUS_ARRAY_END;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusArrayEnd(std::stack<Value>& parse_stack)
{
    if(false == HasStatus(kEXP_STATUS_ARRAY_END))
        return false;
 
    //[1,2,3]也是合法的
    if(1 == parse_stack.size())
    {
        cur_status_ = kEXP_STATUS_DOCUMENT_END;
        return true;
    }

    //[[array]]
    //key:[array] parse_stack.size()>=2
    if(2 > parse_stack.size())
        return false;

    //array
    Value array = parse_stack.top();
    parse_stack.pop();

    //如果当前栈顶元素是key,则说明是{key1:[value]}这种情况,添加[value]到key1所属于的对象中
    if(Value::TYPE_KEY == parse_stack.top().type())
    {
        //此刻当前栈元素必须>=2
        if(2 > parse_stack.size())
            return false;

        std::string key = parse_stack.top().val();
        parse_stack.pop();

        parse_stack.top().PairAdd(std::move(key), std::move(array));

        //此时栈顶是个object,期待下个元素是, or }
        cur_status_ = kEXP_STATUS_SEP_COMMA | kEXP_STATUS_OBJECT_END;
        return true;
    }

    //如果当前栈顶元素是array,
    //则说明是[[key2:value]]这种情况,添加[key2:value]到所属数组中
    if(Value::TYPE_ARRAY == parse_stack.top().type())
    {
        //此刻栈顶元素必须>1
        if(1 > parse_stack.size())
            return false;

        parse_stack.top().ArrayAdd(std::move(array));

        //此时栈顶为数组,期待下个元素是, or ]
        cur_status_ = kEXP_STATUS_SEP_COMMA | kEXP_STATUS_ARRAY_END;
        return true;
    }

    assert(0);
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusSepColon(std::stack<Value>&)
{
    if(!HasStatus(kEXP_STATUS_SEP_COLON))
        return false;

    //: -> { or [ or "value"
    cur_status_ = kEXP_STATUS_OBJECT_BEGIN | kEXP_STATUS_ARRAY_BEGIN | kEXP_STATUS_OBJECT_VALUE;
    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusSepComma(std::stack<Value>&)
{
    if(!HasStatus(kEXP_STATUS_SEP_COMMA))
        return false;

    ///, -> "key" or [ or { or arr_val
    //如果当前状态同时期待kEXP_STATUS_OBJECT_END,代表当前处于key:value状态
    if(HasStatus(kEXP_STATUS_OBJECT_END))
    {
        cur_status_ = kEXP_STATUS_OBJECT_KEY;
        return true;
    }

    //如果当前状态同时期待kEXP_STATUS_ARRAY_END,代表当前处于key:[array]状态
    if(HasStatus(kEXP_STATUS_ARRAY_END))
    {
        cur_status_ = kEXP_STATUS_OBJECT_BEGIN | kEXP_STATUS_ARRAY_BEGIN | kEXP_STATUS_ARRAY_VALUE;
        return true;
    }

    return true;
}
//---------------------------------------------------------------------------
bool JsonReader::CaseStatusDocumentEnd(std::stack<Value>& parse_stack)
{
    if(!HasStatus(kEXP_STATUS_DOCUMENT_END))
        return false;

    (void)parse_stack;
    assert(1 == parse_stack.size());

    return true;
}
//---------------------------------------------------------------------------
}//namespace json

}//namespace base
