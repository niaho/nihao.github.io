#include <stdio.h>
#include <process.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_time.h"
extern "C"{
#include "kci_api.h"
};

static void showError(KCIError *p_err);
void createTestTable(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err);
void insertTestData(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err);
void insertTransTestData(KCISvcCtx  *p_conn,  KCIError   *p_err);
void selectTestData(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err);


int main(int argc, char *argv[])
{
	sword      ret = 0;
	KCIEnv     *p_env = NULL;
	KCISvcCtx  *p_conn = NULL;
	KCIServer  *p_serv = NULL;
	KCISession *p_sess = NULL;	
	KCIStmt    *p_stmt = NULL;
	KCIError   *p_err = NULL;
	

	/**********创建环境,分配句柄**********/
	ret = KCIEnvCreate(&p_env, KCI_OBJECT, 0, 0, 0, 0, 0, 0);
	if (ret == KCI_ERROR)
	{
		printf("create env failed\r\n");
		exit(1);
	}

	ret = KCIHandleAlloc(p_env, (void**)&p_conn, KCI_HTYPE_SVCCTX, 0, 0);
	if (ret == KCI_ERROR)
	{
		printf("alloc server conctx failed\r\n");
		exit(1);
	}

	ret = KCIHandleAlloc(p_env, (void**)&p_serv, KCI_HTYPE_SERVER, 0, 0);
	if (ret == KCI_ERROR)
	{
		printf("alloc server failed\r\n");
		exit(1);
	}

	ret = KCIHandleAlloc(p_env, (void**)&p_sess, KCI_HTYPE_SESSION, 0, 0);
	if (ret == KCI_ERROR)
	{
		printf("alloc session failed\r\n");
		exit(1);
	}

	ret = KCIHandleAlloc(p_env, (void**)&p_err, KCI_HTYPE_SERVER, 0, 0);
	if (ret == KCI_ERROR)
	{
		printf("alloc server failed\r\n");
		exit(1);
	}
		

	ret = KCIHandleAlloc(p_env, (void**)&p_stmt, KCI_HTYPE_STMT, 0, 0);
	if (ret == KCI_ERROR)
	{
		printf("alloc statement failed\r\n");
		exit(1);
	}

	/**********设置句柄***********/
	char *link_str = "localhost:6688/system";
	ub4  str_len = (ub4)strlen(link_str);

	ret = KCIServerAttach(p_serv, p_err, (oratext*)link_str, str_len, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	KCIAttrSet(p_sess, KCI_HTYPE_SESSION, "SYSDBA", 6, KCI_ATTR_USERNAME, p_err);
	KCIAttrSet(p_sess, KCI_HTYPE_SESSION, "SYSDBA", 6, KCI_ATTR_PASSWORD, p_err);
	KCIAttrSet(p_conn, KCI_HTYPE_SVCCTX, p_serv, 0, KCI_ATTR_SERVER, p_err);	
	/**********开始会话***********/
	ret = KCISessionBegin(p_conn, p_err, p_sess, KCI_CRED_RDBMS, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	ret = KCIAttrSet(p_conn, KCI_HTYPE_SVCCTX, &p_sess, 0, KCI_ATTR_SESSION, p_err);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	
	/*******准备语句***************/
	/////创建表结构
	//createTestTable(p_conn, p_stmt, p_err);	
	/////插入数据
	insertTestData(p_conn,p_stmt,p_err);	
	/////查询数据
	selectTestData(p_conn, p_stmt, p_err);
	/////
	/* release the statement handle */
	ret = KCIStmtRelease(p_stmt, p_err,
		(OraText *)NULL, 0, KCI_DEFAULT);

}

static void showError(KCIError *p_err)
{
	sb4      err_code;
	OraText  err_info[256];

	KCIErrorGet(p_err, 1, NULL, &err_code, err_info, 254, KCI_HTYPE_ERROR);
	printf("ErrCode:%d ErrInfo:%s", err_code, err_info);
}

void createTestTable(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err)
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
	KCIDefine *defhp1;
	OraText *sql = (OraText*)"select count(*) from user_tables where table_name = 'TEST1'";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = KCIStmtPrepare(p_stmt, p_err, sql, sql_len, KCI_NTV_SYNTAX, KCI_DEFAULT);
	/******执行******************/
	ret = KCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, KCI_DEFAULT);
	/* Define output buffers */
	long long num =0 ;
	ret = KCIDefineByPos(p_stmt, &defhp1, p_err, 1,
		(void *)&num, (sb4) sizeof(num),
		SQLT_LNG, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	/* Fetch one row */
	int num_rows = 1;
	ret = KCIStmtFetch(p_stmt, p_err, num_rows, KCI_FETCH_NEXT,
		KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	if (num > 0)
	{	
		OraText *sql2 = (OraText*)"DROP TABLE TEST1";
		ub4  sql_len2 = (ub4)strlen((char*)sql2);
		KCIStmtPrepare(p_stmt, p_err, sql2, sql_len2, 0, KCI_DEFAULT);
		KCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, KCI_DEFAULT);
	}

	OraText *sql3 = (OraText*)"CREATE TABLE  TEST1 ( id     INTEGER,v1     DATE,v2     DATETIME,v3     DATETIME WITH TIME ZONE,v4     INTERVAL YEAR TO MONTH,v5     INTERVAL DAY TO SECOND)";
	ub4  sql_len3 = (ub4)strlen((char*)sql3);
	KCIStmtPrepare(p_stmt, p_err, sql3, sql_len3, 0, KCI_DEFAULT);
	KCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, KCI_DEFAULT);
}

void insertTestData(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err)
{
	OraText *sql = (OraText*)"insert into TEST1 values(:1,:2,:3,:4,:5,:6)";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = KCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, KCI_DEFAULT);


	/**********绑定参数************/
	KCIBind *p_bind;
	int  id = 1;
	KCIDate v1;
	KCIDate  v2;
	KCIDate v3;
	int v4 = 17;
	int v5  = 57;
	/*
	 v1     DATE,
	v2     DATETIME,
	v3     DATETIME WITH TIME ZONE,
	v4     INTERVAL YEAR TO MONTH,
	v5     INTERVAL DAY TO SECOND
	*/
	KCIDateSetDate(&v1, 2016, 8, 1);
	
	KCIDateSetDate(&v2, 2016, 8, 2);
	KCIDateSetTime(&v2, 10, 25, 49);
	
	KCIDateSetDate(&v3, 2016, 8, 3);
	KCIDateSetTime(&v3, 10, 25, 49);	
	
	/*
	v1     DATE,
	v2     DATETIME,
	v3     DATETIME WITH TIME ZONE,
	v4     INTERVAL YEAR TO MONTH,
	v5     INTERVAL DAY TO SECOND
	*/

	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 1, &id, sizeof(id), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 2, &v1, sizeof(KCIDate *), SQLT_DAT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 3, &v2, sizeof(KCIDate *), SQLT_TIME, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 4, &v3, sizeof(KCIDate *), SQLT_TIME_TZ, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 5, &v4, sizeof(KCIInterval *), SQLT_INTERVAL_YM, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 6, &v5, sizeof(KCIInterval *), SQLT_INTERVAL_DS, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);

	/******执行******************/
	ret = KCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, KCI_DEFAULT);
}


void selectTestData(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err)
{
	KCIDefine *defhp1, *defhp2, *defhp3, *defhp4, *defhp5, *defhp6;
	OraText *sql = (OraText*)"select * from test1";
	ub4  sql_len = (ub4)strlen((char*)sql);

	sword ret = KCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, KCI_DEFAULT);

	/* execute the statement */
	ret = KCIStmtExecute(p_conn, p_stmt, p_err, 0, 0,
		(KCISnapshot *)NULL, (KCISnapshot *)NULL,
		KCI_DEFAULT);
	if (ret == KCI_ERROR)
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
	ret = KCIDefineByPos(p_stmt, &defhp1, p_err, 1,
		(void *)&id, (sb4) sizeof(id),
		SQLT_INT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	KCIDate v1;
	KCIDate  v2;
	KCIDate v3;
	int v4 ;
	int v5;
//	KCIDateToText()
	ret = KCIDefineByPos(p_stmt, &defhp2, p_err, 2,
		(void *)&v1, (sb4) sizeof(v1),
		SQLT_DAT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	ret = KCIDefineByPos(p_stmt, &defhp3, p_err, 3,
		(void *)&v2, (sb4) sizeof(v2),
		SQLT_TIME, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	ret = KCIDefineByPos(p_stmt, &defhp4, p_err, 4,
		(void *)&v3, (sb4) sizeof(v3),
		SQLT_TIME_TZ, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	ret = KCIDefineByPos(p_stmt, &defhp5, p_err, 5,
		(void *)&v4, (sb4) sizeof(v4),
		SQLT_INTERVAL_YM, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	
	ret = KCIDefineByPos(p_stmt, &defhp6, p_err, 6,
		(void *)&v5, (sb4) sizeof(v5),
		SQLT_INTERVAL_DS, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	
	/* Fetch one row */
	int num_rows = 1;
	ret = KCIStmtFetch(p_stmt, p_err, num_rows, KCI_FETCH_NEXT,
		KCI_DEFAULT);
	switch (ret)
	{
	case KCI_SUCCESS:
		printf("id=[%d],v1=[%d],v2=[%d],v3=[%d],v4=[%d],v5=[%d].\n",id,v1.KCIDateDD,v2.KCIDateDD,v3.KCIDateDD,v4,v5);
		break;

	case KCI_NO_DATA:
		printf("fetched  no results.\n");
		break;
	case KCI_ERROR:
		showError(p_err);
		exit(1);
	default:
		break;
	}	
}