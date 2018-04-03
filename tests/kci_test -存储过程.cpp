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
void proceTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);


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
	OCIAttrSet(p_conn, OCI_HTYPE_SVCCTX, &p_serv, 0, OCI_ATTR_SERVER, p_err);	
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
//	createTestTable(p_conn, p_stmt, p_err);	
	/////插入数据
//	insertTestData(p_conn,p_stmt,p_err);	
	/////查询数据
	selectTestData(p_conn, p_stmt, p_err);
	proceTestData(p_conn, p_stmt, p_err);
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

	OraText *sql3 = (OraText*)"CREATE TABLE  TEST1 ( id     INTEGER primary key,v1     INTEGER,	v2     BIGINT,v3     FLOAT,v4     DOUBLE,v5     TINYINT,v6     SMALLINT,v7     number(9,4))";
	ub4  sql_len3 = (ub4)strlen((char*)sql3);
	OCIStmtPrepare(p_stmt, p_err, sql3, sql_len3, 0, OCI_DEFAULT);
	OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}

void insertTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OraText *sql = (OraText*)"insert into TEST1 values(:1,:2,:3,:4,:5,:6,:7,:8) returning v2 into :id";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);


	/**********绑定参数************/
	OCIBind *p_bind;
	int  id = 1;
	int v1 = 2;
	long  v2 = 1234567890;
	float v3 = 123.4;
	double v4 = 123.4567890;
	int v5 = 3;
	int v6 = 4;
	double v7 = 1234.1234;
	long retid=-7;
	printf("id(int)=[%d],v1(int)=[%d],v2(long)=[%ld],v3(float)=[%f],v4(double)=[%f],v5(int)=[%d],v6(int)=[%d],v7(double)=[%f].\n", id, v1, v2, v3, v4, v5, v6, v7);

	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 1, &id, sizeof(id), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 2, &v1, sizeof(v1), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 3, &v2, sizeof(v2), SQLT_LNG, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 4, &v3, sizeof(v3), SQLT_FLT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 5, &v4, sizeof(v4), SQLT_NUM, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 6, &v5, sizeof(v5), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 7, &v6, sizeof(v6), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 8, &v7, sizeof(v7), SQLT_NUM, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 9, &retid, sizeof(retid), SQLT_LNG, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);

	/******执行******************/
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
	printf("----%ld---\n", retid);
}


void selectTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OCIBind *bndp1;
	OCIDefine *defhp1, *defhp2, *defhp3, *defhp4, *defhp5, *defhp6, *defhp7, *defhp8;
	OraText *sql = (OraText*)"select * from test1 where v1 >:v1";
	ub4  sql_len = (ub4)strlen((char*)sql);

	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);

	/* bind input parameters */
	float valTemp = 1;
	ret = OCIBindByName(p_stmt, &bndp1, p_err, (text *) ":v1",
		-1, (void *)&valTemp, sizeof(valTemp),
		SQLT_FLT, (void *)NULL,
		(ub2 *)NULL, (ub2 *)NULL, 0, (ub4 *)NULL,
		OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	/* execute the statement */
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 0, 0,
		(OCISnapshot *)NULL, (OCISnapshot *)NULL,
		OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}


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
	int v1;
	ret = OCIDefineByPos(p_stmt, &defhp2, p_err, 2,
		(void *)&v1, (sb4) sizeof(v1),
		SQLT_INT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	long  v2 ;
	ret = OCIDefineByPos(p_stmt, &defhp3, p_err, 3,
		(void *)&v2, (sb4) sizeof(v2),
		SQLT_LNG, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	float v3;
	ret = OCIDefineByPos(p_stmt, &defhp4, p_err, 4,
		(void *)&v3, (sb4) sizeof(v3),
		SQLT_FLT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	double v4;
	ret = OCIDefineByPos(p_stmt, &defhp5, p_err, 5,
		(void *)&v4, (sb4) sizeof(v4),
		SQLT_NUM, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	short v5 ;
	ret = OCIDefineByPos(p_stmt, &defhp6, p_err, 6,
		(void *)&v5, (sb4) sizeof(v5),
		SQLT_INT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	short v6;
	ret = OCIDefineByPos(p_stmt, &defhp7, p_err, 7,
		(void *)&v6, (sb4) sizeof(v6),
		SQLT_INT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	double v7;
	ret = OCIDefineByPos(p_stmt, &defhp8, p_err, 8,
		(void *)&v7, (sb4) sizeof(v7),
		SQLT_NUM, (void *)NULL, (ub2 *)NULL,
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
		printf("id(int)=[%d],v1(int)=[%d],v2(long)=[%ld],v3(float)=[%f],v4(double)=[%f],v5(int)=[%d],v6(int)=[%d],v7(double)=[%f].\n",id,v1,v2,v3,v4,v5,v6,v7);
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

void proceTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OCIBind *bndp1, *bndp2, *bndp3, *bndp4, *bndp5;
	
	OraText *sql = (OraText*)"out_test(:1,:2,:3,:4,:5);";
	ub4  sql_len = (ub4)strlen((char*)sql);

	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);

	/* bind input parameters */
	int id = 1;
	int v1;
	long  v2;
	float v3;
	double v4 = 0;
	ret = OCIBindByPos(p_stmt, &bndp1, p_err, 1, &id, sizeof(id), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &bndp2, p_err, 2, &v1, sizeof(v1), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &bndp3, p_err, 3, &v2, sizeof(v2), SQLT_LNG, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &bndp4, p_err, 4, &v3, sizeof(v3), SQLT_FLT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &bndp5, p_err, 5, &v4, sizeof(v4), SQLT_NUM, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);


	/* execute the statement */
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 0, 0,
		(OCISnapshot *)NULL, (OCISnapshot *)NULL,
		OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	printf("id(int)=[%d],v1(int)=[%d],v2(long)=[%ld],v3(float)=[%f],v4(double)=[%f].\n", id, v1, v2, v3, v4);
	return;
}