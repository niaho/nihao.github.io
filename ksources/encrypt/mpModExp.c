/* mpModExp.c */

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


int mpModExp(DIGIT_T y[], DIGIT_T x[], 
			DIGIT_T e[], DIGIT_T m[], unsigned int ndigits)
{	/*	Computes y = x^e mod m */
	/*	Binary left-to-right method
	*/
	DIGIT_T mask;
	unsigned int n;
	
	if (ndigits == 0) return -1;

	/* Find second-most significant bit in e */
	n = mpSizeof(e, ndigits);
	for (mask = HIBITMASK; mask > 0; mask >>= 1)
	{
		if (e[n-1] & mask)
			break;
	}
	mpNEXTBITMASK(mask, n);

	/* Set y = x */
	mpSetEqual(y, x, ndigits);

	/* For bit j = k-2 downto 0 step -1 */
	while (n)
	{
		mpModMult(y, y, y, m, ndigits);		/* Square */
		if (e[n-1] & mask)
			mpModMult(y, y, x, m, ndigits);	/* Multiply */
		
		/* Move to next bit */
		mpNEXTBITMASK(mask, n);
	}

	return 0;
}



