/* bigdUtils.c */

/******************* SHORT COPYRIGHT EX_NOTICE*************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 1.0 originally written by David Ireland,
copyright (c) 2001 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with
this library. To obtain a copy send an email to
<code@di-mgt.com.au> or visit <www.di-mgt.com.au/crypto.html>.
This notice must be retained in any copy.
****************** END OF COPYRIGHT EX_NOTICE*************************/

#include <stdio.h>
#include "kci_bigint.h"

void mpPrint(DIGIT_T *p, unsigned int len)
{
	while (len--)
	{
		printf("%08x ", p[len]);
	}
}

void mpPrintNL(DIGIT_T *p, unsigned int len)
{
	unsigned int i = 0;

	while (len--)
	{
		if ((i % 8) == 0 && i)
			printf("\n");
		printf("%08x ", p[len]);
		i++;
	}
	printf("\n");
}

void mpPrintTrim(DIGIT_T *p, unsigned int len)
{
	/* Trim leading zeroes */
	while (len--)
	{
		if (p[len] != 0)
			break;
	}
	len++;
	while (len--)
	{
		printf("%08x ", p[len]);
	}
}

void mpPrintTrimNL(DIGIT_T *p, unsigned int len)
{
	unsigned int i = 0;

	/* Trim leading zeroes */
	while (len--)
	{
		if (p[len] != 0)
			break;
	}
	len++;
	while (len--)
	{
		if ((i % 8) == 0 && i)
			printf("\n");
		printf("%08x ", p[len]);
		i++;
	}
	printf("\n");
}

/* Print functions for BigDigit structures */

void bpPrint(BIGD_T *p)
{
	unsigned int i = p->ndigits;
	while (i--)
	{
		printf("%08x ", p->digits[i]);
	}
}

