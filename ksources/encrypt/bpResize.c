/* bpResize.c */

/******************* SHORT COPYRIGHT NOTICE*************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 1.0 originally written by David Ireland,
copyright (c) 2001 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with
this library. To obtain a copy send an email to
<code@di-mgt.com.au> or visit <www.di-mgt.com.au/crypto.html>.
This notice must be retained in any copy.
****************** END OF COPYRIGHT NOTICE*************************/

#include <stdlib.h> 
#include "kci_bigint.h"

BIGD_T *bpResize(BIGD_T *b, unsigned int newsize)
{
	/* Return ptr to re-sized BIGD structure 
	   with newsize digits */

	unsigned int i;

	/* Check just in case NULL */
	if (b == NULL)
		return bpInit(b, newsize);

	/* Increase (we assume) size of digit array */
	b->digits = (DIGIT_T *)realloc(b->digits, newsize * sizeof(DIGIT_T));
	
	/* Check for failure */
	if (!b->digits)
		return NULL;

	b->maxdigits = newsize;	/* Remember new max */

	/* Make sure new digits are zero */
	for (i = b->ndigits; i < newsize; i++)
		b->digits[i] = 0;

	return b;
}

