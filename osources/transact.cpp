/*************************************************************************** 
* The following ODBC APIs are implemented in this file:                   *
*                                                                         *
*   SQLEndTran          (ISO 92)                                          *
*   SQLTransact         (ODBC, Deprecated)                                *
*                                                                         *
****************************************************************************/

#include "odbc.h"

/*
@type    : internal
@purpose : do the transaction at the connection level
*/

SQLRETURN rh_transact(SQLHDBC hdbc,SQLSMALLINT CompletionType)
{
	SQLDBC *p_dbc = (SQLDBC *)hdbc;
	switch(CompletionType) 
	{
	case SQL_COMMIT:
		return KCITransCommit(p_dbc, &(p_dbc->error), 0);
	case SQL_ROLLBACK:
		return KCITransRollback(p_dbc, &(p_dbc->error), 0);
	default:
		return setError(hdbc,RHERR_S1012,NULL,0);
	}
	return(SQL_SUCCESS);
}

/*
@type    : ODBC 3.0
@purpose : requests a commit or rollback operation for all active
operations on all statements associated with a connection
*/

SQLRETURN SQL_API
SQLEndTran(SQLSMALLINT HandleType,
		   SQLHANDLE   Handle,
		   SQLSMALLINT CompletionType)
{
	SQLRETURN result=SQL_SUCCESS;
	DBUG_ENTER("SQLEndTran");
	
	switch (HandleType) {
	case SQL_HANDLE_ENV:
    /*
	TODO : The simple logic could be to call rh_transact
	with all connection handles in the current environment,
	but as majority of the applications doesn't use this
	option, lets wait for the request or any bug-report
	*/
		break;
		
	case SQL_HANDLE_DBC:
		result=rh_transact(Handle,CompletionType);
		break;	
	default:
		result=SQL_ERROR;
		setError(Handle,RHERR_S1092,NULL,0);
		break;
	}
	DBUG_RETURN_STATUS(result);
}

/*
@type    : ODBC 1.0
@purpose : Requests a commit or rollback operation for all active
operations on all statement handles (hstmts) associated
with a connection or for all connections associated
with the environment handle, henv
*/

SQLRETURN SQL_API SQLTransact(SQLHENV henv,SQLHDBC hdbc,SQLUSMALLINT fType)
{
	SQLRETURN result=SQL_SUCCESS;
	DBUG_ENTER("SQLTransact");	
	if (hdbc)
		result = rh_transact(hdbc,fType);
	DBUG_RETURN_STATUS(result);
}
