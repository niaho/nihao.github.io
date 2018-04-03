/* bpEqual.c */

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

int bpEqual(BIGD_T *a, BIGD_T *b)
{
	/*	Returns true if a == b, else false
	*/
	unsigned int n;

	if (a->ndigits == 0 || b->ndigits == 0) return -1;

	n = (a->ndigits > b->ndigits ? a->ndigits : b->ndigits);

	while (n--)
	{
		if (a->digits[n] != b->digits[n])
			return 0;	/* False */
	}

	return (!0);	/* True */
}

