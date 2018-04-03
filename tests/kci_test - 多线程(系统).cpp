#include <stdio.h>
#include <process.h>
#include <string.h>
#include "time.h"
#include "kci_defs.h"b
extern "C"{
#include "ociap.h"
};


#define array_length  1

struct parameter
{
	OCISvcCtx  *p_conn;
	OCIStmt    *p_stmt;
	OCIError   *p_err;
	int max;
	int min;
};


DWORD WINAPI work(LPVOID lpParam);

int RuningThread(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err, int indexpos);

static void showError(OCIError *p_err);
void createTestTable(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);
void insertTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err,int id);
void insertTransTestData(OCISvcCtx  *p_conn,  OCIError   *p_err);
void selectTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);
static sb4 cbf_in_data(dvoid *ctxp, OCIBind *bindp, ub4 iter, ub4 index,
	dvoid **bufpp, ub4 *alenpp, ub1 *piecep, dvoid **indpp);

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
	OCIAttrSet(p_conn, OCI_HTYPE_SVCCTX, (dvoid *)p_serv, 0, OCI_ATTR_SERVER, p_err);	
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
	////多线程插入数据
	{
		long starttime = clock();
		OCIStmt    *p_stmt2[200000];
		for (int i = 0; i < 200000; i++)
		{
			p_stmt2[i] = NULL;
			OCIHandleAlloc(p_env, (void**)&p_stmt2[i], OCI_HTYPE_STMT, 0, 0);
			RuningThread(p_conn, p_stmt2[i], p_err, i);
		}
		long finishtime = clock();
		double  duration = (double)(finishtime - starttime) / 1000;
		printf("%f seconds(%d)\n", duration, 10000);

	}
//	insertTestData(p_conn,p_stmt,p_err,1);	
//	insertTestData(p_conn, p_stmt, p_err, 2);
//	insertTestData(p_conn, p_stmt, p_err, 3);
//	insertTestData(p_conn, p_stmt, p_err, 4);
	/////查询数据
//	selectTestData(p_conn, p_stmt, p_err);
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

	OraText *sql3 = (OraText*)"CREATE TABLE  TEST1 ( id     INTEGER primary key,v1     INTEGER,	v2     BIGINT,v3     FLOAT,v4     DOUBLE,v5  varchar(128),v6   INTEGER,v7     number(9,4))";
	ub4  sql_len3 = (ub4)strlen((char*)sql3);
	OCIStmtPrepare(p_stmt, p_err, sql3, sql_len3, 0, OCI_DEFAULT);
	OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}

void insertTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err,int id)
{
	OraText *sql = (OraText*)"insert into TEST1 values(:1,:2,:3,:4,:5,:6,:7,:8)";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);


	/**********绑定参数************/
	OCIBind *p_bind1, *p_bind2, *p_bind3, *p_bind4, *p_bind5, *p_bind6, *p_bind7, *p_bind8;
#define COUNT 3
	int  aid[COUNT] = { id * 3, id * 3 + 1, id * 3+2 };
	int v1[COUNT] = { 10, 11, 12 };
	long  v2 = 123456789;
	float v3[COUNT] = { 123.45, 234.56, 345.67};
	double v4[COUNT] = { 123.45, 2340.56, 34500.67 };
	char v5[COUNT][64];
	int v51[COUNT] = { 10, 11, 12 };
	int v6[COUNT] = { 1,10, 100 };
	double v7[COUNT] = { 123.456, 1234.456, 12345.678 };
//	printf("id(int)=[%d],v1(int)=[%d],v2(long)=[%ld],v3(float)=[%f],v4(double)=[%f],v5(int)=[%d],v6(int)=[%d],v7(double)=[%f].\n", aid[0], v1[0], v2, v3[0], v4[0], v5[0], v6[0], v7[0]);

	ret = OCIBindByPos(p_stmt, &p_bind1, p_err, 1, &aid[0], sizeof(aid[0]), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	OCIBindArrayOfStruct(p_bind1, p_err, sizeof(aid[0]), 0, 0, 0);
	ret = OCIBindByPos(p_stmt, &p_bind2, p_err, 2, &v1[0], sizeof(v1[0]), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	OCIBindArrayOfStruct(p_bind2, p_err, sizeof(v1[0]), 0, 0, 0);
	////改为动态绑定
	ret = OCIBindByPos(p_stmt, &p_bind3, p_err, 3, &v2, sizeof(v2), SQLT_LNG, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindDynamic(p_bind3, p_err, &v2, cbf_in_data, NULL, NULL);

	ret = OCIBindByPos(p_stmt, &p_bind4, p_err, 4, &v3[0], sizeof(v3[0]), SQLT_FLT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	OCIBindArrayOfStruct(p_bind4, p_err, sizeof(v3[0]), 0, 0, 0);
	ret = OCIBindByPos(p_stmt, &p_bind5, p_err, 5, &v4[0], sizeof(v4[0]), SQLT_NUM, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	OCIBindArrayOfStruct(p_bind5, p_err, sizeof(v4[0]), 0, 0, 0);

	memset(v5[0], 0, sizeof(v5[0]));
    strcpy(v5[0], "firset insert data");
	memset(v5[1], 0, sizeof(v5[1]));
	strcpy(v5[1], "各不相同则由于的呢！？？？");
	memset(v5[2], 0, sizeof(v5[2]));
	strcpy(v5[2], "中ab1英b2b文3混杂");
	ret = OCIBindByPos(p_stmt, &p_bind6, p_err, 6, v5[0], sizeof(v5[0]), SQLT_STR, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	OCIBindArrayOfStruct(p_bind6, p_err, sizeof(v5[0]), 0, 0, 0);
	ret = OCIBindByPos(p_stmt, &p_bind7, p_err, 7, &v6[0], sizeof(v6[0]), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	OCIBindArrayOfStruct(p_bind7, p_err, sizeof(v6[0]), 0, 0, 0);
	ret = OCIBindByPos(p_stmt, &p_bind8, p_err, 8, &v7[0], sizeof(v7[0]), SQLT_NUM, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	OCIBindArrayOfStruct(p_bind8, p_err, sizeof(v7[0]), 0, 0, 0);

	/******执行******************/
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, COUNT, 0, NULL, NULL, OCI_DEFAULT);
}

////多条同时获得
void selectTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OCIBind *bndp1;
	OCIDefine *defhp1, *defhp2, *defhp3, *defhp4, *defhp5, *defhp6, *defhp7, *defhp8;
	OraText *sql = (OraText*)"select * from test1 where v1 >:v1";
	ub4  sql_len = (ub4)strlen((char*)sql);

	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);

	/* bind input parameters */
	float valTemp = 0;
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

    #define ARRAY_SIZE 3
	/* Define output buffers */
	int id[ARRAY_SIZE];
	int id_ind[ARRAY_SIZE];
	ret = OCIDefineByPos(p_stmt, &defhp1, p_err, 1,
		(void *)&id, (sb4) sizeof(id[0]),
		SQLT_INT, (void *)id_ind, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	int v1[ARRAY_SIZE];
	int v1_ind[ARRAY_SIZE];
	ret = OCIDefineByPos(p_stmt, &defhp2, p_err, 2,
		(void *)&v1, (sb4) sizeof(v1[0]),
		SQLT_INT, (void *)v1_ind, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	long v2[ARRAY_SIZE];
	long v2_ind[ARRAY_SIZE];
	ret = OCIDefineByPos(p_stmt, &defhp3, p_err, 3,
		(void *)&v2, (sb4) sizeof(v2[0]),
		SQLT_LNG, (void *)v2_ind, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	float v3[ARRAY_SIZE];
	float v3_ind[ARRAY_SIZE];
	ret = OCIDefineByPos(p_stmt, &defhp4, p_err, 4,
		(void *)&v3, (sb4) sizeof(v3[0]),
		SQLT_FLT, (void *)v3_ind, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	double v4[ARRAY_SIZE];
	double v4_ind[ARRAY_SIZE];
	ret = OCIDefineByPos(p_stmt, &defhp5, p_err, 5,
		(void *)&v4, (sb4) sizeof(v4[0]),
		SQLT_NUM, (void *)v4_ind, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	short v5[ARRAY_SIZE];
	short v5_ind[ARRAY_SIZE];
	ret = OCIDefineByPos(p_stmt, &defhp6, p_err, 6,
		(void *)&v5, (sb4) sizeof(v5[0]),
		SQLT_INT, (void *)v5_ind, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	short v6[ARRAY_SIZE];
	short v6_ind[ARRAY_SIZE];
	ret = OCIDefineByPos(p_stmt, &defhp7, p_err, 7,
		(void *)&v6, (sb4) sizeof(v6[0]),
		SQLT_INT, (void *)v6_ind, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	double v7[ARRAY_SIZE];
	double v7_ind[ARRAY_SIZE];
	ret = OCIDefineByPos(p_stmt, &defhp8, p_err, 8,
		(void *)&v7, (sb4) sizeof(v7[0]),
		SQLT_NUM, (void *)v7_ind, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	/* Fetch one row */
	int num_rows = ARRAY_SIZE;
	ret = OCIStmtFetch(p_stmt, p_err, num_rows, OCI_FETCH_NEXT,
		OCI_DEFAULT);
	ub4 size = sizeof(num_rows);
	OCIAttrGet(p_stmt, OCI_HTYPE_STMT, &num_rows, &size, OCI_ATTR_ROWS_FETCHED, p_err);
	for (int i=0; i < num_rows; i++)
		printf("\nid(int)=[%d],v1(int)=[%d],v2(long)=[%ld],v3(float)=[%f],v4(double)=[%f],v5(int)=[%d],v6(int)=[%d],v7(double)=[%f].\n", id[i], v1[i], v2[i], v3[i], v4[i], v5[i], v6[i], v7[i]);

}


static sb4 cbf_in_data(dvoid *ctxp, OCIBind *bindp, ub4 iter, ub4 index,
	dvoid **bufpp, ub4 *alenpp, ub1 *piecep, dvoid **indpp)
{

	long *v = (long *)bufpp;
	*v = 1234;
	int len =sizeof(v);
	*alenpp = len;
	*indpp = (dvoid *)0;
	*piecep = OCI_ONE_PIECE;	
	return OCI_CONTINUE;
}


int RuningThread(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err, int indexpos)
{
    struct parameter* args;
	HANDLE threads[array_length];
	for (int i = 0; i<array_length; i++)
	{
		args = (struct parameter*)malloc(sizeof(struct parameter));
		args->p_conn = p_conn;
		args->p_err = p_err;
		args->p_stmt = p_stmt;
		args->min = indexpos;
		args->max = args->min + array_length;
		threads[i] = CreateThread(NULL, 0, work, (LPVOID)args, 0, NULL);
	}
	//设置等待  
	WaitForMultipleObjects(array_length, threads, TRUE, INFINITE);
	
	for (int i = 0; i<array_length; i++)
		CloseHandle(threads[i]);	
	return 0;
}

DWORD WINAPI work(LPVOID lpParam)
{
	struct parameter* para = (struct parameter *)lpParam;
	for (int i = para->min; i<para->max;i++)
	  insertTestData(para->p_conn, para->p_stmt, para->p_err, i);
	return 0;

}