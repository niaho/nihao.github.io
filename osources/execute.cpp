/*************************************************************************** 
* The following ODBC APIs are implemented in this file:                   *
*                                                                         *
*   SQLExecute          (ISO 92)                                          *
*   SQLExecDirect       (ISO 92)                                          *
*   SQLParamData        (ISO 92)                                          *
*   SQLPutData          (ISO 92)                                          *
*   SQLCancel           (ISO 92)                                          *
*                                                                         *
****************************************************************************/
#include "odbc.h" 
#include <locale.h>

/*
@type    : ODBC 1.0 API
@purpose : executes a prepared statement, using the current values
of the parameter marker variables if any parameter markers
exist in the statement
*/

SQLRETURN SQL_API SQLExecute(SQLHSTMT hstmt)
{
	DBUG_ENTER("SQLExecute");
	SQLSTMT *p_stmt = (SQLSTMT*)hstmt;
	return KCIStmtExecute(p_stmt->p_svctx, p_stmt, &p_stmt->error, 1, 0,0, 0, 0);
}



/*
@type    : ODBC 1.0 API
@purpose : executes a preparable statement, using the current values of
the parameter marker variables if any parameters exist in thestatement
*/

SQLRETURN SQL_API SQLExecDirect(SQLHSTMT hstmt, 
								SQLCHAR FAR *szSqlStr, 
								int cbSqlStr)
{
	DBUG_ENTER("SQLExecDirect");
	SQLSTMT *p_stmt = (SQLSTMT*)hstmt;
	KCIStmtPrepare(p_stmt, &(p_stmt->error), szSqlStr, cbSqlStr, 0, 0);
	return KCIStmtExecute(p_stmt->p_svctx, p_stmt, &p_stmt->error, 1, 0, 0, 0, 0);
}

/*
@type    : ODBC 1.0 API
@purpose : returns the SQL string as modified by the driver
*/

SQLRETURN SQL_API SQLNativeSql(SQLHDBC hdbc,
							   SQLCHAR FAR *szSqlStrIn, 
							   int cbSqlStrIn,
							   SQLCHAR FAR *szSqlStr,
							   int cbSqlStrMax,
							   int FAR *pcbSqlStr)
{
	ulong offset=0;
	DBUG_ENTER("SQLNativeSql");
	DBUG_RETURN(copy_lresult(SQL_HANDLE_DBC, hdbc,
		szSqlStr,cbSqlStrMax,pcbSqlStr,(char*)szSqlStrIn, cbSqlStrIn,0L,0L,&offset,0));
}

/*
@type    : ODBC 1.0 API
@purpose : is used in conjunction with SQLPutData to supply parameter
data at statement execution time
*/

SQLRETURN SQL_API SQLParamData(SQLHSTMT hstmt, SQLPOINTER FAR *prbgValue)
{
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	SQLRETURN ret = 0;
	DBUG_ENTER("SQLParamData");
	DBUG_RETURN( ret);
}

/*
@type    : ODBC 1.0 API
@purpose : allows an application to send data for a parameter or column to
the driver at statement execution time. This function can be used
to send character or binary data values in parts to a column with
a character, binary, or data source specific data type.
*/

SQLRETURN SQL_API SQLPutData(SQLHSTMT hstmt, SQLPOINTER rgbValue,
							 int cbValue)
{
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : ODBC 1.0 API
@purpose : cancels the processing on a statement
*/
SQLRETURN SQL_API rh_SQLCancel(SQLHSTMT hstmt)
{
	DBUG_ENTER("SQLCancel");
	DBUG_RETURN(SQLFreeStmt(hstmt,SQL_CLOSE));
}

SQLRETURN SQL_API SQLCancel(SQLHSTMT hstmt)
{
	DBUG_ENTER("SQLCancel");
	DBUG_RETURN(SQLFreeStmt(hstmt,SQL_CLOSE));
}
