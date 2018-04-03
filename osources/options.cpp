
/*************************************************************************** 
* The following ODBC APIs are implemented in this file:                   *
*                                                                         *
*   SQLSetEnvAttr       (ISO 92)                                          *
*   SQLGetEnvAttr       (ISO 92)                                          *
*   SQLSetConnectAttr   (ISO 92)                                          *
*   SQLGetConnectAttr   (ISO 92)                                          *
*   SQLSetStmtAttr      (ISO 92)                                          *
*   SQLGetStmtAttr      (ISO 92)                                          *
*   SQLSetConnectOption (ODBC, Deprecated)                                *
*   SQLGetConnectOption (ODBC, Deprecated)                                *
*   SQLSetStmtOption    (ODBC, Deprecated)                                *
*   SQLGetStmtOption    (ODBC, Deprecated)                                *
*                                                                         *
****************************************************************************/

#include "odbc.h"

/*
@type    : myodbc3 internal
@purpose : sets the common connection/stmt attributes
*/

static SQLRETURN set_constmt_attr(SQLSMALLINT  HandleType,
								  SQLHANDLE    Handle,
								  STMT_OPTIONS *options,
								  int			Attribute,
								  SQLPOINTER   ValuePtr)
{
	DBUG_ENTER("set_constmt_attr");
	
	switch(Attribute){
		
	case SQL_ATTR_ASYNC_ENABLE:
		if( (SQLUINTEGER)ValuePtr == SQL_ASYNC_ENABLE_ON )
			DBUG_RETURN(setError(HandleType,Handle,RHERR_01S02,
			"Doesn't support asynchronous, changed to default",0));
		break;
		
	case SQL_ATTR_CURSOR_SENSITIVITY:
		if ((SQLUINTEGER)ValuePtr != SQL_UNSPECIFIED )
		{
			DBUG_RETURN(setError(HandleType,Handle,RHERR_01S02,
				"Option value changed to default cursor sensitivity(unspecified)",0));
		}
		break;
		
	case SQL_ATTR_CURSOR_TYPE:
		options->cursor_type= (SQLUINTEGER)ValuePtr;
		/*
		if (((SQLSTMT FAR*)Handle)->dbc->flag & FLAG_DYNAMIC_CURSOR)
		{
			if ((SQLUINTEGER) ValuePtr != SQL_CURSOR_KEYSET_DRIVEN)
				options->cursor_type= (SQLUINTEGER)ValuePtr;
			
			else
			{
				options->cursor_type=SQL_CURSOR_STATIC;
				DBUG_RETURN(setError(HandleType,Handle,RHERR_01S02,
					"Option value changed to default static cursor",0));
			}
		}
		else
		{
			if ((SQLUINTEGER) ValuePtr == SQL_CURSOR_FORWARD_ONLY ||
				(SQLUINTEGER) ValuePtr == SQL_CURSOR_STATIC)
				options->cursor_type= (SQLUINTEGER)ValuePtr;
			
			else
			{
				options->cursor_type=SQL_CURSOR_STATIC;
				DBUG_RETURN(setError(HandleType,Handle,RHERR_01S02,
					"Option value changed to default static cursor",0));
			}
		}
*/
		break;
		
	case SQL_ATTR_MAX_LENGTH:
		options->max_length = (SQLUINTEGER) ValuePtr;
		break;
		
	case SQL_ATTR_MAX_ROWS:
		options->max_rows = (SQLUINTEGER) ValuePtr;
		break;
		
	case SQL_ATTR_METADATA_ID:
		if ((SQLUINTEGER)ValuePtr == SQL_TRUE)
			DBUG_RETURN(setError(HandleType,Handle,RHERR_01S02,
			"Doesn't support SQL_ATTR_METADATA_ID to true, changed to default",0));
		break;
		
	case SQL_ATTR_RETRIEVE_DATA:
		DBUG_PRINT("info",("SQL_ATTR_RETRIEVE_DATA value is ignored"));
		break;
		
	case SQL_ATTR_ROW_BIND_TYPE:
		options->bind_type = (SQLUINTEGER )ValuePtr;
		break;
		
	case SQL_ATTR_SIMULATE_CURSOR:
		options->simulateCursor = (SQLUINTEGER)ValuePtr;
		break;
		
	case SQL_ATTR_ROW_BIND_OFFSET_PTR:
		options->bind_offset = (int *)ValuePtr;
		break;
		
	case 1226:/* MS SQL Server Extension */
	case 1227:
	case 1228:
		break;
		
	case SQL_ATTR_QUERY_TIMEOUT:
	case SQL_ATTR_KEYSET_SIZE:
	case SQL_ATTR_CONCURRENCY:
	case SQL_ATTR_NOSCAN:
	case SQL_ATTR_ROW_OPERATION_PTR: /* need to support this ....*/
		DBUG_PRINT("info",("Attribute, %d is ignored", Attribute));
		break;
		
	case SQL_ATTR_FETCH_BOOKMARK_PTR:
		options->bookmark_ptr = (void *)ValuePtr;
		break;
	case SQL_ATTR_USE_BOOKMARKS:
		options->bookmark_opt = (int)ValuePtr;
		break;
	default:
		DBUG_PRINT("info",("Attribute, %d is ignored", Attribute));
		break;
  }
  DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : myodbc3 internal
@purpose : returns the common connection/stmt attributes
*/

static SQLRETURN get_constmt_attr(SQLSMALLINT  HandleType,
								  SQLHANDLE    Handle,
								  STMT_OPTIONS *options,
								  int   Attribute,
								  SQLPOINTER   ValuePtr,
								  int   *StringLengthPtr)
{
	DBUG_ENTER("get_constmt_attr");
	
	switch(Attribute) {
		
	case SQL_ATTR_ASYNC_ENABLE:
		*((SQLUINTEGER *) ValuePtr)= SQL_ASYNC_ENABLE_OFF;
		break;
		
	case SQL_ATTR_CURSOR_SENSITIVITY:
		*((SQLUINTEGER *) ValuePtr)= SQL_UNSPECIFIED;
		break;
		
	case SQL_ATTR_CURSOR_TYPE:
		*((SQLUINTEGER *) ValuePtr)= options->cursor_type;
		break;
		
	case SQL_ATTR_MAX_LENGTH:
		*((SQLUINTEGER *) ValuePtr)= options->max_length;
		break;
		
	case SQL_ATTR_MAX_ROWS:
		*((SQLUINTEGER *) ValuePtr)= options->max_rows;
		break;
		
	case SQL_ATTR_METADATA_ID:
		*((SQLUINTEGER *) ValuePtr)= SQL_TRUE;
		break;
		
	case SQL_ATTR_QUERY_TIMEOUT:
		*((SQLUINTEGER *) ValuePtr)= SQL_QUERY_TIMEOUT_DEFAULT;
		break;
		
	case SQL_ATTR_RETRIEVE_DATA:
		*((SQLUINTEGER *) ValuePtr)= SQL_RD_DEFAULT;
		break;
		
	case SQL_ATTR_ROW_BIND_TYPE:
		*((SQLUINTEGER *) ValuePtr)= options->bind_type;
		break;
		
	case SQL_ATTR_SIMULATE_CURSOR:
		*((SQLUINTEGER *) ValuePtr)= SQL_SC_UNIQUE;
		break;
		
	case SQL_ATTR_CONCURRENCY:
		*((SQLUINTEGER *) ValuePtr)= SQL_CONCUR_READ_ONLY;
		break;
		
	case SQL_KEYSET_SIZE:
		*((SQLUINTEGER *) ValuePtr)=0L;
		break;
		
	case SQL_ROWSET_SIZE:
		*(SQLUINTEGER *)ValuePtr = options->rows_in_set;
		break;
		
	case SQL_NOSCAN:
		*((SQLUINTEGER *) ValuePtr)=SQL_NOSCAN_ON;
		break;
		
	case SQL_ATTR_ROW_BIND_OFFSET_PTR:
		*((int *) ValuePtr) = options->bind_offset ?
			*(options->bind_offset):
		0;
		break;
		
	case SQL_ATTR_ROW_OPERATION_PTR: /* need to support this ....*/
		DBUG_PRINT("info",("Attribute, %d is ignored", Attribute));
		DBUG_RETURN_STATUS(SQL_SUCCESS_WITH_INFO);
		break;
	case SQL_ATTR_FETCH_BOOKMARK_PTR:
		*((SQLUINTEGER*)ValuePtr) = (SQLUINTEGER)(options->bookmark_ptr);
		break;
	case SQL_ATTR_USE_BOOKMARKS:
		*(SQLUINTEGER*)ValuePtr = (SQLUINTEGER)(options->bookmark_opt);
		break;		
	case 1226:/* MS SQL Server Extension */
	case 1227:
	case 1228:
		break;
		
	default:
		DBUG_PRINT("error",("Invalid attribute/option identifier:%d", Attribute));
	}
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : myodbc3 internal
@purpose : sets the connection attributes
*/

static SQLRETURN setConnAttr(SQLHDBC    hdbc,
							  int Attribute,
							  SQLPOINTER ValuePtr,
							  int StringLengthPtr)
{
	SQLDBC FAR *dbc=(SQLDBC FAR*) hdbc;
	DBUG_ENTER("SQLSetConnectAttr");
	DBUG_PRINT("enter",("Atrr: %d, ValuePtr: 0x%lx, StrLenPtr: %d",Attribute,ValuePtr,StringLengthPtr));
	
	switch(Attribute) {
		
	case SQL_ATTR_ACCESS_MODE:
		DBUG_PRINT("info",("SQL_ATTR_ACCESS_MODE %ld ignored",
			(SQLUINTEGER)ValuePtr));
		break;
		
	case SQL_ATTR_AUTOCOMMIT:
		if (!(dbc->auto_commit))
		{
			if(doCommand(dbc,"set auto commit on"))
			{
				dbc->auto_commit = true;
				DBUG_RETURN(SQL_SUCCESS);		
			}
			DBUG_RETURN(SQL_ERROR);
		}
		else
			DBUG_RETURN(SQL_SUCCESS);
		break;
		
	case SQL_ATTR_CONNECTION_TIMEOUT:
		{
			uint timeout = (SQLUINTEGER)ValuePtr;  
		}
		break;
		
	case SQL_ATTR_CURRENT_CATALOG:
		{
			char *qry_str = malloc(strlen((char*)ValuePtr)+10);
			sprintf(qry_str,"login  database=%s",(char*)ValuePtr);
			if(doCommand(dbc,qry_str))
			{
				safe_free(dbc->db_name);
				dbc->db_name =Strdup((char*) ValuePtr);
				DBUG_RETURN(SQL_SUCCESS);
			}
			else
				DBUG_RETURN(SQL_ERROR);
		}
		break;
		
	case SQL_ATTR_LOGIN_TIMEOUT:
		dbc->login_timeout=(SQLUINTEGER)ValuePtr;
		break;
		
	case SQL_ATTR_ODBC_CURSORS:
		if ((dbc->flag & FLAG_FORWARD_CURSOR) && 
			(SQLUINTEGER)ValuePtr != SQL_CUR_USE_ODBC)
			DBUG_RETURN(setError(hdbc,RHERR_01S02,
			"Forcing the Driver Manager to use ODBC cursor library",0));
		break;	  
		
	case SQL_OPT_TRACE:
	case SQL_OPT_TRACEFILE:
	case SQL_QUIET_MODE:
	case SQL_TRANSLATE_DLL:
	case SQL_TRANSLATE_OPTION:
		{
			char buff[100];
			sprintf(buff,"Suppose to set this attribute '%d' through driver manager, not by the driver",Attribute);
			DBUG_RETURN(setError(hdbc,RHERR_01S02,buff,0));
		}
		
	case SQL_ATTR_PACKET_SIZE:
		DBUG_PRINT("info",("SQL_ATTR_PACKET_SIZE %ld ignored",
			(SQLUINTEGER)ValuePtr));
		break;
		
	case SQL_ATTR_TXN_ISOLATION:
		{
			char buff[80];
			const char *level;
			
			if ((int)ValuePtr & SQL_TXN_SERIALIZABLE)
				level="SERIALIZABLE";
			else if ((int)ValuePtr & SQL_TXN_REPEATABLE_READ)
				level="REPEATABLE READ";
			else if ((int)ValuePtr & SQL_TXN_REPEATABLE_READ)
				level="READ COMMITTED";
			else
				level="READ COMMITTED";
			sprintf(buff,"SET TRANSACTION ISOLATION LEVEL %s",level);
			if (doCommand(dbc,buff))
				dbc->txn_isolation=(int)ValuePtr;
		}
		break;
		
		/* 3.x driver doesn't support any statement attributes
		at connection level, but to make sure all 2.x apps
		works fine...lets support it..nothing to loose..
		*/
	default:
		DBUG_RETURN(set_constmt_attr(2,dbc,&dbc->stmt_options,
			Attribute,ValuePtr));
	}
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : myodbc3 internal
@purpose : returns the connection attribute values
*/

static SQLRETURN getConnAttr(SQLHDBC    hdbc,
							  int Attribute,
							  SQLPOINTER ValuePtr,
							  int BufferLength,
							  int *StringLengthPtr)
{
	SQLDBC FAR *dbc=(SQLDBC FAR*) hdbc;
	SQLRETURN result=SQL_SUCCESS;
	int str_len;
	SQLPOINTER vparam=0;
	DBUG_ENTER("SQLGetConnectAttr");
	
	if (!ValuePtr)
		ValuePtr = vparam;
	
	if (!StringLengthPtr)
		StringLengthPtr = &str_len;
	
	switch(Attribute){
		
	case SQL_ATTR_ACCESS_MODE:
		*((SQLUINTEGER *) ValuePtr)= SQL_MODE_READ_WRITE;
		break;
		
	case SQL_ATTR_AUTO_IPD:
		*((SQLUINTEGER *) ValuePtr)= SQL_FALSE;
		break;
		
	case SQL_ATTR_AUTOCOMMIT:
		*((SQLUINTEGER *)ValuePtr)= (dbc->auto_commit?
									SQL_AUTOCOMMIT_ON :
									SQL_AUTOCOMMIT_OFF);
		break;
		
	case SQL_ATTR_CONNECTION_DEAD:
		if (check_server_alive(dbc))
			*((SQLUINTEGER *) ValuePtr)= SQL_CD_TRUE;
		else
			*((SQLUINTEGER *) ValuePtr)= SQL_CD_FALSE;
		break;
		
	case SQL_ATTR_CONNECTION_TIMEOUT:
		*((SQLUINTEGER *) ValuePtr)= 0;
		break;
		
	case SQL_ATTR_CURRENT_CATALOG:
		*StringLengthPtr = 0;
		*(char*)ValuePtr = 0x0;
		//*StringLengthPtr=(SQLSMALLINT) (strncpy1((char*)ValuePtr,dbc->db_name,
		//	BufferLength) - (char*) ValuePtr);
		break;
		
	case SQL_ATTR_LOGIN_TIMEOUT:
		*((SQLUINTEGER *) ValuePtr)= dbc->login_timeout;
		break;
		
	case SQL_ATTR_ODBC_CURSORS:
		if ((dbc->flag & FLAG_FORWARD_CURSOR))	  
			*((SQLUINTEGER *) ValuePtr)= SQL_CUR_USE_ODBC;
		else 	  	  
			*((SQLUINTEGER *) ValuePtr)= SQL_CUR_USE_IF_NEEDED;
		break;	  
		
	case SQL_OPT_TRACE:
	case SQL_OPT_TRACEFILE:
	case SQL_QUIET_MODE:
	case SQL_TRANSLATE_DLL:
	case SQL_TRANSLATE_OPTION:
		{
			char buff[100];
			sprintf(buff,"Suppose to get this attribute '%d' through driver manager, not by the driver",Attribute);
			result = setError(hdbc,RHERR_01S02,buff,0);
			break;
		}
		
	case SQL_ATTR_PACKET_SIZE:
		*((SQLUINTEGER *) ValuePtr)= 64*1024;
		break;
		
	case SQL_ATTR_TXN_ISOLATION:
		*((int *) ValuePtr)= dbc->txn_isolation;
		break;
		
		/*
		3.x driver doesn't support any statement attributes
		at connection level, but to make sure all 2.x apps
		works fine...lets support it..nothing to loose..
		*/
	default:
		result=get_constmt_attr(2,dbc,&dbc->stmt_options,
			Attribute,ValuePtr,StringLengthPtr);
	}
	DBUG_RETURN_STATUS(result);
}

/*
@type    : myodbc3 internal
@purpose : sets the statement attributes
*/

static SQLRETURN setStmtAttr(SQLHSTMT   hstmt,
	int Attribute,
	SQLPOINTER ValuePtr,
	int StringLengthPtr)
{
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	SQLRETURN result=SQL_SUCCESS;
	STMT_OPTIONS *options = &stmt->stmt_options;
	DBUG_ENTER("SQLSetStmtAttr");
	DBUG_PRINT("enter",("Atrr: %d, ValuePtr: 0x%lx, StrLenPtr: %d",Attribute,ValuePtr,StringLengthPtr));
	
	switch(Attribute){
		
	case SQL_ATTR_CURSOR_SCROLLABLE:
		if ((SQLUINTEGER)ValuePtr == SQL_NONSCROLLABLE && 
			options->cursor_type != SQL_CURSOR_FORWARD_ONLY)
			options->cursor_type = SQL_CURSOR_FORWARD_ONLY;   
		
		else if ((SQLUINTEGER)ValuePtr == SQL_SCROLLABLE && 
			options->cursor_type == SQL_CURSOR_FORWARD_ONLY)
			options->cursor_type = SQL_CURSOR_STATIC;
		break;
		
	case SQL_ATTR_AUTO_IPD:
		if ((SQLUINTEGER) ValuePtr != SQL_FALSE )
			DBUG_RETURN(set_error(hstmt,RHERR_01S02,
			"Option value changed to default auto ipd",0));
		break;
		
	case SQL_ATTR_PARAM_STATUS_PTR: /* need to support this ....*/
		options->paramStatusPtr = (SQLUSMALLINT *)ValuePtr;
		break;
		
	case SQL_ATTR_PARAMS_PROCESSED_PTR: /* need to support this ....*/
		options->paramProcessedPtr = (SQLUINTEGER *)ValuePtr;
		break;
		
	case SQL_ATTR_PARAMSET_SIZE:
		if ((SQLUINTEGER)ValuePtr != 1 )
			DBUG_RETURN(set_error(hstmt,RHERR_01S02,
			"Option value changed to default parameter size",0));
		break;
		
	case SQL_ATTR_ROW_ARRAY_SIZE:
	case SQL_ROWSET_SIZE:
		options->rows_in_set = (SQLUINTEGER)ValuePtr;
		break;
		
	case SQL_ATTR_ROW_NUMBER:
		DBUG_RETURN(set_error(hstmt,RHERR_S1000,
			"Trying to set read-only attribute",0));
		break;
		
	case SQL_ATTR_ROW_OPERATION_PTR:
		options->rowOperationPtr = (SQLUSMALLINT *)ValuePtr;
		break;
		
	case SQL_ATTR_ROW_STATUS_PTR:
		options->rowStatusPtr = (SQLUSMALLINT *)ValuePtr;
		break;
		
	case SQL_ATTR_ROWS_FETCHED_PTR:
		options->rowsFetchedPtr = (SQLUINTEGER *)ValuePtr;
		break;
		
	case SQL_ATTR_SIMULATE_CURSOR:
		options->simulateCursor = (SQLUINTEGER)ValuePtr;
		break;
		
		/*
		3.x driver doesn't support any statement attributes
		at connection level, but to make sure all 2.x apps
		works fine...lets support it..nothing to loose..
		*/
	default:
		result = set_constmt_attr(3,hstmt,options,
			Attribute,ValuePtr);
	}
	DBUG_RETURN_STATUS(result);
}

/*
@type    : myodbc3 internal
@purpose : returns the statement attribute values
*/

static SQLRETURN getStmtAttr(SQLHSTMT   hstmt,
	int Attribute,
	SQLPOINTER ValuePtr,
	int BufferLength,
	int *StringLengthPtr)
{
	SQLRETURN result=SQL_SUCCESS;
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	STMT_OPTIONS *options = &stmt->stmt_options;
	SQLPOINTER vparam;
	int str_len;
	DBUG_ENTER("SQLGetStmtAttr");
	
	if (!ValuePtr)
		ValuePtr = &vparam;
	
	if (!StringLengthPtr)
		StringLengthPtr = &str_len;
	
	switch(Attribute) {
		
	case SQL_ATTR_CURSOR_SCROLLABLE:
		if (options->cursor_type == SQL_CURSOR_FORWARD_ONLY)
			*(SQLUINTEGER*)ValuePtr = SQL_NONSCROLLABLE;
		else
			*(SQLUINTEGER*)ValuePtr = SQL_SCROLLABLE;
		break;
		
	case SQL_ATTR_AUTO_IPD:
		*(SQLUINTEGER *)ValuePtr = SQL_FALSE;
		break;
		
	case SQL_ATTR_PARAM_STATUS_PTR: /* need to support this ....*/
		ValuePtr = (SQLUSMALLINT *)options->paramStatusPtr;
		break;
		
	case SQL_ATTR_PARAMS_PROCESSED_PTR: /* need to support this ....*/
		ValuePtr = (SQLUSMALLINT *)options->paramProcessedPtr;
		break;
		
	case SQL_ATTR_PARAMSET_SIZE:
		*(SQLUINTEGER *)ValuePtr = 1;
		break;
		
	case SQL_ATTR_ROW_ARRAY_SIZE:
		*(SQLUINTEGER *)ValuePtr = options->rows_in_set;
		break;
		
	case SQL_ATTR_ROW_NUMBER:
		*(SQLUINTEGER *)ValuePtr = stmt->curr_row_no+1;
		break;
		
	case SQL_ATTR_ROW_OPERATION_PTR: /* need to support this ....*/
		ValuePtr = (SQLUSMALLINT *)options->rowOperationPtr;
		break;
		
	case SQL_ATTR_ROW_STATUS_PTR:
		ValuePtr = (SQLUSMALLINT *)options->rowStatusPtr;
		break;
		
	case SQL_ATTR_ROWS_FETCHED_PTR:
		ValuePtr = (SQLUINTEGER *)options->rowsFetchedPtr;
		break;
		
	case SQL_ATTR_SIMULATE_CURSOR:
		ValuePtr = (SQLUSMALLINT *)options->simulateCursor;
		break;
		
		/*
		To make iODBC and MS ODBC DM to work, return the following cases
		as success, by just allocating...else
		- iODBC is hanging at the time of stmt allocation
		- MS ODB DM is crashing at the time of stmt allocation
		*/
		// case SQL_ATTR_APP_ROW_DESC:
		//   *(SQLPOINTER *)ValuePtr = &stmt->ard;
		//   *StringLengthPtr = sizeof(SQL_IS_POINTER);
		//   break;
		
		// case SQL_ATTR_IMP_ROW_DESC:
		//   *(SQLPOINTER *)ValuePtr = &stmt->ird;
		//   *StringLengthPtr = sizeof(SQL_IS_POINTER);
		//   break;
		
		// case SQL_ATTR_APP_PARAM_DESC:
		//   *(SQLPOINTER *)ValuePtr = &stmt->apd;
		//   *StringLengthPtr = sizeof(SQL_IS_POINTER);
		//   break;
		
		// case SQL_ATTR_IMP_PARAM_DESC:
		//   *(SQLPOINTER *)ValuePtr = &stmt->ipd;
		//   *StringLengthPtr = sizeof(SQL_IS_POINTER);
		//   break;
		
		/*
		3.x driver doesn't support any statement attributes
		at connection level, but to make sure all 2.x apps
		works fine...lets support it..nothing to loose..
		*/
	default:
		result=get_constmt_attr(3,hstmt,options,
			Attribute,ValuePtr,
			StringLengthPtr);
	}
	
	DBUG_RETURN_STATUS(result);
}

/*
@type    : ODBC 1.0 API
@purpose : sets the connection options
*/

SQLRETURN SQL_API SQLSetConnectOption(SQLHDBC hdbc, SQLUSMALLINT fOption,
									  SQLUINTEGER  vParam)
{
	SQLRETURN result=SQL_SUCCESS;
	DBUG_ENTER("SQLSetConnectOption");
	
	result=setConnAttr(hdbc,fOption,(SQLPOINTER)vParam,SQL_NTS);
	DBUG_RETURN_STATUS(result);
}

/*
@type    : ODBC 1.0 API
@purpose : returns the connection options
*/

SQLRETURN SQL_API SQLGetConnectOption(SQLHDBC hdbc,SQLUSMALLINT fOption,
									  SQLPOINTER vParam)
{
	SQLRETURN result=SQL_SUCCESS;
	DBUG_ENTER("SQLGetConnectOption");
	
	result=getConnAttr(hdbc,fOption,vParam,SQL_NTS,(int *)NULL);
	DBUG_RETURN_STATUS(result);
}

/*
@type    : ODBC 1.0 API
@purpose : sets the statement options
*/

SQLRETURN SQL_API SQLSetStmtOption(SQLHSTMT hstmt,SQLUSMALLINT fOption,
								   SQLUINTEGER vParam)
{
	SQLRETURN result=SQL_SUCCESS;
	DBUG_ENTER("SQLSetStmtOption");
	
	result=setStmtAttr(hstmt,fOption,(SQLPOINTER)vParam,SQL_NTS);
	DBUG_RETURN_STATUS(result);
}

/*
@type    : ODBC 1.0 API
@purpose : returns the statement options
*/

SQLRETURN SQL_API SQLGetStmtOption(SQLHSTMT hstmt,SQLUSMALLINT fOption,
								   SQLPOINTER vParam)
{
	SQLRETURN result=SQL_SUCCESS;
	DBUG_ENTER("SQLGetStmtOption");
	
	result=getStmtAttr(hstmt,fOption,vParam,SQL_NTS,(int *)NULL);
	DBUG_RETURN_STATUS(result);
}

/*
@type    : ODBC 3.0 API
@purpose : sets the environment attributes
*/

SQLRETURN SQL_API SQLSetEnvAttr(SQLHENV    henv,
								int Attribute,
								SQLPOINTER ValuePtr,
								int StringLength)
{
	DBUG_ENTER("SQLSetEnvAttr");
		
	switch (Attribute) {
		
	case SQL_ATTR_ODBC_VERSION:
		((SQLENV*)henv)->odbc_ver = (int)ValuePtr;
		break;
		
	case SQL_ATTR_OUTPUT_NTS:
		if ((int)ValuePtr == SQL_TRUE )
			break;
		
	default:
		DBUG_RETURN(setError(henv,RHERR_S1C00,NULL,0));
	}
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : ODBC 3.0 API
@purpose : returns the environment attributes
*/

SQLRETURN SQL_API SQLGetEnvAttr(SQLHENV    henv,
								int Attribute,
								SQLPOINTER ValuePtr,
								int BufferLength,
								int *StringLengthPtr)
{
	DBUG_ENTER("SQLGetEnvAttr");
	DBUG_PRINT("enter",("Atrr: %d",Attribute));
	
	switch(Attribute) {
		
	case SQL_ATTR_ODBC_VERSION:
		*(int*)ValuePtr = ((SQLENV FAR *)henv)->odbc_ver;
		break;
		
	case SQL_ATTR_OUTPUT_NTS:
		*((int*)ValuePtr) = SQL_TRUE;
		break;
		
	default:
		DBUG_RETURN(setError(henv,RHERR_S1C00,NULL,0));
	}
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : ODBC 3.0 API
@purpose : sets the connection attributes
*/

SQLRETURN SQL_API SQLSetConnectAttr(SQLHDBC hdbc,
									int Attribute,
									SQLPOINTER ValuePtr,
									int StringLength)
{
	return setConnAttr(hdbc,Attribute,
		      ValuePtr,StringLength);
}

/*
@type    : ODBC 3.0 API
@purpose : returns the connection attribute values
*/

SQLRETURN SQL_API SQLGetConnectAttr(SQLHDBC hdbc,
									int Attribute,
									SQLPOINTER ValuePtr,
									int BufferLength,
									int *StringLengthPtr)
{
	return getConnAttr(hdbc,Attribute,
		      ValuePtr,BufferLength,
			  StringLengthPtr);
}

/*
@type    : ODBC 3.0 API
@purpose : sets the statement attributes
*/

SQLRETURN SQL_API SQLSetStmtAttr(SQLHSTMT   hstmt,
								 int Attribute,
								 SQLPOINTER ValuePtr,
								 int StringLength)
{
	
	return setStmtAttr(hstmt, Attribute, ValuePtr, StringLength);
}

/*
@type    : ODBC 3.0 API
@purpose : returns the statement attribute values
*/
SQLRETURN SQL_API SQLGetStmtAttr(SQLHSTMT   hstmt,
								 int Attribute,
								 SQLPOINTER ValuePtr,
								 int BufferLength,
								 int *StringLengthPtr)
{
	return getStmtAttr(hstmt,Attribute, ValuePtr,BufferLength,
		StringLengthPtr);
}
