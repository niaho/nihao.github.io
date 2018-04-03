#include <stdio.h>
#include <process.h>
#include <string.h>
#include "kci_defs.h"
extern "C"{
#include "kci_api.h"
};

static void showError(KCIError *p_err);
void createTestTable(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err);
void insertTestData(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err,int id);
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
	createTestTable(p_conn, p_stmt, p_err);	
	/////插入数据
	insertTestData(p_conn,p_stmt,p_err,1);	
	insertTestData(p_conn, p_stmt, p_err, 2);
	insertTestData(p_conn, p_stmt, p_err, 3);
	insertTestData(p_conn, p_stmt, p_err, 4);
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
    v1     INTEGER,
	v2     BIGINT,
	v3     FLOAT,
	v4     DOUBLE,
	v5     TINYINT,
	v6     SMALLINT,
	v7     number(9,4)
	*/
	/*
	INTEGER 整型数据类型
BIGINT 长整型数据类型
FLOAT 单精度浮点型数据类型
DOUBLE 双精度浮点型数据类型
TINYINT 微整型数据类型
SMALLINT 短整型数据类型
NUMERIC 固定精度数据类型
	*/
	KCIDefine *defhp1;
	OraText *sql = (OraText*)"select count(*) from test1";
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

	//OraText *sql3 = (OraText*)"CREATE TABLE  TEST1 ( id     INTEGER ,v1     INTEGER,	v2     BIGINT,v3     FLOAT,v4     DOUBLE,v5     TINYINT,v6     SMALLINT,v7     number(9,4))";
	OraText *sql3 = (OraText*)"CREATE TABLE  TEST1 ( id     INTEGER ,v1     INTEGER,	v2     INTEGER,v3     INTEGER,v4     INTEGER,v5     TINYINT,v6     INTEGER,v7    INTEGER)";
	ub4  sql_len3 = (ub4)strlen((char*)sql3);
	KCIStmtPrepare(p_stmt, p_err, sql3, sql_len3, 0, KCI_DEFAULT);
	KCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, KCI_DEFAULT);
}

void insertTestData(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err,int id)
{
	OraText *sql = (OraText*)"insert into TEST1 values(:1,:2,:3,:4,:5,:6,:7,:8)";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = KCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, KCI_DEFAULT);


	/**********绑定参数***********
	KCIBind *p_bind;
	int  aid = id;
	int v1 = 2 + aid*1;
	long  v2 = 1234567890 + aid * 1;
	float v3 = 123.40 + aid * 1;
	double v4 = 123.4567890 + aid * 1;
	int v5 = 3 + aid * 1;
	int v6 = 4 + aid * 1;
	double v7 = 1234.1234 + aid * 1;
	*/
	KCIBind *p_bind;
	int  aid = id;
	int v1 = 2;
	int  v2 = 1234567891;
	int v3 = 123;
	int v4 = 123;
	int v5 = 3 ;
	int v6 = 4;
	int v7 = 1234;
	//printf("id(int)=[%d],v1(int)=[%d],v2(long)=[%ld],v3(float)=[%f],v4(double)=[%lf],v5(int)=[%d],v6(int)=[%d],v7(double)=[%lf].\n", id, v1, v2, v3, v4, v5, v6, v7);
	printf("id(int)=[%d],v1(int)=[%d],v2(int)=[%d],v3(int)=[%d],v4(int)=[%d],v5(int)=[%d],v6(int)=[%d],v7(int)=[%d].\n", id, v1, v2, v3, v4, v5, v6, v7);
/*
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 1, &aid, sizeof(aid), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 2, &v1, sizeof(v1), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 3, &v2, sizeof(v2), SQLT_LNG, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 4, &v3, sizeof(v3), SQLT_FLT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 5, &v4, sizeof(v4), SQLT_NUM, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 6, &v5, sizeof(v5), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 7, &v6, sizeof(v6), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 8, &v7, sizeof(v7), SQLT_NUM, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
*/
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 1, &aid, sizeof(aid), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 2, &v1, sizeof(v1), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 3, &v2, sizeof(v2), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 4, &v3, sizeof(v3), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 5, &v4, sizeof(v4), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 6, &v5, sizeof(v5), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 7, &v6, sizeof(v6), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	ret = KCIBindByPos(p_stmt, &p_bind, p_err, 8, &v7, sizeof(v7), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, KCI_DEFAULT);
	/******执行******************/
	ret = KCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, KCI_DEFAULT);
}

////多条同时获得
void selectTestData(KCISvcCtx  *p_conn, KCIStmt    *p_stmt, KCIError   *p_err)
{
	KCIBind *bndp1;
	KCIDefine *defhp1, *defhp2, *defhp3, *defhp4, *defhp5, *defhp6, *defhp7, *defhp8;
	OraText *sql = (OraText*)"select * from test1 where v1 >:v1";
	ub4  sql_len = (ub4)strlen((char*)sql);

	sword ret = KCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, KCI_DEFAULT);

	/* bind input parameters */
	float valTemp = 0;
	ret = KCIBindByName(p_stmt, &bndp1, p_err, (text *) ":v1",
		strlen(":v1"), (void *)&valTemp, sizeof(valTemp),
		SQLT_FLT, (void *)NULL,
		(ub2 *)NULL, (ub2 *)NULL, 0, (ub4 *)NULL,
		KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	/* execute the statement */
	ret = KCIStmtExecute(p_conn, p_stmt, p_err, 0, 0,
		(KCISnapshot *)NULL, (KCISnapshot *)NULL,
		KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

    #define ARRAY_SIZE 3
	/* Define output buffers */
	int id[ARRAY_SIZE];
	int id_ind[ARRAY_SIZE];
	ret = KCIDefineByPos(p_stmt, &defhp1, p_err, 1,
		(void *)&id, (sb4) sizeof(id[0]),
		SQLT_INT, (void *)id_ind, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	int v1[ARRAY_SIZE];
	int v1_ind[ARRAY_SIZE];
	ret = KCIDefineByPos(p_stmt, &defhp2, p_err, 2,
		(void *)&v1, (sb4) sizeof(v1[0]),
		SQLT_INT, (void *)v1_ind, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	long v2[ARRAY_SIZE];
	long v2_ind[ARRAY_SIZE];
	ret = KCIDefineByPos(p_stmt, &defhp3, p_err, 3,
		(void *)&v2, (sb4) sizeof(v2[0]),
		SQLT_LNG, (void *)v2_ind, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	float v3[ARRAY_SIZE];
	float v3_ind[ARRAY_SIZE];
	ret = KCIDefineByPos(p_stmt, &defhp4, p_err, 4,
		(void *)&v3, (sb4) sizeof(v3[0]),
		SQLT_FLT, (void *)v3_ind, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	double v4[ARRAY_SIZE];
	double v4_ind[ARRAY_SIZE];
	ret = KCIDefineByPos(p_stmt, &defhp5, p_err, 5,
		(void *)&v4, (sb4) sizeof(v4[0]),
		SQLT_NUM, (void *)v4_ind, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	short v5[ARRAY_SIZE];
	short v5_ind[ARRAY_SIZE];
	ret = KCIDefineByPos(p_stmt, &defhp6, p_err, 6,
		(void *)&v5, (sb4) sizeof(v5[0]),
		SQLT_INT, (void *)v5_ind, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	short v6[ARRAY_SIZE];
	short v6_ind[ARRAY_SIZE];
	ret = KCIDefineByPos(p_stmt, &defhp7, p_err, 7,
		(void *)&v6, (sb4) sizeof(v6[0]),
		SQLT_INT, (void *)v6_ind, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	double v7[ARRAY_SIZE];
	double v7_ind[ARRAY_SIZE];
	ret = KCIDefineByPos(p_stmt, &defhp8, p_err, 8,
		(void *)&v7, (sb4) sizeof(v7[0]),
		SQLT_NUM, (void *)v7_ind, (ub2 *)NULL,
		(ub2 *)NULL, KCI_DEFAULT);
	if (ret == KCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	/* Fetch one row */
	int num_rows = ARRAY_SIZE;
	ret = KCIStmtFetch(p_stmt, p_err, num_rows, KCI_FETCH_NEXT,
		KCI_DEFAULT);
	ub4 size = sizeof(num_rows);
	KCIAttrGet(p_stmt, KCI_HTYPE_STMT, &num_rows, &size, KCI_ATTR_ROWS_FETCHED, p_err);
	for (int i=0; i < num_rows; i++)
		printf("\nid(int)=[%d],v1(int)=[%d],v2(long)=[%ld],v3(float)=[%f],v4(double)=[%f],v5(int)=[%d],v6(int)=[%d],v7(double)=[%f].\n", id[i], v1[i], v2[i], v3[i], v4[i], v5[i], v6[i], v7[i]);

}