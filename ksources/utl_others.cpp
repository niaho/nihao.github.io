#if defined(LINUX)
#include <setjmp.h>
#include <signal.h>
#endif
#include "kci_mem.h"
#include "kci_defs.h"
#include "kci_utils.h"

#define PRIME1		37		
#define PRIME2		1048583

union int_float_bits {
	sb4 int_bits;
	float float_bits;
};

union int_double_bits {
	sb8 int_bits;
	double float_bits;
};

float intBitsToFloat(sb4 x)
{
	union int_float_bits bits;
	bits.int_bits = x;
	return bits.float_bits;
}

double intBitsToDouble(sb8 x)
{
	union int_float_bits bits;
	bits.int_bits = x;
	return bits.float_bits;
}

int n_tu(int number, int count)
{
	int result = 1;
	while (count-- > 0)
		result *= number;

	return result;
}

/***Convert float to string***/
void float_to_string(float f, char r[])
{
	long long int length, length2, i, number, position, sign;
	float number2;

	sign = -1;   // -1 == positive number
	if (f < 0)
	{
		sign = '-';
		f *= -1;
	}
	number2 = f;
	number = f;
	length = 0;  // size of decimal part
	length2 = 0; //  size of tenth

	/* calculate length2 tenth part*/
	while ((number2 - (float)number) != 0.0 && !((number2 - (float)number) < 0.0))
	{
		number2 = f * (n_tu(10.0, length2 + 1));
		number = number2;

		length2++;
	}

	/* calculate length decimal part*/
	for (length = (f > 1) ? 0 : 1; f > 1; length++)
		f /= 10;

	position = length;
	length = length + 1 + length2;
	number = number2;
	if (sign == '-')
	{
		length++;
		position++;
	}

	for (i = length; i >= 0; i--)
	{
		if (i == (length))
			r[i] = '\0';
		else if (i == (position))
			r[i] = '.';
		else if (sign == '-' && i == 0)
			r[i] = '-';
		else
		{
			r[i] = (number % 10) + '0';
			number /= 10;
		}
	}
}
void double_to_string(double f, char r[])
{
	long long int length, length2, i, number, position, sign;
	double number2;

	sign = -1;   // -1 == positive number
	if (f < 0)
	{
		sign = '-';
		f *= -1;
	}
	number2 = f;
	number = f;
	length = 0;  // size of decimal part
	length2 = 0; //  size of tenth

	/* calculate length2 tenth part*/
	while ((number2 - (double)number) != 0.0 && !((number2 - (double)number) < 0.0))
	{
		number2 = f * (n_tu(10.0, length2 + 1));
		number = number2;

		length2++;
	}

	/* calculate length decimal part*/
	for (length = (f > 1) ? 0 : 1; f > 1; length++)
		f /= 10;

	position = length;
	length = length + 1 + length2;
	number = number2;
	if (sign == '-')
	{
		length++;
		position++;
	}

	for (i = length; i >= 0; i--)
	{
		if (i == (length))
			r[i] = '\0';
		else if (i == (position))
			r[i] = '.';
		else if (sign == '-' && i == 0)
			r[i] = '-';
		else
		{
			r[i] = (number % 10) + '0';
			number /= 10;
		}
	}
}

uint strn_hash(const char *key, int len)
{
	int  h = 0;
	if (!key)
	{
		srand(time(NULL));
		h = rand();
	}
	else
	while (len--)
	{
		if (*key != 0x0)
			h = (h * PRIME1) ^ (*key++);
		else
			key++;
	}
	h %= PRIME2;
	return (uint32)h;
}

uint str_hash(const char *key)
{
	int	 h = 0;
	if (!key)
	{
	//	srand(time(NULL));
		h = 0;// rand();
	}		
	else
	while (*key)
	{
		h = (h * PRIME1) ^ (*key++);
	}
	h %= PRIME2;

	return (uint)h;
}

uint str_upper_hash(const char *key)
{
	int	 h = 0;
	if (!key)
	{
		srand(time(NULL));
		h = rand();
	}
	else
	while (*key)
	{
		char ch = *key++;
		if (ch >= 'a' && ch <= 'z')
			ch -= ('a' - 'A');
		h = (h * PRIME1) ^ ch;
	}
	h %= PRIME2;

	return (uint32)h;
}

void gen_randstr(char * string, sb4 length)
{
	int flag, i;
	
	for (i = 0; i < length - 1; i++)
	{
		flag = rand() % 3;
		switch (flag)
		{
		case 0:
			string[i] = 'A' + rand() % 26;
			break;
		case 1:
			string[i] = 'a' + rand() % 26;
			break;
		case 2:
			string[i] = '0' + rand() % 10;
			break;
		default:
			string[i] = 'x';
			break;
		}
	}
	string[length - 1] = '\0';
	return ;
}

sword KCIUnicodeToCharSet(void  *envhp, OraText *dst, size_t dstlen,
	const ub2 *src, size_t srclen, size_t *rsize)
{
	switch ((int)src)
	{
	case KCI_GBK:
		*rsize = 3;
		memcpy(dst, "GBK", *rsize);
		break;
	case KCI_GB18030:
		*rsize = 7;
		memcpy(dst, "GB18030", *rsize);
		break;
	case KCI_UTF16ID:
	case KCI_UTF8:
	default:
		*rsize = 4;
		memcpy(dst, "UTF8", *rsize);
		break;
	}
	return KCI_SUCCESS;
};

sword KCICharSetToUnicode(void  *envhp, ub2 *dst, size_t dstlen,
	const OraText *src, size_t  srclen, size_t *rsize)
{
	int i= astrcmp((cstr )src, "GBK");
	if (!i)
	{
		*dst = KCI_GBK;
		return KCI_SUCCESS;
	}
	i = astrcmp((cstr)src, "GB18030");
	if (!i)
	{
		*dst = KCI_GB18030;
		return KCI_SUCCESS;
	}
	*dst = KCI_UTF8;		
	return KCI_SUCCESS;
};


#if defined(LINUX)
static volatile sig_atomic_t segv_flag;
static jmp_buf jmp_env;


static void segv_handler(int dummy)
{
	segv_flag = 1;
	longjmp(jmp_env, 1);
}

/*
********************************************
* Pre: A pointer and it's length
* Post: Return true is the pointer is bad
*	   or false if otherwise
********************************************
*/

int IsBadWritePtr(void * ptr_buffer, unsigned long buffer_size)
{
	struct sigaction oldaction, newaction;
	volatile char dummy;
	char *ptr = (char *)ptr_buffer;

	if (!buffer_size)
		return 0;
	/* storing the old signal environment and trapping SIGSEGV */
	newaction.sa_handler = segv_handler;
	newaction.sa_flags = 0;
	sigemptyset(&newaction.sa_mask);

	sigaction(SIGSEGV, &newaction, &oldaction);
	sigaction(SIGBUS, &newaction, &oldaction);

	segv_flag = 0;

	/* Storing the process state so if any failure happens
	we can restore it to the original state */
	if (setjmp(jmp_env) == 0)
	{
		/* testing the pointer: only the first and the end are 	   needed here since any failure to any of this would 	     indicate there would an error on the entire range */
		dummy = ptr[0];
		dummy = ptr[buffer_size - 1];
                char c = ptr[0];
                ptr[0] = ~c;
                ptr[0] = c;  
	}

	/* restoring the original signal environment */
	sigaction(SIGSEGV, &oldaction, NULL);
	sigaction(SIGBUS, &oldaction, NULL);
	return segv_flag;
}


#endif
