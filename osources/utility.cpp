
#include "odbc.h"

/*
  @type    : myodbc internal
  @purpose : copies the string data to rgbValue buffer. If rgbValue
       is NULL, then returns warning with full length, else
       copies the cbValueMax length from 'src' and returns it.
*/

SQLRETURN copy_str_data(SQLSMALLINT HandleType, SQLHANDLE Handle,
			SQLCHAR FAR *rgbValue,SQLSMALLINT cbValueMax,
			SQLSMALLINT FAR *pcbValue,char FAR *src)
{
  SQLSMALLINT dummy;

  if (!pcbValue)
    pcbValue = &dummy;

  if (cbValueMax == SQL_NTS)
    cbValueMax = *pcbValue = (sb2)strlen(src);

  else if (cbValueMax < 0)
    return setError(HandleType,Handle,RHERR_S1090,NULL,0);

  else
  {
    cbValueMax = cbValueMax ? cbValueMax - 1 : 0;
    *pcbValue =(sb2) strlen(src);
  }

  if (rgbValue)
    strncpy1((char*)rgbValue, src, cbValueMax);

  if (min(*pcbValue , cbValueMax ) != *pcbValue)
    return SQL_SUCCESS_WITH_INFO;
  return SQL_SUCCESS;
}

/*
  @purpose : returns (possibly truncated) results
       if result is truncated the result length contains
       length of the truncted result
*/

SQLRETURN
copy_lresult(SQLSMALLINT HandleType, SQLHANDLE Handle,
	     SQLCHAR FAR *rgbValue, int cbValueMax,
	     int FAR *pcbValue,char *src,long src_length,
	     long max_length,long fill_length,ulong *offset,
	     bool binary_data)
{
  char *dst=(char*) rgbValue;
  ulong length;
  int arg_length;

  if (src && src_length == SQL_NTS)
    src_length= strlen(src);

  arg_length= cbValueMax;
  if (cbValueMax && !binary_data)   /* If not length check */
    cbValueMax--;       /* Room for end null */
  else if (!cbValueMax)
    dst=0;          /* Don't copy anything! */
  if (max_length)       /* If limit on char lengths */
  {
    set_if_smaller(cbValueMax,(long) max_length);
    set_if_smaller(src_length,max_length);
    set_if_smaller(fill_length,max_length);
  }
  if (HandleType == SQL_HANDLE_DBC)
  { 
    if (fill_length < src_length || !Handle ||
        !(((SQLDBC FAR*)Handle)->flag & FLAG_PAD_SPACE))
      fill_length=src_length;
  }
  else 
  { 
    if (fill_length < src_length || !Handle || 
        !(((SQLSTMT FAR*)Handle)->p_svctx->flag & FLAG_PAD_SPACE))
      fill_length=src_length;
  }
  if (*offset == (ulong) ~0L)
    *offset=0;          /* First call */
  else if (arg_length && *offset >= (ulong)fill_length)
    return SQL_NO_DATA_FOUND;
  src+= *offset ; src_length-= *offset; fill_length-= *offset;
  length=min(fill_length, cbValueMax);
  (*offset)+=length;        /* Fix for next call */
  if (pcbValue)
    *pcbValue=fill_length;
  if (dst)          /* Bind allows null pointers */
  {
    ulong copy_length= ((long) src_length >= (long) length ? length :
      ((long) src_length >= 0 ? src_length : 0L));
    memcpy(dst,src,copy_length);
    memset(dst+copy_length,' ',length-copy_length);
    if (!binary_data || length != (ulong) cbValueMax)
      dst[length]=0;
  }
  if (arg_length && cbValueMax >= fill_length)
    return SQL_SUCCESS;
  DBUG_PRINT("info",("Returned %ld characters from offset: %ld",
         length,*offset - length));
  setError(HandleType,Handle,RHERR_01004,NULL,0);
  return SQL_SUCCESS_WITH_INFO;
}

/*
  @purpose : is used when converting a binary string to a SQL_C_CHAR
*/

char NEAR _dig_vec[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

SQLRETURN
copy_binary_result(SQLSMALLINT HandleType, SQLHANDLE Handle,
		   SQLCHAR FAR *rgbValue,int cbValueMax,
		   int FAR *pcbValue,char *src,ulong src_length,
		   ulong max_length,ulong *offset)
{
  char *dst=(char*) rgbValue;
  ulong length;

  if (!cbValueMax)
    dst=0;       /* Don't copy anything! */
  if (max_length) /* If limit on char lengths */
  {
    set_if_smaller(cbValueMax,(long) max_length+1);
    set_if_smaller(src_length,(max_length+1)/2);
  }
  if (*offset == (ulong) ~0L)
    *offset=0;    /* First call */
  else if (*offset >= src_length)
    return SQL_NO_DATA_FOUND;
  src+= *offset ; src_length-= *offset;
  length = cbValueMax ? (ulong)(cbValueMax-1)/2 : 0;
   length=min(src_length,length);
  (*offset)+=length;      /* Fix for next call */
  if (pcbValue)
    *pcbValue=src_length*2;
  if (dst)      /* Bind allows null pointers */
  {
    ulong i;
    for (i=0 ; i < length ; i++)
    {
      *dst++= _dig_vec[(uchar) *src >> 4];
      *dst++= _dig_vec[(uchar) *src++ & 15];
    }
    *dst=0;
  }
  if ((ulong) cbValueMax > length*2)
    return SQL_SUCCESS;
  DBUG_PRINT("info",("Returned %ld characters from offset: %ld",
         length,*offset - length));

  setError(HandleType,Handle,RHERR_01004,NULL,0);
  return SQL_SUCCESS_WITH_INFO;
}

/*
  @purpose : get type, transfer length and precision for a unireg column
       note that timestamp is changed to YYYY-MM-DD HH:MM:SS type
*/

int getFieldInfo(SQLSTMT FAR *stmt, 
			     KCIFld *field, 
				 char   *buff,
				 ulong  *transfer_length, 
				 ulong  *size,
				 ulong  *precision,
				 ulong  *display_size)
{
	char *pos;
  
	*precision = 0;
	switch(field->type_id) 
	{
	case TYPE_NUM:
		if(field->scale>0)
		{
			*display_size = *size = *transfer_length = (field->scale)>>16;
			*precision = (field->scale)&0x0ffff;
		}
		else
		{
			*display_size = *size = *transfer_length = 24;
			*precision = 6;
		}
		if (buff) strmov(buff,"DECIMAL");
		return SQL_DECIMAL;
	case TYPE_BOOL:
		if (buff) strmov(buff,"CHAR");
		*transfer_length = *size= *display_size = 1;
		return SQL_CHAR;
	case TYPE_I1:
		if (buff)
			pos=strmov(buff,"TINYINT");
			*transfer_length= 1;
			*size= *display_size= 3;
		 return SQL_TINYINT;
	case TYPE_I2:
		if (buff)
			pos=strmov(buff,"SMALLINT");
		*transfer_length= 2;
		*size= *display_size= 6;
		return SQL_SMALLINT;
	case TYPE_I4:
		if (buff)
			pos=strmov(buff,"INTEGER");
		*transfer_length= 4;
		*size= *display_size= 12;
		return SQL_INTEGER;
	case TYPE_I8:
		if (buff)
			pos= strmov(buff,"BIGINT");
		*transfer_length=20;
		*size= *display_size= 20;
		return SQL_BIGINT;
	case TYPE_R4:
		if (buff)
			pos= strmov(buff,"FLOAT");
        *transfer_length=4;
		*size= *display_size= 18;
		return SQL_REAL;
	case TYPE_R8:
		if (buff)
			pos= strmov(buff,"DOUBLE");    
		*transfer_length=8;
		*size= *display_size= 18;
		return SQL_DOUBLE;
	case TYPE_NULL:
		if (buff) strmov(buff,"NULL");
			return SQL_VARCHAR;
	case TYPE_DATETIME:
		if (buff) strmov(buff,"DATETIME");
		*transfer_length=16;      /* size of timestamp_struct */
		*size= *display_size=19;
		if (stmt->p_env->odbc_ver == SQL_OV_ODBC3)
			return SQL_TYPE_TIMESTAMP;
		return SQL_TIMESTAMP;
	case TYPE_DATE:
		if (buff) strmov(buff,"DATE");
		*transfer_length=6;       /* size of date struct */
		*size= *display_size=10;
		if (stmt->p_env->odbc_ver == SQL_OV_ODBC3)
			return SQL_TYPE_DATE;
		return SQL_DATE;
	case TYPE_TIME:
		if (buff) strmov(buff,"TIME");
		*transfer_length=6;       /* size of time struct */
		*size= *display_size=8;
		if (stmt->p_env->odbc_ver == SQL_OV_ODBC3)
			return SQL_TYPE_TIME;
		return SQL_TIME;
	case TYPE_CHAR:
		if (field->flag & FIELD_IS_VARYING)
		{
			if (buff) strmov(buff, "VARCHAR");
			if (field->scale>0)
			{
				*transfer_length = *size = field->scale;
				*display_size = field->scale;
			}
			else
			{
				*transfer_length = *size = 32768;
				*display_size = 32768;
			}
			return SQL_VARCHAR;
		}
		else
		{
			if (buff) strmov(buff, "CHAR");
			*transfer_length = *size = field->scale;
			*display_size = field->scale;
			return SQL_CHAR;
		}		
	case TYPE_CLOB:
		if (buff)
			strmov(buff,"ClOB");
		*transfer_length= *size= *display_size= -1;
		return SQL_CHAR;

	case TYPE_BINARY:
		if (buff)
			strmov(buff,"BINARY");
		*transfer_length= *size= field->scale;
		return SQL_BINARY;
	case TYPE_BLOB:
		if (buff)
			strmov(buff,"BLOB");
		*transfer_length= *size= *display_size= -1;
		return SQL_LONGVARBINARY;
	case TYPE_POINT:
		if (buff)
			strmov(buff,"POINT");
		*transfer_length= *size= *display_size= 16;
		return SQL_BINARY;
	case TYPE_BOX:
		if (buff)
			strmov(buff,"BOX");
		*transfer_length= *size= *display_size= 32;
		return SQL_BINARY;
	case TYPE_GEOM:
		if (buff)
			strmov(buff,"GEOMETRY");
		*transfer_length= *size= *display_size= -1;
		return SQL_VARBINARY;
  }
  return 0; /* Impossible */
}


/*
  @purpose : returns internal type to C type
*/

int getSQLCType(int type_id)
{
	switch(type_id) 
	{
	case TYPE_BOOL:
		return SQL_C_CHAR;
	case TYPE_I1:
		return SQL_C_TINYINT;
	case TYPE_I2:
		return SQL_C_SHORT;
	case TYPE_I4:
		return SQL_C_LONG;
	case TYPE_I8:
		return SQL_C_SBIGINT;
	case TYPE_R4:
		return SQL_C_FLOAT;
	case TYPE_R8:
		return SQL_C_DOUBLE;
	case TYPE_NUM:
		return SQL_C_NUMERIC;
	case TYPE_DATETIME:
		return SQL_C_TIMESTAMP;
	case TYPE_DATE:
		return SQL_C_DATE;
	case TYPE_TIME:
		return SQL_C_TIME;
	case TYPE_BINARY:
	case TYPE_BLOB:   
	case TYPE_POINT:
	case TYPE_BOX:
	case TYPE_GEOM:
		return SQL_C_BINARY;
	default:
		return SQL_C_CHAR;
  }
}

/*
  @purpose : returns default C type for a given SQL type
*/

int default_c_type(int sql_data_type)
{
  switch (sql_data_type) {
  case SQL_CHAR:
  case SQL_VARCHAR:
  case SQL_LONGVARCHAR:
  case SQL_DECIMAL:
  case SQL_NUMERIC:
  default:
    return SQL_C_CHAR;
  case SQL_BIGINT:
    return SQL_C_SBIGINT;
  case SQL_BIT:
    return SQL_C_BIT;
  case SQL_TINYINT:
    return SQL_C_TINYINT;
  case SQL_SMALLINT:
    return SQL_C_SHORT;
  case SQL_INTEGER:
    return SQL_C_LONG;
  case SQL_REAL:
  case SQL_FLOAT:
    return SQL_C_FLOAT;
  case SQL_DOUBLE:
    return SQL_C_DOUBLE;
  case SQL_BINARY:
  case SQL_VARBINARY:
  case SQL_LONGVARBINARY:
    return SQL_C_BINARY;
  case SQL_DATE:
  case SQL_TYPE_DATE:
    return SQL_C_DATE;
  case SQL_TIME:
  case SQL_TYPE_TIME:
    return SQL_C_TIME;
  case SQL_TIMESTAMP:
  case SQL_TYPE_TIMESTAMP:
    return SQL_C_TIMESTAMP;
  }
}

/*
  @purpose : convert a possible string to a data value
*/
bool str_to_date(DATE_STRUCT *rgbValue, const char *str,uint length)
{
  uint field_length,year_length,digits,i,date[3];
  const char *pos;
  const char *end=str+length;
  for (; !isdigit(*str) && str != end ; str++) ;
  /*
    Calculate first number of digits.
    If length= 4, 8 or >= 14 then year is of format YYYY
    (YYYY-MM-DD,  YYYYMMDD)
  */
  for (pos=str; pos != end && isdigit(*pos) ; pos++) ;
  digits= (uint) (pos-str);
  year_length= (digits == 4 || digits == 8 || digits >= 14) ? 4 : 2;
  field_length=year_length-1;

  for (i=0 ; i < 3 && str != end; i++)
  {
    uint tmp_value=(uint) (uchar) (*str++ - '0');
    while (str != end && isdigit(str[0]) && field_length--)
    {
      tmp_value=tmp_value*10 + (uint) (uchar) (*str - '0');
      str++;
    }
    date[i]=tmp_value;
    while (str != end && !isdigit(*str))
      str++;
    field_length=1;   /* Rest fields can only be 2 */
  }
  if (i <= 1 || date[1] == 0)   /* Wrong date */
    return 1;
  while (i < 3)
    date[i++]=1;

  rgbValue->year=date[0];
  rgbValue->month=date[1];
  rgbValue->day=date[2];
  return 0;
}

/*
  @purpose : convert a time string to a (ulong) value.
       At least following formats are recogniced
       HHMMSS HHMM HH HH.MM.SS   {t HH:MM:SS }
  @return  : HHMMSS
*/

ulong str_to_time(const char *str,uint length)
{
  uint i,date[3];
  const char *end=str+length;
  for (; !isdigit(*str) && str != end ; str++) ;

  for (i=0 ; i < 3 && str != end; i++)
  {
    uint tmp_value=(uint) (uchar) (*str++ - '0');
    while (str != end && isdigit(str[0]))
    {
      tmp_value=tmp_value*10 + (uint) (uchar) (*str - '0');
      str++;
    }
    date[i]=tmp_value;
    while (str != end && !isdigit(*str))
      str++;
  }
  while (i < 3)
    date[i++]=0;
  if (date[0] > 10000L)			   /* Properly handle HHMMSS format */
    return (ulong) date[0];
  else if (date[0] > 100)      /* Properly handle HHMM format */
    return (ulong) date[0]*100;
  return (ulong) date[0]*10000L + (ulong) (date[1]*100+date[2]);
}

/*
  @type    : myodbc internal
  @purpose : compare strings without regarding to case
*/

int _strcasecmp(const char *s, const char *t)
{
#ifdef USE_MB
  if (use_mb(default_charset_info))
  {
    register uint32 l;
    register const char *end=s+strlen(s);
    while (s<end)
    {
      if ((l=my_ismbchar(default_charset_info, s,end)))
      {
        while (l--)
          if (*s++ != *t++) return 1;
      }
      else if (my_ismbhead(default_charset_info, *t)) return 1;
      else if (toupper((uchar) *s++) != toupper((uchar) *t++)) return 1;
    }
    return *t;
  }
  else
#endif
  {
    while (toupper((uchar) *s) == toupper((uchar) *t++))
      if (!*s++) return 0;
    return ((int) toupper((uchar) s[0]) - (int) toupper((uchar) t[-1]));
  }
}

/*
  @type    : myodbc internal
  @purpose : compare strings without regarding to case
*/

int _casecmp(const char *s, const char *t, uint len)
{
#ifdef USE_MB
  if (use_mb(default_charset_info))
  {
    register uint32 l;
    register const char *end=s+len;
    while (s<end)
    {
      if ((l=my_ismbchar(default_charset_info, s,end)))
      {
        while (l--)
          if (*s++ != *t++) return 1;
      }
      else if (my_ismbhead(default_charset_info, *t)) return 1;
      else if (toupper((uchar) *s++) != toupper((uchar) *t++)) return 1;
    }
    return 0;
  }
  else
#endif
  {
    while (len-- != 0 && toupper(*s++) == toupper(*t++)) ;
    return (int) len+1;
  }
}

/*
  @type    : myodbc3 internal
  @purpose : logs the queries sent to server
*/
  
bool is_minimum_version(cstr server_version,cstr version, 
                        uint length)
{
  if (strncmp(server_version,version,length) >= 0)
    return TRUE;
  return FALSE;
}

TypeInfo type_infos[]=
{
	{"BOOLEAN",	TYPE_BOOL,		SQL_CHAR,		1,	3,	false,true,false},
	{"TINYINT",	TYPE_I1,		SQL_TINYINT,	1,	3,	false,true,false},
	{"SMALLINT",TYPE_I2,		SQL_SMALLINT,	2,	5,	false,true,false},
	{"INTEGER",	TYPE_I4,		SQL_INTEGER,	4,	10, false,true,false},
	{"BIGINT",	TYPE_I8,		SQL_BIGINT,		8,	20, false,true,false},
	{"FLOAT",   TYPE_R4,		SQL_REAL,		4,	16, false,true,false},
	{"DOUBLE",  TYPE_R8,		SQL_DOUBLE,		8,	32, false,true,false},
	{"NUMERIC", TYPE_NUM,	    SQL_NUMERIC,	sizeof(SQL_NUMERIC_STRUCT),	40,	false,true,false},
	{"CHAR",	TYPE_CHAR,		SQL_CHAR,		0,	0,  false,true,true},
	{"VARCHAR", TYPE_CHAR,		SQL_VARCHAR,	0,	0,  false,true,true},
	{"CLOB",	TYPE_CLOB,		SQL_VARCHAR,	0,	0,  false,true,true},
	{"DATETIME",TYPE_DATETIME,	SQL_TIMESTAMP,	sizeof(TIMESTAMP_STRUCT),32,	false,true,false},
	{"TIMESTAMP",TYPE_DATETIME,	SQL_TIMESTAMP,	sizeof(TIMESTAMP_STRUCT),32,	false,true,false},
	{"BINARY",  TYPE_BINARY,	SQL_BINARY,		0,	0,	false,true,false},
	{"BLOB",    TYPE_BLOB,		SQL_LONGVARBINARY,	0,	0,	false,true,false},
	{"IMAGE",	TYPE_BLOB,		SQL_LONGVARBINARY,		0,	0,  false,true,false},
	{"POINT",	TYPE_BINARY,	SQL_BINARY,		0,	0,	false,true,false},
	{"BOX",		TYPE_BINARY,	SQL_BINARY,		0,	0,	false,true,false},
	{"GEOMETRY",TYPE_BINARY,	SQL_VARBINARY,	0,	0,	false,true,false},
};

/*给定类型名,返回OLEDB类型ID*/
TypeInfo *getTypeInfo(char* type_name)
{
	int i;
	int num = sizeof(type_infos)/sizeof(TypeInfo);

	for(i=0;i<num;i++)
	{
		if(!_stricmp(type_name,type_infos[i].type_name))
			return &type_infos[i];
	}
	return NULL;
}

/*取字段在row中占用的长度,若是定义型,则返回其占用的长度,否则,返回指针的长度(表示指针型)*/
int getTypeLen(int type_id)
{
	switch(type_id)
	{
	case TYPE_BOOL:
		return sizeof(int);
	case TYPE_I1:
		return 1;
	case TYPE_I2:
		return 2;
	case TYPE_I4:
		return 4;
	case TYPE_I8:
		return 8;
	case TYPE_R4:
		return 4;
	case TYPE_R8:
		return 8;
	case TYPE_DATE:
		return sizeof(DATE_STRUCT);
	case TYPE_TIME:
		return sizeof(TIME_STRUCT);
	case TYPE_DATETIME:
		return sizeof(TIMESTAMP_STRUCT);
	case TYPE_NUM:
		return sizeof(SQL_NUMERIC_STRUCT);
	case TYPE_INTERVAL_Y2M:
		return 4;
	case TYPE_INTERVAL_D2S:
		return 8;
		return 8;
	case TYPE_POINT:
		return 16;
	case TYPE_BOX:
		return 32;
	default:
		return 8;
	}
}

