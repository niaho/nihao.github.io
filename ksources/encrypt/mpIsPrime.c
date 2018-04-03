/* mpIsPrime.c */

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

static DIGIT_T SMALL_PRIMES[] = 
{ 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 
  53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107 };
#define N_SMALL_PRIMES sizeof(SMALL_PRIMES)/sizeof(DIGIT_T)

int mpIsPrime(DIGIT_T w[], unsigned int ndigits, unsigned int t)
{	/*	Returns true if w is a probable prime 
		Carries out t iterations
		(Use t = 50 for DSS Standard) */
	/*	Uses Rabin-Miller Probabilistic Primality Test,
		Ref: FIPS-186-2 Appendix 2.
		Also Schneier 2nd ed p 260 & Knuth Vol 2, p 379.
	*/

	/* Temp big digits */
	DIGIT_T m[MAX_DIG_LEN], a[MAX_DIG_LEN], b[MAX_DIG_LEN];
	DIGIT_T z[MAX_DIG_LEN], w1[MAX_DIG_LEN];
	DIGIT_T j[MAX_DIG_LEN];
	unsigned int i;
	int failed, isprime;

	/* Check the obvious */
	if (mpISEVEN(w, ndigits))
		return 0;

	/*	First check for small primes */
	for (i = 0; i < N_SMALL_PRIMES; i++)
	{
		if (mpShortMod(w ,SMALL_PRIMES[i], ndigits) == 0)
			return 0;	/* Failed */
	}

	/*	Now do Rabin-Miller  */
	/*	Step 2. Find a and m where w = 1 + (2^a)m
		m is odd and 2^a is largest power of 2 dividing w - 1 */
	mpShortSub(w1, w, 1, ndigits);	/* Store w1 = w - 1 */
	mpSetEqual(m, w1, ndigits);		/* Set m = w - 1 */
	/* for (a = 0; iseven(m); a++) */
	for (mpSetZero(a, ndigits); mpISEVEN(m, ndigits); 
		mpShortAdd(a, a, 1, ndigits))
	{	/* Divide by 2 until m is odd */
		mpShiftRight(m, m, 1, ndigits);
	}

	/* assert((1 << a) * m + 1 == w); */

	isprime = 1;
	for (i = 0; i < t; i++)
	{
		failed = 1;	/* Assume fail unless passed in loop */
		/* Step 3. Generate random integer 1 < b < w */
		mpSetZero(b, ndigits);
		do
		{
			b[0] = spPseudoRand(2, MAX_DIGIT);
		} while (mpCompare(b, w, ndigits) >= 0);

		/* assert(1 < b && b < w); */

		/* Step 4. Set j = 0 and z = b^m mod w */
		mpSetZero(j, ndigits);
		mpModExp(z, b, m, w, ndigits);
		do
		{
			/* Step 5. If j = 0 and z = 1, or if z = w - 1 */
			/* i.e. if ((j == 0 && z == 1) || (z == w - 1)) */
			if ((mpIsZero(j, ndigits) 
				&& mpShortCmp(z, 1, ndigits) == 0)
				|| (mpCompare(z, w1, ndigits) == 0))
			{	/* Passes on this loop  - go to Step 9 */
				failed = 0;
				break;
			}

			/* Step 6. If j > 0 and z = 1 */
			if (!mpIsZero(j, ndigits) 
				&& (mpShortCmp(z, 1, ndigits) == 0))
			{	/* Fails - go to Step 8 */
				failed = 1;
				break;
			}

			/* Step 7. j = j + 1. If j < a set z = z^2 mod w */
			mpShortAdd(j, j, 1, ndigits);
			if (mpCompare(j, a, ndigits) < 0)
				mpModMult(z, z, z, w, ndigits);
			/* Loop: if j < a go to Step 5 */
		} while (mpCompare(j, a, ndigits) < 0);

		if (failed)
		{	/* Step 8. Not a prime - clean */
			isprime = 0;
			break;
		}
	}	/* Step 9. Go to Step 3 until i >= n */
	/* Else, if i = n, w is probably prime => success */

	/* Clean up */
	mpSetZero(m, ndigits);
	mpSetZero(a, ndigits);
	mpSetZero(b, ndigits);
	mpSetZero(z, ndigits);
	mpSetZero(w1, ndigits);

	return isprime;
}


