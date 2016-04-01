//---------------------------------------------------------------------------
#include "token_reader.h"
//---------------------------------------------------------------------------
namespace base
{

namespace json
{

TokenReader::TokenType TokenReader::ReadNextToken()
{
    SkipWhitespace();

    if(!char_reader_.HasMore())
        return DOCUMENT_END;

    char c = char_reader_.Peek();
    switch(c)
    {
        case '{':
            char_reader_.Next();
            return OBJECT_BEGIN;

        case '}':
            char_reader_.Next();
            return OBJECT_END;

        case '[':
            char_reader_.Next();
            return ARRAY_BEGIN;

        case ']':
            char_reader_.Next();
            return ARRAY_END;

        case ':':
            char_reader_.Next();
            return SEP_COLON;

        case ',':
            char_reader_.Next();
            return SEP_COMMA;

        case '\"':
            char_reader_.Next();
            return STRING;

        case 'n':
            char_reader_.Next();
            return NUL;

        case 't':
        case 'f':
            char_reader_.Next();
            return BOOLEAN;

        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            char_reader_.Next();
            return NUMBER;
    }

    return INVALID;
}
//---------------------------------------------------------------------------
bool TokenReader::ReadString(std::string& str)
{
    //跳过空白
    SkipWhitespace();

    std::string value;
    for(;;)
    {
        if(!char_reader_.HasMore())
            break;

        char c = char_reader_.Next();
        switch(c)
        {
            case '\\'://转义字符
            {
                if(!char_reader_.HasMore())
                    break;

                char esc = char_reader_.Next();
                switch(esc)
                {
                    case '\"':
                        str.push_back('\"');
                        break;

                    case '\\':
                        str.push_back('\\');
                        break;

                    case '/':
                        str.push_back('/');
                        break;

                    case 'b':
                        str.push_back('b');
                        break;

                    case 'f'://换页
                        str.push_back('f');
                        break;

                    case 'n':
                        str.push_back('n');
                        break;

                    case 'r':
                        str.push_back('r');
                        break;

                    case 't':
                        str.push_back('t');
                        break;

                    case 'u'://unicode字符
                        //todo
                        break;

                    default:
                        return false;
                }
            }
                break;

            case '\"':  //结束
                return true;
            
            default:    //普通字符
                str.push_back(c);
                break;
        }
    }

    //不是从 case '\"'返回的,说明字符串读取不完整,返回失败
    return false;
}
//---------------------------------------------------------------------------
bool TokenReader::ReadNumber(Number& number)
{
    //数值分为2个部分构成
    //整数部分和小数部分(不支持指数表示)
    
    const static short STATUS_NUMBER_INT       = 1;
    const static short STATUS_NUMBER_DECIMAL   = 2;
    const static short STATUS_NUMBER_END       = 3;

    //第一个字符必须是数字或者'-'
    char c = char_reader_.Peek();
    if((!CheckNumber(c)) && ('-'!=c))
        return false;

    std::string str;
    bool        has_sign = false;
    if('-' == c)
    {
        if(!char_reader_.HasMore())
            return false;

        has_sign = true;
        str.push_back(c);
        char_reader_.Next();
    }

    bool    has_decimal = false;
    short   status      = STATUS_NUMBER_INT;
    for(;;)
    {
        if(char_reader_.HasMore())
        {
            c = char_reader_.Peek();

            //检查是否是数字,如果不是数字,则有可能该数字不是正确的数字(例如:123a),这种情况留給上一层处理
            if(false == CheckNumber(c))
                status = STATUS_NUMBER_END;
        }
        else
            status = STATUS_NUMBER_END;//这代表JSON格式出错,但是留給上一层处理

        switch(status)
        {
            case STATUS_NUMBER_INT:

                //有小数点表示有小数部分
                if('.' == c)
                {
                    has_decimal = true;
                    status      = STATUS_NUMBER_DECIMAL;
                }

                str.push_back(c);
                char_reader_.Next();
                
                break;

            case STATUS_NUMBER_DECIMAL:

                if(('E'==c) || ('e'==c))
                {
                    //todo 指数
                }
                
                str.push_back(c);
                char_reader_.Next();
                break;

            case STATUS_NUMBER_END:
                
                //如果是有符号,则str值必须大于1才表示有值
                if(true == has_sign)
                {
                    if(1 >= str.size())
                        return false;
                }
                else
                {
                    if(str.empty())
                        return false;
                }

                //值是小数
                if(true == has_decimal)
                {
                    number.set_value(static_cast<double>(std::stod(str)));
                    return true;
                }

                if(true == has_sign)    //值是有符号整数
                    number.set_value(static_cast<int64_t>(std::stoll(str)));
                else                    //无符号整数
                    number.set_value(static_cast<uint64_t>(std::stoull(str)));

                return true;
        }
    }
}
//---------------------------------------------------------------------------
bool TokenReader::ReadBoolean(bool& boolean)
{
    //第一个字符必须是't' or 'f'

    char c = char_reader_.Peek();
    switch(c)
    {
        case 't':
            {
            if(4 > char_reader_.Remain())
                return false;

            std::string value = char_reader_.Next(4);
            if("true" != value)
                return false;

            boolean = true;
            }

            break;

        case 'f':
            {
            if(5 > char_reader_.Remain())
                return false;

            std::string value = char_reader_.Next(5);
            if("false" != value)
                return false;

            boolean = false;
            }

            break;

        default:
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool TokenReader::ReadNull()
{
    if(4 > char_reader_.Remain())
        return false;

    std::string value = char_reader_.Next(4);
    if("null" != value)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void TokenReader::SkipWhitespace()
{
    for(;;)
    {
        if(!char_reader_.HasMore())
            break;

        char c = char_reader_.Peek();
        if(!IsWhitespace(c))
            break;

        char_reader_.Next();
    }

    return;
}
//---------------------------------------------------------------------------
}//namespace json

}//namespace base
