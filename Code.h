#pragma once

#ifndef __CODE__
#define __CODE__

#include <stdio.h>
#include <string>
using namespace std;

//#if(_WIN32_WINNT >= 0x0501)
//typedef unsigned __int64 u_int64;
//#endif //(_WIN32_WINNT >= 0x0501)

//替换字符串内容
//例:string_replace(str,"old","new")
inline void string_replace(std::string & strBig, const std::string & strsrc, const std::string &strdst)
{
    std::string::size_type pos=0;
    std::string::size_type srclen=strsrc.size();
    std::string::size_type dstlen=strdst.size();
    while( (pos=strBig.find(strsrc, pos)) != std::string::npos)
    {
        strBig.replace(pos, srclen, strdst);
        pos += dstlen;
    }
}



//字符串编码转换
class Code
{
public:
    Code(void);
    ~Code(void);

public:
    //gb2312 to unicode 返回字串长度为:实际长度+1, 末尾\0站一字节
    wchar_t* GB2U(const char* pszGbs,unsigned __int64* wLen);
    //unicode to utf8 返回字串长度为:实际长度+1, 末尾\0站一字节
    char* U2U8(const wchar_t* wszUnicode,unsigned __int64* aLen);
    //utf8 to unicode 返回字串长度为:实际长度+1, 末尾\0站一字节
    wchar_t* U82U(const char* szU8,unsigned __int64* wLen);
    //unicode to gb2312 返回字串长度为:实际长度+1, 末尾\0站一字节
    char* U2GB(const wchar_t* wszUnicode,unsigned __int64* aLen);

public:
    //gb2312 to utf8 返回字串长度为:实际长度+1, 末尾\0站一字节
    char* GB2U8(const char* pszGbs,unsigned __int64* wLen);
    //utf8 to gb2312 返回字串长度为:实际长度+1, 末尾\0站一字节
    char* U82GB(const char* szU8,unsigned __int64* wLen);
};

//Base64
class Base64
{
    static const string base64_table;
public:
    Base64();
    ~Base64();

    static inline bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string Encode(unsigned char const* bytes_to_encode, unsigned int in_len);
    std::string Decode(std::string const& encoded_string);
};

//Hash1
class Hash1
{
//#undef BIG_ENDIAN_HOST

typedef unsigned int u32;
struct SHA1_CONTEXT{
    u32  h0,h1,h2,h3,h4;
    u32  nblocks;
    u32  count;
    unsigned char buf[64];
};

    SHA1_CONTEXT hd;
    bool bFinal;
public:
    Hash1();
    ~Hash1();
    void Reset();
    /* Update the message digest with the contents of INBUF with length INLEN. */
    void Write(unsigned char *inbuf, size_t inlen);

    Hash1& operator<<(std::string data);

    /* The routine final terminates the computation and returns the digest.
     * The handle is prepared for a new cycle, but adding bytes to the
     * handle will the destroy the returned buffer.
     * Returns: 20 bytes representing the digest.
     */
    void Final();

    unsigned char* GetHash1();

private:
/*
 * Transform the message X which consists of 16 32-bit-words
 */
 void transform( unsigned char *data );

};

//字符串工具函数
class Chars
{
public:
	Chars();
	~Chars();
	size_t url_encode(const char *url, char *buf, size_t len);//url编码
	char* url_decode(char *s);//url解码

};


#endif //__CODE__

