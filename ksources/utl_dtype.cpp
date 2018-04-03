#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_net.h"
#include "kci_time.h"
#include "kci_utils.h"

#if defined(LINUX)
extern int IsBadWritePtr(void * ptr_buffer, unsigned long buffer_size);
#endif

/*将Ora数据类型转换成Kunlun数据类型,通过ret_ptr,dlen,dt返回转换后的数据的地址,长度,类型号
ret_ptr返回的地址应该有三种情况:
1.当原数据不需要进行转换时,rep_ptr=sdat
2.转换后数据不超过512字节时,应将数据转换至buff中,而ret_ptr=buff
3.转换拮数据超过512字节时，应在p_mem中申请内存
*/
sword toKunData(MemCtx *p_mem, void *sdat, int &dlen, sb2 &dt, char buff[512], char *&ret_ptr)
{
	switch (dt)
	{
	case SQLT_CHR:
		ret_ptr = (char*)sdat;
		dt = TYPE_CHAR;
		break;
	case SQLT_AFC:    //add by mazh 2016-10-20
		ret_ptr = (char*)sdat;
		dt = TYPE_CHAR;
		break;
	case SQLT_AVC:  //add by mazh 2016-10-20
		ret_ptr = (char*)sdat;
		dt = TYPE_CHAR;
		break;
	case SQLT_INT:
		switch (dlen)
		{
		case 1:
			*(sb1*)buff = *(sb1*)sdat;
			ret_ptr = buff;
			dt = TYPE_I1;
			break;
		case 2:
			*(sb2*)buff = *(sb2*)sdat;
			_xg_hton2(buff);
			ret_ptr = buff;
			dt = TYPE_I2;
			break;
		default:
			*(int*)buff = *(int*)sdat;
			_xg_hton4(buff);
			ret_ptr = buff;
			dt = TYPE_I4;
			dlen = 4;
			break;
		}		
		break;
	case SQLT_LNG:
		*(sb8*)buff = *(sb8*)sdat;
		_xg_hton8(buff);
		ret_ptr = buff;
		dt = TYPE_I8;
		dlen = 8;
		break;
	case SQLT_STR:
		ret_ptr = (char*)sdat;
		dlen = (int)strlen((char*)sdat);
		dt = TYPE_CHAR;
		break;
	case SQLT_FLT:
		*(sb4*)buff = *(sb4*)sdat;
		_xg_hton4(buff);
		ret_ptr = buff;
		dt = TYPE_R4;
		dlen = 4;
		break;
	case SQLT_BIN: //add by mazh 2016-10-20
		ret_ptr = (char*)sdat;
		dt = TYPE_BINARY;
		break;
	case SQLT_NUM:
		double d;
		d = *(double*)sdat;
		double_to_string(d, buff);
		ret_ptr = (char*)buff;
		dlen = (int)strlen(buff);
		dt = TYPE_NUM;
		break;
	case SQLT_DAT:  //7字节形式的oracle日期
		{
			DateTime d = { 0 };
			ub1     *ptr = (ub1*)sdat;
			
			d.year = (((int)(*ptr))-100)*100 + ((int)(*(ptr+1))-100);
			d.month = *(ptr+2);
			d.mday = *(ptr+3);
			d.s = ((int)*(ptr+4))*3600 + ((int)*(ptr+5)) * 60 + *(ptr+6);
			*(sb8*)buff =  tm2dt(&d);
			_xg_hton8(buff);
			ret_ptr = buff;
			dt = TYPE_DATETIME;
			dlen = 8;
		}
		break;
	case SQLT_DATE:		/* ANSI Date */
		break;
	case SQLT_TIME:     /* TIME */
	case SQLT_TIMESTAMP:
	case SQLT_ODT:      /* KCIDate type */
		{
			////8字节网络序整型，为1970-1-1 0:0:0秒起的微秒数
			sb8     kndate;
			KCIDate *ocid = (KCIDate *)sdat;
			DateTime d;
			memset(&d, 0, sizeof(d));
			d.year = ocid->KCIDateYYYY;
			d.month = ocid->KCIDateMM;
			d.mday = ocid->KCIDateDD;
			d.s = ocid->KCIDateTime.KCITimeHH * 3600 + 
				  ocid->KCIDateTime.KCITimeMI * 60 + 
				  ocid->KCIDateTime.KCITimeSS; //15 * 60-41;		
			kndate = tm2dt(&d);

			*(sb8*)buff = kndate;
			_xg_hton8(buff);
			ret_ptr = buff;
			dt = TYPE_DATETIME;
			dlen = 8;
		}
		break;
	case SQLT_TIME_TZ:        /* TIME WITH TIME ZONE */
	case SQLT_TIMESTAMP_TZ:      /* TIMESTAMP WITH TIME ZONE */
	case SQLT_TIMESTAMP_LTZ:      /* TIMESTAMP WITH LOCAL TZ */
		{
			////10字节，前8字节为Datatime值，后两字节为2字节网络序整数，表示该时区与格林威治时间相差的分钟数
			sb8 kndate;
			KCIDate *ocid = (KCIDate *)sdat;
			DateTime d;
			memset(&d, 0, sizeof(d));
			d.year = ocid->KCIDateYYYY;
			d.month = ocid->KCIDateMM;
			d.mday = ocid->KCIDateDD;
			d.s = ocid->KCIDateTime.KCITimeHH * 3600 + 
				ocid->KCIDateTime.KCITimeMI * 60 + 
				ocid->KCIDateTime.KCITimeSS;		
			kndate = tm2dt(&d);
			*(sb8*)buff = kndate;
			_xg_hton8(buff);
			sb2 k2;
			k2 = 8;
			*(sb2*)(buff + 8) = k2;
			_xg_hton2(buff + 8);
			ret_ptr = buff;
			dt = TYPE_DATETIMEZ;
			dlen = 10;
		}
		break;
	case SQLT_INTERVAL_YM:       /* INTERVAL YEAR TO MONTH */
		////4字节整型，月数
		*(sb4*)buff = *(sb4*)sdat;
		_xg_hton4(buff);
		ret_ptr = buff;
		dt = TYPE_INTERVAL_Y2M;
		dlen = 4;
		break;
	case SQLT_INTERVAL_DS:    /* INTERVAL DAY TO SECOND */
		///8字节整型，微秒数
		*(sb8*)buff = *(sb8*)sdat;
		_xg_hton8(buff);
		ret_ptr = buff;
		dt = TYPE_INTERVAL_D2S;
		dlen = 8;
		break;
	case SQLT_BLOB:    /* 标志位不含 PARAM_IS，则数据就是clob的数据，否则，内含8字节流标志符 */
		{
		    KCILobLocator   *v1 = (KCILobLocator *)sdat;
			if  ((IsBadWritePtr(v1, sizeof(KCILobLocator))==0) &&  (v1->lobEmpty))  ///按照标签模式			
        	{
				memcpy(buff, v1->name, 8);
				ret_ptr = buff; 
				buff[8] = 0;
				dt = TYPE_BLOB;
				dlen = 8;
			}
			else
			{
				ret_ptr = (char *)(v1->dat);
				dt = TYPE_BLOB;
				dlen = (int)(v1->datLen);
			}			
		}
		break;
	case SQLT_CLOB:    /* 标志位不含 PARAM_IS，则数据就是clob的数据，否则，内含8字节流标志符 */
		{	
			KCILobLocator   *v1 = (KCILobLocator *)sdat;
			if (v1->lobEmpty)  ///按照标签模式
			{
				memcpy(buff, v1->name, 8);
				ret_ptr = buff;
				buff[8] = 0;
				dt = TYPE_CLOB;
				dlen = 8;
			}
			else
			{
				ret_ptr = (char *)(v1->dat);
				dt = TYPE_CLOB;
				dlen = (int)(v1->datLen);
			}
		}
		break;
	default:
		/*todo*/
		break;
	}
	return KCI_SUCCESS;
}

sb2 sqlT2KunT(sb2 dt, sb2 val_len)
{
	switch (dt)
	{
	case SQLT_CHR:
		return TYPE_CHAR;
	case SQLT_INT:
		switch (val_len)
		{
		case 1:
		    return  TYPE_I1;
		case 2:
			return  TYPE_I2;
		default:
			return  TYPE_I4;
		}
	case SQLT_LNG:
		return TYPE_I8;
	case SQLT_AFC:
		return TYPE_CHAR;
	case SQLT_STR:
		return TYPE_CHAR;
	case SQLT_FLT:
		return TYPE_R4;
	case SQLT_NUM:
		return TYPE_NUM;
	case SQLT_DAT:  /* date in oracle format */
		return TYPE_DATETIME;
	case SQLT_BIN:
		return TYPE_BINARY;
	case SQLT_DATE: /* ANSI Date */
		return TYPE_DATETIME;
	case SQLT_TIME:          /* TIME */
		return TYPE_TIME;
	case SQLT_TIMESTAMP:
		return TYPE_DATETIME;
	case SQLT_ODT:                  /* KCIDate type */
		return TYPE_DATETIME;
		break;
	case SQLT_TIME_TZ:            /* TIME WITH TIME ZONE */
		return TYPE_TIMEZ;
	case SQLT_TIMESTAMP_TZ:      /* TIMESTAMP WITH TIME ZONE */
		return TYPE_DATETIMEZ;
	case SQLT_TIMESTAMP_LTZ:      /* TIMESTAMP WITH LOCAL TZ */
		return TYPE_DATETIMEZ;
		break;
	case SQLT_INTERVAL_YM:       /* INTERVAL YEAR TO MONTH */
		return TYPE_INTERVAL_Y2M;
	case SQLT_INTERVAL_DS:    /* INTERVAL DAY TO SECOND */
		return TYPE_INTERVAL_D2S;
	case SQLT_BLOB:
		return TYPE_BLOB;
	case SQLT_CLOB:
		return TYPE_CLOB;
	default:
		return TYPE_BINARY;
	}
	return  TYPE_BINARY;
}

#define  loadI1(ptr)  ((sb1)(*(sb1*)ptr))

inline static sb2 loadI2(void *ptr)
{
	sb2  i2 = *(sb2*)ptr;
	_xg_hton2(&i2);
	return i2;
}
inline static sb4 loadI4(void *ptr)
{
	sb4  i4 = *(sb4*)ptr;
	_xg_hton4(&i4);
	return i4;
}

inline static sb8 loadI8(void *ptr)
{
	sb8  i8 = *(sb8*)ptr;
	_xg_hton8(&i8);
	return i8;
}

inline static float loadR4(void *ptr)
{
	float  r4 = *(float*)ptr;
	_xg_hton4(&r4);
	return r4;
}

inline static double loadR8(void *ptr)
{
	double  r8 = *(double*)ptr;
	_xg_hton8(&r8);
	return r8;
}

inline static double str2R8(void *str, int slen)
{
	char buff[256];
	memcpy(buff, str, slen);
	buff[slen] = 0x0;
	return atof(buff);
}

inline static void i4ToNum(char *num_buf,sb4 i4)
{
	sprintf(num_buf, "%d", i4);
}

inline static void i8ToNum(char *num_buf, sb8 i4)
{
	sprintf(num_buf, "%ld", i4);
}

inline static void r4ToNum(char *num_buf, float r4)
{
	sprintf(num_buf, "%f", r4);
}

inline static void r8ToNum(char *num_buf, double r4)
{
	sprintf(num_buf, "%lf", r4);
}

inline static void str2Num(char *num_buf, void *str, int slen)
{	
	memcpy(num_buf, (char *)str, slen);
	num_buf[slen] = 0;
}


/*数据库原始数据转换，返回结果长度(负数表示截断后的长度)*/
int   toSysData(int kun_dt, void *kun_dat,  int kun_siz, int val_dt, char *val_ptr, int var_size)
{
	switch (val_dt)
	{
	case SQLT_CHR:   
	{
		if (kun_siz > var_size)
		{
			memcpy((char *)(val_ptr), (char *)kun_dat, var_size);
			return -var_size;
		}
		else
		{
			memcpy((char *)(val_ptr), (char *)kun_dat, kun_siz);
			return kun_siz;
		}
	}
	break;
	case SQLT_AFC:  
	case SQLT_AVC: 
	{
		if (kun_siz > var_size)
		{
			memcpy((char *)(val_ptr), (char *)kun_dat, var_size);
			return -var_size;
		}
		else
		{
			memcpy((char *)(val_ptr), (char *)kun_dat, kun_siz);
			return kun_siz;
		}
	}
	break;
	case SQLT_STR:
	{
		if (kun_siz > (var_size - 1))
		{
			memcpy((char *)(val_ptr), (char *)kun_dat, var_size - 1);
			val_ptr[var_size-1] = 0x0;
			return -var_size;
		}
		else
		{
			memcpy((char *)(val_ptr), (char *)kun_dat, kun_siz);
			val_ptr[kun_siz] = 0x0;
			return kun_siz+1;
		}
	}
	break;
	case SQLT_INT:
	{
		switch (kun_dt)
		{
		case TYPE_BOOL:
			*(sb4*)val_ptr = loadI1(kun_dat);
			break;
		case TYPE_I1:
			*(sb4*)val_ptr = loadI1(kun_dat);
			break;
		case TYPE_I2:
			*(sb4*)val_ptr = loadI2(kun_dat);
			break;
		case TYPE_I4:
			*(sb4*)val_ptr = loadI4(kun_dat);
			break;
		case TYPE_I8:
			*(sb4*)val_ptr = (sb4)loadI8(kun_dat);
			break;
		case TYPE_R4:
			*(sb4*)val_ptr = (sb4)loadR4(kun_dat);
			break;
		case TYPE_R8:
			*(sb4*)val_ptr = (sb4)loadR8(kun_dat);
			break;
		case TYPE_NUM:
			*(sb4*)val_ptr = (sb4)str2R8(kun_dat, kun_siz);
			break;
		case TYPE_CHAR:
		case TYPE_CHR0:
			*(sb4*)val_ptr = (sb4)str2R8(kun_dat, kun_siz);
			break;
		}
		return 4;
	}
	break;
	case SQLT_LNG:
		switch (kun_dt)
		{
		case TYPE_I1:
			*(sb8*)val_ptr = loadI1(kun_dat);
			break;
		case TYPE_I2:
			*(sb8*)val_ptr = loadI2(kun_dat);
			break;
		case TYPE_I4:
			*(sb8*)val_ptr = loadI4(kun_dat);
			break;
		case TYPE_I8:
			*(sb8*)val_ptr = loadI8(kun_dat);
			break;
		case TYPE_R4:
			*(sb8*)val_ptr = (sb8)loadR4(kun_dat);
			break;
		case TYPE_R8:
			*(sb8*)val_ptr = (sb8)loadR8(kun_dat);
			break;
		case TYPE_NUM:
			*(sb8*)val_ptr = (sb8)str2R8(kun_dat, kun_siz);
			break;
		case TYPE_CHAR:
		case TYPE_CHR0:
			*(sb8*)val_ptr = (sb8)str2R8(kun_dat, kun_siz);
			break;
		}
		return 8;
		break;
	case SQLT_BIN: 
	{
		if (kun_siz > var_size)
		{
			memcpy((char *)(val_ptr), (char *)kun_dat, var_size);
			return -var_size;
		}
		else
		{
			memcpy((char *)(val_ptr), (char *)kun_dat, kun_siz);
			return kun_siz;
		}
	}
	break;
	case SQLT_FLT:
	case SQLT_BFLOAT:
		switch (kun_dt)
		{
		case TYPE_I1:
			*(float*)val_ptr = (float)loadI1(kun_dat);
			break;
		case TYPE_I2:
			*(float*)val_ptr = (float)loadI2(kun_dat);
			break;
		case TYPE_I4:
			*(float*)val_ptr = (float)loadI4(kun_dat);
			break;
		case TYPE_I8:
			*(float*)val_ptr = (float)loadI8(kun_dat);
			break;
		case TYPE_R4:
			*(float*)val_ptr = (float)loadR4(kun_dat);
			break;
		case TYPE_R8:
			*(float*)val_ptr = (float)loadR8(kun_dat);
			break;
		case TYPE_NUM:
			*(float*)val_ptr = (float)str2R8(kun_dat, kun_siz);
			break;
		case TYPE_CHAR:
		case TYPE_CHR0:
			*(float*)val_ptr = (float)str2R8(kun_dat, kun_siz);
			break;
		}
		return 4;
	case SQLT_NUM:
	case SQLT_BDOUBLE:
		switch (kun_dt)
		{
		case TYPE_I1:
			*(double*)val_ptr = (double)loadI1(kun_dat);
			break;
		case TYPE_I2:
			*(double*)val_ptr = (double)loadI2(kun_dat);
			break;
		case TYPE_I4:
			*(double*)val_ptr = (double)loadI4(kun_dat);
			break;
		case TYPE_I8:
			*(double*)val_ptr = (double)loadI8(kun_dat);
			break;
		case TYPE_R4:
			*(double*)val_ptr = (double)loadR4(kun_dat);
			break;
		case TYPE_R8:
			*(double*)val_ptr = (double)loadR8(kun_dat);
			break;
		case TYPE_NUM:
			*(double*)val_ptr = (double)str2R8(kun_dat, kun_siz);
			break;
		case TYPE_CHAR:
		case TYPE_CHR0:
			*(double*)val_ptr = (double)str2R8(kun_dat, kun_siz);
			break;
		}
		return 8;

	case SQLT_DAT:
	{
		sb8      t;
		sb2      tz;
		DateTime dt = { 0 };
		ub1  *ptr = (ub1*)val_ptr;

		switch (kun_dt)
		{
		case TYPE_DATE:
			date2tm(loadI4(kun_dat), &dt);
			break;
		case TYPE_DATETIME:
			dt2tm(loadI8(kun_dat)*1000, &dt);
			break;
		case TYPE_DATETIMEZ:
			t = loadI8(kun_dat);
			tz = loadI2(((char*)kun_dat) + 8);
			t += tz * 60000000LL;
			dt2tm(t, &dt);
			break;
		}
		*ptr = (dt.year / 100) + 100;
		*(ptr + 1) = (dt.year % 100) + 100;
		*(ptr + 2) = dt.month;
		*(ptr + 3) = dt.mday;
		*(ptr + 4) = dt.s / 3600;
		*(ptr + 5) = (dt.s % 3600) / 60;
		*(ptr + 6) = dt.s % 60;
		return 7;
	}
	break;
	case SQLT_TIME:          /* TIME */
	case SQLT_TIMESTAMP:
	case SQLT_ODT:
		//// DATETIME 转 KCIDate为类型    DATETIME， 8字节网络序整型，为1970-1-1 0:0:0秒起的微秒数
	{
		char fd[32];
		memcpy(fd, kun_dat, kun_siz);
		fd[kun_siz] = 0;
		DateTime d;
		if (kun_siz == 4)
		{
			_xg_hton4(fd);
			sb4 *i2 = (sb4 *)fd;
			date2tm(*i2, &d);
		}
		else
		{
			_xg_ntoh8(fd);
			sb8 *kndate = (sb8*)fd;
			*kndate = (*kndate) * 1000;   ///Datetime因历史问题，返回时以 ms为单位，入库时以us为单位,所以这里要乘以1000.					
			memset(&d, 0, sizeof(d));
			dt2tm(*kndate, &d);
		}

		KCIDate *ocid = (KCIDate *)(val_ptr);
		ocid->KCIDateYYYY = d.year;
		ocid->KCIDateMM = d.month;
		ocid->KCIDateDD = d.mday;
		int ds = d.s;
		ocid->KCIDateTime.KCITimeHH = ds / 3600;
		ds = ds % 3600;
		ocid->KCIDateTime.KCITimeMI = ds / 60;
		ocid->KCIDateTime.KCITimeSS = ds % 60;
		return sizeof(ocid);
	}
	break;
	case SQLT_TIME_TZ:        /* TIME WITH TIME ZONE */
	case SQLT_TIMESTAMP_TZ:      /* TIMESTAMP WITH TIME ZONE */
	case SQLT_TIMESTAMP_LTZ:      /* TIMESTAMP WITH LOCAL TZ */
	{
		////10字节，前8字节为Datatime值，后两字节为2字节网络序整数，表示该时区与格林威治时间相差的分钟数
		char fd[32];
		memcpy(fd, kun_dat, 8);
		fd[8] = 0;
		_xg_hton8(fd);
		sb8 *kndate = (sb8*)fd;
		DateTime d;
		memset(&d, 0, sizeof(d));
		dt2tm(*kndate, &d);

		KCIDate *ocid = (KCIDate *)(val_ptr);
		ocid->KCIDateYYYY = d.year;
		ocid->KCIDateMM = d.month;
		ocid->KCIDateDD = d.mday;
		long ds = d.s;
		ocid->KCIDateTime.KCITimeHH = ds / 3600;
		ds = ds % 3600;
		ocid->KCIDateTime.KCITimeMI = ds / 60;
		ocid->KCIDateTime.KCITimeSS = ds % 60;
		///时区暂时不加
		memcpy(fd, (char *)kun_dat + 8, 2);
		fd[2] = 0;
		_xg_hton2(fd);
		sb2 *kntz = (sb2*)fd;
		if (*kntz > 8)
			*kntz = 0;
		return sizeof(ocid);
	}
	break;
	case SQLT_INTERVAL_YM:       /* INTERVAL YEAR TO MONTH */
	{	////4字节整型，月数
		char buff[8];
		*(sb4*)buff = *(sb4*)kun_dat;
		sb4 *i2 = (sb4 *)(val_ptr);
		_xg_hton4(buff);
		*i2 = *(sb4*)buff;
		return 4;
	}
	break;
	case SQLT_INTERVAL_DS:    /* INTERVAL DAY TO SECOND */
	{
		///8字节整型，微秒数
		char fd[32];
		memcpy(fd, kun_dat, kun_siz);
		fd[kun_siz] = 0;
		_xg_hton8(fd);
		sb8 *i2 = (sb8 *)(val_ptr);
		*i2 = *(sb8*)fd;
		return 8;
	}
	break;
	case SQLT_CLOB:
	{
		//////TYPE_CLOB     若首字节为‘C’，则该列数据的后面部分为clob对象的描述符（descriptor）,否则为'c'，后面部分就是clob的数据 
		KCILobLocator  *v1 = (KCILobLocator *)(val_ptr);
		v1->dat = (char *)kun_dat + 1;
		v1->datLen = kun_siz - 1;
		v1->flags = ((char *)kun_dat)[0] == 'C' ? 1 : 0;
		return sizeof(KCILobLocator);
	}
	break;
	case SQLT_BLOB:
	{
		//////TYPE_BLOB     若首字节为‘B’，则该列数据的后面部分为blob对象的描述符（descriptor）,否则为'b'，后面部分就是blob的数据   
		KCILobLocator  *v1 = (KCILobLocator *)(val_ptr);
		v1->dat = (char *)kun_dat + 1;
		v1->datLen = kun_siz - 1;
		v1->flags = ((char *)kun_dat)[0] == 'B' ? 1 : 0;
		return sizeof(KCILobLocator);
	}
	break;
	default:
		/*todo*/
		break;
	}
	return 0;
}
