//#include <../stdfx.h>
#include <stdlib.h>
#ifdef __GNU__
#include <iconv.h>

//代码转换:从一种编码转为另一种编码
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	iconv_t cd;
	int rc=0;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd==0)
		return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen)==-1)
		rc= -1;
	iconv_close(cd);
	return rc;
}
#else
#include <windows.h>
#define sscanf sscanf_s
#endif
#include <string.h>
#include "Code.h"


//-----------------------------------------------------------------------------------Code
#pragma region Code

Code::Code(){}
Code::~Code(){}

//gb2312 to unicode
wchar_t* Code::GB2U(const char* pszGbs,unsigned __int64* wLen)
{
#ifdef __GNU__
#else
	*wLen = MultiByteToWideChar(CP_ACP, 0, pszGbs, -1, NULL, 0);
	wchar_t* wStr = new wchar_t[*wLen];
	MultiByteToWideChar(CP_ACP, 0, pszGbs, -1, wStr, *wLen);
#endif
	return wStr;
}
//unicode to utf8
char* Code::U2U8(const wchar_t* wszUnicode,unsigned __int64* aLen)
{
	*aLen = WideCharToMultiByte(CP_UTF8, 0, (PWSTR)wszUnicode, -1, NULL, 0, NULL, NULL);
	char* szStr = new char[*aLen];
	WideCharToMultiByte(CP_UTF8, 0, (PWSTR)wszUnicode, -1, szStr, *aLen, NULL, NULL);
	return szStr;
}
//utf8 to unicode
wchar_t* Code::U82U(const char* szU8,unsigned __int64* wLen)
{
	*wLen = MultiByteToWideChar(CP_UTF8, 0, szU8, -1, NULL, 0);
	wchar_t* wStr = new wchar_t[*wLen];
	MultiByteToWideChar(CP_UTF8, 0, szU8, -1, wStr, *wLen);
	return wStr;
}
//unicode to gb2312
char* Code::U2GB(const wchar_t* wszUnicode,unsigned __int64* aLen)
{
	*aLen = WideCharToMultiByte(CP_ACP, 0, wszUnicode, -1, NULL, 0, NULL, NULL);
	char* szStr = new char[*aLen];
	WideCharToMultiByte(CP_ACP, 0, wszUnicode, -1, szStr, *aLen, NULL, NULL);
	return szStr;
}
//gb2312 to utf8
char* Code::GB2U8(const char* pszGbs,unsigned __int64* wLen)
{
	*wLen = MultiByteToWideChar(CP_ACP, 0, pszGbs, -1, NULL, 0);
	wchar_t* wStr = new wchar_t[*wLen];
	MultiByteToWideChar(CP_ACP, 0, pszGbs, -1, wStr, *wLen);

	*wLen = WideCharToMultiByte(CP_UTF8, 0, (PWSTR)wStr, -1, NULL, 0, NULL, NULL);
	char* szStr = new char[*wLen];
	WideCharToMultiByte(CP_UTF8, 0, (PWSTR)wStr, -1, szStr, *wLen, NULL, NULL);
	delete[] wStr;
	return szStr;
}
//utf8 to gb2312
char* Code::U82GB(const char* szU8,unsigned __int64* wLen)
{
	*wLen = MultiByteToWideChar(CP_UTF8, 0, szU8, -1, NULL, 0);
	wchar_t* wStr = new wchar_t[*wLen];
	MultiByteToWideChar(CP_UTF8, 0, szU8, -1, wStr, *wLen);

	*wLen = WideCharToMultiByte(CP_ACP, 0, wStr, -1, NULL, 0, NULL, NULL);
	char* szStr = new char[*wLen];
	WideCharToMultiByte(CP_ACP, 0, wStr, -1, szStr, *wLen, NULL, NULL);
	delete[] wStr;
	return szStr;
}

#pragma endregion

//-----------------------------------------------------------------------------------Base64
#pragma region Base64

const std::string Base64::base64_table="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
Base64::Base64(){};
Base64::~Base64(){};

std::string Base64::Encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_table[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_table[char_array_4[j]];

		while((i++ < 3))
			ret += '=';

	}
	return ret;
}

std::string Base64::Decode(std::string const& encoded_string)
{
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_table.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_table.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}
	return ret;
}
#pragma endregion

//-----------------------------------------------------------------------------------Hash1
#pragma region Hash1
/****************
 * Rotate a 32 bit integer by n bytes
 ****************/
#if defined(__GNUC__) && defined(__i386__)
static inline u32 rol( u32 x, int n)
{
    __asm__("roll %%cl,%0"
            :"=r" (x)
            :"0" (x),"c" (n));
    return x;
}
#else
#define rol(x,n) ( ((x) << (n)) | ((x) >> (32-(n))) )
#endif

#define K1  0x5A827999L
#define K2  0x6ED9EBA1L
#define K3  0x8F1BBCDCL
#define K4  0xCA62C1D6L
#define F1(x,y,z)   ( z ^ ( x & ( y ^ z ) ) )
#define F2(x,y,z)   ( x ^ y ^ z )
#define F3(x,y,z)   ( ( x & y ) | ( z & ( x | y ) ) )
#define F4(x,y,z)   ( x ^ y ^ z )


#define M(i) ( tm = x[i&0x0f] ^ x[(i-14)&0x0f]    \
               ^ x[(i-8)&0x0f] ^ x[(i-3)&0x0f]    \
               , (x[i&0x0f] = rol(tm,1)) )


#define R(a,b,c,d,e,f,k,m)  do { e += rol( a, 5 ) \
            + f( b, c, d )                        \
            + k                                   \
            + m;                                  \
        b = rol( b, 30 );                         \
    } while(0)

Hash1::Hash1(){
    Reset();
};
Hash1::~Hash1(){};

void Hash1::Reset()
{
    hd.h0 = 0x67452301;
    hd.h1 = 0xefcdab89;
    hd.h2 = 0x98badcfe;
    hd.h3 = 0x10325476;
    hd.h4 = 0xc3d2e1f0;
    hd.nblocks = 0;
    hd.count = 0;
    memset(hd.buf,0,64);
    bFinal=false;
}
/*
 * Transform the message X which consists of 16 32-bit-words
 */
void Hash1::transform(unsigned char *data )
{
    u32 a,b,c,d,e,tm;
    u32 x[16];

    /* get values from the chaining vars */
    a = hd.h0;
    b = hd.h1;
    c = hd.h2;
    d = hd.h3;
    e = hd.h4;

#ifdef BIG_ENDIAN_HOST
    memcpy( x, data, 64 );
#else
    { int i;
        unsigned char *p2;
        for(i=0, p2=(unsigned char*)x; i < 16; i++, p2 += 4 ) {
            p2[3] = *data++;
            p2[2] = *data++;
            p2[1] = *data++;
            p2[0] = *data++;
        }
    }
#endif

    R( a, b, c, d, e, F1, K1, x[ 0] );
    R( e, a, b, c, d, F1, K1, x[ 1] );
    R( d, e, a, b, c, F1, K1, x[ 2] );
    R( c, d, e, a, b, F1, K1, x[ 3] );
    R( b, c, d, e, a, F1, K1, x[ 4] );
    R( a, b, c, d, e, F1, K1, x[ 5] );
    R( e, a, b, c, d, F1, K1, x[ 6] );
    R( d, e, a, b, c, F1, K1, x[ 7] );
    R( c, d, e, a, b, F1, K1, x[ 8] );
    R( b, c, d, e, a, F1, K1, x[ 9] );
    R( a, b, c, d, e, F1, K1, x[10] );
    R( e, a, b, c, d, F1, K1, x[11] );
    R( d, e, a, b, c, F1, K1, x[12] );
    R( c, d, e, a, b, F1, K1, x[13] );
    R( b, c, d, e, a, F1, K1, x[14] );
    R( a, b, c, d, e, F1, K1, x[15] );
    R( e, a, b, c, d, F1, K1, M(16) );
    R( d, e, a, b, c, F1, K1, M(17) );
    R( c, d, e, a, b, F1, K1, M(18) );
    R( b, c, d, e, a, F1, K1, M(19) );
    R( a, b, c, d, e, F2, K2, M(20) );
    R( e, a, b, c, d, F2, K2, M(21) );
    R( d, e, a, b, c, F2, K2, M(22) );
    R( c, d, e, a, b, F2, K2, M(23) );
    R( b, c, d, e, a, F2, K2, M(24) );
    R( a, b, c, d, e, F2, K2, M(25) );
    R( e, a, b, c, d, F2, K2, M(26) );
    R( d, e, a, b, c, F2, K2, M(27) );
    R( c, d, e, a, b, F2, K2, M(28) );
    R( b, c, d, e, a, F2, K2, M(29) );
    R( a, b, c, d, e, F2, K2, M(30) );
    R( e, a, b, c, d, F2, K2, M(31) );
    R( d, e, a, b, c, F2, K2, M(32) );
    R( c, d, e, a, b, F2, K2, M(33) );
    R( b, c, d, e, a, F2, K2, M(34) );
    R( a, b, c, d, e, F2, K2, M(35) );
    R( e, a, b, c, d, F2, K2, M(36) );
    R( d, e, a, b, c, F2, K2, M(37) );
    R( c, d, e, a, b, F2, K2, M(38) );
    R( b, c, d, e, a, F2, K2, M(39) );
    R( a, b, c, d, e, F3, K3, M(40) );
    R( e, a, b, c, d, F3, K3, M(41) );
    R( d, e, a, b, c, F3, K3, M(42) );
    R( c, d, e, a, b, F3, K3, M(43) );
    R( b, c, d, e, a, F3, K3, M(44) );
    R( a, b, c, d, e, F3, K3, M(45) );
    R( e, a, b, c, d, F3, K3, M(46) );
    R( d, e, a, b, c, F3, K3, M(47) );
    R( c, d, e, a, b, F3, K3, M(48) );
    R( b, c, d, e, a, F3, K3, M(49) );
    R( a, b, c, d, e, F3, K3, M(50) );
    R( e, a, b, c, d, F3, K3, M(51) );
    R( d, e, a, b, c, F3, K3, M(52) );
    R( c, d, e, a, b, F3, K3, M(53) );
    R( b, c, d, e, a, F3, K3, M(54) );
    R( a, b, c, d, e, F3, K3, M(55) );
    R( e, a, b, c, d, F3, K3, M(56) );
    R( d, e, a, b, c, F3, K3, M(57) );
    R( c, d, e, a, b, F3, K3, M(58) );
    R( b, c, d, e, a, F3, K3, M(59) );
    R( a, b, c, d, e, F4, K4, M(60) );
    R( e, a, b, c, d, F4, K4, M(61) );
    R( d, e, a, b, c, F4, K4, M(62) );
    R( c, d, e, a, b, F4, K4, M(63) );
    R( b, c, d, e, a, F4, K4, M(64) );
    R( a, b, c, d, e, F4, K4, M(65) );
    R( e, a, b, c, d, F4, K4, M(66) );
    R( d, e, a, b, c, F4, K4, M(67) );
    R( c, d, e, a, b, F4, K4, M(68) );
    R( b, c, d, e, a, F4, K4, M(69) );
    R( a, b, c, d, e, F4, K4, M(70) );
    R( e, a, b, c, d, F4, K4, M(71) );
    R( d, e, a, b, c, F4, K4, M(72) );
    R( c, d, e, a, b, F4, K4, M(73) );
    R( b, c, d, e, a, F4, K4, M(74) );
    R( a, b, c, d, e, F4, K4, M(75) );
    R( e, a, b, c, d, F4, K4, M(76) );
    R( d, e, a, b, c, F4, K4, M(77) );
    R( c, d, e, a, b, F4, K4, M(78) );
    R( b, c, d, e, a, F4, K4, M(79) );

    /* Update chaining vars */
    hd.h0 += a;
    hd.h1 += b;
    hd.h2 += c;
    hd.h3 += d;
    hd.h4 += e;

}

/* Update the message digest with the contents
 * of INBUF with length INLEN.
 */
void Hash1::Write( unsigned char *inbuf, size_t inlen)
//static void sha1_write( SHA1_CONTEXT *hd, char *inbuf, size_t inlen)
{
    if(bFinal)
        Reset();
    if( hd.count == 64 ) { /* flush the buffer */
        transform( hd.buf );
        hd.count = 0;
        hd.nblocks++;
    }
    if( !inbuf )
        return;
    if( hd.count ) {
        for( ; inlen && hd.count < 64; inlen-- )
            hd.buf[hd.count++] = *inbuf++;
        Write( NULL, 0 );
        if( !inlen )
            return;
    }

    while( inlen >= 64 ) {
        transform( inbuf );
        hd.count = 0;
        hd.nblocks++;
        inlen -= 64;
        inbuf += 64;
    }
    for( ; inlen && hd.count < 64; inlen-- )
        hd.buf[hd.count++] = *inbuf++;
}

Hash1& Hash1::operator<<(std::string data)
{
    Write((unsigned char*)data.c_str(),data.size());
    return *this;
}

/* The routine final terminates the computation and
 * returns the digest.
 * The handle is prepared for a new cycle, but adding bytes to the
 * handle will the destroy the returned buffer.
 * Returns: 20 bytes representing the digest.
 */
void Hash1::Final()
{
    u32 t, msb, lsb;
    unsigned char *p;

    Write(NULL, 0); /* flush */;

    t = hd.nblocks;
    /* multiply by 64 to make a byte count */
    lsb = t << 6;
    msb = t >> 26;
    /* add the count */
    t = lsb;
    if( (lsb += hd.count) < t )
        msb++;
    /* multiply by 8 to make a bit count */
    t = lsb;
    lsb <<= 3;
    msb <<= 3;
    msb |= t >> 29;

    if( hd.count < 56 ) { /* enough room */
        hd.buf[hd.count++] = 0x80; /* pad */
        while( hd.count < 56 )
            hd.buf[hd.count++] = 0;  /* pad */
    }
    else { /* need one extra block */
        hd.buf[hd.count++] = 0x80; /* pad character */
        while( hd.count < 64 )
            hd.buf[hd.count++] = 0;
        Write( NULL, 0);  /* flush */;
        memset(hd.buf, 0, 56 ); /* fill next block with zeroes */
    }
    /* append the 64 bit count */
    hd.buf[56] = msb >> 24;
    hd.buf[57] = msb >> 16;
    hd.buf[58] = msb >>  8;
    hd.buf[59] = msb      ;
    hd.buf[60] = lsb >> 24;
    hd.buf[61] = lsb >> 16;
    hd.buf[62] = lsb >>  8;
    hd.buf[63] = lsb      ;
    transform( hd.buf );

    p = hd.buf;
#ifdef BIG_ENDIAN_HOST
#define X(a) do { *(u32*)p = hd.h##a ; p += 4; } while(0)
#else /* little endian */
#define X(a) do { *p++ = hd.h##a >> 24; *p++ = hd.h##a >> 16; \
        *p++ = hd.h##a >> 8; *p++ = hd.h##a; } while(0)
#endif
    X(0);
    X(1);
    X(2);
    X(3);
    X(4);
#undef X
    //Hash1 operation finally
    bFinal=true;
}

unsigned char* Hash1::GetHash1()
{
    if(!bFinal)
        Final();
    return hd.buf;
}

#pragma endregion

//-----------------------------------------------------------------------------------Chars
#pragma region Chars

Chars::Chars(){}
Chars::~Chars(){}

//url编码
size_t Chars::url_encode(const char *url, char *buf, size_t len)
{
	if (!buf || !url)
		return 0;
	
	const char *p;
	size_t x = 0;
	const char urlunsafe[] = "\r\n \"#%&+:;<=>?@[\\]^`{|}";
	const char hex[] = "0123456789ABCDEF";
	len--;

	for (p = url; *p; p++) {
		if (x >= len)
			break;

		if (*p < ' ' || *p > '~' || strchr(urlunsafe, *p)) {
			if ((x + 3) >= len)
				break;

			buf[x++] = '%';
			buf[x++] = hex[*p >> 4];
			buf[x++] = hex[*p & 0x0f];
		} else {
			buf[x++] = *p;
		}
	}
	buf[x] = '\0';

	return x;
}

//url解码
char* Chars::url_decode(char *s)
{
	char *o;
	unsigned int tmp;

	for (o = s; *s; s++, o++) {
		if (*s == '%' && strlen(s) > 2 && sscanf(s + 1, "%2x", &tmp) == 1) {
			*o = (char) tmp;
			s += 2;
		} else {
			*o = *s;
		}
	}
	*o = '\0';
	return s;
}

#pragma endregion

