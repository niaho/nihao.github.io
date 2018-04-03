/* spModExp.c */

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

static int spModExpK(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T n, DIGIT_T d);
static int spModExpB(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T n, DIGIT_T d);

int spModExp(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T n, DIGIT_T d)
{
	return spModExpB(exp, x, n, d);
}

static int spModExpK(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T n, DIGIT_T d)
{	/*	Computes exp = x^n mod d */
	/*	Ref: Knuth Vol 2 Ch 4.6.3 p 462 Algorithm A
	*/
	DIGIT_T y = 1;		/* Step A1. Initialise */

	while (n > 0)
	{							/* Step A2. Halve N */
		if (n & 0x1)			/* If odd */
			spModMult(&y, y, x, d);	/*   Step A3. Multiply Y by Z */	
		
		n >>= 1;					/* Halve N */
		if (n > 0)				/* Step A4. N = 0? Y is answer */
			spModMult(&x, x, x, d);	/* Step A5. Square Z */
	}

	*exp = y;
	return 0;
}

static int spModExpB(DIGIT_T *exp, DIGIT_T x, 
			DIGIT_T e, DIGIT_T m)
{	/*	Computes exp = x^e mod m */
	/*	Binary left-to-right method
	*/
	DIGIT_T mask;
	DIGIT_T y;	/* Temp variable */

	/* Find most significant bit in e */
	for (mask = HIBITMASK; mask > 0; mask >>= 1)
	{
		if (e & mask)
			break;
	}

	y = x;
	/* For j = k-2 downto 0 step -1 */
	for (mask >>= 1; mask > 0; mask >>= 1)
	{
		spModMult(&y, y, y, m);		/* Square */
		if (e & mask)
			spModMult(&y, y, x, m);	/* Multiply */
	}

	*exp = y;
	return 0;
}


