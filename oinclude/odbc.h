#ifndef _ODBC_DRIVER_H
#define _ODBC_DRIVER_H

#define __KUNLUN_ODBC__

#define  VERSION		"3.01"	/*表示版本号为:3.01*/
#include <stdio.h>
#include "kci_defs.h"
#define	RHSQL_PORT	6688	/*默认端口*/

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
#include	<windows.h>
#include	<sqltypes.h>
#include	<sql.h>
#include	<sqlext.h>
#include	<odbcinst.h>
#ifdef		_USRDLL
#define		MY_DLL_API __declspec(dllexport)
#endif
#else
#define		MY_DLL_API __declspec(dllimport)
#endif
#ifndef DEFAULT_TXN_ISOLATION
#define DEFAULT_TXN_ISOLATION SQL_TXN_READ_COMMITTED
#endif

#ifndef RC_INVOKED
#pragma pack(1)
#endif

#include "error.h"

typedef struct tagDateTime
{
	int16	year;	/*年*/
	int8	month;	/*月*/
	int8    day;	/*日*/
	int32	ms;		/*一天中的毫秒数*/
}DateTime;

#define NullS	NULL
#define false   FALSE
#define true	TRUE
/*最大参数个数*/
#define MAX_PARAM_NUM	600
#define MAX_NAME_LEN	63
#define NAME_LEN		31
#define NET_BUFF_LEN	8*1024
#define SERVER_NAME		"Kunlun Server"

/*
   Internal driver definations
*/
#define MYSQL_RESET_BUFFERS 1000  /* param to SQLFreeStmt */
#define MYSQL_RESET			1001  /* param to SQLFreeStmt */
#define MYSQL_3_21_PROTOCOL 10    /* OLD protocol */
#define CHECK_IF_ALIVE      3600  /* Seconds between queries for ping */

#define MYSQL_MAX_CURSOR_LEN 18   /* Max cursor name length */
#define MYSQL_STMT_LEN 1024       /* Max statement length */
#define MYSQL_STRING_LEN 1024     /* Max string length */
#define MYSQL_MAX_SEARCH_STRING_LEN NAME_LEN+10 /* Max search string length */
#define MY_MAX_PK_PARTS 32        /* Max Primary keys in a cursor*/

#if defined(_WIN32) || defined(WIN32)
#define INTFUNC  __stdcall
#define EXPFUNC  __stdcall
#if !defined(HAVE_LOCALTIME_R)
#define HAVE_LOCALTIME_R 1
#endif
#else
#define INTFUNC PASCAL
#define EXPFUNC __export CALLBACK
#endif

#if !defined(_DEBUG)
#define  DBUG_OFF
#endif

#if !defined(DBUG_OFF)
extern int h_file;
#define DBUG_PRINT(a,b)	//printf("%s(%s)",a,b)
#define DBUG_RETURN(rc) return(rc)
#define DBUG_RETURN_STATUS(r) \
  { \
    SQLRETURN rc= r; \
    switch (rc) { \
    case 0: \
      DBUG_PRINT("exit ",("SQL_SUCCESS")); \
      break; \
    case 1: \
      DBUG_PRINT("exit ",("SQL_SUCCESS_WITH_INFO")); \
      break; \
    case -1: \
      DBUG_PRINT("exit ",("SQL_ERROR")); \
      break; \
    case 100: \
      DBUG_PRINT("exit ",("SQL_NO_DATA_FOUND")); \
      break; \
    case 99: \
      DBUG_PRINT("exit ",("SQL_NEED_DATA")); \
      break; \
    default: \
      DBUG_PRINT("exit ",("INVALID_ERROR_CODE")); \
    } \
    DBUG_RETURN(rc); \
  } 
#include<io.h>
#define  DBUG_ENTER(a) //do { write(h_file,a,strlen(a));write(h_file,"\r\n",2);}while(0)
#else
#define  DBUG_RETURN_STATUS(r) return(r)
#define  DBUG_RETURN(rc)  return(rc)
#define  DBUG_ENTER(a)
#define  DBUG_PRINT(a,b)
#endif


/*
  Connection parameters, that affects the driver behaviour
*/
#define FLAG_FIELD_LENGTH       1   /* field_length instead of max_length */
#define FLAG_FOUND_ROWS         2   /* Access wants can't handle affected_rows */
#define FLAG_DEBUG              4   /* Put a debug log on C:\myodbc.log */
#define FLAG_BIG_PACKETS        8   /* Allow BIG packets. */
#define FLAG_NO_PROMPT          16  /* Don't prompt on connection */
#define FLAG_DYNAMIC_CURSOR     32  /* Enables the dynamic cursor */
#define FLAG_NO_SCHEMA          64  /* Ignore the schema defination */
#define FLAG_NO_DEFAULT_CURSOR  128 /* No default cursor */
#define FLAG_NO_LOCALE          256  /* No locale specification */
#define FLAG_PAD_SPACE          512  /* Pad CHAR:s with space to max length */
#define FLAG_FULL_COLUMN_NAMES  1024 /* Extends SQLDescribeCol */
#define FLAG_COMPRESSED_PROTO   2048 /* Use compressed protocol */
#define FLAG_IGNORE_SPACE       4096 /* Ignore spaces after function names */
#define FLAG_NAMED_PIPE         8192 /* Force use of named pipes */
#define FLAG_NO_BIGINT          16384   /* Change BIGINT to INT */
#define FLAG_NO_CATALOG         32768   /* No catalog support */
#define FLAG_USE_MYCNF          65536L  /* Read my.cnf at start */
#define FLAG_SAFE               131072L /* Try to be as safe as possible */
#define FLAG_NO_TRANSACTIONS  (FLAG_SAFE << 1) /* Disable transactions */
#define FLAG_LOG_QUERY        (FLAG_SAFE << 2) /* Query logging, debug */
#define FLAG_NO_CACHE         (FLAG_SAFE << 3) /* Don't cache the resultset */
#define FLAG_FORWARD_CURSOR   (FLAG_SAFE << 4) /* Force use of forward-only cursors */

#define SQL_MY_PRIMARY_KEY	1212	/*支持ADO*/

#define RHSQL_MAX_CURSOR_LEN 18   /* Max cursor name length */
#define RHSQL_STMT_LEN 1024       /* Max statement length */
#define RHSQL_STRING_LEN 1024     /* Max string length */
#define RHSQL_MAX_SEARCH_STRING_LEN NAME_LEN+10 /* Max search string*/

#ifdef  SQL_SPEC_STRING
#undef  SQL_SPEC_STRING
#define SQL_SPEC_STRING   "03.0"
#endif

#define DRIVER_VERSION    "03.00.00"
#define ODBC_DRIVER       "ODBC 3.0 Driver"
#define DRIVER_NAME       "Kunlun ODBC 3.0 Driver"
#define DRIVER_NONDSN_TAG "DRIVER={Kunlun ODBC 3.0 Driver}"

#ifndef _UNIX_
#define DRIVER_DLL_NAME   "libkunodbc.dll"
#ifndef DBUG_OFF
#define DRIVER_QUERY_LOGFILE "kunlun_odbc.log"
#endif
#else
#define DRIVER_DLL_NAME   "libkunodbc.so"
#ifndef DBUG_OFF
#define DRIVER_QUERY_LOGFILE "kunlun_odbc.log"
#endif
#endif

typedef enum ResultType
{
	RESULT_UNKNOW,
	RESULT_INSERT,
	RESULT_SELECT,
	RESULT_UPDATE,
	RESULT_DELETE,
}ResultType;


enum ST_STATE { ST_UNKNOWN, ST_PREPARED, ST_PRE_EXECUTED, ST_EXECUTED };
enum MY_DUMMY_STATE { ST_DUMMY_UNKNOWN, ST_DUMMY_PREPARED, ST_DUMMY_EXECUTED };

typedef struct CATA_FIELD {
	char *name;
	char *desc;
	char *ss;
	int	 data_type;
	int	 scale;
	int	 preci;
	bool notnull;
}CATA_FIELD;

 typedef struct stmt_options {
  int			   max_length, max_rows, *bind_offset;
  SQLUINTEGER      bind_type,rows_in_set,cursor_type;
  SQLUSMALLINT     *paramStatusPtr;
  SQLUINTEGER      *paramProcessedPtr;
  bool				retrieve_data;
  SQLUSMALLINT     *rowStatusPtr;
  SQLUSMALLINT     *rowOperationPtr;
  SQLUINTEGER      *rowsFetchedPtr;
  void				*bookmark_ptr;
  SQLUINTEGER       simulateCursor;
  SQLUINTEGER		bookmark_opt;
} STMT_OPTIONS;

#define PRI_KEY_FLAG 1

/*类型信息结构*/
typedef struct TypeInfo
{
	char *type_name;
	int	 type_id;
	int  ole_type_id;
	int  len;
	int  disp_len;
	bool is_long;
	bool is_nullable;
	bool is_sensitive; 
}TypeInfo;

/*SQLENV实际定义*/
#define  SQLENV   KCIEnv
#define  SQLDBC   KCISvcCtx
#define  SQLSTMT  KCIStmt

SQLRETURN SQL_API
sql_get_data(SQLSTMT *stmt,
	SQLSMALLINT    fCType,
	int			   col_no,
	SQLPOINTER     rgbValue,
	int			   cbValueMax,
	int FAR		   *pcbValue);

SQLRETURN
copy_lresult(SQLSMALLINT HandleType, SQLHANDLE Handle,
	SQLCHAR FAR *rgbValue, int cbValueMax,
	int FAR *pcbValue, char *src, long src_length,
	long max_length, long fill_length, ulong *offset,
	bool binary_data);

int getFieldInfo(SQLSTMT FAR *stmt,
	KCIFld *field,
	char   *buff,
	ulong  *transfer_length,
	ulong  *size,
	ulong  *precision,
	ulong  *display_size);

SQLRETURN copy_str_data(SQLSMALLINT HandleType, 
	SQLHANDLE Handle,SQLCHAR *rgbValue,SQLSMALLINT cbValueMax,
	SQLSMALLINT *pcbValue, const char *src);

SQLRETURN
setError(SQLSMALLINT HandleType, SQLHANDLE handle,
	rhodbc_errid errid, cstr errtext, int errcode);

SQLRETURN
setError(SQLHANDLE handle,rhodbc_errid errid, cstr errtext, int errcode);

SQLRETURN
setError(SQLHANDLE handle, cstr errtype, cstr errtext, int errcode);

#define strmov(d,s)  strcpy(d,s)
#define if_dynamic_cursor(st)	((st)->stmt_options.cursor_type == SQL_CURSOR_DYNAMIC)
#define if_forward_cache(st)	(st)->stmt_options.cursor_type == SQL_CURSOR_FORWARD_ONLY && \
								(st)->dbc->flag & FLAG_NO_CACHE )
#define true_dynamic(flag)		(!(flag &FLAG_FORWARD_CURSOR ) && (flag & FLAG_DYNAMIC_CURSOR))

int	  default_c_type(int sql_data_type);
int	  _casecmp(const char *s, const char *t, uint len);
void   InitFieldsInfos();
void   init_getfunctions();

/*给定数据类型获取数据长度*/
int getTypeLen(int type_id);
/*取当前记录集的第i个记录*/
#define nthRow(stmt,i)	stmt->rows[stmt->rowset_off+i]
void   fetchRowset(SQLSTMT *stmt,int dir,int irow);

/*向结果集中加入预定义记录*/
void	appendRecord(SQLSTMT *stmt,char *rec[]);
/*释放已接收的记录集*/
void	freeResult(SQLSTMT *stmt);

/*将本地类型转化为ODBC的SQL_C_XXX类型*/
int		getSQLCType(int type_id);
int		closeConnect(SQLDBC *p_conn);
#endif





