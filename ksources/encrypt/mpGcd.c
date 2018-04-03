/* mpGcd.c */

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

int mpGcd(DIGIT_T g[], DIGIT_T x[], DIGIT_T y[], unsigned int ndigits)
{	
	/* Computes g = gcd(x, y) */
	/* Ref: Schneier  */

	/*	NB This function requires temp storage 
	*/
	DIGIT_T yy[MAX_DIG_LEN], xx[MAX_DIG_LEN];
	
	mpSetZero(yy, MAX_DIG_LEN);
	mpSetZero(xx, MAX_DIG_LEN);
	mpSetEqual(xx, x, ndigits);
	mpSetEqual(yy, y, ndigits);

	mpSetEqual(g, yy, ndigits);		/* g = y */
	
	while (!mpIsZero(xx, ndigits))	/* while (x > 0) */
	{
		mpSetEqual(g, xx, ndigits);	/* g = x */
		mpModulo(xx, yy, ndigits, xx, ndigits);	/* x = y mod x */
		mpSetEqual(yy, g, ndigits);	/* y = g; */
	}

	mpSetZero(xx, ndigits);
	mpSetZero(yy, ndigits);

	return 0;	/* gcd is in g */
}
