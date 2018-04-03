/* spModInv.c */

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

int spModInv(DIGIT_T *inv, DIGIT_T u, DIGIT_T v)
{	/*	Computes inv = u^(-1) mod v */
	/*	Ref: Knuth Algorithm X Vol 2 p 342 
		ignoring u2, v2, t2
		and avoiding negative numbers
	*/
	DIGIT_T u1, u3, v1, v3, t1, t3, q, w;
	int bIterations = 1;
	
	/* Step X1. Initialise */
	u1 = 1;
	u3 = u;
	v1 = 0;
	v3 = v;

	while (v3 != 0)	/* Step X2. */
	{	/* Step X3. */
		q = u3 / v3;	/* Divide and */
		t3 = u3 % v3;
		w = q * v1;		/* "Subtract" */
		t1 = u1 + w;
		/* Swap */
		u1 = v1;
		v1 = t1;
		u3 = v3;
		v3 = t3;
		bIterations = -bIterations;
	}

	if (bIterations < 0)
		*inv = v - u1;
	else
		*inv = u1;

	return 0;
}

