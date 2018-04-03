/* mpShortMod.c */

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

DIGIT_T mpShortMod(DIGIT_T a[], DIGIT_T d, unsigned int ndigits)
{
	/*	Calculates r = a mod d
		where a is a multiprecision integer of ndigits
		and r, d are single precision digits
		usings bit-by-bit method from left to right.

		Use remainder from divide function.
	*/

	DIGIT_T q[MAX_DIG_LEN * 2];
	DIGIT_T r = 0;

	r= mpShortDiv(q, a, d, ndigits);

	mpSetZero(q, ndigits);

	return r;
}

