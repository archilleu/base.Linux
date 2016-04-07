//---------------------------------------------------------------------------
#include "test_md5.h"
#include "../md5.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestMD5::DoTest()
{
    if(false == Test_Normal())  return false;
    if(false == Test_File())    return false;

    return true;
}
//---------------------------------------------------------------------------
bool TestMD5::Test_Normal()
{
    std::map<std::string, std::string> str_md5;
    str_md5["admin"]    = "21232f297a57a5a743894a0e4a801fc3";
    str_md5["ADMIN"]    = "73ACD9A5972130B75066C82595A1FAE3";
    str_md5["123456"]   = "e10adc3949ba59abbe56e057f20f883e";
    str_md5["admin888"] = "7fef6171469e80d32c0559f88b377245";
    
    MD5 md5;
    for(auto iter : str_md5)
    {
        md5.BufferAppend(iter.first.data(), iter.first.size());
        std::string str = md5.Done();
        MY_ASSERT(0 == strcasecmp(str.c_str(), iter.second.c_str()));
    }

    for(auto iter : str_md5)
    {
        std::string str1 = md5.DataMD5(iter.first.data(), iter.first.size());
        std::string str2 = md5.StringMD5(iter.first);
        MY_ASSERT(0 == strcasecmp(str1.c_str(), iter.second.c_str()));
        MY_ASSERT(0 == strcasecmp(str2.c_str(), iter.second.c_str()));
    }

    return true;
}
//---------------------------------------------------------------------------
bool TestMD5::Test_File()
{
    std::string md5_str = "061d9674bbf86b5cbe4cefdad02bcf3d";
    MD5 md5;
    std::string result = md5.FileMD5("./md5_file/json_file.zip");
    MY_ASSERT(0 == strcasecmp(md5_str.c_str(), result.c_str()));
    MY_ASSERT(0 == strcasecmp(md5_str.c_str(), md5.FileMD5("./md5_file/json_file.zip").c_str()));
    
    return true;
}
//---------------------------------------------------------------------------
