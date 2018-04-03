#include <sys/stat.h>
#include <locale.h>
#include "fcntl.h"
#include "odbc.h"
#include "io.h"

int   h_file = 0;
char *default_locale = NULL;
char *decimal_point = NULL;
char *thousands_sep = NULL;
int  decimal_point_length;
int  thousands_sep_length;

void initODBCEnv()
{
	int		i = 1;
	struct  lconv *tmp;
#ifdef WIN32
	unsigned short wVersionRequested;
	static bool init_afx = false;
	WSADATA wsaData;
#endif

#ifdef WIN32
	if (!init_afx)
	{
		int err;
		init_afx = TRUE;
		wVersionRequested = MAKEWORD(2, 2);
		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0)
			return;
	}
#endif

	InitFieldsInfos();
	init_getfunctions();
	default_locale = strdup(setlocale(LC_NUMERIC, NullS));
	setlocale(LC_NUMERIC, "");
	tmp = localeconv();
	decimal_point = strdup(tmp->decimal_point);
	decimal_point_length = strlen(decimal_point);
	thousands_sep = strdup(tmp->thousands_sep);
	thousands_sep_length = strlen(thousands_sep);
	setlocale(LC_NUMERIC, default_locale);
}

#ifdef WIN32
HANDLE  s_hModule;
bool APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	s_hModule = hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		initODBCEnv();
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
void __declspec(dllexport) FAR PASCAL LoadByOrdinal(void);
/* Entry point to cause DM to load using ordinals */
void __declspec(dllexport) FAR PASCAL LoadByOrdinal(void) {}
#endif

/*
@type    : ODBC 1.0 API
@purpose : to allocate the environment handle
*/
SQLRETURN SQL_API SQLAllocEnv(SQLHENV FAR * phenv)
{
	DBUG_ENTER("SQLAllocEnv");
	initODBCEnv();
#if !defined(DBUG_OFF)
	remove("./odbc_dbg.log");
	if (h_file == 0)
		h_file = _open("./odbc_dbg.txt", O_BINARY | 
		O_CREAT | O_RDWR, _S_IREAD | _S_IWRITE);
#endif
	return KCIEnvInit((SQLENV**)&phenv, 0, _1M, NULL);
}

/*
@type    : ODBC 1.0 API
@purpose : to free the environment handle
*/

SQLRETURN SQL_API SQLFreeEnv(SQLHENV henv)
{
	DBUG_ENTER("SQLFreeEnv");
	return KCIHandleFree(henv, 0);
}

/*
@type    : ODBC 1.0 API
@purpose : to allocate the connection handle and to
maintain the connection list
*/

SQLRETURN SQL_API SQLAllocConnect(SQLHENV henv, SQLHDBC FAR *phdbc)
{
	DBUG_ENTER("SQLAllocConnect");
	return KCIHandleAlloc(henv, phdbc, 0, 0, 0);
}

/*
@type    : ODBC 1.0 API
@purpose : to allocate the connection handle and to
maintain the connection list
*/
SQLRETURN SQL_API SQLFreeConnect(SQLHDBC hdbc)
{
	return KCIHandleFree(hdbc, 0);
}

/*
@type    : ODBC 1.0 APO
@purpose : allocates the statement handle
*/
SQLRETURN SQL_API SQLAllocStmt(SQLHDBC hdbc,SQLHSTMT FAR *phstmt)
{
	DBUG_ENTER("SQLAllocStmt");
	SQLSTMT *p_stmt = NULL;
	CHECK(KCIHandleAlloc(hdbc, (void**)&p_stmt, 0, 0, 0));
	p_stmt->p_svctx = (SQLDBC*)hdbc;
	return SQL_SUCCESS;
}

/*
@type    : myodbc3 internal
@purpose : stops processing associated with a specific statement,
closes any open cursors associated with the statement,
discards pending results, or, optionally, frees all
resources associated with the statement handle
*/
SQLRETURN SQL_API SQLFreeStmt(SQLHSTMT hstmt,SQLUSMALLINT fOption)
{
	return KCIStmtPartFree((SQLSTMT*)hstmt,fOption);
}

/*
@type    : ODBC 3.0 API
@purpose : allocates an environment, connection, statement, or
descriptor handle
*/

SQLRETURN SQL_API SQLAllocHandle(SQLSMALLINT HandleType,
								 SQLHANDLE   InputHandle,
								 SQLHANDLE   *OutputHandlePtr)
{
	SQLRETURN error = SQL_ERROR;
	switch (HandleType) 
	{
	case SQL_HANDLE_ENV:
		error = SQLAllocEnv(OutputHandlePtr);
		break;
		
	case SQL_HANDLE_DBC:
		error = SQLAllocConnect(InputHandle,OutputHandlePtr);
		break;
		
	case SQL_HANDLE_STMT:
		error = SQLAllocStmt(InputHandle,OutputHandlePtr);
		break;
		
	default:
		return(setError(HandleType,InputHandle,RHERR_S1C00,NULL,0));
	}
	return(error);
}

/*
@type    : ODBC 3.0 API
@purpose : frees resources associated with a specific environment,
connection, statement, or descriptor handle
*/

SQLRETURN SQL_API SQLFreeHandle(SQLSMALLINT handleType,
								SQLHANDLE   handle)
{
	return KCIHandleFree(handle, 0);
}

