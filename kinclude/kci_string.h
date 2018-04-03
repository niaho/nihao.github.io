#ifndef __KCI_STRING_H__
#define __KCI_STRING_H__
#include "kci_types.h"

/*将16位整数字节序反转*/
#define SG_SWAP2(_p_) do{\
	char _ch_,*_p_ch_;\
	_p_ch_ = (char*)(_p_);\
	_ch_ = _p_ch_[0];\
	_p_ch_[0]=_p_ch_[1];\
	_p_ch_[1] = _ch_;\
		}while(0)

/*将32位整数字节序反转*/
#define SG_SWAP4(_p_) do{\
	char _ch_,*_p_ch_;\
	_p_ch_ = (char*)(_p_);\
	_ch_ = _p_ch_[0];\
	_p_ch_[0]=_p_ch_[3];\
	_p_ch_[3] = _ch_;\
	_ch_=_p_ch_[1];\
	_p_ch_[1]=_p_ch_[2];\
	_p_ch_[2] = _ch_;\
		}while(0)

/*将64位整数字节序反转*/
#define SG_SWAP8(_p_) do{\
	char _ch_,*_p_ch_;\
	_p_ch_ = (char*)(_p_);\
	_ch_ = _p_ch_[0];\
	_p_ch_[0]=_p_ch_[7];\
	_p_ch_[7] = _ch_;\
	_ch_=_p_ch_[1];\
	_p_ch_[1]=_p_ch_[6];\
	_p_ch_[6] = _ch_;\
	_ch_ = _p_ch_[2];\
	_p_ch_[2]=_p_ch_[5];\
	_p_ch_[5] = _ch_;\
	_ch_=_p_ch_[3];\
	_p_ch_[3]=_p_ch_[4];\
	_p_ch_[4] = _ch_;\
		}while(0)

#ifdef _BIG_ENDIAN_
#define  W_C(c) c
#define _xg_ntoh2(_p_) 
#define _xg_hton2(_p_) 
#define _xg_ntoh4(_p_) 
#define _xg_hton4(_p_) 
#define _xg_ntoh8(_p_) 
#define _xg_hton8(_p_) 
#else
#define  W_C(c)  (((c)>>8)|((c)<<8)) 
/*将16位整数转换为网络序*/
#define _xg_ntoh2(_p_) SG_SWAP2(_p_) 
#define _xg_hton2(_p_) SG_SWAP2(_p_) 
#define _xg_ntoh4(_p_) SG_SWAP4(_p_) 
#define _xg_hton4(_p_) SG_SWAP4(_p_) 
#define _xg_ntoh8(_p_) SG_SWAP8(_p_) 
#define _xg_hton8(_p_) SG_SWAP8(_p_) 
#endif

inline static void i4toa(int32 i, char *p_b)
{
	char	*p_e;
	char	ch;
	if (i<0)
	{
		*p_b++ = '-';
		p_e = p_b;
		do{
			*p_e++ = '0' - (char)(i % 10);
			i /= 10;
		} while (i);
	}
	else
	{
		p_e = p_b;
		do{
			*p_e++ = '0' + (char)(i % 10);
			i /= 10;
		} while (i);
	}
	*p_e = 0x0;
	p_e--;
	while (p_e>p_b)
	{
		ch = *p_b;
		*p_b = *p_e;
		*p_e = ch;
		p_b++;
		p_e--;
	}
}

inline static void ui4toa(uint32 i, char *p_b)
{
	char	*p_e;
	char	ch;

	p_e = p_b;
	do{
		*p_e++ = '0' + (char)(i % 10);
		i /= 10;
	} while (i);
	*p_e = 0x0;
	p_e--;
	while (p_e>p_b)
	{
		ch = *p_b;
		*p_b = *p_e;
		*p_e = ch;
		p_b++;
		p_e--;
	}
}

inline static void i8toa(int64 i, char *p_b)
{
	char	*p_e;
	char	ch;
	if (i<0)
	{
		*p_b++ = '-';
		p_e = p_b;
		do{
			*p_e++ = '0' - (char)(i % 10);
			i /= 10;
		} while (i);
	}
	else
	{
		p_e = p_b;
		do{
			*p_e++ = '0' + (char)(i % 10);
			i /= 10;
		} while (i);
	}
	*p_e = 0x0;
	p_e--;
	while (p_e>p_b)
	{
		ch = *p_b;
		*p_b = *p_e;
		*p_e = ch;
		p_b++;
		p_e--;
	}
}
#define r4toa(r,b) sprintf(b,"%.8g",r)
#define r8toa(r,b) sprintf(b,"%.16g",r)
inline static void i4toh(int32 i, char *p_b)
{
	uchar	*p_ch = (uchar*)&i;
	uchar	ch;

	_xg_hton4(&i);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
}

inline static void ui4toh(uint32 i, char *p_b)
{
	uchar	*p_ch = (uchar*)&i;
	uchar	ch;

	_xg_hton4(&i);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
}

inline static void i8toh(int64 i, char *p_b)
{
	uchar	*p_ch = (uchar*)&i;
	uchar	ch;

	_xg_hton8(&i);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
}

inline static void ui8toh(uint64 i, char *p_b)
{
	uchar	*p_ch = (uchar*)&i;
	uchar	ch;

	_xg_hton8(&i);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch;
	ch >>= 4;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
	ch = *p_ch++;
	ch &= 0x0f;
	*p_b++ = ch<10 ? ch + '0' : ch + ('A' - 10);
}
inline static int astrcmp(const char *str1, const char *str2)
{
	char ch1, ch2;

	if ((str1 != 0x0) && (str2 == 0x0))
		return 1;
	if ((str1 == 0x0) && (str2 != 0x0))
		return -1;

	while ((ch1 = (*str1)) != 0x0 && (ch2 = (*str2)) != 0x0)
	{
		if (ch1>ch2)
			return 1;
		else if (ch1<ch2)
			return -1;
		str1++;
		str2++;
	}
	if ((*str1) != 0x0)
		return 1;
	else if ((*str2) != 0x0)
		return -1;
	else
		return 0;
}

inline static int astricmp(const char *str1, const char *str2)
{
	char ch1, ch2;
	if (!str1)
	{
		if (!str2)
			return 1;
		else
			return 0;
	}
	else
	{
		if (!str2)
			return 0;
	}
	while ((ch1 = (*str1)) != 0x0 && (ch2 = (*str2)) != 0x0)
	{
		if (ch1 >= 'a' && ch1 <= 'z')
			ch1 -= 0x20;
		if (ch2 >= 'a' && ch2 <= 'z')
			ch2 -= 0x20;
		if (ch1>ch2)
			return 1;
		else if (ch1<ch2)
			return -1;
		str1++;
		str2++;
	}
	if ((*str1) != 0x0)
		return 1;
	else if ((*str2) != 0x0)
		return -1;
	else
		return 0;
}

inline static int astrnicmp(const char *str1, const char *str2, int len)
{
	char ch1, ch2;
	int  n = 0;

	while ((ch1 = (*str1)) != 0x0 && (ch2 = (*str2)) != 0x0 && n<len)
	{
		if (ch1 >= 'a' && ch1 <= 'z')
			ch1 -= 0x20;
		if (ch2 >= 'a' && ch2 <= 'z')
			ch2 -= 0x20;
		if (ch1>ch2)
			return 1;
		else if (ch1<ch2)
			return -1;
		str1++;
		str2++;
		n++;
	}
	if (n == len)
		return 0;
	else if ((*str1) != 0x0)
		return 1;
	else if ((*str2) != 0x0)
		return -1;
	else
		return 0;
}
inline static int astrnquery(const char *pSrc, const char *pDst)
{
	char ch1, ch2;
	int  i = 0;
	int j = 0;
	while ((ch1 = (*pSrc)) != 0x0 )
	{
		if (ch1 >= 'a' && ch1 <= 'z')
			ch1 -= 0x20;
		pSrc++;
		i++;
		if (ch1 != pDst[0])
			continue;
		j = 1;
		while ((ch2 = pDst[j]) != 0x0 && (ch1 = (*pSrc)) != 0x0)
		{
			if (ch1 >= 'a' && ch1 <= 'z')
				ch1 -= 0x20;
			if (ch2 >= 'a' && ch1 <= 'z')
				ch2 -= 0x20;
			if (ch1 != ch2)
				break;
			pSrc++;
			j++;
			i++;
		}
		if (pDst[j] == 0x0)
			return i;
	}
	return -1;
}

static inline  SizeT __strlen(const char* cstr)
{
	SizeT	len = 0;
	char *str = (char*)cstr;
	while (*str++)
		len++;
	return len;
}

/* 从字串的尾部开始搜索给定字符,若未找到,则返回空,否则返回字符所在位置的指针*/
static inline char  *strcharr(char *str, char ch)
{
	char *p_ch = str + __strlen((const char*)str);
	while (p_ch >= str)
	{
		if ((*p_ch) == ch)
			return p_ch;
		p_ch--;
	}
	return NULL;
}

uint strn_hash(const char *key, int len);
uint str_hash(const char *key);
char *strnmov(char *dst, const char *src, uint n);
bool is_prefix(char *str, char *head_str);
char *fix_str(char *to, char *from, int length);
char *dupp_str(char *from, int length);
bool empty_str(char *from, int length);
char *strncpy1(char *buff, char *str, int buff_len);
void srtncpy2(char *to, int max_length, char *from, int length);
int kvsnprintf(char *to, size_t n, const char* fmt, va_list ap);
#endif
