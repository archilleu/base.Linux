//---------------------------------------------------------------------------
#include "test_append_file.h"
#include "../append_file.h"
#include "../function.h"
//---------------------------------------------------------------------------
using namespace base;
using namespace base::test;
//---------------------------------------------------------------------------
bool TestAppendFile::DoTest()
{
    //非法
    {
    AppendFile file1;
    MY_ASSERT(false == file1.Open("/"));

    AppendFile file2;
    MY_ASSERT(false == file2.Open("/etc/passwd"));
    }

    const char* path = "/tmp/append_file";
    const char* info1 = "god damn smart 1\n";
    const char* info2 = "god damn smart 2\n";

    {
    FileDelete(path);

    AppendFile  file;
    size_t      size = strlen(info1);
    MY_ASSERT(true == file.Open(path));
    
    for(int i=0; i<100; i++)
        MY_ASSERT(true == file.Append(info1, size));
    size_t file_size = file.Size();
    file.Flush();
    file.Close();

    FILE* fp = fopen(path, "r");
    MY_ASSERT(fp!=0);

    for(int i=0; i<100; i++)
    {
        char buffer[128];
        if(0 != fgets(buffer, 128, fp))
            MY_ASSERT(0==strcmp(buffer, info1));
        MY_ASSERT(0==strcmp(buffer, info1));
    }
    MY_ASSERT(file_size == static_cast<size_t>(ftell(fp)));
    fclose(fp);

    //再次追加
    MY_ASSERT(true == file.Open(path));
    MY_ASSERT(file_size == file.Size());
    for(int i=0; i<100; i++)
    {
        MY_ASSERT(true == file.Append(info2, size));
    }
    MY_ASSERT(file_size*2 == file.Size());
    }

    //追加完成后再次检测
    FILE* fp = fopen(path, "r");
    MY_ASSERT(fp!=0);

    for(int i=0; i<100; i++)
    {
        char buffer[128];
        if(0 != fgets(buffer, 128, fp))
            MY_ASSERT(0==strcmp(buffer, info1));
        MY_ASSERT(0==strcmp(buffer, info1));
    }
    for(int i=100; i<200; i++)
    {
        char buffer[128];
        if(0 != fgets(buffer, 128, fp))
            MY_ASSERT(0==strcmp(buffer, info2));
        MY_ASSERT(0==strcmp(buffer, info2));
    }

    fclose(fp);

    return true;
}
//---------------------------------------------------------------------------
