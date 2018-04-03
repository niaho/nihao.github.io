/* mpShortSub.c */

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

DIGIT_T mpShortSub(DIGIT_T w[], DIGIT_T u[], DIGIT_T v, 
			   unsigned int ndigits)
{
	/*	Calculates w = u - v
		where w, u are multiprecision integers of ndigits each
		and v is a single precision digit.
		Returns borrow: 0 if u >= v, or 1 if v > u.

		Ref: Derived from Knuth Algorithm S.
	*/

	DIGIT_T k;
	unsigned int j;

	k = 0;

	/* Subtract v from first digit of u */
	w[0] = u[0] - v;
	if (w[0] > MAX_DIGIT - v)
		k = 1;
	else
		k = 0;

	/* Subtract borrow from subsequent digits */
	for (j = 1; j < ndigits; j++)
	{
		w[j] = u[j] - k;
		if (w[j] > MAX_DIGIT - k)
			k = 1;
		else
			k = 0;
	}

	return k;
}

