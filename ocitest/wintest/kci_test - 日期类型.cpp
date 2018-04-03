#include <stdio.h>
#include <process.h>
#include <string.h>
#include "kci_defs.h"
extern "C"{
#include "ociap.h"
};

static void showError(OCIError *p_err);
void createTestTable(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);
void insertTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);
void insertTransTestData(OCISvcCtx  *p_conn,  OCIError   *p_err);
void selectTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);


int main(int argc, char *argv[])
{
	sword      ret = 0;
	OCIEnv     *p_env = NULL;
	OCISvcCtx  *p_conn = NULL;
	OCIServer  *p_serv = NULL;
	OCISession *p_sess = NULL;	
	OCIStmt    *p_stmt = NULL;
	OCIError   *p_err = NULL;
	

	/**********创建环境,分配句柄**********/
	ret = OCIEnvCreate(&p_env, OCI_OBJECT, 0, 0, 0, 0, 0, 0);
	if (ret == OCI_ERROR)
	{
		printf("create env failed\r\n");
		exit(1);
	}

	ret = OCIHandleAlloc(p_env, (void**)&p_conn, OCI_HTYPE_SVCCTX, 0, 0);
	if (ret == OCI_ERROR)
	{
		printf("alloc server conctx failed\r\n");
		exit(1);
	}

	ret = OCIHandleAlloc(p_env, (void**)&p_serv, OCI_HTYPE_SERVER, 0, 0);
	if (ret == OCI_ERROR)
	{
		printf("alloc server failed\r\n");
		exit(1);
	}

	ret = OCIHandleAlloc(p_env, (void**)&p_sess, OCI_HTYPE_SESSION, 0, 0);
	if (ret == OCI_ERROR)
	{
		printf("alloc session failed\r\n");
		exit(1);
	}

	ret = OCIHandleAlloc(p_env, (void**)&p_err, OCI_HTYPE_SERVER, 0, 0);
	if (ret == OCI_ERROR)
	{
		printf("alloc server failed\r\n");
		exit(1);
	}
		

	ret = OCIHandleAlloc(p_env, (void**)&p_stmt, OCI_HTYPE_STMT, 0, 0);
	if (ret == OCI_ERROR)
	{
		printf("alloc statement failed\r\n");
		exit(1);
	}

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
	OCIAttrSet(p_conn, OCI_HTYPE_SVCCTX, p_serv, 0, OCI_ATTR_SERVER, p_err);	
	/**********开始会话***********/
	ret = OCISessionBegin(p_conn, p_err, p_sess, OCI_CRED_RDBMS, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	ret = OCIAttrSet(p_conn, OCI_HTYPE_SVCCTX, &p_sess, 0, OCI_ATTR_SESSION, p_err);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	
	/*******准备语句***************/
	/////创建表结构
	createTestTable(p_conn, p_stmt, p_err);	
	/////插入数据
	insertTestData(p_conn,p_stmt,p_err);	
	/////查询数据
	selectTestData(p_conn, p_stmt, p_err);
	/////
	/* release the statement handle */
	ret = OCIStmtRelease(p_stmt, p_err,
		(OraText *)NULL, 0, OCI_DEFAULT);

}

static void showError(OCIError *p_err)
{
	sb4      err_code;
	OraText  err_info[256];

	OCIErrorGet(p_err, 1, NULL, &err_code, err_info, 254, OCI_HTYPE_ERROR);
	printf("ErrCode:%d ErrInfo:%s", err_code, err_info);
}

void createTestTable(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	/*
	测试前创建表test1:
	CREATE TABLE  test1 (
    id     INTEGER primary key,
    v1     DATE,
	v2     DATETIME,
	v3     DATETIME WITH TIME ZONE,
	v4     INTERVAL YEAR TO MONTH,
	v5     INTERVAL DAY TO SECOND
	*/
	/*
	DATE 日期数据类型格式’YYYY-MM-DD’
	DATETIME 包含年月日时间数据类型格式’YYYY-MM-DD HH:MI:SS’
	DATETIME WITH TIME ZONE 包含年月日时区时间数据类型
	格式’YYYY-MM-DD HH:MI:SS TZH:TZM’
	TIME 不包含年月日时间数据类型格式‘HH24:MI:SS’
	TIME WITH TIME ZONE 不包含年月日但包含时区时间数据类型
	格式‘HH24:MI:SS TZH:TZM’
	INTERVAL 数据类型用来存储两个时间戳之间的时间间隔。可以指定YEARS AND
	MONTHS，或者DAYS,HOURS,MINUTS,SECONDS 之间的间隔。
	INTERVAL {YEAR | MONTH| DAY| HOUR|MINUTE|SECOND} [(PRECISION)][TO {MONTH|HOUR|MINUTE|SECOND}]
	INTERVAL YEAR
	INTERVAL MONTH
	INTERVAL DAY
	INTERVAL HOUR
	INTERVAL MINUTE
	INTERVAL SECOND
	INTERVAL YEAR TO MONTH
	INTERVAL DAY TO HOUR
	INTERVAL DAY TO MINUTE
	INTERVAL DAY TO SECOND
	INTERVAL HOUR TO MINUTE
	INTERVAL HOUR TO SECOND
	INTERVAL MINUTE TO SECOND

	*/
	OCIDefine *defhp1;
	OraText *sql = (OraText*)"select count(*) from user_tables where table_name = 'TEST1'";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, OCI_NTV_SYNTAX, OCI_DEFAULT);
	/******执行******************/
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
	/* Define output buffers */
	long long num =0 ;
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
		OraText *sql2 = (OraText*)"DROP TABLE TEST1";
		ub4  sql_len2 = (ub4)strlen((char*)sql2);
		OCIStmtPrepare(p_stmt, p_err, sql2, sql_len2, 0, OCI_DEFAULT);
		OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
	}

	OraText *sql3 = (OraText*)"CREATE TABLE  TEST1 ( id     INTEGER primary key,v1     DATE,v2     DATETIME,v3     DATETIME WITH TIME ZONE,v4     INTERVAL YEAR TO MONTH,v5     INTERVAL DAY TO SECOND)";
	ub4  sql_len3 = (ub4)strlen((char*)sql3);
	OCIStmtPrepare(p_stmt, p_err, sql3, sql_len3, 0, OCI_DEFAULT);
	OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}

void insertTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OraText *sql = (OraText*)"insert into TEST1 values(:1,:2,:3,:4,:5,:6)";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);


	/**********绑定参数************/
	OCIBind *p_bind;
	int  id = 1;
	OCIDate v1;
	OCIDate  v2;
	OCIDate v3;
	int v4 = 17;
	int v5  = 57;
	/*
	 v1     DATE,
	v2     DATETIME,
	v3     DATETIME WITH TIME ZONE,
	v4     INTERVAL YEAR TO MONTH,
	v5     INTERVAL DAY TO SECOND
	*/
	OCIDateSetDate(&v1, 2016, 8, 1);
	
	OCIDateSetDate(&v2, 2016, 8, 2);
	OCIDateSetTime(&v2, 10, 25, 49);
	
	OCIDateSetDate(&v3, 2016, 8, 3);
	OCIDateSetTime(&v3, 10, 25, 49);	
	
	/*
	v1     DATE,
	v2     DATETIME,
	v3     DATETIME WITH TIME ZONE,
	v4     INTERVAL YEAR TO MONTH,
	v5     INTERVAL DAY TO SECOND
	*/

	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 1, &id, sizeof(id), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 2, &v1, sizeof(OCIDate *), SQLT_DAT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 3, &v2, sizeof(OCIDate *), SQLT_TIME, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 4, &v3, sizeof(OCIDate *), SQLT_TIME_TZ, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 5, &v4, sizeof(OCIInterval *), SQLT_INTERVAL_YM, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 6, &v5, sizeof(OCIInterval *), SQLT_INTERVAL_DS, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);

	/******执行******************/
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}


void selectTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OCIDefine *defhp1, *defhp2, *defhp3, *defhp4, *defhp5, *defhp6, *defhp7, *defhp8;
	OraText *sql = (OraText*)"select * from test1";
	ub4  sql_len = (ub4)strlen((char*)sql);

	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);

	/* execute the statement */
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 0, 0,
		(OCISnapshot *)NULL, (OCISnapshot *)NULL,
		OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	/*
	v1     DATE,
	v2     DATETIME,
	v3     DATETIME WITH TIME ZONE,
	v4     INTERVAL YEAR TO MONTH,
	v5     INTERVAL DAY TO SECOND
	*/

	/* Define output buffers */
	int id;
	ret = OCIDefineByPos(p_stmt, &defhp1, p_err, 1,
		(void *)&id, (sb4) sizeof(id),
		SQLT_INT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	OCIDate v1;
	OCIDate  v2;
	OCIDate v3;
	int v4 ;
	int v5;
//	OCIDateToText()
	ret = OCIDefineByPos(p_stmt, &defhp2, p_err, 2,
		(void *)&v1, (sb4) sizeof(v1),
		SQLT_DAT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	ret = OCIDefineByPos(p_stmt, &defhp3, p_err, 3,
		(void *)&v2, (sb4) sizeof(v2),
		SQLT_TIME, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	ret = OCIDefineByPos(p_stmt, &defhp4, p_err, 4,
		(void *)&v3, (sb4) sizeof(v3),
		SQLT_TIME_TZ, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	ret = OCIDefineByPos(p_stmt, &defhp5, p_err, 5,
		(void *)&v4, (sb4) sizeof(v4),
		SQLT_INTERVAL_YM, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	
	ret = OCIDefineByPos(p_stmt, &defhp6, p_err, 6,
		(void *)&v5, (sb4) sizeof(v5),
		SQLT_INTERVAL_DS, (void *)NULL, (ub2 *)NULL,
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
	switch (ret)
	{
	case OCI_SUCCESS:
		printf("id=[%d],v1=[%d],v2=[%d],v3=[%d],v4=[%d],v5=[%d].\n",id,v1.OCIDateDD,v2.OCIDateDD,v3.OCIDateDD,v4,v5);
		break;

	case OCI_NO_DATA:
		printf("fetched  no results.\n");
		break;
	case OCI_ERROR:
		showError(p_err);
		exit(1);
	default:
		break;
	}	
}