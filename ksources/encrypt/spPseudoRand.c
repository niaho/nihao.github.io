/* spPseudoRand */

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
#include <time.h>
#include "kci_bigint.h"

DIGIT_T spPseudoRand(DIGIT_T lower, DIGIT_T upper)
{	/*	Returns a pseudo-random digit.
		Handles own seeding usings time
		NOT for cryptographically-secure random numbers.
	*/
	static unsigned seeded = 0;
	unsigned int i;
	double f;

	if (!seeded)
	{
		/* seed with system time */
		long	clk = clock();
		time_t	t = time(NULL);

		srand((unsigned int)(clk+t));
		//srand((unsigned)time(NULL));
		/* Throw away a random few to avoid similar starts */
		i = rand() & 0xFF;
		while (i--)
			rand();
		seeded = 1;
	}
	f = (double)rand() / RAND_MAX * upper;
	return (lower + (DIGIT_T)f);
}
