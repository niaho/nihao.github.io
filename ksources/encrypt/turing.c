#include <stdlib.h>
#include "rh_ssl.h"

#define keylen (turing_data->_keylen)
#define K		turing_data->_K
#define R		turing_data->_R
#define S0		turing_data->_S0
#define S1		turing_data->_S1
#define S2		turing_data->_S2
#define S3		turing_data->_S3


/* give correct offset for the current position of the register,
 * where logically R[0] is at position "zero".
 */
#define OFF(zero, i) (((zero)+(i)) % LFSRLEN)

/* step the LFSR */
/* After stepping, "zero" moves right one place */
#define STEP(z) \
    R[OFF(z,0)] = R[OFF(z,15)] ^ R[OFF(z,4)] ^ \
	(R[OFF(z,0)] << 8) ^ Multab[(R[OFF(z,0)] >> 24) & 0xFF]

/*
 * This does a reversible transformation of a word, based on the S-boxes.
 * The reversibility isn't used, but it guarantees no loss of information,
 * and hence no equivalent keys or IVs.
 */
static WORD4
fixedS(WORD4 w)
{
    WORD4    b;

    b = Sbox[B(w, 0)]; w = ((w ^      Qbox[b])     & 0x00FFFFFF) | (b << 24);
    b = Sbox[B(w, 1)]; w = ((w ^ ROTL(Qbox[b],8))  & 0xFF00FFFF) | (b << 16);
    b = Sbox[B(w, 2)]; w = ((w ^ ROTL(Qbox[b],16)) & 0xFFFF00FF) | (b << 8);
    b = Sbox[B(w, 3)]; w = ((w ^ ROTL(Qbox[b],24)) & 0xFFFFFF00) | b;
    return w;
}

/*
 * Push a word through the keyed S-boxes.
 */
#define S(w,b) (S0[B((w), ((0+b)&0x3))] \
		^ S1[B((w), ((1+b)&0x3))] \
		^ S2[B((w), ((2+b)&0x3))] \
		^ S3[B((w), ((3+b)&0x3))])

/* two variants of the Pseudo-Hadamard Transform */

/* Mix 5 words in place */
#define PHT(A,B,C,D,E) { \
	(E) += (A) + (B) + (C) + (D); \
	(A) += (E); \
	(B) += (E); \
	(C) += (E); \
	(D) += (E); \
}

/* General word-wide n-PHT */
void
mixwords1(WORD4 w[], int n)
{
    register WORD4	sum;
    register int	i;

    for (sum = i = 0; i < n-1; ++i)
	sum += w[i];
    w[n-1] += sum;
    sum = w[n-1];
    for (i = 0; i < n-1; ++i)
	w[i] += sum;
}

/*
 * Key the cipher.
 * Table version; gathers words, mixes them, saves them.
 * Then compiles lookup tables for the keyed S-boxes.
 */
void
turing_key(TuringData *turing_data,const BYTE key[],  int keylength)
{
    register int	i, j, k;
    register WORD4	w;

    if ((keylength & 0x03) != 0 || keylength > MAXKEY)
	{
		if(keylength>MAXKEY)
			keylength = MAXKEY;
		else
			keylength &= (~0x03);
	}

    keylen = 0;
    for (i = 0; i < keylength; i += 4)
	K[keylen++] = fixedS(BYTE2WORD(&key[i]));
    mixwords1(K, keylen);

    /* build S-box lookup tables */
    for (j = 0; j < 256; ++j) {
	w = 0;
	k = j;
	for (i = 0; i < keylen; ++i) {
	    k = Sbox[B(K[i], 0) ^ k];
	    w ^= ROTL(Qbox[k], i + 0);
	}
	S0[j] = (w & 0x00FFFFFFUL) | (k << 24);
    }
    for (j = 0; j < 256; ++j) {
	w = 0;
	k = j;
	for (i = 0; i < keylen; ++i) {
	    k = Sbox[B(K[i], 1) ^ k];
	    w ^= ROTL(Qbox[k], i + 8);
	}
	S1[j] = (w & 0xFF00FFFFUL) | (k << 16);
    }
    for (j = 0; j < 256; ++j) {
	w = 0;
	k = j;
	for (i = 0; i < keylen; ++i) {
	    k = Sbox[B(K[i], 2) ^ k];
	    w ^= ROTL(Qbox[k], i + 16);
	}
	S2[j] = (w & 0xFFFF00FFUL) | (k << 8);
    }
    for (j = 0; j < 256; ++j) {
	w = 0;
	k = j;
	for (i = 0; i < keylen; ++i) {
	    k = Sbox[B(K[i], 3) ^ k];
	    w ^= ROTL(Qbox[k], i + 24);
	}
	S3[j] = (w & 0xFFFFFF00UL) | k;
    }
}

/*
 * Load the Initialization Vector.
 * Actually, this fills the LFSR, with IV, key, length, and more.
 * IV goes through the fixed S-box, key is premixed, the rest go through
 * the keyed S-boxes. The reason for this is to avoid nasty interactions
 * between the mixed key and the S-boxes that depend on them, and also
 * to avoid any "chosen-IV" interactions with the keyed S-boxes, not that I
 * can think of any.
 */
void
turing_IV(TuringData *turing_data,const BYTE iv[], const int ivlength)
{
    register int	i, j;

    /* check args */
    if ((ivlength & 0x03) != 0 || (ivlength + 4*keylen) > MAXKIV)
	abort();
    /* first copy in the IV, mixing as we go */
    for (i = j = 0; j < ivlength; j +=4)
	R[i++] = fixedS(BYTE2WORD(&iv[j]));
    /* now continue with the premixed key */
    for (j = 0 /* i continues */; j < keylen; ++j)
	R[i++] = K[j];
    /* now the length-dependent word */
    R[i++] = (keylen << 4) | (ivlength >> 2) | 0x01020300UL;
    /* ... and fill the rest of the register */
    for (j = 0 /* i continues */; i < LFSRLEN; ++i, ++j)
	R[i] = S(R[j] + R[i-1], 0);
    /* finally mix all the words */
    mixwords1(R, LFSRLEN);
}

/* a single round */
#define ROUND(z,b) \
{ \
    STEP(z); \
    A = R[OFF(z+1,16)]; \
		B = R[OFF(z+1,13)]; \
			    C = R[OFF(z+1,6)]; \
					D = R[OFF(z+1,1)]; \
						    E = R[OFF(z+1,0)]; \
    PHT(A,	B,	    C,		D,	    E); \
    A = S(A,0);	B = S(B,1); C = S(C,2);	D = S(D,3); E = S(E,0); \
    PHT(A,	B,	    C,		D,	    E); \
    STEP(z+1); \
    STEP(z+2); \
    STEP(z+3); \
    A += R[OFF(z+4,14)]; \
		B += R[OFF(z+4,12)]; \
			    C += R[OFF(z+4,8)]; \
					D += R[OFF(z+4,1)]; \
						    E += R[OFF(z+4,0)]; \
    WORD2BYTE(A, b); \
		WORD2BYTE(B, b+4); \
			    WORD2BYTE(C, b+8); \
					WORD2BYTE(D, b+12); \
						    WORD2BYTE(E, b+16); \
    STEP(z+4); \
}

/*
 * Generate 17 5-word blocks of output.
 * This ensures that the register is resynchronised and avoids state.
 * Buffering issues are outside the scope of this implementation.
 * Returns the number of bytes of stream generated.
 */
int
turing_gen(TuringData *turing_data,BYTE *buf)
{
    WORD4	    A, B, C, D, E;

    ROUND(0,buf);
    ROUND(5,buf+20);
    ROUND(10,buf+40);
    ROUND(15,buf+60);
    ROUND(3,buf+80);
    ROUND(8,buf+100);
    ROUND(13,buf+120);
    ROUND(1,buf+140);
    ROUND(6,buf+160);
    ROUND(11,buf+180);
    ROUND(16,buf+200);
    ROUND(4,buf+220);
    ROUND(9,buf+240);
    ROUND(14,buf+260);
    ROUND(2,buf+280);
    ROUND(7,buf+300);
    ROUND(12,buf+320);
    return 17*20;
}
