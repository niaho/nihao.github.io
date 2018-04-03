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
//	createTestTable(p_conn, p_stmt, p_err);	
	/////插入数据
//	insertTestData(p_conn,p_stmt,p_err);	
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
    v1     CLOB,
	v2     BINNARY,
	v3     BLOB
	)
	*/
	/*
	  TYPE_CLOB           CLOB	             若标志位不含 PARAM_IS，则数据就是clob的数据，否则，内含8字节流标志符
	
                TYPE_BINARY         BINNARY                  字节串，长度由ParaLen决定
                TYPE_BLOB           BLOB                     若标志位不含 PARAM_IS，则数据就是blob的数据，否则，内含8字节流标志符 
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

	OraText *sql3 = (OraText*)"CREATE TABLE  TEST1 ( id     INTEGER primary key, v1     BLOB)";
	ub4  sql_len3 = (ub4)strlen((char*)sql3);
	OCIStmtPrepare(p_stmt, p_err, sql3, sql_len3, 0, OCI_DEFAULT);
	OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}

void insertTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	FILE *fp;
	ub1  buf[513];
	ub4  offset;

	OraText *sql = (OraText*)"insert into TEST1 values(:1,:2)";
	ub4  sql_len = (ub4)strlen((char*)sql);

	sword ret  = OCIStmtPrepare2(p_conn, &p_stmt,        /* returned stmt handle */
		p_err,                            /* error handle */
		sql,   /* input statement text*/
		sql_len,        /* length of text */
		NULL, 0,         /* tagging parameters: optional */
		OCI_NTV_SYNTAX, OCI_DEFAULT);

	/**********绑定参数************/
	OCILobLocator    *v1;
	OCIBind *p_bind1, *p_bind2;
	int  id = 1;
	
	OCIDescriptorAlloc(p_conn->p_env, (dvoid **)&v1, (ub4)OCI_DTYPE_LOB, (size_t)0, (dvoid **)0);


	ret = OCIBindByPos(p_stmt, &p_bind1, p_err, 1, &id, sizeof(id), SQLT_INT, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	ret = OCIBindByPos(p_stmt, &p_bind2, p_err, 2, &v1, 0, SQLT_BLOB, (void*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0, OCI_DEFAULT);
	
	/******执行******************/
	ret = OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
	/* open file for reading */
	if ((fp = fopen("d:\\test1.txt", "rb")) == NULL)
	{
		printf("Cannot open file for reading \n");
		exit(1);
	}
	ub4 amtp = 512;
	offset = 0;
	while (!feof(fp))
	{
		/* Read the data from file */
		memset((void *)buf, '\0', 512);
		fread((void *)buf, 512, 1, fp);
		buf[512] = '\0';
		

		/*Write it into the locator */
	
		ret = OCILobWrite(p_conn, p_err, v1, (ub4 *)amtp, offset,
			(dvoid *)buf, 512, OCI_ONE_PIECE,
			(dvoid *)0,  NULL,
			(ub2)0, (ub1)SQLCS_IMPLICIT);

		if (ret == OCI_SUCCESS)
			offset += amtp;
		else
		{
			fclose(fp);
			printf("Write_to_loc error : OCILobWrite \n");			
		}
	};
	fclose(fp);

	amtp =0 ;

	/* get length of lob */
	ret = OCILobGetLength(p_conn, p_err, v1, &amtp);

	printf("Written %d bytes into Locator Successfully.\n\n", amtp);
	/* commit transaction */
	OCITransCommit(p_conn, p_err, 0);
	OCIDescriptorFree((dvoid *)v1, (ub4)OCI_DTYPE_LOB);
}


void selectTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OCILobLocator    *lobp;
	FILE *fp;
	ub1  buf[513];
	ub4  offset;
	ub4 lenp;
	ub4  amtp;

	OCIDefine *defhp1, *defhp2, *defhp3, *defhp4, *defhp5, *defhp6, *defhp7, *defhp8;
	OraText *sql = (OraText*)"select id,v1 from test1";
	ub4  sql_len = (ub4)strlen((char*)sql);

	sword ret = OCIStmtPrepare2(p_conn, &p_stmt,        /* returned stmt handle */
		p_err,                            /* error handle */
		sql,   /* input statement text*/
		sql_len,        /* length of text */
		NULL, 0,         /* tagging parameters: optional */
		OCI_NTV_SYNTAX, OCI_DEFAULT);

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
	OCILobLocator    *v1;
	OCIDescriptorAlloc(p_conn->p_env, (dvoid **)&v1, (ub4)OCI_DTYPE_LOB, (size_t)0, (dvoid **)0);
	ret = OCIDefineByPos(p_stmt, &defhp6, p_err, 2,
		(void *)v1, 0,
		SQLT_BLOB, (void *)NULL, (ub2 *)NULL,
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

	/* get length of lob */
	ret = OCILobGetLength(p_conn, p_err, v1, &lenp);

	printf("Read %d bytes from Locator Successfully.\n\n", lenp);
	offset = 0;
	/* Read the LOB - there will be less roundtrips since lob-prefetch data
	*   is fetched with the locator
	*/
	if ((fp = fopen("d:\\test1--11111111.txt", "wb")) == NULL)
	{
		printf("Cannot open file for reading \n");
		exit(1);
	}
	amtp = 0;
	do
	{
		memset((dvoid *)buf, '\0', 512);
		ret = OCILobRead(p_conn, p_err, v1, &amtp, offset,
			(dvoid *)buf, 512, (dvoid *)0,
			(OCICallbackLobRead)0, (ub2)0, (ub1)SQLCS_IMPLICIT);

		if (ret == OCI_SUCCESS || ret == OCI_NEED_DATA)
		{
			buf[512] = '\0';
			fwrite((void *)buf, amtp, 1, fp);
			/* printf ("%s", buf);  */
			offset += amtp;
		}
		else
		{
			printf("Read Fall from Locator.......... \n");
			break;
		}
	} while (offset < lenp);
	
	printf("Read %d bytes from Locator \n", --offset);
	fclose(fp);

	switch (ret)
	{
	case OCI_SUCCESS:
//		printf("id=[%d],v1=[%d],v2=[%d],v3=[%d],v4=[%d],v5=[%d].\n",id,v1.OCIDateDD,v2.OCIDateDD,v3.OCIDateDD,v4,v5);
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