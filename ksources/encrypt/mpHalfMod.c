/* mpHalfMod.c */

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

DIGIT_T mpHalfMod(DIGIT_T a[], HALF_DIGIT_T d, unsigned int ndigits)
{
	/*	Calculates r = a mod d
		where a is a multiprecision integer of ndigits
		and r, d are single precision digits
		usings bit-by-bit method from left to right.

		Ref: Derived from principles in PGP by Phil Zimmermann
		Note: This method will only work until r <<= 1 overflows.
		i.e. for d < HIBITMASK, but we keep HALF_DIGIT
		limit for safety 
		(and also because we don't have a 31/32nds_digit).
	*/

	DIGIT_T mask = HIBITMASK;
	DIGIT_T r = 0;

	if (ndigits == 0) return 0;

	while (ndigits)
	{	/* Work from left to right */

		r <<= 1;	/* Multiply remainder by 2 */

		/* Look at current bit */
		if (a[ndigits-1] & mask)
			r++;
		if (r >= d)
			r -= d;

		/* Move to next bit */
		if (mask == 1)
		{
			mask = HIBITMASK;
			ndigits--;
		}
		else
			mask >>= 1;
	}
	return r;
}

