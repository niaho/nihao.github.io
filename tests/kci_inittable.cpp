#include <stdio.h>
#include <process.h>
#include <string.h>
#include "kci_defs.h"
extern "C"{
#include "ociap.h"
};

static void showError(OCIError *p_err);
void createTable(int index,OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);
void initData(int index, OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);
void connectDB(OCISvcCtx  *p_svcctx, OCIServer  *p_serv, OCISession *p_sess, OCIError   *p_err);

static void showError(OCIError *p_err)
{
	sb4      err_code;
	OraText  err_info[256];

	OCIErrorGet(p_err, 1, NULL, &err_code, err_info, 254, OCI_HTYPE_ERROR);
	printf("ErrCode:%d ErrInfo:%s", err_code, err_info);
}

void connectDB(OCISvcCtx  *p_svcctx, OCIServer  *p_serv, OCISession *p_sess, OCIError   *p_err)
{
	sword      ret = 0;
	/**********设置句柄***********/
	char *link_str = "localhost:5139/system";
	ub4  str_len = (ub4)strlen(link_str);

	ret = OCIServerAttach(p_serv, p_err, (oratext*)link_str, str_len, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	OCIAttrSet(p_sess, OCI_HTYPE_SESSION, "SYSDBA", 6, OCI_ATTR_USERNAME, p_err);
	OCIAttrSet(p_sess, OCI_HTYPE_SESSION, "SYSDBA", 6, OCI_ATTR_PASSWORD, p_err);
	OCIAttrSet(p_svcctx, OCI_HTYPE_SVCCTX, p_serv, 0, OCI_ATTR_SERVER, p_err);
	/**********开始会话***********/
	ret = OCISessionBegin(p_svcctx, p_err, p_sess, OCI_CRED_RDBMS, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	ret = OCIAttrSet(p_svcctx, OCI_HTYPE_SVCCTX, p_sess, 0, OCI_ATTR_SESSION, p_err);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

}
void createTable(int index, OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{

	char tableName[32];
	char tableSQL[128];
	memset(tableName, 0, sizeof(tableName));
	memset(tableSQL, 0, sizeof(tableSQL));
	switch (index)
	{
	case 0:
		strcpy(tableName, "");
		strcpy(tableSQL, "");
		break;
	default:
		strcpy(tableName, "TEST1");
		strcpy(tableSQL, "CREATE TABLE  TEST1 ( id     INTEGER primary key,v1     INTEGER,	v2     BIGINT,v3     FLOAT,v4     DOUBLE,v5     TINYINT,v6     SMALLINT,v7     number(9,4))");
		break;
	}
	OraText sql = (OraText )"";
	OCIDefine *defhp1;
	sprintf((char *)sql, "select count(*) from user_tables where table_name = '%s'", tableName);
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = OCIStmtPrepare(p_stmt, p_err, &sql, sql_len, OCI_NTV_SYNTAX, OCI_DEFAULT);
	/******执行******************/
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
	/* Define output buffers */
	long long num = 0;
	ret = OCIDefineByPos(p_stmt, &defhp1, p_err, 1,
		(void *)&num, (sb4) sizeof(num),
		SQLT_LNG, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	/* Fetch one row */
	int num_rows = 1;
	ret = OCIStmtFetch(p_stmt, p_err, num_rows, OCI_FETCH_NEXT,
		OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	if (num > 0)
	{
		sprintf((char *)sql, "DROP TABLE  %s", tableName);
		ub4  sql_len2 = (ub4)strlen((char*)sql);
		OCIStmtPrepare(p_stmt, p_err, &sql, sql_len2, 0, OCI_DEFAULT);
		OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
	}
	ub4  sql_len3 = (ub4)strlen(tableSQL);
	OCIStmtPrepare(p_stmt, p_err, (OraText *)tableSQL, sql_len3, 0, OCI_DEFAULT);
	OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}

void initData(int index, OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	char tableInit[32];
	char tableSQL[2][128];
	memset(tableInit, 0, sizeof(tableInit));
	memset(tableSQL[0], 0, sizeof(tableSQL[0]));
	memset(tableSQL[1], 0, sizeof(tableSQL[1]));
	switch (index)
	{
	case 0:
		strcpy(tableInit, "");
		strcpy(tableSQL[0], "");
		strcpy(tableSQL[1], "");
		break;
	default:
		strcpy(tableInit, "delete from TEST1");
		strcpy(tableSQL[0], "insert into TEST1 values(1,2,3,4,5,6,7,8)");
		strcpy(tableSQL[0], "insert into TEST1 values(2,3,4,5,6,7,8,9)");
		break;
	}
	ub4  sql_len = (ub4)strlen(tableInit);
	sword ret = OCIStmtPrepare(p_stmt, p_err, (OraText *)tableInit, sql_len, 0, OCI_DEFAULT);
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);

	sql_len = (ub4)strlen(tableSQL[0]);
	ret = OCIStmtPrepare(p_stmt, p_err, (OraText *)tableSQL[0], sql_len, 0, OCI_DEFAULT);
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
	sql_len = (ub4)strlen(tableSQL[1]);
	ret = OCIStmtPrepare(p_stmt, p_err, (OraText *)tableSQL[1], sql_len, 0, OCI_DEFAULT);
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}
