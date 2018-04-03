/* t_mpRSA.c */

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

/*	Carry out mp tests usings RSA mathematics.
	This is meant to test the underlying multiple-precision
	functions. The maths are correct for the process
	involved, but the efficiency could be improved.
	WARNING: this procedure is NOT cryptographically secure
	because the random number generator isn't.
*/

#include <stdio.h>
#include <assert.h>
#include "kci_bigint.h"

#define MOD_SIZE 8

/* RSA keys */

#ifdef NO_USED 
main()
{
	DIGIT_T n[MOD_SIZE], e[MOD_SIZE], d[MOD_SIZE];
	DIGIT_T m[MOD_SIZE], c[MOD_SIZE], m1[MOD_SIZE];

//	DIGIT_T m[100], c[100], m1[100];
	unsigned int i = 0;
	int status;

	/*	Force linker to include copyright notice in 
		executable object image
	*/
	copyright_notice();
		
	/* Generate a public/private key pair */
	status = GenerateRSAKeys(n, e, d, 4);
	
	if (status) return -1;

	/* Create a random test message m < n */
	for (i = 0; i < MOD_SIZE; i++)
		m[i] = spPseudoRand(0, MAX_DIGIT);
	m[MOD_SIZE - 1] &= ~HIBITMASK;

	sprintf(m,"mazh is good manmazh is g");
	printf("m ="); mpPrintNL(m, MOD_SIZE);

	/* Encrypt with public key, c = m^e mod n */
	mpModExp(c, m, e, n, MOD_SIZE);

	printf("c ="); mpPrintNL(c, MOD_SIZE);

	/* Decrypt with private key m = c^d mod n */
	mpModExp(m1, c, d, n, MOD_SIZE);

	
	printf("m'="); mpPrintNL(m1,MOD_SIZE);
	printf("%s",(char*)m);
	/* Check that we got back where we started */
	if (!mpEqual(m1, m, MOD_SIZE)) 
		printf("<= ERROR - no match\n");
	else
		printf("<= OK\n");

	return 0;
}
#endif


int GenerateRSAKeys(DIGIT_T n[], DIGIT_T e[], DIGIT_T d[], unsigned f)
{
	/* Generate an RSA key pair (n, e), (d) */
	/* with key size MOD_SIZE digits */
	DIGIT_T p[MOD_SIZE], q[MOD_SIZE];
	DIGIT_T L[2*MOD_SIZE];
	DIGIT_T g[MOD_SIZE], t[MOD_SIZE];
	DIGIT_T FermatNumber[] = { 0x3, 0x5, 0x11, 0x101, 0x10001 };
	unsigned int ndigits;
	int status;

	/* Set e */
	mpSetZero(e, MOD_SIZE);
	if (f > 4) f = 4;
	e[0] = FermatNumber[f];
	
	/* Generate random prime p, ~ half length of modulus */
	mpSetZero(p, MOD_SIZE);
	ndigits = MOD_SIZE / 2;
	do
	{
		status = GenRsaPrime(p, ndigits);
		if (status) return -1;

		/* Make sure gcd(p-1, e) = 1 */
		mpShortSub(L, p, 1, ndigits);
		mpGcd(g, L, e, ndigits);

//		printf("g="); mpPrintNL(g, ndigits);

	} while (mpShortCmp(g, 1, ndigits) != 0);
	
//	printf("Final p="); mpPrintTrimNL(p, ndigits);

	/* And random prime q for balance of modulus length */
	mpSetZero(q, MOD_SIZE);
	ndigits = MOD_SIZE - ndigits;
	do
	{
		status = GenRsaPrime(q, ndigits);
		if (status) return -1;

		/* Make sure gcd(q-1, e) = 1 */
		mpShortSub(L, q, 1, ndigits);
		mpGcd(g, L, e, ndigits);

	//	printf("g="); mpPrintTrimNL(g, ndigits);

	} while (mpShortCmp(g, 1, ndigits) != 0);

//	printf("Final q="); mpPrintNL(q, ndigits);
//	printf("Final p="); mpPrintTrimNL(p, MOD_SIZE);
	
	/*	Calculate modulus, n = pq 
		usings L as a double-length temp */
	mpMultiply(L, p, q, MOD_SIZE);	/* L is double-length */
	mpSetEqual(n, L, MOD_SIZE);		/* but n is only single */

//	printf("N="); mpPrintNL(n, MOD_SIZE);

	/* L = (p-1)*(q-1) */
	mpShortSub(g, p, 1, MOD_SIZE);
	mpShortSub(t, q, 1, MOD_SIZE);
	mpMultiply(L, g, t, MOD_SIZE);

//	printf("L="); mpPrintNL(L, MOD_SIZE);

	/* Calculate private key, d = e^-1 Mod L */
	mpModInv(d, e, L, MOD_SIZE);

//	printf("d="); mpPrintNL(d, MOD_SIZE);
//	printf("e="); mpPrintTrimNL(e, MOD_SIZE);

	/* Clear up */
	mpSetZero(p, MOD_SIZE);
	mpSetZero(q, MOD_SIZE);
	mpSetZero(g, MOD_SIZE);
	mpSetZero(t, MOD_SIZE);
	mpSetZero(L, 2*MOD_SIZE);
	
	return 0;
}

int GenRsaPrime(DIGIT_T p[], unsigned int ndigits)
{
	/*	WARNING: This needs a cryptographically  
		secure random number generator 
	*/
	unsigned int i;

	for (i = 0; i < ndigits; i++)
		p[i] = spPseudoRand(0, MAX_DIGIT);

//	printf("p="); mpPrintNL(p, ndigits);

	/*	Make sure two highest and the low bits are set.
		Having the two highest bits set means the product
		(pq) will always have its highet bit set. 
	*/
	p[ndigits - 1] |= HIBITMASK | (HIBITMASK >> 1);
	p[0] |= 0x1;

//	printf("p'="); mpPrintNL(p, ndigits);

	/* Check if prime */
	while (!mpIsPrime(p, ndigits, 10))
	{
		mpShortAdd(p, p, 2, ndigits);

	//	printf("p'="); mpPrintNL(p, ndigits);

		/* Check for overflow */
		if (!(p[ndigits - 1] & HIBITMASK))
			return -1;	/* Failed to find a prime */
	}

	return 0;
}
