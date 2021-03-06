/* bpSubtract.c */

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

int bpSubtract(BIGD_T *w, BIGD_T *u, BIGD_T *v)
{
	/* Compute w = u - v 
	   with full memory management */

	unsigned int dig_size;
	unsigned int nu, nv, nw;
	DIGIT_T borrow;

	nu = u->ndigits;
	nv = v->ndigits;
	nw = w->ndigits;
	/* Make sure u and v are the same size */
	if (nu > nv)
	{
		bpResize(v, nu);
		dig_size = nu;
	}
	else if (nv > nu)
	{
		bpResize(u, nv);
		dig_size = nv;
	}
	else
		dig_size = nu;

	/* Now make sure w is big enough */
	if (nw < dig_size)
	{
		bpResize(w, dig_size);
	}

	/* Finally, do the business */
	borrow = mpSubtract(w->digits, u->digits, v->digits, dig_size);

	/* Make sure we've set the right size for w */
	w->ndigits = mpSizeof(w->digits, w->maxdigits);

	return borrow;
}
