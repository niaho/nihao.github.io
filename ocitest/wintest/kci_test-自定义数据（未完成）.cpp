#include <stdio.h>
#include <process.h>
#include <string.h>
#include "kci_defs.h"
extern "C"{
#include "ociap.h"
};

static void showError(OCIError *p_err);
void insertTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);
void selectTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);


void createTestTypeTable(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err);

typedef OCIArray PROP_ARRAY;
typedef OCIRef PROP_TYPE_ref;

struct PROP_TYPE
{
	OCIString * property_name;
	OCIString * property_value;
};
typedef struct PROP_TYPE PROP_TYPE;

struct PROP_TYPE_ind
{
	OCIInd _atomic;
	OCIInd property_name;
	OCIInd property_value;
};
typedef struct PROP_TYPE_ind PROP_TYPE_ind;



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
	createTestTypeTable(p_conn, p_stmt, p_err);	
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



void createTestTypeTable(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OCIDefine *defhp1;
	OraText *sql = (OraText*)"select count(*) from user_tables where table_name = 'PROP_TABLE'";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, OCI_NTV_SYNTAX, OCI_DEFAULT);
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
		OraText *sql2 = (OraText*)"DROP TABLE PROP_TABLE;DROP TYPE PROP_ARRAY;DROP TYPE PROP_TYPE;";
		ub4  sql_len2 = (ub4)strlen((char*)sql2);
		OCIStmtPrepare(p_stmt, p_err, sql2, sql_len2, 0, OCI_DEFAULT);
		OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
	}

	OraText *sql3 = (OraText*)"CREATE TYPE PROP_TYPE AS OBJECT ( PROPERTY_NAME VARCHAR2(128), PROPERTY_VALUE VARCHAR2(255)); CREATE TYPE PROP_ARRAY AS VARRAY(30) OF PROP_TYPE;CREATE TABLE PROP_TABLE ( ID NUMBER PRIMARY KEY, PA PROP_ARRAY );";
	ub4  sql_len3 = (ub4)strlen((char*)sql3);
	OCIStmtPrepare(p_stmt, p_err, sql3, sql_len3, 0, OCI_DEFAULT);
	OCIStmtExecute(p_conn, p_stmt, p_err, 1, 0, NULL, NULL, OCI_DEFAULT);
}

void insertTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OraText *sql = (OraText*)"INSERT INTO prop_table( id, pa ) VALUES ( 2, :pa)";
	ub4  sql_len = (ub4)strlen((char*)sql);
	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);

	PROP_ARRAY *p_array = (PROP_ARRAY *)0;
	PROP_TYPE  *property = (PROP_TYPE *)0;
	PROP_TYPE_ind *property_ind = (PROP_TYPE_ind *)0;
	
	OCIDefine *defnp1 = (OCIDefine *)0;
	OCIBind   *bndnp1 = (OCIBind *)0;
	OCIType   *type_descriptor;
	OCIIter   *varray_iterator;

	printf("bind input varray\n");
	OCIBindByPos(p_stmt	, &bndnp1, p_err, 1, (dvoid *)p_array, (sb4)0, SQLT_NTY, (dvoid *)0	, (ub2 *)0, (ub2 *)0, (ub4)0, (ub4)0, (ub4)OCI_DEFAULT);
	printf("get type descriptor\n");
	OCITypeByName(p_conn->p_env, p_err, p_conn, (text *) "OCI", strlen("OCI"), (text *) "PROP_ARRAY", strlen("PROP_ARRAY"), (text *)0, 0, OCI_DURATION_SESSION, OCI_TYPEGET_ALL, &type_descriptor);

	printf("do the object binding step\n");
	OCIBindObject(bndnp1, p_err, type_descriptor, (dvoid **)p_array, (ub4 *)0, (void **)0, (ub4 *)0);
	printf("insert varray\n");
	OCIStmtExecute(p_conn, p_stmt, p_err,(ub4)1,(ub4)0,(CONST OCISnapshot *) NULL,(OCISnapshot *)NULL,OCI_DEFAULT);
	OCITransCommit(p_conn, p_err, OCI_DEFAULT);
	
}


void selectTestData(OCISvcCtx  *p_conn, OCIStmt    *p_stmt, OCIError   *p_err)
{
	OCIBind *bndp1;
	OraText *sql = (OraText*)"SELECT pa FROM prop_table WHERE ID = 1";
	ub4  sql_len = (ub4)strlen((char*)sql);


	PROP_ARRAY *p_array = (PROP_ARRAY *)0;
	PROP_TYPE  *property = (PROP_TYPE *)0;
	PROP_TYPE_ind *property_ind = (PROP_TYPE_ind *)0;

	OCIDefine *defnp1 = (OCIDefine *)0;
	OCIBind   *bndnp1 = (OCIBind *)0;
	OCIType   *type_descriptor;
	OCIIter   *varray_iterator;
	boolean end_of_collection = FALSE;

	sword ret = OCIStmtPrepare(p_stmt, p_err, sql, sql_len, 0, OCI_DEFAULT);
	printf("define return value\n");
	OCIDefineByPos(p_stmt, &defnp1, p_err, 1, (dvoid *)p_array, (sb4)0, SQLT_NTY, (dvoid *)0, (ub2 *)0, (ub2 *)0, (ub4)OCI_DEFAULT);

	printf("get type descriptor\n");
	OCITypeByName(p_conn->p_env, p_err, p_conn,	(text *) "OCI", strlen("OCI"),	(text *) "PROP_ARRAY", strlen("PROP_ARRAY"),	(text *)0, 0,OCI_DURATION_SESSION,	OCI_TYPEGET_ALL,&type_descriptor);

	printf("define object\n");
	OCIDefineObject(defnp1, p_err, type_descriptor, (dvoid **)p_array, (ub4 *)0, (void **)0, (ub4 *)0);

	printf("define new object\n");
	OCIObjectNew(p_conn->p_env, p_err, p_conn,OCI_TYPECODE_VARRAY, type_descriptor,0, OCI_DURATION_SESSION, TRUE, (void **)&p_array);

	printf("execute statement\n");
	OCIStmtExecute(p_conn, p_stmt, p_err, (ub4)1, (ub4)0,(CONST OCISnapshot *) NULL, (OCISnapshot *)NULL, OCI_DEFAULT);

	// create an iterator to scan through the collection
	// http://download.oracle.com/docs/cd/B19306_01/appdev.102/b14250/oci11oty.htm#sthref1794
	printf("create an iterator over the varray\n");
	OCIIterCreate(p_conn->p_env, p_err
		, (CONST OCIColl*) p_array            //OCIColl is 
		, &varray_iterator);
	printf("use iterator to retrieve next element\n");
	while ((OCIIterNext(p_conn->p_env, p_err
		, varray_iterator
		, (dvoid **)property
		, (dvoid **)property_ind
		, &end_of_collection
		) == OCI_SUCCESS) && !end_of_collection)
	{
		printf("property name  : %s \n", OCIStringPtr(p_conn->p_env, (*property).property_name));
		printf("property value : %s \n", OCIStringPtr(p_conn->p_env, (*property).property_value));
	}
	
}
