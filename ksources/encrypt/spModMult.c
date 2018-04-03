/* spModMult.c */

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

int spModMult(DIGIT_T *a, DIGIT_T x, DIGIT_T y, DIGIT_T m)
{	/*	Computes a = (x * y) mod m */
	
	/* Calc p[2] = x * y */
	DIGIT_T p[2];
	spMultiply(p, x, y);

	/* Then modulo */
	*a = mpShortMod(p, m, 2);
	return 0;
}

