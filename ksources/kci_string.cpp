#include <string.h>
#include <stdio.h>
#if defined(LINUX)
#include <stdarg.h>
#endif
#include "kci_string.h"

/*测试head_str是否是str的头部*/
bool	is_prefix(char *str, char *head_str)
{
	if (strlen(str)>strlen(head_str))
	{
		if (strncmp(str, head_str, strlen(head_str)) == 0)
			return true;
	}
	return false;
}

/*
@type    : myodbc internal
@purpose : change a string + length to a zero terminated string
*/

char *fix_str(char *to, char *from, int length)
{
	if (!from)
		return "";
	if (length == -3)
		return from;
	strncpy1(to, from, length);
	return to;
}

/*
@type    : myodbc internal
@purpose : duplicate the string
*/

char *dupp_str(char *from, int length)
{
	char *to;
	if (!from)
#ifdef __WIN__
		return _strdup("");
#else
		return strdup("");
#endif
	if (length == -3)
		length = strlen(from);
	if ((to = (char*)malloc(length + 2)))
	{
		memcpy(to, from, length);
		to[length] = 0;
	}
	return to;
}

/*
@type    : myodbc internal
@purpose : returns 1 if from is a null pointer or a empty string
*/

bool empty_str(char *from, int length)
{
	if (!from)
		return 1;
	if (length == -3)
		return from[0] == 0;
	return !length;
}

/*将字串拷贝到目标字串中去，若源串长度大于目标，则拷贝一部分*/
char *strncpy1(char *buff, char *str, int buff_len)
{
	if (((int)strlen(str))<buff_len || buff_len<0)
	{
		strcpy(buff, str);
		return buff + strlen(str);
	}
	else
	{
		strncpy(buff, str, buff_len);
		buff[buff_len] = 0x0;
		return buff + buff_len;
	}
}

char *strnmov(char *dst, const char *src, uint n)
{
	while (n-- != 0) {
		if (!(*dst++ = *src++)) {
			return (char*)dst - 1;
		}
	}
	return dst;
}

void srtncpy2(char *to, int max_length, char *from, int length)
{
	if (from)
	{
		if (length == -3)
			length = max_length - 1;
		strncpy1(to, from, length);
	}
}


int kvsnprintf(char *to, size_t n, const char* fmt, va_list ap)
{
	char *start = to, *end = to + n - 1;
	uint length, width, pre_zero, have_long;

	for (; *fmt; fmt++)
	{
		if (fmt[0] != '%')
		{
			if (to == end)			/*End of buffer*/
				break;
			*to++ = *fmt;			/*Copy ordinary char*/
			continue;
		}
		fmt++;					/*skip '%'*/
		/*Read max fill size (only used with %d and %u)*/
		if (*fmt == '-')
			fmt++;
		length = width = pre_zero = have_long = 0;
		if (*fmt == '*')
		{
			fmt++;
			length = va_arg(ap, int);
		}
		else
			for (; isdigit(*fmt); fmt++)
			{
				length = length * 10 + (uint)(*fmt - '0');
				if (!length)
					pre_zero = 1;			/*first digit was 0*/
			}
		if (*fmt == '.')
		{
			fmt++;
			if (*fmt == '*')
			{
				fmt++;
				width = va_arg(ap, int);
			}
			else
				for (; isdigit(*fmt); fmt++)
					width = width * 10 + (uint)(*fmt - '0');
		}
		else
			width = ~0;
		if (*fmt == 'l')
		{
			fmt++;
			have_long = 1;
		}
		if (*fmt == 's')				/*String parameter*/
		{
			char	*par = va_arg(ap, char *);
			uint plen, left_len = (uint)(end - to) + 1;
			if (!par) par = (char*)"(null)";
			plen = (uint)strlen(par);
			set_if_smaller(plen, width);
			if (left_len <= plen)
				plen = left_len - 1;
			to = strnmov(to, par, plen);
			continue;
		}
		else if (*fmt == 'd' || *fmt == 'u' || *fmt == 'x' || *fmt == 'l')	/*Integer parameter*/
		{
			register int64 larg;
			register int32 iarg;
			uint res_length, to_length;
			char *store_start = to, *store_end;
			char buff[32];

			if ((to_length = (uint)(end - to)) < 16 || length)
				store_start = buff;
			if (have_long)
			{
				larg = va_arg(ap, int64);
				i8toa(larg, store_start);
				store_end = store_start + strlen(store_start);
			}
			else
			{
				if (*fmt == 'd')
					iarg = va_arg(ap, int);
				else
					iarg = va_arg(ap, int);
				if (*fmt == 'd')
				{
					i4toa(iarg, store_start);
					store_end = store_start + strlen(store_start);
				}
				else
				{
					if (*fmt == 'u')
					{
						ui4toa(iarg, store_start);
						store_end = store_start + strlen(store_start);
					}
					else
					{
						i4toh(iarg, store_start);
						store_end = store_start + strlen(store_start);
					}
				}
			}

			if ((res_length = (uint)(store_end - store_start)) > to_length)
				break;
			/*If %#d syntax was used, we have to pre-zero/pre-space the string*/
			if (store_start == buff)
			{
				length = length>to_length?to_length:length;
				if (res_length < length)
				{
					uint diff = (length - res_length);
					memset(to, diff, pre_zero ? '0' : ' ');
					to += diff;
				}
				memcpy(to, store_start, res_length);
			}
			to += res_length;
			continue;
		}
		else if (*fmt == 'c')                       /*Character parameter*/
		{
			register int larg;
			if (to == end)
				break;
			larg = va_arg(ap, int);
			*to++ = (char)larg;
			continue;
		}
		else if (*fmt == 'p')
		{
			void *p = va_arg(ap, void*);
			if (to + 16 > end)
				break;
			sprintf(to, "%p", p);
			to += strlen(to);
			continue;
		}

		/*We come here on '%%', unknown code or too long parameter*/
		if (to == end)
			break;
		*to++ = '%';				/*% used as % or unknown code*/
	}
	*to = '\0';					/*End of errmessage*/
	return (uint)(to - start);
}

