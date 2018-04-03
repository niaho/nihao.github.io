#include <stdio.h>
#include "kci_time.h"
#include "kci_defs.h"

#define  IsRn(y) (y%400==0||(y%4==0&&y%100!=0))

cstr g_def_time_format = "yyyy-mm-dd h24-mi-ss.ssssss";

typedef struct  TokenState
{
	int			token_no;
	int8	    jmp_tab[16];
}TokenState;

/*ch2jmpno��states��Ԥ������ʱ���ʽ�Ĵʷ�����״̬ת�������ڿ��ٷ���ʱ���ʽ��*/
const static int8 ch2jmpno[128] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 11, -1, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, 0, 1, 8, 2, -1, -1, -1, 3, 9, -1, -1, -1, 4, 12, 10,
	5, -1, -1, 6, 14, -1, -1, -1, -1, 7, -1, -1, -1, -1, -1, -1,
	-1, 0, 1, 8, 2, -1, -1, -1, 3, 9, -1, -1, -1, 4, 12, 10,
	5, -1, -1, 6, 14, -1, -1, -1, -1, 7, -1, -1, -1, -1, -1, -1 };

const static TokenState states[37] = {
	{ -1, { 1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 9, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, } },
	{ 5, { 12, 0, 13, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 0, 17, 0, 0, 0, 0, 16, 18, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, } },
	{ 1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 2, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 3, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, } },
	{ 6, { 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 8, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 9, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, } },
	{ 11, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 12, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, } },
	{ 15, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 16, { 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 23, { 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, } },
	{ 4, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 7, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, } },
	{ 13, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, } },
	{ 17, { 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 24, { 0, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, } },
	{ 10, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ -1, { 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 18, { 0, 0, 0, 0, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 25, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 14, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 19, { 0, 0, 0, 0, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 20, { 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 21, { 0, 0, 0, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, } },
	{ 22, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } },
};

/*10��n�η���*/
static int32 exp10s[7] = { 1, 10, 100, 1000, 10000, 100000, 1000000 };

//���µĵ�һ���Ӧ�������е�����
static int  mtod[2][13] = { { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 } };

//���¶�Ӧ������
static int  month_days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//��ͨ������Ӧ���·������������е�����ֵ
int	  day2md[365][2] = {
	{ 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 1, 5 }, { 1, 6 }, { 1, 7 }, { 1, 8 }, { 1, 9 }, { 1, 10 }, { 1, 11 }, { 1, 12 }, { 1, 13 }, { 1, 14 }, { 1, 15 }, { 1, 16 },
	{ 1, 17 }, { 1, 18 }, { 1, 19 }, { 1, 20 }, { 1, 21 }, { 1, 22 }, { 1, 23 }, { 1, 24 }, { 1, 25 }, { 1, 26 }, { 1, 27 }, { 1, 28 }, { 1, 29 }, { 1, 30 }, { 1, 31 },

	{ 2, 1 }, { 2, 2 }, { 2, 3 }, { 2, 4 }, { 2, 5 }, { 2, 6 }, { 2, 7 }, { 2, 8 }, { 2, 9 }, { 2, 10 }, { 2, 11 }, { 2, 12 }, { 2, 13 }, { 2, 14 }, { 2, 15 }, { 2, 16 },
	{ 2, 17 }, { 2, 18 }, { 2, 19 }, { 2, 20 }, { 2, 21 }, { 2, 22 }, { 2, 23 }, { 2, 24 }, { 2, 25 }, { 2, 26 }, { 2, 27 }, { 2, 28 },

	{ 3, 1 }, { 3, 2 }, { 3, 3 }, { 3, 4 }, { 3, 5 }, { 3, 6 }, { 3, 7 }, { 3, 8 }, { 3, 9 }, { 3, 10 }, { 3, 11 }, { 3, 12 }, { 3, 13 }, { 3, 14 }, { 3, 15 }, { 3, 16 },
	{ 3, 17 }, { 3, 18 }, { 3, 19 }, { 3, 20 }, { 3, 21 }, { 3, 22 }, { 3, 23 }, { 3, 24 }, { 3, 25 }, { 3, 26 }, { 3, 27 }, { 3, 28 }, { 3, 29 }, { 3, 30 }, { 3, 31 },

	{ 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 4, 6 }, { 4, 7 }, { 4, 8 }, { 4, 9 }, { 4, 10 }, { 4, 11 }, { 4, 12 }, { 4, 13 }, { 4, 14 }, { 4, 15 }, { 4, 16 },
	{ 4, 17 }, { 4, 18 }, { 4, 19 }, { 4, 20 }, { 4, 21 }, { 4, 22 }, { 4, 23 }, { 4, 24 }, { 4, 25 }, { 4, 26 }, { 4, 27 }, { 4, 28 }, { 4, 29 }, { 4, 30 },

	{ 5, 1 }, { 5, 2 }, { 5, 3 }, { 5, 4 }, { 5, 5 }, { 5, 6 }, { 5, 7 }, { 5, 8 }, { 5, 9 }, { 5, 10 }, { 5, 11 }, { 5, 12 }, { 5, 13 }, { 5, 14 }, { 5, 15 }, { 5, 16 },
	{ 5, 17 }, { 5, 18 }, { 5, 19 }, { 5, 20 }, { 5, 21 }, { 5, 22 }, { 5, 23 }, { 5, 24 }, { 5, 25 }, { 5, 26 }, { 5, 27 }, { 5, 28 }, { 5, 29 }, { 5, 30 }, { 5, 31 },

	{ 6, 1 }, { 6, 2 }, { 6, 3 }, { 6, 4 }, { 6, 5 }, { 6, 6 }, { 6, 7 }, { 6, 8 }, { 6, 9 }, { 6, 10 }, { 6, 11 }, { 6, 12 }, { 6, 13 }, { 6, 14 }, { 6, 15 }, { 6, 16 },
	{ 6, 17 }, { 6, 18 }, { 6, 19 }, { 6, 20 }, { 6, 21 }, { 6, 22 }, { 6, 23 }, { 6, 24 }, { 6, 25 }, { 6, 26 }, { 6, 27 }, { 6, 28 }, { 6, 29 }, { 6, 30 },

	{ 7, 1 }, { 7, 2 }, { 7, 3 }, { 7, 4 }, { 7, 5 }, { 7, 6 }, { 7, 7 }, { 7, 8 }, { 7, 9 }, { 7, 10 }, { 7, 11 }, { 7, 12 }, { 7, 13 }, { 7, 14 }, { 7, 15 }, { 7, 16 },
	{ 7, 17 }, { 7, 18 }, { 7, 19 }, { 7, 20 }, { 7, 21 }, { 7, 22 }, { 7, 23 }, { 7, 24 }, { 7, 25 }, { 7, 26 }, { 7, 27 }, { 7, 28 }, { 7, 29 }, { 7, 30 }, { 7, 31 },

	{ 8, 1 }, { 8, 2 }, { 8, 3 }, { 8, 4 }, { 8, 5 }, { 8, 6 }, { 8, 7 }, { 8, 8 }, { 8, 9 }, { 8, 10 }, { 8, 11 }, { 8, 12 }, { 8, 13 }, { 8, 14 }, { 8, 15 }, { 8, 16 },
	{ 8, 17 }, { 8, 18 }, { 8, 19 }, { 8, 20 }, { 8, 21 }, { 8, 22 }, { 8, 23 }, { 8, 24 }, { 8, 25 }, { 8, 26 }, { 8, 27 }, { 8, 28 }, { 8, 29 }, { 8, 30 }, { 8, 31 },

	{ 9, 1 }, { 9, 2 }, { 9, 3 }, { 9, 4 }, { 9, 5 }, { 9, 6 }, { 9, 7 }, { 9, 8 }, { 9, 9 }, { 9, 10 }, { 9, 11 }, { 9, 12 }, { 9, 13 }, { 9, 14 }, { 9, 15 }, { 9, 16 },
	{ 9, 17 }, { 9, 18 }, { 9, 19 }, { 9, 20 }, { 9, 21 }, { 9, 22 }, { 9, 23 }, { 9, 24 }, { 9, 25 }, { 9, 26 }, { 9, 27 }, { 9, 28 }, { 9, 29 }, { 9, 30 },

	{ 10, 1 }, { 10, 2 }, { 10, 3 }, { 10, 4 }, { 10, 5 }, { 10, 6 }, { 10, 7 }, { 10, 8 }, { 10, 9 }, { 10, 10 }, { 10, 11 }, { 10, 12 }, { 10, 13 }, { 10, 14 }, { 10, 15 }, { 10, 16 },
	{ 10, 17 }, { 10, 18 }, { 10, 19 }, { 10, 20 }, { 10, 21 }, { 10, 22 }, { 10, 23 }, { 10, 24 }, { 10, 25 }, { 10, 26 }, { 10, 27 }, { 10, 28 }, { 10, 29 }, { 10, 30 }, { 10, 31 },

	{ 11, 1 }, { 11, 2 }, { 11, 3 }, { 11, 4 }, { 11, 5 }, { 11, 6 }, { 11, 7 }, { 11, 8 }, { 11, 9 }, { 11, 10 }, { 11, 11 }, { 11, 12 }, { 11, 13 }, { 11, 14 }, { 11, 15 }, { 11, 16 },
	{ 11, 17 }, { 11, 18 }, { 11, 19 }, { 11, 20 }, { 11, 21 }, { 11, 22 }, { 11, 23 }, { 11, 24 }, { 11, 25 }, { 11, 26 }, { 11, 27 }, { 11, 28 }, { 11, 29 }, { 11, 30 },

	{ 12, 1 }, { 12, 2 }, { 12, 3 }, { 12, 4 }, { 12, 5 }, { 12, 6 }, { 12, 7 }, { 12, 8 }, { 12, 9 }, { 12, 10 }, { 12, 11 }, { 12, 12 }, { 12, 13 }, { 12, 14 }, { 12, 15 }, { 12, 16 },
	{ 12, 17 }, { 12, 18 }, { 12, 19 }, { 12, 20 }, { 12, 21 }, { 12, 22 }, { 12, 23 }, { 12, 24 }, { 12, 25 }, { 12, 26 }, { 12, 27 }, { 12, 28 }, { 12, 29 }, { 12, 30 }, { 12, 31 },
};

//��������Ӧ���·������������е�����ֵ
int	  day2md_rn[366][2] = {
	{ 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 1, 5 }, { 1, 6 }, { 1, 7 }, { 1, 8 }, { 1, 9 }, { 1, 10 }, { 1, 11 }, { 1, 12 }, { 1, 13 }, { 1, 14 }, { 1, 15 }, { 1, 16 },
	{ 1, 17 }, { 1, 18 }, { 1, 19 }, { 1, 20 }, { 1, 21 }, { 1, 22 }, { 1, 23 }, { 1, 24 }, { 1, 25 }, { 1, 26 }, { 1, 27 }, { 1, 28 }, { 1, 29 }, { 1, 30 }, { 1, 31 },

	{ 2, 1 }, { 2, 2 }, { 2, 3 }, { 2, 4 }, { 2, 5 }, { 2, 6 }, { 2, 7 }, { 2, 8 }, { 2, 9 }, { 2, 10 }, { 2, 11 }, { 2, 12 }, { 2, 13 }, { 2, 14 }, { 2, 15 }, { 2, 16 },
	{ 2, 17 }, { 2, 18 }, { 2, 19 }, { 2, 20 }, { 2, 21 }, { 2, 22 }, { 2, 23 }, { 2, 24 }, { 2, 25 }, { 2, 26 }, { 2, 27 }, { 2, 28 }, { 2, 29 },

	{ 3, 1 }, { 3, 2 }, { 3, 3 }, { 3, 4 }, { 3, 5 }, { 3, 6 }, { 3, 7 }, { 3, 8 }, { 3, 9 }, { 3, 10 }, { 3, 11 }, { 3, 12 }, { 3, 13 }, { 3, 14 }, { 3, 15 }, { 3, 16 },
	{ 3, 17 }, { 3, 18 }, { 3, 19 }, { 3, 20 }, { 3, 21 }, { 3, 22 }, { 3, 23 }, { 3, 24 }, { 3, 25 }, { 3, 26 }, { 3, 27 }, { 3, 28 }, { 3, 29 }, { 3, 30 }, { 3, 31 },

	{ 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 4, 6 }, { 4, 7 }, { 4, 8 }, { 4, 9 }, { 4, 10 }, { 4, 11 }, { 4, 12 }, { 4, 13 }, { 4, 14 }, { 4, 15 }, { 4, 16 },
	{ 4, 17 }, { 4, 18 }, { 4, 19 }, { 4, 20 }, { 4, 21 }, { 4, 22 }, { 4, 23 }, { 4, 24 }, { 4, 25 }, { 4, 26 }, { 4, 27 }, { 4, 28 }, { 4, 29 }, { 4, 30 },

	{ 5, 1 }, { 5, 2 }, { 5, 3 }, { 5, 4 }, { 5, 5 }, { 5, 6 }, { 5, 7 }, { 5, 8 }, { 5, 9 }, { 5, 10 }, { 5, 11 }, { 5, 12 }, { 5, 13 }, { 5, 14 }, { 5, 15 }, { 5, 16 },
	{ 5, 17 }, { 5, 18 }, { 5, 19 }, { 5, 20 }, { 5, 21 }, { 5, 22 }, { 5, 23 }, { 5, 24 }, { 5, 25 }, { 5, 26 }, { 5, 27 }, { 5, 28 }, { 5, 29 }, { 5, 30 }, { 5, 31 },

	{ 6, 1 }, { 6, 2 }, { 6, 3 }, { 6, 4 }, { 6, 5 }, { 6, 6 }, { 6, 7 }, { 6, 8 }, { 6, 9 }, { 6, 10 }, { 6, 11 }, { 6, 12 }, { 6, 13 }, { 6, 14 }, { 6, 15 }, { 6, 16 },
	{ 6, 17 }, { 6, 18 }, { 6, 19 }, { 6, 20 }, { 6, 21 }, { 6, 22 }, { 6, 23 }, { 6, 24 }, { 6, 25 }, { 6, 26 }, { 6, 27 }, { 6, 28 }, { 6, 29 }, { 6, 30 },

	{ 7, 1 }, { 7, 2 }, { 7, 3 }, { 7, 4 }, { 7, 5 }, { 7, 6 }, { 7, 7 }, { 7, 8 }, { 7, 9 }, { 7, 10 }, { 7, 11 }, { 7, 12 }, { 7, 13 }, { 7, 14 }, { 7, 15 }, { 7, 16 },
	{ 7, 17 }, { 7, 18 }, { 7, 19 }, { 7, 20 }, { 7, 21 }, { 7, 22 }, { 7, 23 }, { 7, 24 }, { 7, 25 }, { 7, 26 }, { 7, 27 }, { 7, 28 }, { 7, 29 }, { 7, 30 }, { 7, 31 },

	{ 8, 1 }, { 8, 2 }, { 8, 3 }, { 8, 4 }, { 8, 5 }, { 8, 6 }, { 8, 7 }, { 8, 8 }, { 8, 9 }, { 8, 10 }, { 8, 11 }, { 8, 12 }, { 8, 13 }, { 8, 14 }, { 8, 15 }, { 8, 16 },
	{ 8, 17 }, { 8, 18 }, { 8, 19 }, { 8, 20 }, { 8, 21 }, { 8, 22 }, { 8, 23 }, { 8, 24 }, { 8, 25 }, { 8, 26 }, { 8, 27 }, { 8, 28 }, { 8, 29 }, { 8, 30 }, { 8, 31 },

	{ 9, 1 }, { 9, 2 }, { 9, 3 }, { 9, 4 }, { 9, 5 }, { 9, 6 }, { 9, 7 }, { 9, 8 }, { 9, 9 }, { 9, 10 }, { 9, 11 }, { 9, 12 }, { 9, 13 }, { 9, 14 }, { 9, 15 }, { 9, 16 },
	{ 9, 17 }, { 9, 18 }, { 9, 19 }, { 9, 20 }, { 9, 21 }, { 9, 22 }, { 9, 23 }, { 9, 24 }, { 9, 25 }, { 9, 26 }, { 9, 27 }, { 9, 28 }, { 9, 29 }, { 9, 30 },

	{ 10, 1 }, { 10, 2 }, { 10, 3 }, { 10, 4 }, { 10, 5 }, { 10, 6 }, { 10, 7 }, { 10, 8 }, { 10, 9 }, { 10, 10 }, { 10, 11 }, { 10, 12 }, { 10, 13 }, { 10, 14 }, { 10, 15 }, { 10, 16 },
	{ 10, 17 }, { 10, 18 }, { 10, 19 }, { 10, 20 }, { 10, 21 }, { 10, 22 }, { 10, 23 }, { 10, 24 }, { 10, 25 }, { 10, 26 }, { 10, 27 }, { 10, 28 }, { 10, 29 }, { 10, 30 }, { 10, 31 },

	{ 11, 1 }, { 11, 2 }, { 11, 3 }, { 11, 4 }, { 11, 5 }, { 11, 6 }, { 11, 7 }, { 11, 8 }, { 11, 9 }, { 11, 10 }, { 11, 11 }, { 11, 12 }, { 11, 13 }, { 11, 14 }, { 11, 15 }, { 11, 16 },
	{ 11, 17 }, { 11, 18 }, { 11, 19 }, { 11, 20 }, { 11, 21 }, { 11, 22 }, { 11, 23 }, { 11, 24 }, { 11, 25 }, { 11, 26 }, { 11, 27 }, { 11, 28 }, { 11, 29 }, { 11, 30 },

	{ 12, 1 }, { 12, 2 }, { 12, 3 }, { 12, 4 }, { 12, 5 }, { 12, 6 }, { 12, 7 }, { 12, 8 }, { 12, 9 }, { 12, 10 }, { 12, 11 }, { 12, 12 }, { 12, 13 }, { 12, 14 }, { 12, 15 }, { 12, 16 },
	{ 12, 17 }, { 12, 18 }, { 12, 19 }, { 12, 20 }, { 12, 21 }, { 12, 22 }, { 12, 23 }, { 12, 24 }, { 12, 25 }, { 12, 26 }, { 12, 27 }, { 12, 28 }, { 12, 29 }, { 12, 30 }, { 12, 31 },
};

/*��ʱ���ʽ��Ϊʱ��ṹ��ʹ�ø���������*/
void  dt2tm(int64 t, DateTime *p_dt)
{
	int32   y, d, d1, s, us;	//������������յ�
	int		wday;

	//ʱ����ڸ���������������ʱ�䣿(������������)
	if (t < -12219292800000000LL)
	{
		//jdԪ�굽1970-1-1�յ��ղ�Ϊ: 719164
		//110000��jd���������Ϊ: 40177500
		//����֮��Ϊ: 40896664
		t += 3533503305600000000;
		us = (uint32)(t % 1000000);
		t = t / 1000000;
		s = (int32)(t % 86400);
		d = (int32)(t / 86400);
		wday = (d - 1) % 7;

		d1 = d - 10;
		y = (d1 / 1461) * 4;
		d1 %= 1461;

		y += d1 / 365;
		//���㵽y��1��1�Ŷ�Ӧ������
		d1 = y * 365 + (y - 1) / 4;
		d -= d1;

		if (d >= 365)
		{
			if (IsRn(y))
			{
				if (d > 365)
				{
					y++;
					d -= 366;
				}
			}
			else
			{
				y++;
				d -= 365;
			}
		}

		p_dt->year = y - 110000;
		if (IsRn(y))
		{
			p_dt->month = day2md_rn[d][0];
			p_dt->mday = day2md_rn[d][1];
		}
		else
		{
			p_dt->month = day2md[d][0];
			p_dt->mday = day2md[d][1];
		}
		p_dt->wday = wday;
		p_dt->s = s;
		p_dt->us = us;
	}
	//���򣬰���������������
	else
	{
		t += 3533431852800000000LL;
		us = (uint32)(t % 1000000);
		t = t / 1000000;
		s = (int32)(t % 86400);
		d = (int32)(t / 86400);
		wday = d % 7;

		d1 = d - 10;
		y = (d1 / 146097) * 400;
		d1 %= 146097;

		y += (d1 / 36524) * 100;
		d1 %= 36524;

		y += (d1 / 1461) * 4;
		d1 %= 1461;

		y += d1 / 365;
		//���㵽y��1��1�Ŷ�Ӧ������
		d1 = y * 365 + (y - 1) / 4 - (y - 1) / 100 + (y - 1) / 400;
		d -= d1;

		if (d >= 365)
		{
			if (IsRn(y))
			{
				if (d > 365)
				{
					y++;
					d -= 366;
				}
			}
			else
			{
				y++;
				d -= 365;
			}
		}

		p_dt->year = y - 110000;
		if (IsRn(y))
		{
			p_dt->month = day2md_rn[d][0];
			p_dt->mday = day2md_rn[d][1];
		}
		else
		{
			p_dt->month = day2md[d][0];
			p_dt->mday = day2md[d][1];
		}
		p_dt->wday = wday;
		p_dt->s = s;
		p_dt->us = us;
	}
}

/*�����ڸ�ʽ��Ϊʱ��ṹ��ʹ�ø���������*/
void  date2tm(int32 date, DateTime *p_dt)
{
	int32   y, d, d1;	//������������յ�
	int		wday;

	//���ڵ��ڸ���������������ʱ�䣿(������������)
	if (date < -141427)
	{
		d = date + 40897029;
		wday = (d - 1) % 7;

		d1 = d - 10;
		y = (d1 / 1461) * 4;
		d1 %= 1461;
		y += d1 / 365;

		//���㵽y��1��1�Ŷ�Ӧ������
		d1 = y * 365 + (y - 1) / 4;
		d -= d1;

		if (d >= 365)
		{
			if (IsRn(y))
			{
				if (d > 365)
				{
					y++;
					d -= 366;
				}
			}
			else
			{
				y++;
				d -= 365;
			}
		}

		p_dt->year = y - 110000;
		if (IsRn(y))
		{
			p_dt->month = day2md_rn[d][0];
			p_dt->mday = day2md_rn[d][1];
		}
		else
		{
			p_dt->month = day2md[d][0];
			p_dt->mday = day2md[d][1];
		}
		p_dt->wday = wday;
		p_dt->s = 0;
		p_dt->us = 0;
	}
	//���򣬰���������������
	else
	{
		d = date + 40896202;
		wday = d % 7;

		d1 = d - 10;
		y = (d1 / 146097) * 400;
		d1 %= 146097;

		y += (d1 / 36524) * 100;
		d1 %= 36524;

		y += (d1 / 1461) * 4;
		d1 %= 1461;
		y += d1 / 365;

		//���㵽y��1��1�Ŷ�Ӧ������
		d1 = y * 365 + (y - 1) / 4 - (y - 1) / 100 + (y - 1) / 400;
		d -= d1;

		if (d >= 365)
		{
			if (IsRn(y))
			{
				if (d > 365)
				{
					y++;
					d -= 366;
				}
			}
			else
			{
				y++;
				d -= 365;
			}
		}

		p_dt->year = y - 110000;
		if (IsRn(y))
		{
			p_dt->month = day2md_rn[d][0];
			p_dt->mday = day2md_rn[d][1];
		}
		else
		{
			p_dt->month = day2md[d][0];
			p_dt->mday = day2md[d][1];
		}
		p_dt->wday = wday;
		p_dt->s = 0;
		p_dt->us = 0;
	}
}


/*��ʱ��ṹת��Ϊʱ�䣬1582��10��5��ǰʹ�����������Ժ�ʹ�ø���������*/
int64 tm2dt(DateTime *p_tm)
{
	int64 t;
	int	  rn_num;

	int  y = p_tm->year;
	int  m = p_tm->month;
	int  d = p_tm->mday;
	//1582��10��5��ǰʹ��������
	if (y < 1582 || y == 1582 && (m < 10 || m == 10 && d < 5))
	{
		y += 110000;
		rn_num = (y - 1) / 4;
		if ((y & 3) == 0)
			d = d + mtod[1][m - 1] - 1;
		else
			d = d + mtod[0][m - 1] - 1;
		d = d + 365 * y + rn_num - 40897029;
		t = ((int64)d) * 86400 + p_tm->s;
		return t * 1000000 + p_tm->us;
	}
	//1582��10��5���Ժ�ʹ�ø���������
	else
	{
		y += 110000;
		if ((((y & 3) == 0) && (y % 100 != 0)) || (y % 400 == 0))
			d = d + mtod[1][m - 1] - 1;
		else
			d = d + mtod[0][m - 1] - 1;

		rn_num = (y - 1) / 4 - (y - 1) / 100 + (y - 1) / 400;
		d = d + 365 * y + rn_num - 40896202;
		t = ((int64)d) * 86400 + p_tm->s;
		return t * 1000000 + p_tm->us;
	}
}

/*��ʱ��ṹת��Ϊ����ֵ��1582��10��5��ǰʹ�����������Ժ�ʹ�ø���������*/
int32  tm2date(DateTime *p_tm)
{
	int	 rn_num;
	int  y = p_tm->year;
	int  m = p_tm->month;
	int  d = p_tm->mday;

	//1582��10��5��ǰʹ��������
	if (y < 1582 || y == 1582 && (m < 10 || m == 10 && d < 5))
	{
		y += 110000;
		rn_num = (y - 1) / 4;
		if ((y & 3) == 0)
			d = d + mtod[1][m - 1] - 1;
		else
			d = d + mtod[0][m - 1] - 1;
		return d = d + 365 * y + rn_num - 40897029;
	}
	//1582��10��5���Ժ�ʹ�ø���������
	else
	{
		y += 110000;
		if ((((y & 3) == 0) && (y % 100 != 0)) || (y % 400 == 0))
			d = d + mtod[1][m - 1] - 1;
		else
			d = d + mtod[0][m - 1] - 1;
		rn_num = (y - 1) / 4 - (y - 1) / 100 + (y - 1) / 400;
		return d + 365 * y + rn_num - 40896202;
	}
}

/*����һ��״̬���յ��ַ�ch��Ӧ����ת��״̬*/
inline static int jmp_to(int sta, char ch)
{
	int8  jmp_no = ch2jmpno[(uchar)ch];
	return jmp_no >= 0 ? states[sta].jmp_tab[jmp_no] : 0;
}

/*�Ӹ�ʽ����ȡһ����ʽtoken,���ظ�ʽ�뼰��ʽ�ִ�*/
int  next_tfmt(const char *&str, char word[32])
{
	const char *p_s = str;
	const char *p_e = p_s, *p_e0 = NULL;
	int	 n_ch, state = 0;
	int	 token_no = -1;

	//���ַ�����ĸ����
	if ((!((*p_e) & 0x80)) && isalnum(*p_e))
	{
		do{
			state = jmp_to(state, *p_e++);
			if (state == 0)
				break;
			else if (states[state].token_no>0)
			{
				p_e0 = p_e;
				token_no = states[state].token_no;
			}
		} while (*p_e != 0x0 && state>0);

		if (token_no >= 0)
		{
			str = p_e0;
			return token_no;
		}
		else
		{
			str = p_s;
			return 0;
		}
	}
	else
	{
		if (*p_e == 0x0)
			return 0;
		while ((((*p_e) & 0x80) || !isalnum((*p_e))) && *p_e != 0x0) p_e++;
		str = p_e;
		if ((n_ch = (int)(p_e - p_s))<32)
		{
			strncpy(word, p_s, n_ch);
			word[n_ch] = 0x0;
			return T_WORD;
		}
		else
			return 0;
	}
}

#ifdef __WIN__
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
inline	static  int64  get_sys_time()
{
	struct timeb tb;
	ftime(&tb);
	return (tb.time * 1000 - tb.timezone * 60000 + tb.millitm) * 1000;
}
#else
#include <sys/time.h>   
inline	static  int64 get_sys_time()
{
	struct timeval  tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	return ((int64)(tv.tv_sec - tz.tz_minuteswest * 60)) * 1000000 + tv.tv_usec;
}
#endif

/*���ִ���ת��������ʱ��(��ʱ��)��ʽ*/
bool  str2tm(const char *str, const char *fmt, int scale, DateTime &tm)
{
	DateTime tm1;
	int     token;
	int16	t_z, n;
	char    word[32];
	int     i, y, m, d, h,
		mi, s, us;
	bool	tz_neg = false;
	bool    y2_as_y4 = false;
	bool	have_tz = false;
	const   char *str0 = str;
	const   char *fmt0 = fmt;

	if (scale<0 || scale>6)
		scale = 6;
	if (!fmt || !fmt[0]) fmt = g_def_time_format;

retry:
	//why 2014-12-31 modify 'm=0;d=0'=>'m=1;d=1;'֧��ָֻ���꣬��ָֻ�����µĲ���
	y = 0; m = 1; d = 1; h = 0; mi = 0; s = 0; us = 0; t_z = 0;
	while (*str == ' ') str++;
	while (*str != 0x0 && (token = next_tfmt(fmt, word))>0)
	{
		switch (token)
		{
		case Y2:
			if (!y2_as_y4)
			{
				y2_as_y4 = true;
				while (*str != 0x0 && (*str<'0' || *str >'9'))
					str++;
				for (i = 0; i<2; i++)
				{
					if (*str >= '0'&& *str <= '9')
						y = y * 10 + (*str++ - '0');
					else
						break;
				}
				date2tm((int32)(get_sys_time() / 86400000000LL), &tm1);
				y += (tm1.year - tm1.year % 1000);
			}
			else
			{
				y2_as_y4 = false;
				while (*str != 0x0 && (*str<'0' || *str >'9'))
					str++;
				for (i = 0; i<4; i++)
				{
					if (*str >= '0'&& *str <= '9')
						y = y * 10 + (*str++ - '0');
					else
						break;
				}
			}
			break;
		case Y3:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<3; i++)
			{
				if (*str >= '0'&& *str <= '9')
					y = y * 10 + (*str++ - '0');
				else
					break;
			}
			date2tm((int32)(get_sys_time() / 86400000000LL), &tm1);
			y += (tm1.year - tm1.year % 100);
			break;
		case Y4:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<4; i++)
			{
				if (*str >= '0'&& *str <= '9')
					y = y * 10 + (*str++ - '0');
				else
					break;
			}
			break;
		case MM:
			m = 0; //why 2014-12-31 add
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
					m = m * 10 + (*str++ - '0');
				else
					break;
			}
			break;
		case DD:
			d = 0; //why 2014-12-31 add
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
					d = d * 10 + (*str++ - '0');
				else
					break;
			}
			break;
		case HH:
		case HH24:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
				{
					h = h * 10 + (*str - '0');
					str++;
				}
				else
					break;
			}
			break;
		case MI:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
				{
					mi = mi * 10 + (*str - '0');
					str++;
				}
				else
					break;
			}
			break;
		case S2:
		case S3:
		case S4:
		case S5:
		case S6:
		case S7:
		case S8:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
				{
					s = s * 10 + (*str - '0');
					str++;
				}
				else
					break;
			}

			if (*str == '.')
			{
				str++;
				for (i = 0; i<scale; i++)
				{
					if (*str != 0x0)
					{
						if (*str >= '0' && *str <= '9')
						{
							us = us * 10 + (*str - '0');
							str++;
						}
						else
							break;
					}
					else
						break;
				}
				while (isdigit((uchar)*str)) str++;
				us *= exp10s[6 - i];
			}
			break;
		case T_WORD:
		{
			while (*str == ' ') str++;
			if ((str[0] == 'B' || str[0] == 'b') &&
				(str[1] == 'C' || str[1] == 'c')
				|| (str[0] == 'A' || str[0] == 'a') &&
				(str[1] == 'D' || str[1] == 'd'))
				goto chk_bc_ad;
			while (*str != 0x0 && !isdigit(*str)) str++;
			/*
			char * p_ch = word;
			while(*p_ch != 0x0 && *str!=0x0)
			{
			if(*p_ch++ != *str++)
			return false;
			}
			*/
		}
		break;
		}
	}

	while (*str == ' ')
		str++;
	//�������Ƿ���: +(-) hh:mm
	if (*str == '+' || *str == '-')
	{
		have_tz = true;
		tz_neg = (*str == '-');
		str++;
		n = 0;
		for (i = 0; i<2; i++)
		{
			if (*str >= '0'&& *str <= '9')
			{
				n = n * 10 + (*str - '0');
				str++;
			}
			else
				break;
		}
		t_z = n * 60;

		while (*str == ' ') str++;
		if (*str == ':')
		{
			str++;
			n = 0;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
				{
					n = n * 10 + (*str - '0');
					str++;
				}
				else
					break;
			}
			t_z += n;
		}
		if (tz_neg)
			t_z = -t_z;
	}

chk_bc_ad:
	while (*str == ' ')
		str++;
	if (*str != 0x0)
	{
		if (!astricmp(str, "BC"))
			y = 1 - y;
		else if (astricmp(str, "AD"))
		{
			if (!y2_as_y4)
				return false;
			else
			{
				str = str0;
				fmt = fmt0;
				goto retry;
			}
		}
	}

	if (t_z>1440 || t_z<-1440)
		return false;

	if (y == 0 || d<1 || m<1 || m>12 || h>23 || mi>59 || s>59)
		return false;

	if (m == 2)
	{
		if ((((y & 3) == 0) && (y % 100 != 0)) || (y % 400 == 0))
		{
			if (d>29)
				return false;
		}
		else if (d>28)
			return false;
	}
	else if (d>month_days[m])
		return false;

	tm.year = y;
	tm.month = m;
	tm.mday = d;
	tm.s = h * 3600 + mi * 60 + s;
	tm.us = us;
	if (have_tz)
	{
		tm.btz = true;
		tm.tz = t_z;
	}
	else
	{
		tm.btz = false;
		tm.tz = 0;
	}
	return true;
}

/*������ʱ��ṹ����ʽת�����ַ���*/
sword tm2str(DateTime &dt, char *buff, const char *fmt, int tf)
{
	int     token;
	char	word[32];
	char    *p_str = buff;
	int     s = dt.s;
	int     m = s / 60;
	int     h = m / 60;
	double  s1;
	bool    is_bc = false;

	s %= 60; m %= 60;
	if (!fmt || !fmt[0]) fmt = g_def_time_format;
	while ((token = next_tfmt(fmt, word))>0)
	{
		switch (token)
		{
		case Y4:
			if (dt.year > 0)
				sprintf(p_str, "%04d", dt.year);
			else
			{
				sprintf(p_str, "%04d", 1 - dt.year);
				is_bc = true;
			}
			break;
		case Y3:
			if (dt.year > 0)
				sprintf(p_str, "%03d", (dt.year % 1000));
			else
			{
				sprintf(p_str, "%03d", (1 - dt.year) % 1000);
				is_bc = true;
			}
			break;
		case Y2:
			if (dt.year > 0)
				sprintf(p_str, "%02d", (dt.year % 100));
			else
			{
				sprintf(p_str, "%03d", (1 - dt.year) % 100);
				is_bc = true;
			}
			break;
		case MM:
			sprintf(p_str, "%02d", dt.month);
			break;
		case D1:
			sprintf(p_str, "%d", dt.wday + 1);
			break;
		case DD:
			sprintf(p_str, "%02d", dt.mday);
			break;
		case HH:
			sprintf(p_str, "%02d", h>12 ? h - 12 : h);
			break;
		case HH24:
			sprintf(p_str, "%02d", h);
			break;
		case AM:
		case PM:
			if (h>12)
				strcat(p_str, "PM");
			else
				strcat(p_str, "AM");
			break;
		case MI:
			sprintf(p_str, "%02d", m);
			break;
		case S2:
			sprintf(p_str, "%02d", s);
			break;
		case S3:
			s1 = s + ((double)dt.us) / 1000000.0;
			sprintf(p_str, "%.1f", s1);
			break;
		case S4:
			s1 = s + ((double)dt.us) / 1000000.0;
			sprintf(p_str, "%.2f", s1);
			break;
		case S5:
			s1 = s + ((double)dt.us) / 1000000.0;
			sprintf(p_str, "%.3f", s1);
			break;
		case S6:
			s1 = s + ((double)dt.us) / 1000000.0;
			sprintf(p_str, "%.4f", s1);
			break;
		case S7:
			s1 = s + ((double)dt.us) / 1000000.0;
			sprintf(p_str, "%.5f", s1);
			break;
		case S8:
			s1 = s + ((double)dt.us) / 1000000.0;
			sprintf(p_str, "%.6f", s1);
			break;
		case T_WORD:
			sprintf(p_str, "%s", word);
			break;
		default:
			return KCI_ERROR;
		}
		p_str += astrlen(p_str);
	}
	if (is_bc)
		strcpy(p_str, " BC");
	return KCI_SUCCESS;
}

bool str2Interval(const char *str, const char *fmt, void *ivar)
{
	int     token;
	int16	t_z, n;
	char    word[32];
	int     i, y, m, d, h,
		mi, s, us;
	bool	tz_neg = false;
	bool    y2_as_y4 = false;
	bool	have_tz = false;
	const   char *str0 = str;
	const   char *fmt0 = fmt;

	if (!fmt || !fmt[0]) fmt = g_def_time_format;
	y = 0; m = 1; d = 1; h = 0; mi = 0; s = 0; us = 0; 
	while (*str == ' ') str++;
	while (*str != 0x0 && (token = next_tfmt(fmt, word))>0)
	{
		switch (token)
		{
		case Y2:
			if (!y2_as_y4)
			{
				y2_as_y4 = true;
				while (*str != 0x0 && (*str<'0' || *str >'9'))
					str++;
				for (i = 0; i<2; i++)
				{
					if (*str >= '0'&& *str <= '9')
						y = y * 10 + (*str++ - '0');
					else
						break;
				}				
			}
			else
			{
				y2_as_y4 = false;
				while (*str != 0x0 && (*str<'0' || *str >'9'))
					str++;
				for (i = 0; i<4; i++)
				{
					if (*str >= '0'&& *str <= '9')
						y = y * 10 + (*str++ - '0');
					else
						break;
				}
			}
			break;
		case Y3:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<3; i++)
			{
				if (*str >= '0'&& *str <= '9')
					y = y * 10 + (*str++ - '0');
				else
					break;
			}			
			break;
		case Y4:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<4; i++)
			{
				if (*str >= '0'&& *str <= '9')
					y = y * 10 + (*str++ - '0');
				else
					break;
			}
			break;
		case MM:
			m = 0; 
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
					m = m * 10 + (*str++ - '0');
				else
					break;
			}
			break;
		case DD:
			d = 0; //why 2014-12-31 add
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
					d = d * 10 + (*str++ - '0');
				else
					break;
			}
			break;
		case HH:
		case HH24:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
				{
					h = h * 10 + (*str - '0');
					str++;
				}
				else
					break;
			}
			break;
		case MI:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
				{
					mi = mi * 10 + (*str - '0');
					str++;
				}
				else
					break;
			}
			break;
		case S2:
		case S3:
		case S4:
		case S5:
		case S6:
		case S7:
		case S8:
			while (*str != 0x0 && (*str<'0' || *str >'9'))
				str++;
			for (i = 0; i<2; i++)
			{
				if (*str >= '0'&& *str <= '9')
				{
					s = s * 10 + (*str - '0');
					str++;
				}
				else
					break;
			}

			if (*str == '.')
			{
				str++;
				for (i = 0; i<6; i++)
				{
					if (*str != 0x0)
					{
						if (*str >= '0' && *str <= '9')
						{
							us = us * 10 + (*str - '0');
							str++;
						}
						else
							break;
					}
					else
						break;
				}
				while (isdigit((uchar)*str)) str++;
				us *= exp10s[6 - i];
			}
			break;		
		}
	}
	((KCIInterval *)ivar)->s = d * 24 * 3600 + h * 3600 + mi * 60 + s;
	((KCIInterval *)ivar)->us = us;
	return true;
}
//#define BUILD_JMP_TABLE
#ifdef BUILD_JMP_TABLE
typedef struct TimeToken
{
	const char *token;
	int		 token_id;
}TimeToken;

TimeToken tokens[] = {
	{ "AD", AD },
	{ "AM", AM },
	{ "BC", BC },
	{ "D", D1 },
	{ "DAY", DAY },
	{ "DD", DD },
	{ "DDD", D3 },
	{ "DY", DY },
	{ "HH", HH },
	{ "HH24", HH24 },
	{ "MI", MI },
	{ "MM", MM },
	{ "MON", MON },
	{ "MONTH", MTH },
	{ "PM", PM },
	{ "SS", S2 },
	{ "SSS", S3 },
	{ "SSSS", S4 },
	{ "SSSSS", S5 },
	{ "SSSSSS", S6 },
	{ "SSSSSSS", S7 },
	{ "SSSSSSSS", S8 },
	{ "YY", Y2 },
	{ "YYY", Y3 },
	{ "YYYY", Y4 },
};

typedef struct TkJmpItem
{
	char	shft_ch;
	int		targ_state;
}TkJmpItem;

/*��ǰ׺�ṹ(���γ�״̬ת�Ʊ�Ĺ�����ʹ�ô˽ṹ�����м�״̬������)*/
typedef struct TkHead{
	int		tk_no;		//��ȫƥ��Ĵʺ�
	int		len;		//�ֽ���	
	uchar	*bytes;		//�ֽ�	
	int		jmp_num;	//��ת�����
	TkJmpItem *jmp_tab;//��ת��
}TkHead;

typedef struct TkHeadTab{
	int		slot_num;
	volatile int item_num;
	TkHead	**heads;
}TkHeadTab;

static TkHead *heads[3000] = { 0 };
static int	  item_num = 0;

static int8 ch2jmpno1[128] = { 0 };
static TokenState states1[64] = { 0 };

/*���ǰ׺���м���һ���´�ǰ׺,������ǰ׺�ڱ��е����*/
static int add_tk_head(int tk_no, uchar *bytes, int len)
{
	TkHead *p_head;
	int		head_no;

	p_head = (TkHead *)malloc(sizeof(TkHead));
	memset(p_head, 0x0, sizeof(TkHead));
	p_head->tk_no = tk_no;
	p_head->len = len;		//�ֽ���	
	p_head->bytes = (uchar*)malloc(len);
	memcpy(p_head->bytes, bytes, len);
	head_no = item_num;
	heads[head_no] = p_head;
	item_num++;
	return head_no;
}


/*��ǰ׺��ʵıȽ�*/
inline static int head_cmp_voc(uchar *bytes, int len, TimeToken *token)
{
	int token_len = strlen(token->token);
	if (len == token_len)
		return memcmp(bytes, token->token, len);
	else if (len>token_len)
		return memcmp(bytes, token->token, token_len)<0 ? -1 : 1;
	else
		return memcmp(bytes, token->token, len)>0 ? 1 : -1;
}

/*���һ���ִ��Ƿ��Ǵʻ�ʵ�ǰ׺,���Ǵ�,�򷵻�(�ʺ�+1),����ǰ׺,�򷵻�-1,�����߽Բ���,�򷵻�0*/
static int test_head_match(uchar *bytes, int len)
{
	int		low, high, i;
	int		tklen, cmp_ret;
	TimeToken *token;

	//���ж��ֲ���,�ҵ���С�ʺ�
	low = 0;
	high = lengthof(tokens) - 1;

	while (low<high)
	{
		i = (high + low + 1) / 2;
		token = tokens + i;

		if (head_cmp_voc(bytes, len, token)>0)
			low = i;
		else
			high = i - 1;
	}

	//����ƥ����
	for (i = high; i<lengthof(tokens); i++)
	{
		token = tokens + i;
		tklen = strlen(token->token);
		if (len>tklen)
		{
			if (memcmp(bytes, token->token, tklen)<0)
				return 0;
		}
		else
		{
			cmp_ret = memcmp(bytes, token->token, len);
			if (cmp_ret<0)
				return 0;
			else if (cmp_ret == 0)
				return len == tklen ? i + 1 : -1;
		}
	}
	return 0;
}

char  stmt[64 * 1024];
char  tmp_buff[1024];
void build_tk_jmp_tab()
{
	int		i, j, start, end, head_no;
	int		len, total_jmp_num;
	uchar	head_bytes[32];

	//��head������ʼ��Ŀ
	add_tk_head(-1, (uchar*)"", 0);
	//�γ�head��
	start = 0, end = 1;
	total_jmp_num = 0;
	while (start<item_num)
	{
		//��δ�����head��Ŀ��һ���д���
		for (i = start; i<end; i++)
		{
			char	ch;
			int		head_match;
			TkHead *p_head = heads[i];
			TkJmpItem jmp_tab[256];
			int		jmp_num = 0;

			/*���Խ�head��Ŀ���ִ���չһ���ַ�(��0��0xff),���µ��ִ���ĳ���ʻ�ĳ���ʵ��Ӵ�,
			���ʾ����չ��Ч,��Ӧ����һ����head��Ŀ,���Ե�ǰhead��Ŀ����ת����,����,��ʾ
			��head��Ŀ���ܽ������ַ�,�ʽ�����Ӧת���������Ϊ-1*/
			for (ch = 0; ch<100; ch++)
			{
				len = p_head->len;
				if (len)
					memcpy(head_bytes, p_head->bytes, len);
				head_bytes[len++] = (uchar)ch;
				head_match = test_head_match(head_bytes, len);
				//���ִ��Ƿ���ĳ����ȫƥ��
				if (head_match>0)
				{
					head_no = add_tk_head(head_match - 1, head_bytes, len);
					jmp_tab[jmp_num].shft_ch = ch;
					jmp_tab[jmp_num++].targ_state = head_no;
					//jmp_tab[jmp_num].shft_ch = ch;
					//jmp_tab[jmp_num++].targ_state = -head_match;
				}
				//���ִ��Ƿ���ĳ���ʲ���ƥ��
				else if (head_match<0)
				{
					head_no = add_tk_head(-1, head_bytes, len);
					jmp_tab[jmp_num].shft_ch = ch;
					jmp_tab[jmp_num++].targ_state = head_no;
				}
			}
			total_jmp_num += jmp_num;
			p_head->jmp_num = jmp_num;
			p_head->jmp_tab = (TkJmpItem *)malloc(sizeof(TkJmpItem)*jmp_num);
			memset(p_head->jmp_tab, 0x0, sizeof(TkJmpItem)*jmp_num);
			MemCpy(p_head->jmp_tab, jmp_tab, sizeof(TkJmpItem)*jmp_num);
		}
		start = end;
		end = item_num;
	}

	//����c����״̬ת����
	stmt[0] = 0x0;

	//�ҳ������ַ����������Сֵ
	char  shift_chs[32];
	int   shift_num = 0;
	for (i = 0; i<item_num; i++)
	{
		if (heads[i]->jmp_num)
		{
			for (j = 0; j<heads[i]->jmp_num; j++)
			{
				char ch = heads[i]->jmp_tab[j].shft_ch;
				bool exists = false;
				for (int i = 0; i<shift_num; i++)
				{
					if (shift_chs[i] == ch)
					{
						exists = true;
						break;
					}
				}
				if (!exists)
					shift_chs[shift_num++] = ch;
			}
		}
	}

	//�γ��ַ���ű�
	int8  ch_no = 0;
	for (i = 0; i<128; i++)
		ch2jmpno1[i] = -1;
	for (i = 0; i<shift_num; i++)
	{
		char ch = shift_chs[i];
		char ch1 = tolower(ch);
		ch2jmpno1[ch1] = ch_no;
		ch2jmpno1[ch] = ch_no++;
	}

	//�γ�״̬ת����
	for (i = 0; i<item_num; i++)
	{
		states1[i].token_no = heads[i]->tk_no >= 0 ? tokens[heads[i]->tk_no].token_id : -1;
		for (j = 0; j<heads[i]->jmp_num; j++)
		{
			char ch = heads[i]->jmp_tab[j].shft_ch;
			int8 jmp = ch2jmpno1[ch];
			states1[i].jmp_tab[jmp] = heads[i]->jmp_tab[j].targ_state;
		}
	}

	//��ӡ�ַ�ӳ���
	sprintf(stmt, "const static int8 ch2jmpno[128]={");
	for (int i = 0; i<128; i++)
	{
		char buff[4];
		if (i % 16 == 0)
			strcat(stmt, "\r\n");
		sprintf(buff, "%2d,", ch2jmpno1[i]);
		strcat(stmt, buff);
	}
	strcat(stmt, "};\r\n\r\n");

	//��ӡ״̬ת�Ʊ�
	sprintf(stmt + astrlen(stmt), "const static TokenState states[%d]={\r\n", item_num);
	for (i = 0; i<item_num; i++)
	{
		char buff[4];
		sprintf(tmp_buff, "{%2d,{", states1[i].token_no);
		for (j = 0; j<shift_num; j++)
		{
			sprintf(buff, "%2d,", states1[i].jmp_tab[j]);
			strcat(tmp_buff, buff);
		}

		strcat(tmp_buff, "}},\r\n");
		strcat(stmt, tmp_buff);
	}
	strcat(stmt, "};");
}
#endif
