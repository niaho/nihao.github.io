/* mpModulo.c */

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

int mpModulo(DIGIT_T r[], DIGIT_T u[], unsigned int udigits, 
			 DIGIT_T v[], unsigned int vdigits)
{
	/*	Calculates r = u mod v
		where r, v are multiprecision integers of length vdigits
		and u is a multiprecision integer of length udigits.
		r may overlap v.

		Note that r here is only vdigits long, 
		whereas in mpDivide it is udigits long.

		Use remainder from mpDivide function.
	*/

	/* Double-length temp variable for divide fn */
	DIGIT_T qq[MAX_DIG_LEN * 2];
	/* Use a double-length temp for r to allow overlap of r and v */
	DIGIT_T rr[MAX_DIG_LEN * 2];

	/* rr[2n] = u[2n] mod v[n] */
	mpDivide(qq, rr, u, udigits, v, vdigits);

	mpSetEqual(r, rr, vdigits);
	mpSetZero(rr, udigits);
	mpSetZero(qq, udigits);

	return 0;
}

