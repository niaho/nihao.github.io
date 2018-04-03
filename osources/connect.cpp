#include "odbc.h"
#include "dialogs.h"
#include "kci_string.h"

extern const char ODBC_INI[];  /* ODBC initialization file */
extern const char INI_KDESC[]; /* Data source description */
extern const char INI_KDB[];
extern const char INI_KSERVER[];
extern const char INI_KUSER[];   /* Second option */
extern const char INI_KPASSWORD[];
extern const char INI_KPORT[];
extern const char INI_AUTO_COMMIT[];
extern const char INI_USE_SSL[];
extern const char INI_ISO_LEVEL[];
extern const char INI_CHAR_SET[];
extern const char INI_TIME_ZONE[];
extern HINSTANCE NEAR s_hModule;

/*
@type    : ODBC 1.o API
@purpose : to connect to redhorse server
*/
SQLRETURN SQL_API
SQLConnect(	SQLHDBC     hdbc, 
		   SQLCHAR FAR *szDSN,
		   SQLSMALLINT	cbDSN,
		   SQLCHAR FAR *szUID,
		   SQLSMALLINT  cbUID,
		   SQLCHAR FAR *szAuthStr,
		   SQLSMALLINT  cbAuthStr)
{
	SQLDBC *p_conn;
	SQLENV *p_env;
	Serv   *p_serv = NULL;
	Sess   *p_sess = NULL;
	char   dsn[MAX_NAME_LEN+1],
		   db_name[NAME_LEN+1],
		   host[64],
		   user[32],
		   passwd[64],
		   char_set[32],
		   time_zone[32],
		   iso_level[16],
		   b_val[8];
	char   port[10];
	char   conn_str[512];
	char   auto_commit[8];
	char   *str_ptr = conn_str;
	int    conn_slen = 0;
	int    opt_num = 0;
	bool   use_ssl=false;	

	p_conn = (SQLDBC*)hdbc;
	p_env = p_conn->p_env;
	memset(dsn,0x0,MAX_NAME_LEN+1);
	strncpy(dsn,(char*)szDSN,cbDSN);
	
	/*从数据源的记载中取出主机地址、端口、字符集等信息*/ 
	SQLGetPrivateProfileString(dsn,INI_KUSER,
			     "", user, sizeof(user),"ODBC.INI");
	SQLGetPrivateProfileString(dsn,INI_KPASSWORD,
			     "", passwd, sizeof(passwd),"ODBC.INI");
	SQLGetPrivateProfileString(dsn,INI_KSERVER,
			     "localhost", host, sizeof(host),"ODBC.INI");
	SQLGetPrivateProfileString(dsn,INI_KDB,
			     dsn, db_name, sizeof(db_name),"ODBC.INI");
	SQLGetPrivateProfileString(dsn,INI_KPORT,
			     "0", port, sizeof(port),"ODBC.INI");
	SQLGetPrivateProfileString(dsn,INI_CHAR_SET,
			     "0", char_set, sizeof(char_set),"ODBC.INI");
	SQLGetPrivateProfileString(dsn,INI_TIME_ZONE,
			     "0", time_zone, sizeof(time_zone),"ODBC.INI");
	SQLGetPrivateProfileString(dsn,INI_ISO_LEVEL,
			     "0", iso_level, sizeof(iso_level),"ODBC.INI");
	SQLGetPrivateProfileString(dsn,INI_AUTO_COMMIT,
			     "0", auto_commit, sizeof(auto_commit),"ODBC.INI");
	SQLGetPrivateProfileString(dsn,INI_USE_SSL,
			     "0", b_val, sizeof(b_val),"ODBC.INI");
	use_ssl = (!strcmp(b_val,"true") || !strcmp(b_val,"TRUE"));

	if(use_ssl)
		sprintf(conn_str, "!%s:%s/%s", host, port, db_name);
	else
		sprintf(conn_str, "%s:%s/%s", host, port, db_name);
	str_ptr += astrlen(str_ptr);
	if (char_set[0] != '0' && char_set[0] != 0x0)
	{
		if (opt_num == 0)
			sprintf(str_ptr, "?CHAR_SET=%s", char_set);
		else
			sprintf(str_ptr, "&CHAR_SET=%s", char_set);
		str_ptr += astrlen(str_ptr);
	}
	if (time_zone[0] != '0' && time_zone[0] != 0x0)
	{
		if (opt_num == 0)
			sprintf(str_ptr, "?TIME_ZONE=%s", time_zone);
		else
			sprintf(str_ptr, "&TIME_ZONE=%s", time_zone);
		str_ptr += astrlen(str_ptr);
	}
	if (iso_level[0] != '0' && iso_level[0] != 0x0)
	{
		if (opt_num == 0)
			sprintf(str_ptr, "?ISO_LEVEL=%s", iso_level);
		else
			sprintf(str_ptr, "&ISO_LEVEL=%s", iso_level);
		str_ptr += astrlen(str_ptr);
	}
	if (auto_commit[0] != '0' && auto_commit[0] != 0x0)
	{
		if (opt_num == 0)
			sprintf(str_ptr, "?AUTO_COMMIT=%s", auto_commit);
		else
			sprintf(str_ptr, "&AUTO_COMMIT=%s", auto_commit);
		str_ptr += astrlen(str_ptr);
	}
	
	/*创建DBC*/
	KCIHandleAlloc(p_env, (void**)&p_serv, KCI_HTYPE_SERVER, 0, 0);
	KCIHandleAlloc(p_env, (void**)&p_sess, KCI_HTYPE_SESSION, 0, 0);
	KCIServerAttach(p_serv, &p_conn->error, (uchar*)conn_str, conn_slen, KCI_DEFAULT);
	KCIAttrSet(p_sess, KCI_HTYPE_SESSION, user, astrlen(user), KCI_ATTR_USERNAME, &p_conn->error);
	KCIAttrSet(p_sess, KCI_HTYPE_SESSION, passwd, astrlen(passwd), KCI_ATTR_PASSWORD, &p_conn->error);
	KCIAttrSet(p_conn, KCI_HTYPE_SVCCTX, p_serv, 0, KCI_ATTR_SERVER, &p_conn->error);
	KCIAttrSet(p_conn, KCI_HTYPE_SVCCTX, &p_sess, 0, KCI_ATTR_SESSION, &p_conn->error);
	CHECK(buildServer(p_serv, p_sess, &p_conn->error));
	return SQL_SUCCESS;
}

/*
@type    : ODBC 1.0 API
@purpose : is an alternative to SQLConnect. It supports data sources that
require more connection information than the three arguments in
SQLConnect, dialog boxes to prompt the user for all connection
information, and data sources that are not defined in the
system information.
*/

SQLRETURN SQL_API
SQLDriverConnect(SQLHDBC hdbc,
				 SQLHWND hwnd,
				 SQLCHAR FAR *szConnStrIn,
				 SQLSMALLINT cbConnStrIn,
				 SQLCHAR FAR *szConnStrOut,
				 SQLSMALLINT cbConnStrOutMax,
				 SQLSMALLINT FAR *pcbConnStrOut,
				 SQLUSMALLINT fDriverCompletion)
{
#ifndef _UNIX_
	short	iRet;
	bool    use_ssl = false;
	bool	fPrompt = false,
			maybe_prompt = false;
	SQLDBC  *p_conn = (SQLDBC*)hdbc;
	SQLENV  *p_env = p_conn->p_env;
	Serv    *p_serv = NULL;
	Sess    *p_sess = NULL;
	char    conn_str[1024];
	char    *str_ptr = conn_str;
	char    *host = NULL;
	char    *user = NULL;
	char    *passwd = NULL;
	char    *db_name = NULL;
	char    *port = NULL;
	char    *char_set = NULL;
	char    *time_zone = NULL;
	char    *auto_commit = NULL;
	char    *iso_level = NULL;
	int     conn_slen = 0;
	int     opt_num = 0;
	
	GLOBALHANDLE hglbAttr;
	LPSETUPDLG lpsetupdlg;

	DBUG_ENTER("SQLDriverConnect");
	DBUG_PRINT("enter",("fDriverCompletion: %d",fDriverCompletion));
	
	KCIHandleAlloc(p_env, (void**)&p_serv, KCI_HTYPE_SERVER, 0, 0);
	KCIHandleAlloc(p_env, (void**)&p_sess, KCI_HTYPE_SESSION, 0, 0);
	hglbAttr = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(SETUPDLG));
	if (!hglbAttr)
		DBUG_RETURN(false);  
	lpsetupdlg = (LPSETUPDLG)GlobalLock(hglbAttr);
	
	maybe_prompt=(fDriverCompletion == SQL_DRIVER_COMPLETE ||
		fDriverCompletion == SQL_DRIVER_COMPLETE_REQUIRED);
	if ((szConnStrIn == NULL) || (!cbConnStrIn) ||
		((cbConnStrIn == SQL_NTS) && (!szConnStrIn[0])))
	{
		DBUG_PRINT("info",("No connection arguments"));
		fPrompt = TRUE;
	}
	else
	{
		/* Check connection string for completeness. Prompt if not all params */
		ParseAttributes((char*)szConnStrIn, lpsetupdlg);
		set_attributes(lpsetupdlg);
		if (fDriverCompletion == SQL_DRIVER_PROMPT  ||
			(maybe_prompt &&
			(!lpsetupdlg->aAttr[KEY_SERVER].szAttr[0] ||
			!lpsetupdlg->aAttr[KEY_USER].szAttr[0])))
		{
			DBUG_PRINT("info",("flag: %d  dsn:  '%s'  server: '%s'  user: '%s'",
				flag,
				lpsetupdlg->aAttr[KEY_DSN].szAttr,
				lpsetupdlg->aAttr[KEY_SERVER].szAttr,
				lpsetupdlg->aAttr[KEY_USER].szAttr));
			fPrompt = TRUE;
		}
	}
	
retry:
	if (fPrompt)
	{
		iRet = DialogBoxParam(s_hModule, 
			MAKEINTRESOURCE(IDD_SETUP), 
			hwnd,
			(DLGPROC) DriverConnectProc,
			(LONG)(LPSTR) lpsetupdlg);
		if ((!iRet) || (iRet == -1))
		{
			GlobalUnlock(hglbAttr);
			GlobalFree(hglbAttr);
			DBUG_RETURN_STATUS(SQL_NO_DATA_FOUND);
		}
	}
	if (szConnStrOut)
	{
		char	buff[1024],
			    driver_dsn[255];
		int 	length; 
		ulong	tmp= ~0L;
		
		if (lpsetupdlg->aAttr[KEY_DSN].szAttr[0])
			sprintf(driver_dsn,"DSN=%s", lpsetupdlg->aAttr[KEY_DSN].szAttr);
		else
			sprintf(driver_dsn,"DRIVER={%s}",DRIVER_NAME);
		
		sprintf(buff, "%s;DESC=%s;DB=%s;Server=%s;User=%s;Password=%s;Port=%s;AutoCommit=%s;"
			"UseSSL=%s;CharSet=%s;TimeZone=%s;IsoLevel=%s;",driver_dsn,
            lpsetupdlg->aAttr[KEY_DESC].szAttr,
            lpsetupdlg->aAttr[KEY_DB].szAttr,
            lpsetupdlg->aAttr[KEY_SERVER].szAttr,
            lpsetupdlg->aAttr[KEY_USER].szAttr,
            lpsetupdlg->aAttr[KEY_PASSWORD].szAttr,
            lpsetupdlg->aAttr[KEY_PORT].szAttr,
			lpsetupdlg->aAttr[KEY_AUTO_COMMIT].szAttr,
			lpsetupdlg->aAttr[KEY_USE_SSL].szAttr,
			lpsetupdlg->aAttr[KEY_CHAR_SET].szAttr,
			lpsetupdlg->aAttr[KEY_TIME_ZONE].szAttr,
			lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr);

		copy_lresult(SQL_HANDLE_DBC,(SQLDBC FAR*) 0,szConnStrOut,cbConnStrOutMax,
			&length,buff, strlen(buff)+1,0L,0L,&tmp,0);
		if (pcbConnStrOut)
			*pcbConnStrOut=(SQLSMALLINT) length;
		DBUG_PRINT("info",("SQLDBC string out: %s",szConnStrOut));
	}
		
	/* If no DB, use DSN as database name */
	if (!lpsetupdlg->aAttr[KEY_DB].szAttr[0])
	{
		strcpy(lpsetupdlg->aAttr[KEY_DB].szAttr,
			lpsetupdlg->aAttr[KEY_DSN].szAttr);
	}
	host = lpsetupdlg->aAttr[KEY_SERVER].szAttr;
	db_name = lpsetupdlg->aAttr[KEY_DB].szAttr;
	user = lpsetupdlg->aAttr[KEY_USER].szAttr;
	passwd = lpsetupdlg->aAttr[KEY_PASSWORD].szAttr;
	port = lpsetupdlg->aAttr[KEY_PORT].szAttr;
	auto_commit = lpsetupdlg->aAttr[KEY_AUTO_COMMIT].szAttr;
	char_set = lpsetupdlg->aAttr[KEY_CHAR_SET].szAttr;
	time_zone = lpsetupdlg->aAttr[KEY_TIME_ZONE].szAttr;
	iso_level = lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr;
	use_ssl = !strcmp(lpsetupdlg->aAttr[KEY_USE_SSL].szAttr, "true") ||
		!strcmp(lpsetupdlg->aAttr[KEY_USE_SSL].szAttr, "TRUE");

	if (use_ssl)
		sprintf(conn_str, "!%s:%s/%s", host, port, db_name);
	else
		sprintf(conn_str, "%s:%s/%s", host, port, db_name);
	str_ptr += astrlen(str_ptr);
	if (char_set[0] != '0' && char_set[0] != 0x0)
	{
		if (opt_num == 0)
			sprintf(str_ptr, "?CHAR_SET=%s", char_set);
		else
			sprintf(str_ptr, "&CHAR_SET=%s", char_set);
		str_ptr += astrlen(str_ptr);
	}
	if (time_zone[0] != '0' && time_zone[0] != 0x0)
	{
		if (opt_num == 0)
			sprintf(str_ptr, "?TIME_ZONE=%s", time_zone);
		else
			sprintf(str_ptr, "&TIME_ZONE=%s", time_zone);
		str_ptr += astrlen(str_ptr);
	}
	if (iso_level[0] != '0' && iso_level[0] != 0x0)
	{
		if (opt_num == 0)
			sprintf(str_ptr, "?ISO_LEVEL=%s", iso_level);
		else
			sprintf(str_ptr, "&ISO_LEVEL=%s", iso_level);
		str_ptr += astrlen(str_ptr);
	}
	if (auto_commit[0] != '0' && auto_commit[0] != 0x0)
	{
		if (opt_num == 0)
			sprintf(str_ptr, "?AUTO_COMMIT=%s", auto_commit);
		else
			sprintf(str_ptr, "&AUTO_COMMIT=%s", auto_commit);
		str_ptr += astrlen(str_ptr);
	}

	/*创建DBC*/
	KCIServerAttach(p_serv, &p_conn->error, (uchar*)conn_str, conn_slen, KCI_DEFAULT);
	KCIAttrSet(p_sess, KCI_HTYPE_SESSION, user, astrlen(user), KCI_ATTR_USERNAME, &p_conn->error);
	KCIAttrSet(p_sess, KCI_HTYPE_SESSION, passwd, astrlen(passwd), KCI_ATTR_PASSWORD, &p_conn->error);
	KCIAttrSet(p_conn, KCI_HTYPE_SVCCTX, p_serv, 0, KCI_ATTR_SERVER, &p_conn->error);
	KCIAttrSet(p_conn, KCI_HTYPE_SVCCTX, &p_sess, 0, KCI_ATTR_SESSION, &p_conn->error);
	if (buildServer(p_serv, p_sess, &p_conn->error) != SQL_SUCCESS)
		goto retry;
	return SQL_SUCCESS;
#endif /* IS NOT UNIX */
}

/*
@type    : ODBC 1.0 API
@purpose : supports an iterative method of discovering and enumerating
the attributes and attribute values required to connect to a
data source
*/

SQLRETURN SQL_API
SQLBrowseConnect(SQLHDBC hdbc,SQLCHAR FAR *szConnStrIn,
				 SQLSMALLINT cbConnStrIn,
				 SQLCHAR FAR *szConnStrOut,
				 SQLSMALLINT cbConnStrOutMax,
				 SQLSMALLINT FAR *pcbConnStrOut)
{
	DBUG_ENTER("SQLBrowseConnect");
	DBUG_RETURN(setError(SQL_HANDLE_DBC,hdbc,RHERR_S1000,
			     "Driver Does not support this API",0));
}


/*
@type    : ODBC 1.0 API
@purpose : closes the connection associated with a specific connection handle
*/

SQLRETURN SQL_API SQLDisconnect(SQLHDBC hdbc)
{
	return SQL_SUCCESS;
}
