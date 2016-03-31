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
    //第一个字符必须是'\"'
    char c = char_reader_.Next();
    assert('\"' == c);

    std::string value;
    for(;;)
    {
        if(!char_reader_.HasMore())
            break;

        c = char_reader_.Next();
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
                break;
            
            default:    //普通字符
                str.push_back(c);
                break;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool TokenReader::ReadNumber(Number& number)
{
    //数值分为2个部分构成
    //整数部分和小数部分(不支持指数表示)
    
    /*const static short STATUS_NUMBER_INT       = 1;
    const static short STATUS_NUMBER_DECIMAL   = 2;
    const static short STATUS_NUMBER_END       = 3;

    //第一个字符必须是数字或者'-'
    char c = char_reader_.Next();
    assert(('-'==c) || ('0'<=c) || ('9'>=c));

    bool has_sign   = false;
    bool has_decimal= false;

    short status = STATUS_NUMBER_INT;
    for(;;)
    {
        switch(status)
        {
            case STATUS_NUMBER_INT:
                break;

            case STATUS_NUMBER_DECIMAL:
                break;

            case STATUS_NUMBER_END:
                break;
        }
    }
    */

    return true;
}
//---------------------------------------------------------------------------
bool TokenReader::ReadBoolean()
{
    return true;
}
//---------------------------------------------------------------------------
bool TokenReader::ReadNull()
{
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