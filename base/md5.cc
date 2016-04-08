//---------------------------------------------------------------------------
#include "md5.h"
#include "function.h"
//---------------------------------------------------------------------------
namespace base
{
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21
//---------------------------------------------------------------------------
//数据填充
unsigned char PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
//---------------------------------------------------------------------------
//基本MD5函数：F, G, H, I
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
//---------------------------------------------------------------------------
//循环左移
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
//---------------------------------------------------------------------------
//四轮操作函数
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + static_cast<uint32_t>(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + static_cast<uint32_t>(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + static_cast<uint32_t>(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + static_cast<uint32_t>(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MD5::MD5()
{
    Reset();
    return;
}
//---------------------------------------------------------------------------
MD5::~MD5()
{
    return;
}
//---------------------------------------------------------------------------
void MD5::BufferAppend(const uint8_t* buffer, size_t len)
{
	uint32_t index  = static_cast<uint32_t>((context_.count >> 3) & 0x3F);  //计算context_的buffer中已有的还没处理的内容的长度
	context_.count += static_cast<uint64_t>(len << 3);                      //更新已处理数据的长度
	
	//循环处理计算数据
	uint32_t part_len 	= 64 - index;
	uint32_t buf_index	= 0;
	if(len >= part_len)
	{
		memcpy(&context_.buffer[index], buffer, part_len);
		Transform(context_.state, context_.buffer);

		uint32_t i = 0;
		for(i=part_len; (i+63)<len; i+=64)
			Transform(context_.state, &buffer[i]);

		index 		= 0;
		buf_index 	= i;
	}
	else
		buf_index = 0;

	//剩余的数据拷贝到context_.buffer中
	memcpy(&context_.buffer[index], &buffer[buf_index], len-buf_index);
    return;
}
//---------------------------------------------------------------------------
void MD5::BufferAppend(const char* buffer, size_t len)
{
    return BufferAppend(reinterpret_cast<const uint8_t*>(buffer), len);
}
//---------------------------------------------------------------------------
std::string MD5::Done()
{
    MD5_VAL md5_val;
    Done(md5_val);
    return base::BinToString(md5_val, sizeof(MD5_VAL));
}
//---------------------------------------------------------------------------
void MD5::Done(MD5_VAL md5_val)
{
	//把数据位长度context_.Count(uint64_t值)转换成uint8_t[8]
	uint8_t bits[8] = {0};
	memcpy(bits, &context_.count, 8);

	//填充context_.buffer的数据到448(bits)
	uint32_t index  = static_cast<uint32_t>((context_.count >> 3) & 0x3f);
	uint32_t pad_len= (index < 56) ? (56 - index) : (120 - index);
	BufferAppend(PADDING, pad_len);

	//填充数据位数的大小, 并计算
	BufferAppend(bits, 8);

	//返回最终的计算结果, 即把context_.state[4](uint32_t值)转换成KKMD_VAL
	memcpy(md5_val, context_.state, 16);

    //重置状态
    Reset();
    return;
}
//---------------------------------------------------------------------------
void MD5::StringMD5(const std::string& dat, MD5_VAL md5_val)
{
    return StringMD5(dat.c_str(), md5_val);
}
//---------------------------------------------------------------------------
void MD5::StringMD5(const char* dat, MD5_VAL md5_val)
{
    return DataMD5(dat, strlen(dat), md5_val);
}
//---------------------------------------------------------------------------
std::string MD5::StringMD5(const std::string& dat)
{
    return StringMD5(dat.c_str());
}
//---------------------------------------------------------------------------
std::string MD5::StringMD5(const char* dat)
{
    return DataMD5(dat, strlen(dat)); 
}
//---------------------------------------------------------------------------
void MD5::DataMD5(const char* dat, size_t len, MD5_VAL md5_val)
{
    BufferAppend(dat, len);
    Done(md5_val);
    return;
}
//---------------------------------------------------------------------------
std::string MD5::DataMD5(const char* dat, size_t len)
{
    MD5_VAL md5_val;
    DataMD5(dat, len, md5_val);
    return BinToString(md5_val, sizeof(MD5_VAL));
}
//---------------------------------------------------------------------------
void MD5::FileMD5(const char* path, MD5_VAL md5_val)
{
    int fd = open(path, O_RDONLY);
    if(0 > fd)
        return;

    struct stat file_info;
    int err_code = fstat(fd, &file_info);
    if(0 > err_code)
    {
        close(fd);
        return;
    }

    MemoryBlock mb(file_info.st_blksize);
    for(;0<file_info.st_size;)
    {
        size_t  len = (file_info.st_size > file_info.st_blksize) ? file_info.st_blksize : file_info.st_size;
        ssize_t rlen= read(fd, mb.dat(), len);
        if(0 > rlen)
        {
            close(fd);
            return;
        }

        //计算MD5
        BufferAppend(mb.dat(), rlen);
        file_info.st_size -= rlen;
    }

    Done(md5_val);
    close(fd);

    return;
}
//---------------------------------------------------------------------------
void MD5::FileMD5(const std::string& path, MD5_VAL md5_val)
{
    FileMD5(path.c_str(), md5_val);
    return;
}
//---------------------------------------------------------------------------
std::string MD5::FileMD5(const char* path)
{
    MD5_VAL md5_val;
    FileMD5(path, md5_val);
    return BinToString(md5_val, sizeof(MD5_VAL));
}
//---------------------------------------------------------------------------
std::string MD5::FileMD5(const std::string& path)
{
    return FileMD5(path.c_str());
}
//---------------------------------------------------------------------------
void MD5::Reset()
{
	context_.count		= 0;
    context_.state[0] 	= 0x67452301;
    context_.state[1] 	= 0xefcdab89;
    context_.state[2] 	= 0x98badcfe;
    context_.state[3] 	= 0x10325476;

    return;
}
//---------------------------------------------------------------------------
void MD5::Transform(uint32_t state[4], const uint8_t block[64])
{
    uint32_t a 	    = state[0];
    uint32_t b 	    = state[1];
    uint32_t c 	    = state[2];
    uint32_t d 	    = state[3];
    uint32_t x[16]	= {0};

    //block转换成uint32_t，存在x[16]中。
    for (uint32_t i = 0, j = 0; j < 64; i++, j+=4)
        x[i] = (static_cast<uint32_t>(block[j])) | (static_cast<uint32_t>((block[j+1]))<<8) | 
            ((static_cast<uint32_t>(block[j+2]))<<16) | ((static_cast<uint32_t>(block[j+3]))<<24);

    //第一轮
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); // 1
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); // 2
    FF (c, d, a, b, x[ 2], S13, 0x242070db); // 3
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); // 4
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); // 5
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); // 6
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); // 7
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); // 8
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); // 9
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); // 10
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); // 11
    FF (b, c, d, a, x[11], S14, 0x895cd7be); // 12
    FF (a, b, c, d, x[12], S11, 0x6b901122); // 13
    FF (d, a, b, c, x[13], S12, 0xfd987193); // 14
    FF (c, d, a, b, x[14], S13, 0xa679438e); // 15
    FF (b, c, d, a, x[15], S14, 0x49b40821); // 16

    //第二轮
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); // 17
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); // 18
    GG (c, d, a, b, x[11], S23, 0x265e5a51); // 19
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); // 20
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); // 21
    GG (d, a, b, c, x[10], S22,  0x2441453); // 22
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); // 23
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); // 24
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); // 25
    GG (d, a, b, c, x[14], S22, 0xc33707d6); // 26
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); // 27
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); // 28
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); // 29
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); // 30
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); // 31
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); // 32

    //第三轮
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); // 33
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); // 34
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); // 35
    HH (b, c, d, a, x[14], S34, 0xfde5380c); // 36
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); // 37
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); // 38
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); // 39
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); // 40
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); // 41
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); // 42
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); // 43
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); // 44
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); // 45
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); // 46
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); // 47
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); // 48

	//第四轮
    II (a, b, c, d, x[ 0], S41, 0xf4292244); // 49
    II (d, a, b, c, x[ 7], S42, 0x432aff97); // 50
    II (c, d, a, b, x[14], S43, 0xab9423a7); // 51
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); // 52
    II (a, b, c, d, x[12], S41, 0x655b59c3); // 53
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); // 54
    II (c, d, a, b, x[10], S43, 0xffeff47d); // 55
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); // 56
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); // 57
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); // 58
    II (c, d, a, b, x[ 6], S43, 0xa3014314); // 59
    II (b, c, d, a, x[13], S44, 0x4e0811a1); // 60
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); // 61
    II (d, a, b, c, x[11], S42, 0xbd3af235); // 62
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); // 63
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); // 64

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}
//---------------------------------------------------------------------------
}//namespace base
