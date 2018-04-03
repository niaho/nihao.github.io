/* mpShiftRight.c */

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

#include "kci_bigint.h"

DIGIT_T mpShiftRight(DIGIT_T a[], DIGIT_T b[],
	unsigned int x, unsigned int ndigits)
{	/* Computes a = b >> x */
	unsigned int i, y;
	DIGIT_T mask, carry, nextcarry;

	/* Check input  - NB unspecified result */
	if (x >= BITS_PER_DIGIT)
		return 0;

	/* Construct mask */
	mask = 0x1;
	for (i = 1; i < x; i++)
	{
		mask = (mask << 1) | mask;
	}
	if (x == 0) mask = 0x0;
	
	y = BITS_PER_DIGIT - x;
	carry = 0;
	i = ndigits;
	while (i--)
	{
		nextcarry = (b[i] & mask) << y;
		a[i] = b[i] >> x | carry;
		carry = nextcarry;
	}

	return carry;
}
