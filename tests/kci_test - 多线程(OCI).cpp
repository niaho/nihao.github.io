#include <stdio.h>
#include <process.h>
#include <string.h>
#include "time.h"
#include "kci_defs.h"b
extern "C"{
#include "ociap.h"
};

/*
CREATE TABLE EMPLOYEES (EMPLOYEE_ID NUMBER(6),first_name  VARCHAR2(25),LAST_NAME  VARCHAR2(25),SALARY  NUMBER(8,2), DEPARTMENT_ID  VARCHAR2(10),JOB_ID  VARCHAR2(10),EMAIL  VARCHAR2(25),HIRE_DATE  DATE);
*/

/* Maximum Number of threads  */
#define MAXTHREAD 100000
static ub4 sessMin = 100;
static ub4 sessMax = 1000;
static ub4 sessIncr = 5;

static OCIError   *errhp;
static OCIEnv     *envhp;
static OCISPool   *poolhp = (OCISPool *)0;
static int employeeNum[MAXTHREAD];

static OraText *poolName;
static ub4 poolNameLen;
static CONST OraText *database = (text *)"localhost:5139/system";
static CONST OraText *appusername = (text *)"SYSDBA";
static CONST OraText *apppassword = (text *)"SYSDBA";

/*  Values to be inserted into EMP table */
static char firstname[10][10] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" };
static char lastname[10][10] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J" };
static char email[10][20] = { "A@oracle.com", "B@oracle.com", "C@oracle.com",
"D@oracle.com", "E@oracle.com", "F@oracle.com",
"G@oracle.com", "H@oracle.com", "I@oracle.com",
"J@oracle.com" };

static char ejob[10][11] = { "FI_ACCOUNT", "AC_ACCOUNT", "SA_MAN", "PU_MAN",
"PU_CLERK", "IT_PROG", "MK_REP", "AD_VP", "AC_MGR",
"HR_REP" };
static float esal[10] = { 10000.00,
5000.00,
8000.00,
6000.00,
10000.00,
8000.00,
6000.00,
6000.00,
5000.00,
5000.00 };

static char hiredate[10][20] = { "1994-01-27", "1994-02-27", "1994-03-27", "1994-04-27", "1994-05-27", "1994-06-27", "1994-07-27", "1994-08-27", "1994-09-27", "94-10-27" };

static unsigned int edept[10] = { 10, 20, 10, 20, 30, 10, 30, 20, 10, 20 };

static void checkerr(OCIError *errhp, sword status);
static void threadFunction(dvoid *arg);

int main(void)
{
	int i = 0;
	sword lstat;
	int timeout = 1;
	OCIEnvCreate(&envhp, OCI_THREADED, (dvoid *)0, NULL,
		NULL, NULL, 0, (dvoid **)0);

	(void)OCIHandleAlloc((dvoid *)envhp, (dvoid **)&errhp, OCI_HTYPE_ERROR,
		(size_t)0, (dvoid **)0);

	(void)OCIHandleAlloc((dvoid *)envhp, (dvoid **)&poolhp, OCI_HTYPE_SPOOL,
		(size_t)0, (dvoid **)0);

	/* Create the session pool */
	checkerr(errhp, OCIAttrSet((dvoid *)poolhp,
		(ub4)OCI_HTYPE_SPOOL, (dvoid *)&timeout, (ub4)0,
		OCI_ATTR_SPOOL_TIMEOUT, errhp));

	if (lstat = OCISessionPoolCreate(envhp, errhp, poolhp, (OraText **)&poolName,
		(ub4 *)&poolNameLen, database,
		(ub4)strlen((const  char *)database),
		sessMin, sessMax, sessIncr,
		(OraText *)appusername,
		(ub4)strlen((const  char *)appusername),
		(OraText *)apppassword,
		(ub4)strlen((const  char *)apppassword),
		OCI_DEFAULT))
	{
		checkerr(errhp, lstat);
	}

	printf("Session Pool Created \n");

	/* Multiple threads using the session pool */
	{
		OCIThreadId *thrid[MAXTHREAD];
		OCIThreadHandle *thrhp[MAXTHREAD];

		OCIThreadProcessInit();
		checkerr(errhp, OCIThreadInit(envhp, errhp));
		for (i = 0; i < MAXTHREAD; ++i)
		{
			checkerr(errhp, OCIThreadIdInit(envhp, errhp, &thrid[i]));
			checkerr(errhp, OCIThreadHndInit(envhp, errhp, &thrhp[i]));
		}
		for (i = 0; i < MAXTHREAD; ++i)
		{
			employeeNum[i] = i;
			/* Inserting into EMP table */
			checkerr(errhp, OCIThreadCreate(envhp, errhp, threadFunction,
				(dvoid *)&employeeNum[i], thrid[i], thrhp[i]));
		}
		for (i = 0; i < MAXTHREAD; ++i)
		{
			checkerr(errhp, OCIThreadJoin(envhp, errhp, thrhp[i]));
			checkerr(errhp, OCIThreadClose(envhp, errhp, thrhp[i]));
			checkerr(errhp, OCIThreadIdDestroy(envhp, errhp, &(thrid[i])));
			checkerr(errhp, OCIThreadHndDestroy(envhp, errhp, &(thrhp[i])));
		}
		checkerr(errhp, OCIThreadTerm(envhp, errhp));
	} /* ALL THE THREADS ARE COMPLETE */
	lstat = OCISessionPoolDestroy(poolhp, errhp, OCI_DEFAULT);

	printf("Session Pool Destroyed \n");

	if (lstat != OCI_SUCCESS)
		checkerr(errhp, lstat);

	checkerr(errhp, OCIHandleFree((dvoid *)poolhp, OCI_HTYPE_SPOOL));

	checkerr(errhp, OCIHandleFree((dvoid *)errhp, OCI_HTYPE_ERROR));
	return 0;

} /* end of main () */

/* Inserts records into EMP table */
static void threadFunction(dvoid *arg)
{
//	return ;
	int empindex = *(int *)arg;
	int empno = empindex % 10;
	OCISvcCtx *svchp = (OCISvcCtx *)0;
	char insertst1[256];
	OCIStmt *stmthp = (OCIStmt *)0;
	OCIError  *errhp2 = (OCIError *)0;
	OCIAuthInfo *authp = (OCIAuthInfo *)0;
	sword lstat;
	text name[10];

	(void)OCIHandleAlloc((dvoid *)envhp, (dvoid **)&errhp2, OCI_HTYPE_ERROR,
		(size_t)0, (dvoid **)0);
	
	lstat = OCIHandleAlloc((dvoid *)envhp,
		(dvoid **)&authp, (ub4)OCI_HTYPE_AUTHINFO,
		(size_t)0, (dvoid **)0);
	if (lstat)
		checkerr(errhp2, lstat);

	checkerr(errhp2, OCIAttrSet((dvoid *)authp, (ub4)OCI_HTYPE_AUTHINFO,
		(dvoid *)appusername, (ub4)strlen((char *)appusername),
		(ub4)OCI_ATTR_USERNAME, errhp2));

	checkerr(errhp2, OCIAttrSet((dvoid *)authp, (ub4)OCI_HTYPE_AUTHINFO,
		(dvoid *)apppassword, (ub4)strlen((char *)apppassword),
		(ub4)OCI_ATTR_PASSWORD, errhp2));

	if (lstat = OCISessionGet(envhp, errhp2, &svchp, authp,
		(OraText *)poolName, (ub4)strlen((char *)poolName), NULL,
		0, NULL, NULL, NULL, OCI_SESSGET_SPOOL))
	{
		checkerr(errhp2, lstat);
	}
	memset(insertst1, 0, sizeof(insertst1));
	(void)sprintf(insertst1, "INSERT INTO EMPLOYEES (employee_id, first_name,\
		              last_name, email, hire_date, job_id, salary, department_id) \
					      values (%d, '%s', '%s', '%s','%s', '%s',%7.2f, %d);",
															   empindex, firstname[empno], lastname[empno], email[empno],
															   hiredate[empno], ejob[empno], esal[empno], edept[empno]);

	OCIHandleAlloc(envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, (size_t)0,	(dvoid **)0);

	checkerr(errhp2, OCIStmtPrepare(stmthp, errhp2, (CONST OraText *)insertst1,	(ub4)strlen((const char *)insertst1),OCI_NTV_SYNTAX, OCI_DEFAULT));
//	printf("Insert complete ..0...%d..%0x..\n", svchp->hMutex, &stmthp);
	checkerr(errhp2, OCIStmtExecute(svchp, stmthp, errhp2, (ub4)1, (ub4)0,	(OCISnapshot *)0, (OCISnapshot *)0,	OCI_DEFAULT));
//	printf(" ..¡¾1¡¿.....%d..%0x..\n", svchp->hMutex, &stmthp);
	checkerr(errhp2, OCITransCommit(svchp, errhp2, (ub4)0));

	checkerr(errhp2, OCIHandleFree((dvoid *)stmthp, OCI_HTYPE_STMT));
	checkerr(errhp2, OCISessionRelease(svchp, errhp2, NULL, 0, OCI_DEFAULT));
	OCIHandleFree((dvoid *)authp, OCI_HTYPE_AUTHINFO);
	OCIHandleFree((dvoid *)errhp2, OCI_HTYPE_ERROR);
//	printf(" Free£¡ \n");
} /* end of threadFunction (dvoid *) */

/* This function prints the error */
void checkerr(OCIError *errhp,sword status)
{
	text errbuf[512];
	sb4 errcode = 0;

	switch (status)
	{
	case OCI_SUCCESS:
		break;
	case OCI_SUCCESS_WITH_INFO:
		(void)printf("Error - OCI_SUCCESS_WITH_INFO\n");
		break;
	case OCI_NEED_DATA:
		(void)printf("Error - OCI_NEED_DATA\n");
		break;
	case OCI_NO_DATA:
		(void)printf("Error - OCI_NODATA\n");
		break;
	case OCI_ERROR:
		(void)OCIErrorGet((dvoid *)errhp, (ub4)1, (text *)NULL, &errcode,
			errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
		(void)printf("Error - %.*s\n", 512, errbuf);
		break;
	case OCI_INVALID_HANDLE:
		(void)printf("Error - OCI_INVALID_HANDLE\n");
		break;
	case OCI_STILL_EXECUTING:
		(void)printf("Error - OCI_STILL_EXECUTE\n");
		break;
	case OCI_CONTINUE:
		(void)printf("Error - OCI_CONTINUE\n");
		break;
	default:
		break;
	}
}