/* mpHalfDiv.c */

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

/* Two alternative methods - Knuth and Zimmerman */
DIGIT_T mpHalfDivK(DIGIT_T q[], DIGIT_T u[], HALF_DIGIT_T v, 
				   unsigned int ndigits);
DIGIT_T mpHalfDivZ(DIGIT_T q[], DIGIT_T a[], HALF_DIGIT_T d, 
				   unsigned int ndigits);

DIGIT_T mpHalfDiv(DIGIT_T q[], DIGIT_T u[], HALF_DIGIT_T v, 
				   unsigned int ndigits)
{
	return mpHalfDivK(q, u, v, ndigits);
}

DIGIT_T mpHalfDivK(DIGIT_T q[], DIGIT_T u[], HALF_DIGIT_T v, 
				   unsigned int ndigits)
{
	/*	Calculates quotient q = u div v
		Returns remainder r = u mod v
		where q, u are multiprecision integers of ndigits each
		and d, v are single precision digits
		
		d must be <= MAX_HALF_DIGIT

		Ref: Knuth Vol 2 Ch 4.3.1 Exercise 16 p625
	*/
	unsigned int j;
	DIGIT_T t, r, qHigh, qLow;

	if (ndigits == 0) return 0;
	if (v == 0)	return 0;	/* Divide by zero error */

	/* Step S1. */
	r = 0;
	j = ndigits;
	while (j--)
	{
		/* Step S2. */
		t = TOHIGH(r) | HIHALF(u[j]);
		qHigh = t / v;
		r = t - qHigh * v;

		t = TOHIGH(r) | LOHALF(u[j]);
		qLow = t / v;
		r = t - qLow * v;

		q[j] = TOHIGH(qHigh) | qLow;
	}

	return r;
}

DIGIT_T mpHalfDivZ(DIGIT_T q[], DIGIT_T a[], HALF_DIGIT_T d, 
				   unsigned int ndigits)
{
	/*	Calculates quotient q = a div d
		Returns remainder r = a mod d
		where q, a are multiprecision integers of ndigits each
		and d, r are single precision digits
		usings bit-by-bit method from left to right.

		d must be <= MAX_HALF_DIGIT

		Ref: Principles in PGP by Phil Zimmermann
	*/

	DIGIT_T mask = HIBITMASK;
	DIGIT_T r = 0;
	unsigned int i;

	if (ndigits == 0) return 0;

	/* Initialise quotient */
	for (i = 0; i < ndigits; i++)
		q[i] = 0;
	
	while (ndigits)
	{	/* Work from left to right */

		r <<= 1;	/* Multiply remainder by 2 */

		/* Look at current bit */
		if (a[ndigits-1] & mask)
			r++;
		if (r >= d)
		{
			r -= d;
			q[ndigits-1] |= mask;
		}

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

