/*************************************************************************** 
* The following ODBC APIs are implemented in this file:                   *
*                                                                         *
*   SQLGetInfo          (ISO 92)                                          *
*   SQLGetFunctions     (ISO 92)                                          *
*   SQLGetTypeInfo      (ISO 92)                                          *
*                                                                         *
****************************************************************************/

#include "odbc.h"
#include "string.h"
#define MYINFO_SET_ULONG(val) \
  { \
  *((SQLUINTEGER*)rgbInfoValue) = val; \
  *pcbInfoValue = sizeof(SQLUINTEGER); \
  }

#define MYINFO_SET_USHORT(val) \
  { \
  *((SQLUSMALLINT*)rgbInfoValue) = val; \
  *pcbInfoValue = sizeof(SQLUSMALLINT); \
  }

#define MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,val) \
  { \
  *pcbInfoValue=(sb2)strlen(val); \
  strncpy1((char*)rgbInfoValue,val,cbInfoValueMax); \
  if (*pcbInfoValue >= cbInfoValueMax) \
  DBUG_RETURN_STATUS(SQL_SUCCESS_WITH_INFO); \
  DBUG_RETURN_STATUS(SQL_SUCCESS); \
  }

#define MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,val,len) \
  { \
  *pcbInfoValue=len; \
  strncpy1((char*)rgbInfoValue,val,cbInfoValueMax); \
  if (len >= cbInfoValueMax) \
  DBUG_RETURN_STATUS(SQL_SUCCESS_WITH_INFO); \
  DBUG_RETURN_STATUS(SQL_SUCCESS); \
  }

static bool myodbc_ov2_inited= 0;

/*
@type    : ODBC 1.0 API
@purpose : returns general information about the driver and data
source associated with a connection
*/
char allowed_chars[]= {
	'\307','\374','\351','\342','\344','\340','\345','\347','\352','\353',
		'\350','\357','\356','\354','\304','\305','\311','\346','\306','\364',
		'\366','\362','\373','\371','\377','\326','\334','\341','\355','\363',
		'\372','\361','\321',0};
	
	
#define rhsql_keywords "UNIQUE,ZEROFILL,UNSIGNED,BIGINT,BLOB,TINYBLOB,MEDIMUMBLOB,LONGBLOB,"\
		"MEDIUMINT,PROCEDURE,SHOW,LIMIT,DEFAULT,TABLES,REGEXP,RLIKE,KEYS,TINYTEXT,MEDIUMTEXT"
	
	SQLRETURN SQL_API SQLGetInfo(SQLHDBC hdbc, 
							 SQLUSMALLINT fInfoType,
							 SQLPOINTER rgbInfoValue,
							 SQLSMALLINT cbInfoValueMax,
							 SQLSMALLINT FAR *pcbInfoValue)
	{
		SQLDBC FAR *dbc=(SQLDBC FAR*) hdbc;
		char dummy2[255];
		SQLSMALLINT dummy;
		DBUG_ENTER("SQLGetInfo");
		DBUG_PRINT("enter",("fInfoType: %d, cbInfoValueMax :%d",
			fInfoType, cbInfoValueMax));
		
		if (cbInfoValueMax)
			cbInfoValueMax--;
		
		if (!pcbInfoValue)
			pcbInfoValue=&dummy;
		
		if (!rgbInfoValue)
		{
			rgbInfoValue=dummy2;
			cbInfoValueMax=sizeof(dummy2)-1;
		}
		switch (fInfoType) 
		{
		case SQL_ACTIVE_ENVIRONMENTS:
			*((SQLUSMALLINT*) rgbInfoValue)=0;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_AGGREGATE_FUNCTIONS:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_AF_ALL | SQL_AF_AVG | SQL_AF_COUNT |
				SQL_AF_DISTINCT | SQL_AF_MAX |
				SQL_AF_MIN | SQL_AF_SUM);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_ALTER_DOMAIN:
			*((SQLUINTEGER*) rgbInfoValue)=0;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_ALTER_TABLE:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_AT_ADD_COLUMN | SQL_AT_DROP_COLUMN;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_ASYNC_MODE:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_AM_NONE;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_BATCH_ROW_COUNT:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_BRC_EXPLICIT;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_BATCH_SUPPORT:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_BS_ROW_COUNT_EXPLICIT;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_BOOKMARK_PERSISTENCE:
			*((SQLUINTEGER*) rgbInfoValue)=0L;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_CATALOG_LOCATION:
			*((SQLUSMALLINT*) rgbInfoValue)=SQL_CL_START;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
		case SQL_CATALOG_NAME:	
		case SQL_ACCESSIBLE_TABLES:
		case SQL_COLUMN_ALIAS:
		case SQL_EXPRESSIONS_IN_ORDERBY:
		case SQL_LIKE_ESCAPE_CLAUSE:
		case SQL_MAX_ROW_SIZE_INCLUDES_LONG:
		case SQL_MULT_RESULT_SETS:
		case SQL_MULTIPLE_ACTIVE_TXN:
		case SQL_OUTER_JOINS:
		case SQL_ORDER_BY_COLUMNS_IN_SELECT:
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"Y",1);
			break;
			
		case SQL_CATALOG_NAME_SEPARATOR:     
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,".",1);
			break;
			
		case SQL_CATALOG_TERM:
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"database",8); 
			break;
			
		case SQL_CATALOG_USAGE:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_CU_DML_STATEMENTS |
				SQL_CU_TABLE_DEFINITION |
				SQL_CU_INDEX_DEFINITION |
				SQL_CU_PRIVILEGE_DEFINITION);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_COLLATION_SEQ:
		/*
		We have to change this to return to the active collation sequence
		in the server RhSQL server, as soon as it supports different
		collation sequences / user.
			*/
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"",0);
			break;
			
		case SQL_CONCAT_NULL_BEHAVIOR:
			*((SQLUSMALLINT*) rgbInfoValue)= SQL_CB_NULL;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_CONVERT_BIGINT:
		case SQL_CONVERT_BIT:
		case SQL_CONVERT_CHAR:
		case SQL_CONVERT_DATE:
		case SQL_CONVERT_DECIMAL:
		case SQL_CONVERT_DOUBLE:
		case SQL_CONVERT_FLOAT:
		case SQL_CONVERT_INTEGER:
		case SQL_CONVERT_LONGVARCHAR:
		case SQL_CONVERT_NUMERIC:
		case SQL_CONVERT_REAL:
		case SQL_CONVERT_SMALLINT:
		case SQL_CONVERT_TIME:
		case SQL_CONVERT_TIMESTAMP:
		case SQL_CONVERT_TINYINT:
		case SQL_CONVERT_VARCHAR:
		case SQL_CONVERT_BINARY:
		case SQL_CONVERT_VARBINARY:
		case SQL_CONVERT_LONGVARBINARY:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_CVT_CHAR | SQL_CVT_NUMERIC |
				SQL_CVT_DECIMAL | SQL_CVT_INTEGER |
				SQL_CVT_SMALLINT | SQL_CVT_FLOAT |
				SQL_CVT_REAL | SQL_CVT_DOUBLE |
				SQL_CVT_VARCHAR | SQL_CVT_LONGVARCHAR |
				SQL_CVT_BIT | SQL_CVT_TINYINT |
				SQL_CVT_BIGINT | SQL_CVT_DATE |
				SQL_CVT_TIME | SQL_CVT_TIMESTAMP|
				SQL_CVT_BINARY |  SQL_CVT_VARBINARY |SQL_CVT_LONGVARBINARY              
				);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
//		case SQL_CONVERT_BINARY:
//		case SQL_CONVERT_VARBINARY:
//		case SQL_CONVERT_LONGVARBINARY:
		case SQL_CONVERT_INTERVAL_DAY_TIME:
		case SQL_CONVERT_INTERVAL_YEAR_MONTH:
			*((SQLUINTEGER*) rgbInfoValue)=0L;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
			/* Non supported options..*/
		case SQL_CONVERT_FUNCTIONS: 
		case SQL_CONVERT_WCHAR:
		case SQL_CONVERT_WVARCHAR:
		case SQL_CONVERT_WLONGVARCHAR:
		case SQL_CREATE_ASSERTION:
		case SQL_CREATE_CHARACTER_SET:
		case SQL_CREATE_COLLATION:
		case SQL_CREATE_DOMAIN:
		case SQL_CREATE_TRANSLATION:
		case SQL_DROP_ASSERTION:
		case SQL_DROP_CHARACTER_SET:
		case SQL_DROP_COLLATION:
		case SQL_DROP_DOMAIN:  
		case SQL_DROP_TRANSLATION:
		case SQL_KEYSET_CURSOR_ATTRIBUTES1:
		case SQL_KEYSET_CURSOR_ATTRIBUTES2:
		case SQL_INFO_SCHEMA_VIEWS:
		case SQL_SCHEMA_USAGE:
		case SQL_DROP_SCHEMA:
		case SQL_SQL92_FOREIGN_KEY_DELETE_RULE:
		case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:
		case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
		case SQL_SQL92_PREDICATES:
		case SQL_SQL92_VALUE_EXPRESSIONS:
		case SQL_SUBQUERIES:
		case SQL_TIMEDATE_ADD_INTERVALS:
		case SQL_TIMEDATE_DIFF_INTERVALS:
		case SQL_UNION:
		case SQL_LOCK_TYPES:
			*((SQLUINTEGER*) rgbInfoValue)=0L;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
		case SQL_CREATE_VIEW:
			*((SQLUINTEGER*) rgbInfoValue)=SQL_CV_CREATE_VIEW|
				SQL_CV_CHECK_OPTION|SQL_CV_CASCADED|SQL_CV_LOCAL; 
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
		case SQL_CREATE_SCHEMA:	
			*((SQLUINTEGER*) rgbInfoValue)=SQL_CS_CREATE_SCHEMA; 
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
		case SQL_DROP_VIEW:
			*((SQLUINTEGER*) rgbInfoValue)=SQL_DV_DROP_VIEW; 
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
			/* Un limit, set to default..*/
		case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS:
		case SQL_MAX_BINARY_LITERAL_LEN:
		case SQL_MAX_CHAR_LITERAL_LEN:
		case SQL_MAX_DRIVER_CONNECTIONS:
		case SQL_MAX_ROW_SIZE:
			*((SQLUINTEGER*) rgbInfoValue)=0L;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_MAX_COLUMNS_IN_GROUP_BY:
		case SQL_MAX_COLUMNS_IN_ORDER_BY:
		case SQL_MAX_COLUMNS_IN_SELECT:
		case SQL_MAX_COLUMNS_IN_TABLE:
		case SQL_MAX_CONCURRENT_ACTIVITIES:
		case SQL_MAX_PROCEDURE_NAME_LEN:
		case SQL_MAX_SCHEMA_NAME_LEN:
			*((SQLUSMALLINT*) rgbInfoValue)=0L;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_CORRELATION_NAME:
			*((SQLSMALLINT*) rgbInfoValue)= SQL_CN_DIFFERENT;
			*pcbInfoValue=sizeof(SQLSMALLINT);
			break;
			
		case SQL_CREATE_TABLE:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_CT_CREATE_TABLE |
				SQL_CT_COMMIT_DELETE |
				SQL_CT_LOCAL_TEMPORARY |
				SQL_CT_COLUMN_DEFAULT);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_CURSOR_COMMIT_BEHAVIOR:
		case SQL_CURSOR_ROLLBACK_BEHAVIOR:
			*((SQLUSMALLINT*) rgbInfoValue)=   SQL_CB_PRESERVE;	//SQL_CB_CLOSE;//
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
#ifdef SQL_CURSOR_ROLLBACK_SQL_CURSOR_SENSITIVITY
		case SQL_CURSOR_ROLLBACK_SQL_CURSOR_SENSITIVITY:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_UNSPECIFIED;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
#endif
			
#ifdef SQL_CURSOR_SENSITIVITY
		case SQL_CURSOR_SENSITIVITY:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_UNSPECIFIED;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
#endif
			
		case SQL_DATA_SOURCE_NAME:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,dbc->dsn);
			break;
			
		case SQL_DATA_SOURCE_READ_ONLY:
		case SQL_DESCRIBE_PARAMETER:
		case SQL_INTEGRITY:
		case SQL_NEED_LONG_DATA_LEN:
		case SQL_ROW_UPDATES:
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"N",1);
			break;
	
		case SQL_ACCESSIBLE_PROCEDURES:
		case SQL_PROCEDURES:
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"Y",1);
			break;
	
		case SQL_DATABASE_NAME:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,dbc->db_name);
			break;
			
		case SQL_DATETIME_LITERALS:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_DL_SQL92_DATE |
				SQL_DL_SQL92_TIME |
				SQL_DL_SQL92_TIMESTAMP;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_DBMS_NAME:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"XuguSQL Server");
			break;
			
		case SQL_DBMS_VER:
			{
				if(dbc->server_version)
				{
					MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,dbc->server_version);
				}
				else
				{
					MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"XuguSQL Server 2.0");
				}
				break;
			}
		case SQL_DDL_INDEX:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_DI_CREATE_INDEX | SQL_DI_DROP_INDEX;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_DEFAULT_TXN_ISOLATION:
			*((SQLUINTEGER*) rgbInfoValue)= DEFAULT_TXN_ISOLATION;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_DRIVER_NAME:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,DRIVER_DLL_NAME);
			break;
			
		case SQL_DRIVER_ODBC_VER:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,SQL_SPEC_STRING);
			break;
			
		case SQL_DRIVER_VER:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,DRIVER_VERSION);
			break;
			
		case SQL_DROP_TABLE:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_DT_DROP_TABLE |
				SQL_DT_CASCADE |
				SQL_DT_RESTRICT);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_STATIC_CURSOR_ATTRIBUTES1:
			*((SQLUINTEGER*) rgbInfoValue) = (SQL_CA1_NEXT |
				SQL_CA1_ABSOLUTE |
				SQL_CA1_RELATIVE |
				SQL_CA1_LOCK_NO_CHANGE |
				SQL_CA1_POS_POSITION |
				SQL_CA1_POS_UPDATE |
				SQL_CA1_POS_DELETE |
				SQL_CA1_POS_REFRESH |
				SQL_CA1_POSITIONED_UPDATE |
				SQL_CA1_POSITIONED_DELETE |
				SQL_CA1_BULK_ADD);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_STATIC_CURSOR_ATTRIBUTES2:
			*((SQLUINTEGER*) rgbInfoValue) =  (SQL_CA2_MAX_ROWS_SELECT |
				SQL_CA2_MAX_ROWS_INSERT |
				SQL_CA2_MAX_ROWS_DELETE |
				SQL_CA2_MAX_ROWS_UPDATE |
				SQL_CA2_CRC_EXACT);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
			*((SQLUINTEGER*) rgbInfoValue) = (SQL_CA1_NEXT |
				SQL_CA1_ABSOLUTE |
				SQL_CA1_RELATIVE |
				SQL_CA1_LOCK_NO_CHANGE |
				SQL_CA1_POS_POSITION |
				SQL_CA1_POS_UPDATE |
				SQL_CA1_POS_DELETE |
				SQL_CA1_POS_REFRESH |
				SQL_CA1_POSITIONED_UPDATE |
				SQL_CA1_POSITIONED_DELETE |
				SQL_CA1_BULK_ADD);
			if (dbc->flag & FLAG_FORWARD_CURSOR)
				*((SQLUINTEGER*) rgbInfoValue) = (SQL_CA1_NEXT);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2:
			*((SQLUINTEGER*) rgbInfoValue) =  (SQL_CA2_MAX_ROWS_SELECT |
				SQL_CA2_MAX_ROWS_INSERT |
				SQL_CA2_MAX_ROWS_DELETE |
				SQL_CA2_MAX_ROWS_UPDATE |
				SQL_CA2_CRC_EXACT);
			if (dbc->flag & FLAG_FORWARD_CURSOR)
				*((SQLUINTEGER*) rgbInfoValue) &= ~(SQL_CA2_CRC_EXACT);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
			*((SQLUINTEGER*) rgbInfoValue) = (SQL_CA1_NEXT |
				SQL_CA1_ABSOLUTE |
				SQL_CA1_RELATIVE |
				SQL_CA1_LOCK_NO_CHANGE |
				SQL_CA1_POS_POSITION |
				SQL_CA1_POS_UPDATE |
				SQL_CA1_POS_DELETE |
				SQL_CA1_POS_REFRESH |
				SQL_CA1_POSITIONED_UPDATE |
				SQL_CA1_POSITIONED_DELETE |
				SQL_CA1_BULK_ADD);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_DYNAMIC_CURSOR_ATTRIBUTES2:
			*((SQLUINTEGER*) rgbInfoValue) =  (SQL_CA2_SENSITIVITY_ADDITIONS |
				SQL_CA2_SENSITIVITY_DELETIONS |
				SQL_CA2_SENSITIVITY_UPDATES |
				SQL_CA2_MAX_ROWS_SELECT |
				SQL_CA2_MAX_ROWS_INSERT |
				SQL_CA2_MAX_ROWS_DELETE |
				SQL_CA2_MAX_ROWS_UPDATE |
				SQL_CA2_CRC_EXACT	   |
				SQL_CA2_SIMULATE_TRY_UNIQUE);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_FILE_USAGE:
			*((SQLUSMALLINT*) rgbInfoValue)=SQL_FILE_NOT_SUPPORTED;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_GETDATA_EXTENSIONS:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_GD_ANY_COLUMN |
				SQL_GD_ANY_ORDER |
				SQL_GD_BOUND);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_GROUP_BY:
			*((SQLUSMALLINT*) rgbInfoValue)=SQL_GB_NO_RELATION;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_IDENTIFIER_CASE:
			*((SQLUSMALLINT*) rgbInfoValue)= SQL_IC_MIXED;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_IDENTIFIER_QUOTE_CHAR:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue, "\"" );
			break;
			
		case SQL_INDEX_KEYWORDS:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_IK_NONE;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_INSERT_STATEMENT:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_IS_INSERT_LITERALS |
				SQL_IS_INSERT_SEARCHED |
				SQL_IS_SELECT_INTO);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_KEYWORDS: /* Need to return all keywords..*/
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,rhsql_keywords);
			break;
			
		case SQL_MAX_CATALOG_NAME_LEN:
		case SQL_MAX_COLUMN_NAME_LEN:
		case SQL_MAX_IDENTIFIER_LEN:
		case SQL_MAX_TABLE_NAME_LEN:
			*((SQLUSMALLINT*) rgbInfoValue)= MAX_NAME_LEN;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_MAX_COLUMNS_IN_INDEX:
			*((SQLUSMALLINT*) rgbInfoValue)=MAX_NAME_LEN;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_MAX_CURSOR_NAME_LEN:
			*((SQLUSMALLINT*) rgbInfoValue)=RHSQL_MAX_CURSOR_LEN;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_MAX_INDEX_SIZE:
			*((SQLUINTEGER*) rgbInfoValue)=500;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_MAX_STATEMENT_LEN:
			*((SQLUINTEGER*) rgbInfoValue)=NET_BUFF_LEN;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_MAX_TABLES_IN_SELECT:
			/* This is 63 on 64 bit machines */
			*((SQLUSMALLINT*) rgbInfoValue)=32;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_MAX_USER_NAME_LEN:
			*((SQLUSMALLINT*) rgbInfoValue)=32;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_NON_NULLABLE_COLUMNS:
			*((SQLUSMALLINT*) rgbInfoValue)=SQL_NNC_NON_NULL;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_NULL_COLLATION:
			*((SQLUSMALLINT*) rgbInfoValue)=SQL_NC_START;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_NUMERIC_FUNCTIONS:
			*((SQLUINTEGER*) rgbInfoValue)=
				(SQL_FN_NUM_ABS | SQL_FN_NUM_ACOS | SQL_FN_NUM_ASIN | SQL_FN_NUM_ATAN |
				SQL_FN_NUM_ATAN2 | SQL_FN_NUM_CEILING | SQL_FN_NUM_COS |
				SQL_FN_NUM_COT | SQL_FN_NUM_EXP | SQL_FN_NUM_FLOOR | SQL_FN_NUM_LOG
				| SQL_FN_NUM_MOD | SQL_FN_NUM_SIGN | SQL_FN_NUM_SIN | SQL_FN_NUM_SQRT
				| SQL_FN_NUM_TAN | SQL_FN_NUM_PI | SQL_FN_NUM_RAND |
				SQL_FN_NUM_DEGREES | SQL_FN_NUM_LOG10 | SQL_FN_NUM_POWER |
				SQL_FN_NUM_RADIANS | SQL_FN_NUM_ROUND | SQL_FN_NUM_TRUNCATE);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_ODBC_API_CONFORMANCE:
			*((SQLSMALLINT*) rgbInfoValue)=SQL_OAC_LEVEL1;
			*pcbInfoValue=sizeof(SQLSMALLINT);
			break;
			
		case SQL_ODBC_SQL_CONFORMANCE:
			*((SQLSMALLINT*) rgbInfoValue)=SQL_OSC_CORE;
			*pcbInfoValue=sizeof(SQLSMALLINT);
			break;
			
		case SQL_ODBC_INTERFACE_CONFORMANCE:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_OIC_LEVEL1;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_OJ_CAPABILITIES:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_OJ_LEFT | SQL_OJ_NESTED |
				SQL_OJ_NOT_ORDERED |
				SQL_OJ_INNER | SQL_OJ_ALL_COMPARISON_OPS |SQL_OJ_RIGHT );
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_PARAM_ARRAY_ROW_COUNTS:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_PARC_NO_BATCH;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_PARAM_ARRAY_SELECTS:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_PAS_NO_SELECT;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_PROCEDURE_TERM:
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"procedure",0);
			break;
			
		case SQL_POS_OPERATIONS:
			*((int*) rgbInfoValue)= (SQL_POS_POSITION |
				SQL_POS_UPDATE  |
				SQL_POS_DELETE |
				SQL_POS_ADD |
				SQL_POS_REFRESH);
			if (dbc->flag & FLAG_FORWARD_CURSOR)
				*((int*) rgbInfoValue)= 0L;
			*pcbInfoValue=sizeof(int);
			break;
			
		case SQL_POSITIONED_STATEMENTS:
			*((int*) rgbInfoValue)= (SQL_PS_POSITIONED_DELETE |
				SQL_PS_POSITIONED_UPDATE);
			if (dbc->flag & FLAG_FORWARD_CURSOR)
				*((int*) rgbInfoValue)= 0L;
			*pcbInfoValue=sizeof(int);
			break;
			
		case SQL_QUOTED_IDENTIFIER_CASE:
			*((SQLUSMALLINT*) rgbInfoValue)=SQL_IC_SENSITIVE;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_SCHEMA_TERM:
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"schema",0);
			break;
			
		case SQL_SCROLL_OPTIONS:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_SO_FORWARD_ONLY | SQL_SO_STATIC | SQL_SO_KEYSET_DRIVEN | SQL_SO_DYNAMIC);        
			if (dbc->flag & FLAG_FORWARD_CURSOR)
				*((SQLUINTEGER*) rgbInfoValue) &= ~(SQL_SO_STATIC);
			else if (dbc->flag & FLAG_DYNAMIC_CURSOR)
				*((SQLUINTEGER*) rgbInfoValue) |= (SQL_SO_DYNAMIC);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_SCROLL_CONCURRENCY:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_SS_ADDITIONS |
				SQL_SS_DELETIONS |
				SQL_SS_UPDATES);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_SEARCH_PATTERN_ESCAPE:
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"\\",1);
			break;
			
		case SQL_SERVER_NAME:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,SERVER_NAME);
			break;
			
		case SQL_SPECIAL_CHARACTERS:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,(char *)allowed_chars);
			break;
			
		case SQL_SQL_CONFORMANCE:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_SC_SQL92_INTERMEDIATE;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_STRING_FUNCTIONS:
			*((SQLUINTEGER*) rgbInfoValue)=
				(SQL_FN_STR_CONCAT | SQL_FN_STR_INSERT |
				SQL_FN_STR_LEFT | SQL_FN_STR_LTRIM | SQL_FN_STR_LENGTH |
				SQL_FN_STR_LOCATE | SQL_FN_STR_LCASE | SQL_FN_STR_REPEAT |
				SQL_FN_STR_REPLACE | SQL_FN_STR_RIGHT | SQL_FN_STR_RTRIM |
				SQL_FN_STR_SUBSTRING | SQL_FN_STR_UCASE | SQL_FN_STR_ASCII |
				SQL_FN_STR_CHAR | SQL_FN_STR_LOCATE_2 | SQL_FN_STR_SOUNDEX |
				SQL_FN_STR_SPACE);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_TIMEDATE_FUNCTIONS:
			*((SQLUINTEGER*) rgbInfoValue)=
				(SQL_FN_TD_NOW | SQL_FN_TD_CURDATE | SQL_FN_TD_DAYOFMONTH |
				SQL_FN_TD_DAYOFWEEK | SQL_FN_TD_DAYOFYEAR | SQL_FN_TD_MONTH |
				SQL_FN_TD_QUARTER | SQL_FN_TD_WEEK | SQL_FN_TD_YEAR |
				SQL_FN_TD_CURTIME | SQL_FN_TD_HOUR | SQL_FN_TD_MINUTE |
				SQL_FN_TD_SECOND | SQL_FN_TD_DAYNAME | SQL_FN_TD_MONTHNAME);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_SQL92_DATETIME_FUNCTIONS:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_SDF_CURRENT_DATE |
				SQL_SDF_CURRENT_TIME |
				SQL_SDF_CURRENT_TIMESTAMP);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_SQL92_GRANT:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_SG_DELETE_TABLE |
				SQL_SG_INSERT_COLUMN |
				SQL_SG_INSERT_TABLE |
				SQL_SG_REFERENCES_TABLE |
				SQL_SG_REFERENCES_COLUMN |
				SQL_SG_SELECT_TABLE |
				SQL_SG_UPDATE_COLUMN |
				SQL_SG_UPDATE_TABLE);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_SRJO_CROSS_JOIN |
				SQL_SRJO_INNER_JOIN  |
				SQL_SRJO_LEFT_OUTER_JOIN |
				SQL_SRJO_NATURAL_JOIN |
				SQL_SRJO_RIGHT_OUTER_JOIN);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_SQL92_REVOKE:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_SR_DELETE_TABLE |
				SQL_SR_INSERT_COLUMN |
				SQL_SR_INSERT_TABLE |
				SQL_SR_REFERENCES_TABLE |
				SQL_SR_REFERENCES_COLUMN |
				SQL_SR_SELECT_TABLE |
				SQL_SR_UPDATE_COLUMN |
				SQL_SR_UPDATE_TABLE);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_SQL92_ROW_VALUE_CONSTRUCTOR:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_SRVC_VALUE_EXPRESSION |
				SQL_SRVC_NULL |
				SQL_SRVC_DEFAULT);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_SQL92_STRING_FUNCTIONS:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_SSF_CONVERT |
				SQL_SSF_LOWER |
				SQL_SSF_UPPER |
				SQL_SSF_SUBSTRING |
				SQL_SSF_TRANSLATE |
				SQL_SSF_TRIM_BOTH |
				SQL_SSF_TRIM_LEADING |
				SQL_SSF_TRIM_TRAILING);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_STANDARD_CLI_CONFORMANCE:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_SCC_ISO92_CLI;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_SYSTEM_FUNCTIONS:
			*((SQLUINTEGER*) rgbInfoValue)= (SQL_FN_SYS_DBNAME |
				SQL_FN_SYS_IFNULL |
				SQL_FN_SYS_USERNAME);
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_STATIC_SENSITIVITY:
			*((int*) rgbInfoValue)= (SQL_SS_ADDITIONS |
				SQL_SS_DELETIONS |
				SQL_SS_UPDATES);
			*pcbInfoValue=sizeof(int);
			break;
			
		case SQL_TABLE_TERM:
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"table",5);
			break;
			
		case SQL_TXN_CAPABLE:
			*((SQLUSMALLINT*) rgbInfoValue)=SQL_TC_DDL_COMMIT;
			*pcbInfoValue=sizeof(SQLUSMALLINT);
			break;
			
		case SQL_TXN_ISOLATION_OPTION:
			*((SQLUINTEGER*) rgbInfoValue)= SQL_TXN_READ_COMMITTED |
				SQL_TXN_READ_UNCOMMITTED |
				SQL_TXN_REPEATABLE_READ |
				SQL_TXN_SERIALIZABLE;
			*pcbInfoValue=sizeof(SQLUINTEGER);
			break;
			
		case SQL_USER_NAME:
			MYINFO_SET_STR(rgbInfoValue,cbInfoValueMax,pcbInfoValue,dbc->p_sess->uname);
			break;
			
		case SQL_XOPEN_CLI_YEAR:
			MYINFO_SET_STR_L(rgbInfoValue,cbInfoValueMax,pcbInfoValue,"1992",4);
			break;
			
		case SQL_FETCH_DIRECTION:
			*((long*) rgbInfoValue)=
				(SQL_FD_FETCH_NEXT | SQL_FD_FETCH_FIRST |
				SQL_FD_FETCH_LAST | SQL_FD_FETCH_PRIOR |
				SQL_FD_FETCH_ABSOLUTE | SQL_FD_FETCH_RELATIVE);
			if (dbc->flag & FLAG_NO_DEFAULT_CURSOR)
				*((long*) rgbInfoValue)&= ~ (long) SQL_FD_FETCH_PRIOR;
			if (dbc->flag & FLAG_FORWARD_CURSOR)
				*((long*) rgbInfoValue)=  (long) SQL_FD_FETCH_NEXT;
			*pcbInfoValue=sizeof(long);
			break;
			
		case SQL_ODBC_SAG_CLI_CONFORMANCE:
			*((SQLSMALLINT*) rgbInfoValue)=SQL_OSCC_COMPLIANT;
			*pcbInfoValue=sizeof(SQLSMALLINT);
			break;
			
		default:
			{
				char buff[80];
				sprintf(buff,"Unsupported option: %d to SQLGetInfo",fInfoType);
				DBUG_RETURN(setError(hdbc,RHERR_S1C00,buff,4000));
			}
  }/* end of switch */  
  DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : myodbc3 internal
@purpose : function initializaions
*/

/*
@type    : myodbc3 internal
@purpose : list of supported and unsupported functions in the driver
*/

int myodbc3_functions[] = {
	
		SQL_API_SQLALLOCCONNECT,
		SQL_API_SQLALLOCENV,
		SQL_API_SQLALLOCHANDLE,
		SQL_API_SQLALLOCSTMT,
		SQL_API_SQLBINDCOL,
		SQL_API_SQLBINDPARAM, //
		SQL_API_SQLCANCEL,
		SQL_API_SQLCLOSECURSOR,
		SQL_API_SQLCOLATTRIBUTE,
		SQL_API_SQLCOLUMNS,
		SQL_API_SQLCONNECT,
		SQL_API_SQLCOPYDESC,  //
		SQL_API_SQLDATASOURCES,
		SQL_API_SQLDESCRIBECOL,
		SQL_API_SQLDISCONNECT,
		SQL_API_SQLENDTRAN,
		SQL_API_SQLERROR,
		SQL_API_SQLEXECDIRECT,
		SQL_API_SQLEXECUTE,
		SQL_API_SQLFETCH,
		SQL_API_SQLFETCHSCROLL,
		SQL_API_SQLFREECONNECT,
		SQL_API_SQLFREEENV,
		SQL_API_SQLFREEHANDLE,
		SQL_API_SQLFREESTMT,
		SQL_API_SQLGETCONNECTATTR,
		SQL_API_SQLGETCONNECTOPTION,
		SQL_API_SQLGETCURSORNAME,
		SQL_API_SQLGETDATA,
		SQL_API_SQLGETDESCFIELD, //
		SQL_API_SQLGETDESCREC, //
		SQL_API_SQLGETDIAGFIELD,
		SQL_API_SQLGETDIAGREC,
		SQL_API_SQLGETENVATTR,
		SQL_API_SQLGETFUNCTIONS,
		SQL_API_SQLGETINFO,
		SQL_API_SQLGETSTMTATTR,
		SQL_API_SQLGETSTMTOPTION,
		SQL_API_SQLGETTYPEINFO,
		SQL_API_SQLNUMRESULTCOLS,
		SQL_API_SQLPARAMDATA,
		SQL_API_SQLPREPARE,
		SQL_API_SQLPUTDATA,
		SQL_API_SQLROWCOUNT,
		SQL_API_SQLSETCONNECTATTR,
		SQL_API_SQLSETCONNECTOPTION,
		SQL_API_SQLSETCURSORNAME,
		SQL_API_SQLSETDESCFIELD, //
		SQL_API_SQLSETDESCREC, //
		SQL_API_SQLSETENVATTR,
		SQL_API_SQLSETPARAM, //
		SQL_API_SQLSETSTMTATTR,
		SQL_API_SQLSETSTMTOPTION,
		SQL_API_SQLSPECIALCOLUMNS,
		SQL_API_SQLSTATISTICS,
		SQL_API_SQLTABLES,
		SQL_API_SQLTRANSACT,
		/* SQL_API_SQLALLOCHANDLESTD */
		SQL_API_SQLBULKOPERATIONS,
		SQL_API_SQLBINDPARAMETER,
		SQL_API_SQLBROWSECONNECT,
		SQL_API_SQLCOLATTRIBUTES,
		SQL_API_SQLCOLUMNPRIVILEGES ,
		SQL_API_SQLDESCRIBEPARAM,
		SQL_API_SQLDRIVERCONNECT,
		SQL_API_SQLDRIVERS,
		SQL_API_SQLEXTENDEDFETCH,
		SQL_API_SQLFOREIGNKEYS,
		SQL_API_SQLMORERESULTS,
		SQL_API_SQLNATIVESQL,
		SQL_API_SQLNUMPARAMS,
		SQL_API_SQLPARAMOPTIONS,
		SQL_API_SQLPRIMARYKEYS,
		SQL_API_SQLPROCEDURECOLUMNS,
		SQL_API_SQLPROCEDURES,
		SQL_API_SQLSETPOS,
		SQL_API_SQLSETSCROLLOPTIONS,
		SQL_API_SQLTABLEPRIVILEGES
};

/*
@type    : ODBC 1.0 API
@purpose : returns information about whether a driver supports a specific
ODBC function
*/

SQLRETURN SQL_API SQLGetFunctions(SQLHDBC hdbc,SQLUSMALLINT fFunction,
								  SQLUSMALLINT FAR *pfExists)
{
	SQLDBC FAR *dbc=(SQLDBC FAR*) hdbc;
	int index;
	int myodbc_func_size;
	DBUG_ENTER("SQLGetFunctions");
	DBUG_PRINT("enter",("fFunction: %d",fFunction));
	
	myodbc_func_size = (sizeof(myodbc3_functions)/
		      sizeof(myodbc3_functions[0]));
	
	if (fFunction == SQL_API_ODBC3_ALL_FUNCTIONS)
	{
		for (index = 0; index < myodbc_func_size; index ++)
		{
			int id = myodbc3_functions[index];
			pfExists[id >> 4] |= (1 << ( id & 0x000F));
		}
		DBUG_RETURN_STATUS(SQL_SUCCESS);
	}
	if (fFunction == SQL_API_ALL_FUNCTIONS)
	{
		for (index = 0; index < myodbc_func_size; index ++)
		{
			if (myodbc3_functions[index] < 100)
				pfExists[myodbc3_functions[index]] = SQL_TRUE;
		}
		DBUG_RETURN_STATUS(SQL_SUCCESS);
	}
	*pfExists = SQL_FALSE;
	for (index = 0; index < myodbc_func_size; index ++)
	{
		if (myodbc3_functions[index] == fFunction)
		{
			*pfExists = SQL_TRUE;
			break;
		}
	}
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}
