#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include "kci_time.h"
#include "kci_defs.h"

DateTime KCIDateToDateTime(const KCIDate *date)
{
	DateTime dt;
	dt.year = date->KCIDateYYYY;
	dt.month = date->KCIDateMM;
	dt.mday = date->KCIDateDD;
	dt.s = (date->KCIDateTime.KCITimeHH * 60 + date->KCIDateTime.KCITimeMI) * 60 + date->KCIDateTime.KCITimeSS;
	return dt;
};

KCIDate  KCIDateTimetoKCIDate(const KCIDateTime *date)
{
	KCIDate dt;
	dt.KCIDateYYYY = date->KCIDateYYYY;
	dt.KCIDateMM = date->KCIDateMM;
	dt.KCIDateDD = date->KCIDateDD;
	dt.KCIDateTime.KCITimeSS = date->KCITimeSS;
	dt.KCIDateTime.KCITimeMI = date->KCITimeMI;
	dt.KCIDateTime.KCITimeHH = date->KCITimeHH;
	return dt;
};

KCIDateTime  KCIDatetoKCIDateTime(const KCIDate *date)
{
	KCIDateTime dt;
	dt.KCIDateYYYY = date->KCIDateYYYY;
	dt.KCIDateMM = date->KCIDateMM;
	dt.KCIDateDD = date->KCIDateDD;
	dt.KCITimeSS = date->KCIDateTime.KCITimeSS;
	dt.KCITimeMI = date->KCIDateTime.KCITimeMI;
	dt.KCITimeHH = date->KCIDateTime.KCITimeHH;
	return dt;
};

KCIDate  DateTimetoKCIDate(const DateTime *date)
{
	KCIDate dt;
	dt.KCIDateYYYY = date->year;
	dt.KCIDateMM = date->month;
	dt.KCIDateDD = date->mday;
	int s = date->s;
	dt.KCIDateTime.KCITimeSS = s % 60;
	s /= 60;
	dt.KCIDateTime.KCITimeMI = s % 60;
	s /= 60;
	dt.KCIDateTime.KCITimeHH = s;

	return dt;
};


signed int KCIDateToText(KCIError *err, const KCIDate *date,
	const oratext *fmt, ub1 fmt_length,
	const oratext *lang_name, ub4 lang_length,
	ub4 *buf_size, oratext *buf)
{
	DateTime dt = KCIDateToDateTime(date);	
	tm2str(dt, (char *)buf, (char *)fmt, *buf_size);
	return KCI_SUCCESS;
};

sword KCIDateFromText(KCIError *err, const oratext *date_str,
	ub4 d_str_length, const oratext *fmt, ub1 fmt_length,
	const oratext *lang_name, ub4 lang_length,
	KCIDate *date)
{
	DateTime dt;
	////这里时区，当前还没有对照
	str2tm((char *)date_str, (char *)fmt, 6, dt);
	*date = DateTimetoKCIDate(&dt);
	return KCI_SUCCESS;
};
sword KCIDateTimeToText(void *hndl, KCIError *err, const KCIDateTime *date,
	const OraText *fmt, ub1 fmt_length, ub1 fsprec,
	const OraText *lang_name, size_t lang_length,
	ub4 *buf_size, OraText *buf)
{	
	const KCIDate date2 = KCIDateTimetoKCIDate(date);
	DateTime dt = KCIDateToDateTime(&date2);
	tm2str(dt, (char *)buf, (char *)fmt, *buf_size);
	return KCI_SUCCESS;
};

sword KCIDateTimeFromText(void  *hndl, KCIError *err, const OraText *date_str,
	size_t dstr_length, const OraText *fmt, ub1 fmt_length,
	const OraText *lang_name, size_t lang_length, KCIDateTime *date)
{

	DateTime dt;
	////这里时区，当前还没有对照
	str2tm((char *)date_str, (char *)fmt, 6, dt);
	KCIDate date2 = DateTimetoKCIDate(&dt);
	*date = KCIDatetoKCIDateTime(&date2);
	return KCI_SUCCESS;
};


sword KCIIntervalFromText(void *hndl, KCIError *err, const OraText *inpstr,
	size_t str_len, KCIInterval *result)
{
	str2Interval((char *)inpstr, "dd h24:mi:ss.ssssss", result);
	return KCI_SUCCESS;
}

sword KCIIntervalToText(void  *hndl, KCIError *err, const KCIInterval *inter,
	ub1 lfprec, ub1 fsprec,	OraText *buffer, size_t buflen, size_t *resultlen)
{
	int s = inter->s;
	int SS = s % 60;
	s /= 60;
	int MI = s % 60;
	s /= 60;
	int HH = s % 24;
	s /= 24;
	int dd = s;
        memset(buffer,0, strlen((char *)buffer));
       //  strset((char *)buffer, strlen((char *)buffer));
        sprintf((char *)buffer, "%d %d:%d:%d.%d", dd,HH,MI,SS,inter->us);
	*resultlen = strlen((char *)buffer);
	return KCI_SUCCESS;
}

sword KCIDateTimeIntervalAdd(void *hndl, KCIError *err, KCIDateTime *datetime,
	KCIInterval *inter, KCIDateTime *outdatetime)
{
	KCIDate date2 = KCIDateTimetoKCIDate((const KCIDateTime *)datetime);
	DateTime dt = KCIDateToDateTime(&date2);
	int64 totalUS = tm2dt(&dt);
	totalUS += inter->s * 1000000 + inter->us;
	dt2tm(totalUS, &dt);

	(*outdatetime).KCIDateYYYY = dt.year;
	(*outdatetime).KCIDateMM = dt.month;
	(*outdatetime).KCIDateDD = dt.mday;
	int s = dt.s;
	(*outdatetime).KCITimeSS = s % 60;
	s /= 60;
	(*outdatetime).KCITimeMI = s % 60;
	s /= 60;
	(*outdatetime).KCITimeHH = s;
	(*outdatetime).KCITimeUS = dt.us;
	return KCI_SUCCESS;
}

sword KCIDateTimeConvert(void *hndl, KCIError *err, KCIDateTime *indate,
	KCIDateTime *outdate)
{
	(*outdate).KCIDateYYYY = (*indate).KCIDateYYYY;
	(*outdate).KCIDateMM = (*indate).KCIDateMM;
	(*outdate).KCIDateDD = (*indate).KCIDateDD;
	(*outdate).KCITimeSS = (*indate).KCITimeSS;
	(*outdate).KCITimeMI = (*indate).KCITimeMI;
	(*outdate).KCITimeHH = (*indate).KCITimeHH;
	(*outdate).KCITimeUS = (*indate).KCITimeUS;
	return KCI_SUCCESS;
}
