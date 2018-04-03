#ifndef __RH_SSL_H
#define __RH_SSL_H

#ifdef __cplusplus
extern "C"{
#endif

#include "turing.h"		/* interface definitions */
typedef struct TuringData
{
	int		_keylen;		/* adjusted to count WORDs */
	WORD4	_K[MAXKEY/4];	/* storage for mixed key */
	WORD4	_R[LFSRLEN];	/* the shift register */
	/* precalculated S-boxes */
	WORD4	_S0[256],_S1[256],_S2[256],_S3[256];

	int		pos;
	unsigned char	key_stream[340];
}TuringData;

void 
turing_key(TuringData *turing_data,const BYTE key[],  int keylength);
void
turing_IV(TuringData *turing_data,const BYTE iv[], const int ivlength);
int
turing_gen(TuringData *turing_data,BYTE *buf);

#ifdef __cplusplus
}
#endif

#endif
