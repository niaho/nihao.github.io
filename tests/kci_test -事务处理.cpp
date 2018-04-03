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
	createTestTable(p_conn, p_stmt, p_err);	
	/////插入数据
	insertTestData(p_conn,p_stmt,p_err);	
	////按照事务处理模式插入数据
	insertTransTestData(p_conn, p_err);
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
    empno     number(4) primary key,
    ename     varchar2(10),
    age       number(4),
    hiredate  datetime,
    sal       number(7,2))
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

	OraText *sql3 = (OraText*)"CREATE TABLE  TEST1 (id number(4) primary key,name  varchar2(10),age  number(4),hiredate  datetime,	sal number(7, 2))";
	ub4  sql_len3 = (ub4)strlen((char*)sql3);
	OCIStmtPrepare(p_stmt, p_err, sql3, sql_len3, 0, OCI_DEFAULT);
	OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}

void insertTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OraText *sql = (OraText*)"insert into TEST1 values(:1,:2,:3,:4,:5)";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);


	/**********绑定参数************/
	OCIBind *p_bind;
	int  id = 7;
	char *name = "中国test1";
	int age = 42;
	float sal = 3.33;
	OCIDate hiredate;

	hiredate.OCIDateYYYY = 2016;
	hiredate.OCIDateMM = 8;
	hiredate.OCIDateDD = 12;
	hiredate.OCIDateTime.OCITimeHH = 11;
	hiredate.OCIDateTime.OCITimeMI = 40;
	hiredate.OCIDateTime.OCITimeSS = 40;
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 1, &id, 4, SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 2, name, strlen(name) + 1, SQLT_STR, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 3, &age, 4, SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 4, &hiredate, sizeof(hiredate), SQLT_DAT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind, p_err, 5, &sal, 4, SQLT_FLT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);

	/******执行******************/
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}

//////同时启动2个事务
void insertTransTestData(OCISvcCtx  *p_conn, OCIError   *p_err)
{
	OCIStmt *stmthp;
	OCITrans *p_trans;
	text sqlstmt[128];
	sword ret;
	XID *gxid;

	OCIHandleAlloc(p_conn->p_env, (dvoid **)&stmthp, OCI_HTYPE_STMT, 0, 0);

	/* 事务准备 */
	ret = OCIHandleAlloc(p_conn->p_env, (void**)&p_trans, OCI_HTYPE_TRANS, 0, 0);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	ret = OCIAttrSet(p_conn, OCI_HTYPE_SVCCTX, &p_trans, 0, OCI_ATTR_TRANS, p_err);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	/* start a transaction with global transaction id = [1000, 123, 1] */
	gxid = mcNew(&(p_conn->p_env->mem), XID);
	gxid->formatID = 1000; /* format id = 1000 */
	gxid->gtrid_length = 3; /* gtrid = 123 */
	gxid->data[0] = 1;
	gxid->data[1] = 2;
	gxid->data[2] = 3;
	gxid->bqual_length = 1; /* bqual = 1 */
	gxid->data[3] = 1;
	ret = OCIAttrSet(p_trans, OCI_HTYPE_TRANS, &gxid, sizeof(XID), OCI_ATTR_XID, p_err);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	ret = OCITransStart(p_conn, p_err, 60, OCI_TRANS_NEW);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	/* update TEST1 id=7, increment salary */
	sprintf((char *)sqlstmt, "update TEST1 set sal= sal+10 where id=7");
	ret = OCIStmtPrepare(stmthp, p_err, sqlstmt, strlen((char *)sqlstmt), OCI_NTV_SYNTAX, 0);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	ret = OCIStmtExecute(p_conn, stmthp, p_err, 1, 0, 0, 0, 0);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}

	/* update TEST1 id=7, increment salary */
	memset(sqlstmt, 0, sizeof(sqlstmt));
	sprintf((char *)sqlstmt, "insert into TEST1 values(1,NULL,17,NULL,123)");
	ret = OCIStmtPrepare(stmthp, p_err, sqlstmt, strlen((char *)sqlstmt), OCI_NTV_SYNTAX, 0);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	ret = OCIStmtExecute(p_conn, stmthp, p_err, 1, 0, 0, 0, 0);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
//	ret = OCITransRollback(p_conn, p_err, (ub4)0);
	ret = OCITransCommit(p_conn, p_err, (ub4)0);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
}

void selectTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OCIBind *bndp1;
	OCIDefine *defhp1, *defhp2, *defhp3, *defhp4, *defhp5;
	OraText *sql = (OraText*)"select id,name,age,hiredate,sal from test1 where sal >:sal";
	ub4  sql_len = (ub4)strlen((char*)sql);

	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);

	/* bind input parameters */
	float valTemp = 1;
	ret = OCIBindByName(p_stmt, &bndp1, p_err, (text *) ":sal",
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
	char name[32];
	memset(name, 0, sizeof(name));
	ret = OCIDefineByPos(p_stmt, &defhp2, p_err, 2,
		(void *)name, (sb4)sizeof(name),
		SQLT_STR, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	int age =0;
	ret = OCIDefineByPos(p_stmt, &defhp3, p_err, 3,
		(void *)&age, (sb4) sizeof(age),
		SQLT_INT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	OCIDate hiredate;
	ret = OCIDefineByPos(p_stmt, &defhp4, p_err, 4,
		(void *)&hiredate, (sb4) sizeof(hiredate),
		SQLT_DAT, (void *)NULL, (ub2 *)NULL,
		(ub2 *)NULL, OCI_DEFAULT);
	if (ret == OCI_ERROR)
	{
		showError(p_err);
		exit(1);
	}
	double sal;
	ret = OCIDefineByPos(p_stmt, &defhp5, p_err, 5,
		(void *)&sal, (sb4) sizeof(sal),
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
		printf("fetched results: id=%d,name=%s,age=%ld,hiredate=%d,sal=%lf, \n", id, name, age, hiredate.OCIDateYYYY, sal);
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