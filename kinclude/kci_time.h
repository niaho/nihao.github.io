#ifndef __KCI_TIME_H__
#define __KCI_TIME_H__
#include "kci_types.h"

typedef enum TFmt
{
	AD = 1, AM, BC, DAY, D1,
	DD, D3, DY, HH, HH24, MI,
	MM, MON, MTH, PM, S2,
	S3, S4, S5, S6, S7, S8,
	Y2, Y3, Y4, T_WORD,
}TFmt;

typedef struct tagDateTime
{
	int		year;	/*年*/
	int		month;	/*月(1-12)*/
	int	    mday;	/*日(1-31)*/
	int		wday;	/*一周中日期数(0-6)*/
	int		yday;	/*在一年中的天数(0-365)*/
	int		s;		/*一天中秒数*/
	int		us;		/*微秒值*/
	short   tz;     /*时区差*/
	bool    btz;    /*时区域有效*/
}DateTime;

#define KCIDateGetTime(date, hour, min, sec) \
{ \
	*hour = (date)->KCIDateTime.KCITimeHH; \
	*min = (date)->KCIDateTime.KCITimeMI; \
	*sec = (date)->KCIDateTime.KCITimeSS; \
}

#define KCIDateGetDate(date, year, month, day) \
{ \
	*year = (date)->KCIDateYYYY; \
	*month = (date)->KCIDateMM; \
	*day = (date)->KCIDateDD; \
}

#define KCIDateSetTime(date, hour, min, sec) \
{ \
	(date)->KCIDateTime.KCITimeHH = hour; \
	(date)->KCIDateTime.KCITimeMI = min; \
	(date)->KCIDateTime.KCITimeSS = sec; \
}

#define KCIDateSetDate(date, year, month, day) \
{ \
	(date)->KCIDateYYYY = year; \
	(date)->KCIDateMM = month; \
	(date)->KCIDateDD = day; \
}

/*将时间格式化为时间结构，使用格利高利历*/
void  dt2tm(int64 t, DateTime *p_dt);

/*将日期格式化为时间结构，使用格利高利历*/
void  date2tm(int32 date, DateTime *p_dt);
/*将时间结构转化为时间，1582年10月5日前使用儒略历，以后使用格利高利历*/
int64 tm2dt(DateTime *p_tm);
/*将时间结构转化为日期值，1582年10月5日前使用儒略历，以后使用格利高利历*/
int32  tm2date(DateTime *p_tm);
/*将字串按转换成日期时间(带时区)格式*/
bool  str2tm(const char *str, const char *fmt, int scale, DateTime &tm);

/*将日期时间(带时区)转换成字串按格式*/
signed int tm2str(DateTime &dt, char *buff, const char *fmt, int tf);

bool str2Interval(const char *str, const char *fmt, void *ivar);


#endif
