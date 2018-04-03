#include "odbc.h"

/*
@type    : myodbc3 internal error structure
@purpose : set of internal errors, in the following order
- SQLSTATE2 (when version is SQL_OV_ODBC2)
- SQLSTATE3 (when version is SQL_OV_ODBC3)
- error message text
- return code
*/

static ODBC_ERR_STR odbc_errors[]={
	
	{"01000","General warning",1 },
	{"01004","String data, right truncated",1 },
	{"01S02","Option value changed",1 },
	{"01S03","No rows updated/deleted",1 },
	{"01S04","More than one row updated/deleted",1 },
	{"01S06","Attempt to fetch before the result set returned the first rowset",1 },
	{"07002","SQLBindParameter not used for all parameters",-1 },
	{"07005","Prepared statement not a cursor-specification",-1 },
	{"07006","Restricted data type attribute violation",-1 },
	{"07009","Invalid descriptor index",-1 },
	{"08002","Connection name in use",-1 },
	{"08003","Connection does not exist",-1 },
	{"24000","Invalid cursor state",-1 },
	{"25000","Invalid transaction state",-1 },
	{"25S01","Transaction state unknown",-1 },
	{"34000","Invalid cursor name",-1 },
	{"HY000","General driver defined error",-1 },
	{"HY001","Memory allocation error",-1 },
	{"HY002","Invalid column number",-1 },
	{"HY003","Invalid application buffer type",-1 },
	{"HY004","Invalid SQL data type",-1 },
	{"HY009","Invalid use of null pointer",-1 },
	{"HY010","Function sequence error",-1 },
	{"HY011","Attribute can not be set now",-1 },
	{"HY012","Invalid transaction operation code",-1 },
	{"HY013","Memory management error",-1 },
	{"HY015","No cursor name available",-1 },
	{"HY024","Invalid attribute value",-1 },
	{"HY090","Invalid string or buffer length",-1 },
	{"HY091","Invalid descriptor field identifier",-1 },
	{"HY092","Invalid attribute/option identifier",-1 },
	{"HY093","Invalid parameter number",-1 },
	{"HY095","Function type out of range",-1 },
	{"HY106","Fetch type out of range",-1 },
	{"HY107","Row value out of range",-1 },
	{"HY109","Invalid cursor position",-1 },
	{"HYC00","Optional feature not implemented",-1 },
	/* server related..*/
	{"21S01","Column count does not match value count",-1},
	{"23000","Integrity constraint violation",-1},
	{"42000","Syntax error or access violation",-1},
	{"42S01","Base table or view already exists ",-1},
	{"42S02","Base table or view not found",-1},
	{"42S12","Index not found",-1},
	{"42S21","Column already exists",-1},
	{"42S22","Column not found",-1},
	{"08S01","Communication link failure",-1},
};

/*
@type    : myodbc3 internal
@purpose : If ODBC version is SQL_OV_ODBC2, initialize old SQLSTATEs
*/

void odbc_err_code2_init(void)
{
/* 
As accessing the SQLSTATE2 is very rear, set this to 
global when required .. one time ..for quick processing
in set_error/setConnError
	*/     
	uint  i;
	for (i = RHERR_S1000; i <= RHERR_S1C00; i++)
	{
		odbc_errors[i].sqlstate[0] = 'S';
		odbc_errors[i].sqlstate[1] = '1';
	}
	strncpy((char*)odbc_errors[RHERR_07005].sqlstate,"24000",5);
	strncpy((char*)odbc_errors[RHERR_42000].sqlstate,"37000",5);
	strncpy((char*)odbc_errors[RHERR_42S01].sqlstate,"S0001",5);
	strncpy((char*)odbc_errors[RHERR_42S02].sqlstate,"S0002",5);
	strncpy((char*)odbc_errors[RHERR_42S12].sqlstate,"S0012",5);
	strncpy((char*)odbc_errors[RHERR_42S21].sqlstate,"S0021",5);
	strncpy((char*)odbc_errors[RHERR_42S22].sqlstate,"S0022",5);
}

/*
@type    : myodbc3 internal
@purpose : If ODBC version is SQL_OV_ODBC3, initialize to original SQLSTATEs
*/

void odbc_sqlstate3_init(void)
{
	uint i;
	
	for (i = RHERR_S1000; i <= RHERR_S1C00; i++)
	{
		odbc_errors[i].sqlstate[0] = 'H';
		odbc_errors[i].sqlstate[1] = 'Y';
	}
	strncpy((char*)odbc_errors[RHERR_07005].sqlstate,"07005",5);
	strncpy((char*)odbc_errors[RHERR_42000].sqlstate,"42000",5);
	strncpy((char*)odbc_errors[RHERR_42S01].sqlstate,"42S01",5);
	strncpy((char*)odbc_errors[RHERR_42S02].sqlstate,"42S02",5);
	strncpy((char*)odbc_errors[RHERR_42S12].sqlstate,"42S12",5);
	strncpy((char*)odbc_errors[RHERR_42S21].sqlstate,"42S21",5);
	strncpy((char*)odbc_errors[RHERR_42S22].sqlstate,"42S22",5);
}

/*
@type    : myodbc3 internal
@purpose : copies error from one handdle to other
*/

SQLRETURN copy_stmt_error(SQLSTMT FAR *dst,SQLSTMT FAR *src)
{
	strmov(dst->error.sqlstat,src->error.sqlstat);
	strmov(dst->error.message, src->error.message);
	return(SQL_SUCCESS);
}

/*
@type    : myodbc3 internal
@purpose : sets the connection level errors, which doesn't need any
conversion
*/

SQLRETURN set_dbc_error(SQLDBC FAR *dbc,char *state,
                        const char *message,uint errcode)
{
	strmov(dbc->error.sqlstat,state);
	sprintf(dbc->error.message,"%s%s",MYODBC3_ERROR_PREFIX,message);
	dbc->error.err_no = errcode;
	return SQL_ERROR;
}

/*
@type    : myodbc3 internal
@purpose : sets the statement level errors, which doesn't need any
conversion
*/

SQLRETURN set_stmt_error(SQLSTMT FAR *stmt,char *state,
                         const char *message,uint errcode)
{
	DBUG_ENTER("set_stmt_error");
	DBUG_PRINT("error",("message: %s",message));
	
	strmov(stmt->error.sqlstat,state);
	strmov(stmt->error.message,message);
	stmt->error.err_no=errcode;
	DBUG_RETURN_STATUS(SQL_ERROR);
}

/*
@type    : myodbc3 internal
@purpose : translates SQL error to ODBC error
*/

/*
@type    : myodbc3 internal
@purpose : copy error to error structure
*/

static SQLRETURN copy_error(KCIError *error, int  errid, 
							cstr errtext, int errcode,
							cstr prefix)
{
	SQLRETURN   sqlreturn;
	SQLCHAR     *errmsg;
	int		    code;
	
	DBUG_ENTER("copy_error");
	
	errmsg = errtext ? (SQLCHAR *)errtext : (SQLCHAR *)odbc_errors[errid].message;
	code   = errcode ? errcode : errid + MYODBC_ERROR_CODE_START;
	
	DBUG_PRINT("error",("code :%d, state: %s, err :%s", 
		code, odbc_errors[errid].sqlstat, errtext));
	
	sqlreturn = error->err_no = odbc_errors[errid].retcode; /* RETCODE */
	error->err_no = code;									/* NATIVE */
	strmov(error->sqlstat, (char*)odbc_errors[errid].sqlstate);	/* SQLSTATE */
	//strxmov(error->message,prefix,errmsg,NullS);          /* MESSAGE */
	sprintf(error->message,"%s%s",prefix,errmsg);           /* MESSAGE */
	
	DBUG_RETURN_STATUS(sqlreturn);
}

/*
@type    : myodbc3 internal
@purpose : sets the error information to appropriate handle.
it also sets the SQLSTATE based on the ODBC VERSION
*/

SQLRETURN setError(SQLSMALLINT HandleType, SQLHANDLE handle,
				   rhodbc_errid errid, const char *errtext,
				   int errcode)
{
	switch (HandleType) {
		
	case SQL_HANDLE_ENV:
		return copy_error(&((SQLENV*)handle)->error,errid,errtext,
			errcode,MYODBC3_ERROR_PREFIX);
		
	case SQL_HANDLE_DBC:
		return copy_error(&((SQLDBC *)handle)->error,errid,errtext,
			errcode,MYODBC3_ERROR_PREFIX);
		
	default:
		return copy_error(&((SQLSTMT *)handle)->error,errid,errtext,errcode,MYODBC3_ERROR_PREFIX);
	}
}


/*
@type    : myodbc3 internal
@purpose : returns the current values of multiple fields of a diagnostic
record that contains error, warning, and status information.
Unlike SQLGetDiagField, which returns one diagnostic field
per call, SQLGetDiagRec returns several commonly used fields
of a diagnostic record, including the SQLSTATE, the native
error code, and the diagnostic message text
*/

SQLRETURN rh_SQLGetDiagRec(SQLSMALLINT HandleType,
						   SQLHANDLE Handle,
						   SQLSMALLINT RecNumber,
						   SQLCHAR *Sqlstate,
						   int  *NativeErrorPtr,
						   SQLCHAR *MessageText,
						   SQLSMALLINT BufferLength,
						   SQLSMALLINT *TextLengthPtr)
{
	SQLCHAR     *errmsg;
	SQLRETURN   result = SQL_SUCCESS;
	SQLSMALLINT tmp_size;
	SQLCHAR     tmp_state[6];
	int  tmp_error;
	DBUG_ENTER("SQLGetDiagRec");
	DBUG_PRINT("values",("%d,0x%x,%d,0x%x,0x%x,0x%x,%d,0x%x",HandleType,Handle,
		RecNumber,Sqlstate,NativeErrorPtr,MessageText,
		BufferLength,TextLengthPtr));
	
	if (!TextLengthPtr)
		TextLengthPtr= &tmp_size;
	
	if (!Sqlstate)
		Sqlstate= tmp_state;
	
	if (!NativeErrorPtr)
		NativeErrorPtr= &tmp_error;
	
	if (RecNumber <= 0 || BufferLength < 0 || !Handle)
		DBUG_RETURN_STATUS(SQL_ERROR);
	
		/*
		Currently we are not supporting error list, so
		if RecNumber > 1, return no data found
	*/
	
	if (RecNumber > 1)
		DBUG_RETURN_STATUS(SQL_NO_DATA_FOUND);
	
	switch(HandleType) {
	case SQL_HANDLE_STMT:
		errmsg=(SQLCHAR*)((SQLSTMT FAR*) Handle)->error.message;
		strmov((char*) Sqlstate,((SQLSTMT FAR*) Handle)->error.sqlstat);
		*NativeErrorPtr=((SQLSTMT FAR*) Handle)->error.err_no;
		break;
		
	case SQL_HANDLE_DBC:
		errmsg= (SQLCHAR*)((SQLDBC FAR*) Handle)->error.message;
		strmov((char*) Sqlstate,((SQLDBC FAR*) Handle)->error.sqlstat);
		*NativeErrorPtr=((SQLDBC FAR*) Handle)->error.err_no;
		
		break;
		
	case SQL_HANDLE_ENV:
		errmsg= (SQLCHAR*)((SQLENV FAR*) Handle)->error.message;
		strmov((char*) Sqlstate,((SQLENV FAR*) Handle)->error.sqlstat);
		*NativeErrorPtr=((SQLENV FAR*) Handle)->error.err_no;
		break;
		
	default:
		DBUG_RETURN_STATUS(SQL_INVALID_HANDLE);
	}
	if (!errmsg || !errmsg[0])
	{
		*TextLengthPtr=0;
		strmov((char*)Sqlstate,(char*)"00000");
		DBUG_RETURN_STATUS(SQL_NO_DATA_FOUND);
	}
	DBUG_RETURN(copy_str_data(HandleType,Handle,MessageText,
		BufferLength, TextLengthPtr, (char*)errmsg));
}

/*
@type    : ODBC 3.0 API
@purpose : returns the current value of a field of a record of the
diagnostic data structure (associated with a specified handle)
that contains error, warning, and status information
*/

SQLRETURN SQL_API SQLGetDiagField(SQLSMALLINT HandleType,
								  SQLHANDLE   Handle,
								  SQLSMALLINT RecNumber,
								  SQLSMALLINT DiagIdentifier,
								  SQLPOINTER  DiagInfoPtr,
								  SQLSMALLINT BufferLength,
								  SQLSMALLINT *StringLengthPtr)
{
	SQLRETURN   error = SQL_SUCCESS;
	SQLPOINTER  szDiagInfo=NULL;
	SQLSMALLINT tmp_size;
	DBUG_ENTER("SQLGetDiagField");
	DBUG_PRINT("values",("%d,0x%x,%d,%d,0x%x,%d,0x%x",HandleType,Handle,
		RecNumber,DiagIdentifier,DiagInfoPtr,
		BufferLength,StringLengthPtr));
	
	if (!StringLengthPtr)
		StringLengthPtr= &tmp_size;
	
	if (!DiagInfoPtr)
		DiagInfoPtr= szDiagInfo;
	
	if (!Handle ||
		!(HandleType == SQL_HANDLE_STMT ||
        HandleType == SQL_HANDLE_DBC ||
        HandleType == SQL_HANDLE_ENV ))
		DBUG_RETURN_STATUS(SQL_ERROR);
	
	if (RecNumber > 1)
		DBUG_RETURN_STATUS(SQL_NO_DATA_FOUND);
	
		/* If record number is 0 and if it is not a
		diag header field request, return error
	*/
	if (RecNumber == 0 && DiagIdentifier > 0)
		DBUG_RETURN_STATUS(SQL_ERROR);
	
	switch(DiagIdentifier ) {
		
		/* DIAG HEADER FIELDS SECTION */
	case SQL_DIAG_DYNAMIC_FUNCTION:
		if (HandleType != SQL_HANDLE_STMT)
			DBUG_RETURN_STATUS(SQL_ERROR);
		
		error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr, "");
		break;
		
	case SQL_DIAG_DYNAMIC_FUNCTION_CODE:
		*(int *) DiagInfoPtr = 0;
		break;
		
	case SQL_DIAG_ROW_NUMBER:
		*(int *) DiagInfoPtr = SQL_ROW_NUMBER_UNKNOWN;
		break;
		
	case SQL_DIAG_NUMBER:
		*(int *) DiagInfoPtr = 1;
		break;
		
	case SQL_DIAG_RETURNCODE:
		if (HandleType == SQL_HANDLE_STMT)
			*(SQLRETURN *) DiagInfoPtr = ((SQLSTMT FAR *)Handle)->error.err_no;
		
		else if (HandleType == SQL_HANDLE_DBC)
			*(SQLRETURN *) DiagInfoPtr = ((SQLDBC FAR *)Handle)->error.err_no;
		
		else
			*(SQLRETURN *) DiagInfoPtr = ((SQLENV FAR *)Handle)->error.err_no;
		break;
		
	case SQL_DIAG_CURSOR_ROW_COUNT:/* at present, return total rows in rs */
		if (HandleType != SQL_HANDLE_STMT)
			DBUG_RETURN_STATUS(SQL_ERROR);
		
		if (!((SQLSTMT FAR *)Handle)->max_row_idx)
			*(int *) DiagInfoPtr=0;
		
		else
			*(int *) DiagInfoPtr = (int)
			((SQLSTMT FAR *) Handle)->max_row_idx;
		break;
		
	case SQL_DIAG_ROW_COUNT:
		if (HandleType != SQL_HANDLE_STMT)
			DBUG_RETURN_STATUS(SQL_ERROR);	
		*(int *) DiagInfoPtr = (int)
		((SQLSTMT FAR *) Handle)->updt_num;
		break;
		
		/* DIAG RECORD FIELDS SECTION */
		
	case SQL_DIAG_CLASS_ORIGIN:
	case SQL_DIAG_SUBCLASS_ORIGIN:
		error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr, "ISO 9075");
		break;
		
	case SQL_DIAG_COLUMN_NUMBER:
		
		*(int *) DiagInfoPtr = SQL_COLUMN_NUMBER_UNKNOWN;
		break;
		
	case SQL_DIAG_CONNECTION_NAME:
    /*
	When the connection fails, ODBC DM calls this function, so don't
	return dbc->dsn as the connection name instead return empty string
	*/
		/*
		if (HandleType == SQL_HANDLE_STMT)
			error=copy_str_data(HandleType, Handle, (SQLCHAR*), BufferLength,
			StringLengthPtr,((SQLSTMT FAR *)Handle)->dbc->dsn ?
			((SQLSTMT FAR *)Handle)->dbc->dsn:"");
		
		else if (HandleType == SQL_HANDLE_DBC)
			error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr,((SQLDBC FAR *)Handle)->dsn ?
			((SQLDBC FAR *)Handle)->dsn:"");
		
		else
		{
			*(SQLCHAR *) DiagInfoPtr = 0;
			*StringLengthPtr = 0;
		}
		break;
		*/
	case SQL_DIAG_MESSAGE_TEXT:
		if (HandleType == SQL_HANDLE_STMT)
			error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr,((SQLSTMT FAR *)Handle)->error.message);
		
		else if (HandleType == SQL_HANDLE_DBC)
			error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr,((SQLDBC FAR *)Handle)->error.message);
		else
			error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr,((SQLENV FAR *)Handle)->error.message);
		break;
		
	case SQL_DIAG_NATIVE:
		if (HandleType == SQL_HANDLE_STMT)
			*(int *) DiagInfoPtr = ((SQLSTMT FAR *)Handle)->error.err_no;
		
		else if (HandleType == SQL_HANDLE_DBC)
			*(int *) DiagInfoPtr = ((SQLDBC FAR *)Handle)->error.err_no;
		
		else
			*(int *) DiagInfoPtr = ((SQLENV FAR *)Handle)->error.err_no;
		break;
		
	case SQL_DIAG_SERVER_NAME:
		if (HandleType == SQL_HANDLE_STMT)
			error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr,"");
		else if (HandleType == SQL_HANDLE_DBC)
			error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr,"");
		else
		{
			*(SQLCHAR *) DiagInfoPtr = 0;
			*StringLengthPtr = 0;
		}
		break;
		
	case SQL_DIAG_SQLSTATE:
		if (HandleType == SQL_HANDLE_STMT)
			error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr,((SQLSTMT FAR *)Handle)->error.sqlstat);
		
		else if (HandleType == SQL_HANDLE_DBC)
			error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr,((SQLDBC FAR *)Handle)->error.sqlstat);
		
		else
			error=copy_str_data(HandleType, Handle, (SQLCHAR*)DiagInfoPtr, BufferLength,
			StringLengthPtr,((SQLENV FAR *)Handle)->error.sqlstat);
		break;
		
	default:
		DBUG_RETURN_STATUS(SQL_ERROR);
  }
  DBUG_RETURN_STATUS(error);
}

/*
@type    : ODBC 3.0 API
@purpose : returns the current diagnostic record information
*/

SQLRETURN SQL_API SQLGetDiagRec(SQLSMALLINT HandleType,
								SQLHANDLE   Handle,
								SQLSMALLINT RecNumber,
								SQLCHAR     *Sqlstate,
								int  *NativeErrorPtr,
								SQLCHAR     *MessageText,
								SQLSMALLINT BufferLength,
								SQLSMALLINT *TextLengthPtr)
{
	return rh_SQLGetDiagRec(HandleType,Handle,RecNumber,Sqlstate,
		NativeErrorPtr,MessageText,
		BufferLength,TextLengthPtr);
}

/*
@type    : ODBC 1.0 API - depricated
@purpose : returns error or status information
*/

SQLRETURN SQL_API SQLError(SQLHENV henv, SQLHDBC hdbc, SQLHSTMT hstmt,
						   SQLCHAR FAR    *szSqlState,
						   int FAR *pfNativeError,
						   SQLCHAR FAR    *szErrorMsg,
						   SQLSMALLINT    cbErrorMsgMax,
						   SQLSMALLINT FAR *pcbErrorMsg)
{
	SQLRETURN error=SQL_INVALID_HANDLE;
	DBUG_ENTER("SQLError");
	
	if (hstmt){
		error = rh_SQLGetDiagRec(SQL_HANDLE_STMT,hstmt,1,szSqlState,
			pfNativeError, szErrorMsg,
			cbErrorMsgMax,pcbErrorMsg);
		if(error == SQL_SUCCESS)
			CLEAR_STMT_ERROR(hstmt);
	}
	else if (hdbc){
		error = rh_SQLGetDiagRec(SQL_HANDLE_DBC,hdbc,1,szSqlState,
			pfNativeError, szErrorMsg,
			cbErrorMsgMax,pcbErrorMsg);
		if(error == SQL_SUCCESS)
			CLEAR_DBC_ERROR(hdbc);
	}
	else if (henv){
		error = rh_SQLGetDiagRec(SQL_HANDLE_ENV,henv,1,szSqlState,
			pfNativeError, szErrorMsg,
			cbErrorMsgMax,pcbErrorMsg);
		if(error == SQL_SUCCESS)
			CLEAR_ENV_ERROR(henv);
	}
	DBUG_RETURN_STATUS(error);
}

