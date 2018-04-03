#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "kci_bigint.h"

#define MOD_SIZE 8
#define PICE_LEN1  (sizeof(DIGIT_T)*(MOD_SIZE-1))
#define PICE_LEN2  (sizeof(DIGIT_T)*MOD_SIZE)

/*使用rsa算法对给定的内存区进行加密,不改变源数据区内容，输出out内存区*/
void rsa_encrypt(DIGIT_T *encrypt_key, DIGIT_T *trail_key, 
				 char *buff, int len, char *out, int &out_len)
{
	DIGIT_T  s_buff[MOD_SIZE];
	DIGIT_T  t_buff[MOD_SIZE];

	out_len = 0;
	while (len > 0)
	{
		s_buff[MOD_SIZE - 1] = 0;
		if (len > 4 * (MOD_SIZE - 1))
		{
			memcpy(s_buff, buff, 4 * (MOD_SIZE - 1));
			buff += sizeof(DIGIT_T) * (MOD_SIZE - 1);
			len -= sizeof(DIGIT_T) * (MOD_SIZE - 1);
		}
		else
		{
			memset(s_buff, 0x0, 4 * MOD_SIZE);
			memcpy(s_buff, buff, len);
			len = 0;
		}
#ifdef _BIG_ENDIAN_
		for (n = 0; n<MOD_SIZE; n++)
			SG_SWAP4(s_buff + n);
#endif
		mpModExp(t_buff, s_buff, encrypt_key, trail_key, MOD_SIZE);
#ifdef _BIG_ENDIAN_
		for (n = 0; n<MOD_SIZE; n++)
			SG_SWAP4(t_buff + n);
#endif
		memcpy(out, t_buff, sizeof(DIGIT_T)*MOD_SIZE);
		out += 4 * MOD_SIZE;
		out_len += 4 * MOD_SIZE;
	}
}

/*使用rsa算法对给定的内存区进行解密,不改变源数据区内容，输出out内存区*/
void rsa_decrypt(DIGIT_T *decrypt_key, DIGIT_T *trail_key,
				char *buff, int len, char *out, int &out_len)
{
	DIGIT_T  s_buff[MOD_SIZE];
	DIGIT_T  t_buff[MOD_SIZE];

	out_len = 0;
	while (len > 0)
	{
		memcpy(s_buff, buff, 4 * MOD_SIZE);
		buff += sizeof(DIGIT_T) * MOD_SIZE;
		len -= sizeof(DIGIT_T) * MOD_SIZE;

#ifdef _BIG_ENDIAN_
		for (n = 0; n<MOD_SIZE; n++)
			SG_SWAP4(s_buff + n);
#endif
		mpModExp(t_buff, s_buff, decrypt_key, trail_key, MOD_SIZE);
#ifdef _BIG_ENDIAN_
		for (n = 0; n<MOD_SIZE; n++)
			SG_SWAP4(t_buff + n);
#endif
		memcpy(out, t_buff, sizeof(DIGIT_T)*(MOD_SIZE - 1));
		out += 4 * (MOD_SIZE - 1);
		out_len += 4 * (MOD_SIZE - 1);
	}
}

/*使用rsa算法对给定的内存区进行原地加密,解密后的长度会比原数据长度长(每(28->32)),故buff的实际内存应该足够大*/
int  rsa_encrypt(DIGIT_T *encrypt_key, DIGIT_T *trail_key, char *buff, int len)
{
	DIGIT_T  s_buff[MOD_SIZE];
	DIGIT_T  t_buff[MOD_SIZE];
	int      pice_num = len / PICE_LEN1;
	int      more = len % PICE_LEN1;
	
	if(more) pice_num++;
	for (int pice_no = pice_num - 1; pice_no >= 0; pice_no--)
	{
		char *in_ptr = buff + PICE_LEN1 * pice_no;
		char *out_ptr = buff + PICE_LEN2 * pice_no;

		//拷贝当前片的前28字节至加密源buff,对源buff的尾4字节置0
		memcpy(s_buff, in_ptr, PICE_LEN1);
		s_buff[MOD_SIZE-1] = 0x0;
		//进行加密处理
#ifdef _BIG_ENDIAN_
		for (n = 0; n< MOD_SIZE; n++)
			SG_SWAP4(s_buff + *n);
#endif
		mpModExp(t_buff, s_buff, encrypt_key, trail_key, MOD_SIZE);
#ifdef _BIG_ENDIAN_
		for (n = 0; n< MOD_SIZE; n++)
			SG_SWAP4(t_buff + n);
#endif
		//拷贝加密后的32字节到输出区
		memcpy(out_ptr, t_buff, PICE_LEN2);
	}
	return PICE_LEN2*pice_num;
}

/*使用rsa算法对给定的内存区进行原地解密,解密后的长度会比原数据长度小(每(32->28))*/
int  rsa_decrypt(DIGIT_T *decrypt_key, DIGIT_T *trail_key, char *buff, int len)
{
	DIGIT_T  s_buff[MOD_SIZE];
	DIGIT_T  t_buff[MOD_SIZE];
	int      pice_num = len / PICE_LEN2;
	int      more = len % PICE_LEN2;

	for (int pice_no = 0; pice_no < pice_num; pice_no++)
	{
		DIGIT_T *in_ptr = (DIGIT_T*)(buff + PICE_LEN2 * pice_no);
		DIGIT_T *out_ptr = (DIGIT_T*)(buff + PICE_LEN1 * pice_no);

		//拷贝当前片的32字节至解密源buff
		memcpy(s_buff, in_ptr, PICE_LEN1);
		//进行解密处理
#ifdef _BIG_ENDIAN_
		for (n = 0; n<MOD_SIZE; n++)
			SG_SWAP4(s_buff + n);
#endif
		mpModExp(t_buff, s_buff, decrypt_key, trail_key, MOD_SIZE);
#ifdef _BIG_ENDIAN_
		for (n = 0; n<MOD_SIZE; n++)
			SG_SWAP4(t_buff + n);
#endif
		//拷贝解密后的前28字节到输出区
		memcpy(out_ptr, t_buff, PICE_LEN1);
	}
	return PICE_LEN1*pice_num;
}
