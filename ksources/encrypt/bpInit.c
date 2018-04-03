/* bpInit.c */

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

#include <stdlib.h>
#include "kci_bigint.h"

BIGD_T *bpInit(BIGD_T *b, unsigned int precision)
{
	/* Return ptr to initialised BIGD structure 
	   with precision digits */

	/* Allocate required memory */
	b->digits = (DIGIT_T *)calloc(precision, sizeof(DIGIT_T));
	/* Check for failure */
	if (!b->digits)
		return NULL;
	b->ndigits = 0;	/* Nothing set yet */
	b->maxdigits = precision;	/* Remember max allocated */
	return b;
}

