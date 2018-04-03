#include "odbc.h"

/*
  @type    : ODBC 1.0 API
  @purpose : associates a cursor name with an active statement.
       If an application does not call SetCursorName, the driver
       generates cursor names as needed for SQL statement processing
*/

SQLRETURN SQL_API SQLSetCursorName(SQLHSTMT hstmt, SQLCHAR *szCursor, 
				   SQLSMALLINT cbCursor)
{
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	DBUG_ENTER("SQLSetCursorName");

	CLEAR_STMT_ERROR(stmt);

	if (!szCursor)
		DBUG_RETURN(setError(stmt,RHERR_S1009,NULL,0));

	if (cbCursor == SQL_NTS)
		cbCursor = (SQLSMALLINT) strlen((cstr)szCursor);

	if (cbCursor < 0 )
		DBUG_RETURN(setError(stmt,RHERR_S1090,NULL,0));
	if(stmt->cursor)
		mfree(stmt->cursor);
	stmt->cursor = mc_strndup(&(stmt->mem), (cstr)szCursor, cbCursor);
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}


SQLRETURN SQL_API SQLGetCursorName(SQLHSTMT hstmt, 
				SQLCHAR FAR *szCursor,
				SQLSMALLINT cbCursorMax, 
				SQLSMALLINT FAR *pcbCursor)
{
	SQLSTMT FAR  *stmt=(SQLSTMT FAR*) hstmt;
	int  nLength;
	SQLSMALLINT nDummyLength;
	DBUG_ENTER("SQLGetCursorName");

	CLEAR_STMT_ERROR(stmt);

	if (cbCursorMax < 0)
		DBUG_RETURN(setError(stmt,RHERR_S1090,NULL,0));

	if (!pcbCursor)
		pcbCursor = &nDummyLength;

	if (cbCursorMax)
		cbCursorMax -= sizeof(char);

	*pcbCursor = strlen(stmt->cursor);
	if (szCursor && cbCursorMax > 0)
		strncpy1((char*)szCursor, stmt->cursor, cbCursorMax);

	nLength = min(*pcbCursor , cbCursorMax );
	if (nLength != *pcbCursor)
		DBUG_RETURN(setError(stmt,RHERR_01004,NULL,0));
	
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
  @type    : ODBC 1.0 API
  @purpose : sets the cursor position in a rowset and allows an application
       to refresh data in the rowset or to update or delete data in
       the result set
*/

SQLRETURN SQL_API SQLSetPos( SQLHSTMT hstmt, SQLUSMALLINT irow,
					  SQLUSMALLINT fOption, SQLUSMALLINT fLock)
{
	SQLSTMT  *stmt = (SQLSTMT FAR*) hstmt;
	SQLDBC	 *p_conn;

	p_conn = stmt->p_svctx;
	switch (fOption)
	{
	case SQL_POSITION:
		//todo
		break;
	case SQL_REFRESH:
		//todo
		break;
	case SQL_DELETE:
		//todo
		break;
	case SQL_UPDATE:
		break;
	default:
		return SQL_ERROR;
	}
	return SQL_SUCCESS;
}

/*
  @type    : ODBC 1.0 API
  @purpose : performs bulk insertions and bulk m_bmk operations,
       including update, delete, and fetch by m_bmk
*/

SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT  hstmt, SQLSMALLINT Operation)
{
	DBUG_ENTER("SQLBulkOperations");
	SQLSTMT *stmt = (SQLSTMT*) hstmt;
	SQLDBC  *p_conn = stmt->p_svctx;
	
	switch(Operation)
	{
	case SQL_ADD:
		//todo
		break;
	case SQL_UPDATE_BY_BOOKMARK:
		//todo
		break;
	case SQL_DELETE_BY_BOOKMARK:
		//todo
		break;
	case SQL_FETCH_BY_BOOKMARK:
		//todo
		break;
	default:
		return SQL_ERROR;
	}
	return SQL_SUCCESS;
}

/*
  @type    : ODBC 3.0 API
  @purpose : closes a cursor that has been opened on a statement
       and discards any pending results
*/

SQLRETURN SQL_API SQLCloseCursor(SQLHSTMT Handle)
{

	DBUG_ENTER("SQLCloseCursor");
	return SQLFreeStmt(Handle, SQL_CLOSE);
}



