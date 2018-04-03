
#ifndef __ERROR_H__
#define __ERROR_H__

#include <sqltypes.h>
/*
  myodbc internal error constants
*/
#define ER_INVALID_CURSOR_NAME	 514
#define ER_ALL_COLUMNS_IGNORED	 537

/*
  myodbc3 error prefix
*/
#define MYODBC3_ERROR_PREFIX     "[Kunlun][ODBC 3.0 Driver]"
#define MYODBC_ERROR_CODE_START  500

#define CLEAR_ENV_ERROR(env)   ((SQLENV FAR *)env)->error.message[0]='\0'
#define CLEAR_DBC_ERROR(dbc)   ((SQLDBC FAR *)dbc)->error.message[0]='\0'
#define CLEAR_STMT_ERROR(stmt) ((SQLSTMT FAR *)stmt)->error.message[0]='\0'

/*
  list of MyODBC3 error codes
*/
typedef enum rhodbc_errid
{
    RHERR_01000 = 0,
    RHERR_01004,
    RHERR_01S02,
    RHERR_01S03,
    RHERR_01S04,
    RHERR_01S06,
    RHERR_07001,
    RHERR_07005,
    RHERR_07006,
    RHERR_07009,
    RHERR_08002,
    RHERR_08003,
    RHERR_24000,
    RHERR_25000,
    RHERR_25S01,
    RHERR_34000,
    RHERR_S1000,
    RHERR_S1001,
    RHERR_S1002,
    RHERR_S1003,
    RHERR_S1004,
    RHERR_S1009,
    RHERR_S1010,
    RHERR_S1011,
    RHERR_S1012,
    RHERR_S1013,
    RHERR_S1015,
    RHERR_S1024,
    RHERR_S1090,
    RHERR_S1091,
    RHERR_S1092,
    RHERR_S1093,
    RHERR_S1095,
    RHERR_S1106,
    RHERR_S1107,
    RHERR_S1109,
    RHERR_S1C00,
    RHERR_21S01,
    RHERR_23000,
    RHERR_42000,
    RHERR_42S01,
    RHERR_42S02,
    RHERR_42S12,
    RHERR_42S21,
    RHERR_42S22,
    RHERR_08S01,

} rhodbc_errid;

/*
  error handler-predefined structure
  odbc2 state, odbc3 state, message and return code
*/
#ifdef SQL_MAX_MESSAGE_LENGTH
#undef SQL_MAX_MESSAGE_LENGTH
#define SQL_MAX_MESSAGE_LENGTH 256
#endif
typedef struct odbc_err_str {
  SQLCHAR     sqlstate[6];  /* ODBC3 STATE, if SQL_OV_ODBC2, then ODBC2 STATE */
  SQLCHAR     message[SQL_MAX_MESSAGE_LENGTH+1];/* ERROR MSG */
  SQLRETURN   retcode;	    /* RETURN CODE */

} ODBC_ERR_STR;

#endif /* __ERROR_H__ */

