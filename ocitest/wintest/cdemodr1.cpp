#ifdef RCSID
static char *RCSid =
   "$Header: cdemodr1.c 14-jul-99.13:07:43 mjaeger Exp $ ";
#endif /* RCSID */

/* Copyright (c) 1997, 1999, Oracle Corporation.  All rights reserved.
*/

/*

   NAME
     cdemord1.c - C DEMO program for DML with RETURNING clause -  #1.

   DESCRIPTION
     This Demo program demonstrates the use of  INSERT/UPDATE/DELETE
     statements with a RETURNING clause in it.

   PUBLIC FUNCTION(S)
     <list of external functions declared/defined - with one-line descriptions>

   PRIVATE FUNCTION(S)
     <list of static functions defined in .c file - with one-line descriptions>

   RETURNS
     <function return values, for .c file with single function>

   NOTES
     Adopted from tkp8rv1.c; need to run cdemodr1.sql before running
     this demo program.

   MODIFIED   (MM/DD/YY)
   svedala     10/18/99 -
   mjaeger     07/14/99 - bug 808870: OCCS: convert tabs, no long lines
   svedala     09/09/98 - lines longer than 79 chars reformatted - bug 722491
   azhao       08/19/97 - remove explicit bindhp allocation
   echen       07/30/97 - fix bug 516406
   azhao       05/30/97 - Creation

*/

/*
create table tab1 (c1 integer, c2 char(40), c3 varchar2(40), c4 float,
c5 decimal, c6 decimal(8,3), c7 numeric, c8 numeric(7,2),
C9 date, C10 raw(40));
*/

#include "cdemodr1.h"

/*------------------------ Global Variables -------------------------------*/

static bool logged_on = false;

/* TAB1 columns */
static int   in1[MAXITER];                    /* for INTEGER      */
static char  in2[MAXITER][40];                /* for CHAR(40)     */
static char  in3[MAXITER][40];                /* for VARCHAR2(40) */
static float in4[MAXITER];                    /* for FLOAT        */
static int   in5[MAXITER];                    /* for DECIMAL      */
static float in6[MAXITER];                    /* for DECIMAL(8,3) */
static int   in7[MAXITER];                    /* for NUMERIC      */
static float in8[MAXITER];                    /* for NUMERIC(7,2) */
static ub1   in9[MAXITER][7];                 /* for DATE         */
static ub1   in10[MAXITER][40];               /* for RAW(40)      */

/* output buffers */
static int   *p1[MAXITER];                     /* for INTEGER      */
static text  *p2[MAXITER];                     /* for CHAR(40)     */
static text  *p3[MAXITER];                     /* for VARCHAR2(40) */
static float *p4[MAXITER];                     /* for FLOAT        */
static int   *p5[MAXITER];                     /* for DECIMAL      */
static float *p6[MAXITER];                     /* for DECIMAL(8,3) */
static int   *p7[MAXITER];                     /* for NUMERIC      */
static float *p8[MAXITER];                     /* for NUMERIC(7,2) */
static ub1   *p9[MAXITER];                     /* for DATE         */
static ub1   *p10[MAXITER];                    /* for RAW(40)      */

static short *ind[MAXCOLS][MAXITER];           /* indicators */
static ub2   *rc[MAXCOLS][MAXITER];            /* return codes */
static ub4   *rl[MAXCOLS][MAXITER];            /* return lengths */

/* skip values for binding TAB1 */
static ub4   s1 = (ub4) sizeof(in1[0]);
static ub4   s2 = (ub4) sizeof(in2[0]);
static ub4   s3 = (ub4) sizeof(in3[0]);
static ub4   s4 = (ub4) sizeof(in4[0]);
static ub4   s5 = (ub4) sizeof(in5[0]);
static ub4   s6 = (ub4) sizeof(in6[0]);
static ub4   s7 = (ub4) sizeof(in7[0]);
static ub4   s8 = (ub4) sizeof(in8[0]);
static ub4   s9 = (ub4) sizeof(in9[0]);
static ub4   s10= (ub4) sizeof(in10[0]);

/* Rows returned in each iteration */
static ub2 rowsret[MAXITER];

/*  indicator skips */
static ub4   indsk[MAXCOLS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/*  return length skips */
static ub4   rlsk[MAXCOLS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
/*  return code skips */
static ub4   rcsk[MAXCOLS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static int   lowc1[MAXITER], highc1[MAXITER];

static ub4   pos[MAXCOLS];

static KCIError *errhp;

/*------------------------end of Global variables--------------------*/


/*========================== UTILITY FUNCTIONS ======================*/
/*
 * These functions are generic functions that can be used in any
 * KCI program.
 */

/* ----------------------------------------------------------------- */
/* Initialize environment, allocate handles                          */
/* ----------------------------------------------------------------- */
sword init_handles(KCIEnv **envhp, KCISvcCtx **svchp,	KCIError **errhp, KCIServer **svrhp,	KCISession **authp, ub4 mode)
{
  (void) printf("Environment setup ....\n");
  /* Initialize the KCI Process */
  if (KCIInitialize(mode, (dvoid *)0,
                    (dvoid * (*)(dvoid *, size_t)) 0,
                    (dvoid * (*)(dvoid *, dvoid *, size_t))0,
                    (void (*)(dvoid *, dvoid *)) 0 ))
  {
    (void) printf("FAILED: KCIInitialize()\n");
    return KCI_ERROR;
  }

  /* Inititialize the KCI Environment */
  if (KCIEnvInit((KCIEnv **) envhp, (ub4) KCI_DEFAULT,
                 (size_t) 0, (dvoid **) 0 ))
  {
    (void) printf("FAILED: KCIEnvInit()\n");
    return KCI_ERROR;
  }

  /* Allocate a service handle */
  if (KCIHandleAlloc((dvoid *) *envhp, (dvoid **) svchp,
                     (ub4) KCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **) 0))
  {
    (void) printf("FAILED: KCIHandleAlloc() on svchp\n");
    return KCI_ERROR;
  }

  /* Allocate an error handle */
  if (KCIHandleAlloc((dvoid *) *envhp, (dvoid **) errhp,
                     (ub4) KCI_HTYPE_ERROR, (size_t) 0, (dvoid **) 0))
  {
    (void) printf("FAILED: KCIHandleAlloc() on errhp\n");
    return KCI_ERROR;
  }

  /* Allocate a server handle */
  if (KCIHandleAlloc((dvoid *)*envhp, (dvoid **)svrhp,
                     (ub4) KCI_HTYPE_SERVER, (size_t) 0, (dvoid **) 0))
  {
    (void) printf("FAILED: KCIHandleAlloc() on srvhp\n");
    return KCI_ERROR;
  }

  /* Allocate a authentication handle */
  if (KCIHandleAlloc((dvoid *) *envhp, (dvoid **) authp,
                     (ub4) KCI_HTYPE_SESSION, (size_t) 0, (dvoid **) 0))
  {
    (void) printf("FAILED: KCIHandleAlloc() on authp\n");
    return KCI_ERROR;
  }

  return KCI_SUCCESS;
}

/* ----------------------------------------------------------------- */
/* Attach to server with a given mode.                               */
/* ----------------------------------------------------------------- */
sword attach_server(ub4 mode, KCIServer *srvhp,	KCIError *errhp, KCISvcCtx *svchp)
{
  text *cstring = (text *)"localhost:6688/system";

  if (KCIServerAttach(srvhp, errhp, (text *) cstring,
                     (sb4) strlen((char *)cstring), (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: KCIServerAttach()\n");
    return KCI_ERROR;
  }

  /* Set the server handle in the service handle */
  if (KCIAttrSet((dvoid *) svchp, (ub4) KCI_HTYPE_SVCCTX,
                 (dvoid *) srvhp, (ub4) 0, (ub4) KCI_ATTR_SERVER, errhp))
  {
    (void) printf("FAILED: KCIAttrSet() server attribute\n");
    return KCI_ERROR;
  }

  return KCI_SUCCESS;
}
/* ----------------------------------------------------------------- */
/* Logon to the database using given username, password & credentials*/
/* ----------------------------------------------------------------- */

sword log_on(KCISession *authp, KCIError *errhp, KCISvcCtx *svchp,text *uid, text *pwd, ub4 credt, ub4 mode)
{
  /* Set attributes in the authentication handle */
  if (KCIAttrSet((dvoid *) authp, (ub4) KCI_HTYPE_SESSION,
                 (dvoid *) uid, (ub4) strlen((char *) uid),
                 (ub4) KCI_ATTR_USERNAME, errhp))
  {
    (void) printf("FAILED: KCIAttrSet() userid\n");
    return KCI_ERROR;
  }
  if (KCIAttrSet((dvoid *) authp, (ub4) KCI_HTYPE_SESSION,
                 (dvoid *) pwd, (ub4) strlen((char *) pwd),
                 (ub4) KCI_ATTR_PASSWORD, errhp))
  {
    (void) printf("FAILED: KCIAttrSet() passwd\n");
    return KCI_ERROR;
  }

  (void) printf("Logging on as %s  ....\n", uid);

  if (KCISessionBegin(svchp, errhp, authp, credt, mode))
  {
    (void) printf("FAILED: KCIAttrSet() passwd\n");
    return KCI_ERROR;
  }

   printf("%s logged on.\n", uid);

  /* Set the authentication handle in the Service handle */
  if (KCIAttrSet((dvoid *) svchp, (ub4) KCI_HTYPE_SVCCTX,
                 (dvoid *) authp, (ub4) 0, (ub4) KCI_ATTR_SESSION, errhp))
  {
    (void) printf("FAILED: KCIAttrSet() session\n");
    return KCI_ERROR;
  }

  return KCI_SUCCESS;
}

/*---------------------------------------------------------------------*/
/* Allocate all required bind handles                                  */
/*---------------------------------------------------------------------*/

sword init_bind_handle(KCIStmt *stmthp, KCIBind *bndhp[],int nbinds)
{
  int  i;
  /*
   * This function init the specified number of bind handles
   * from the given statement handle.
   */
  for (i = 0; i < nbinds; i++)
    bndhp[i] = (KCIBind *) 0;

  return KCI_SUCCESS;
}

/* ----------------------------------------------------------------- */
/* Print the returned raw data.                                      */
/* ----------------------------------------------------------------- */
void print_raw(ub1 *raw, ub4 rawlen)
{
  ub4 i;
  ub4 lim;
  ub4 clen = 0;

  if (rawlen > 120)
  {
    ub4 llen = rawlen;

    while (llen > 120)
    {
      lim = clen + 120;
      for(i = clen; i < lim; ++i)
          (void) printf("%02.2x", (ub4) raw[i] & 0xFF);

      (void) printf("\n");
      llen -= 120;
      clen += 120;
    }
    lim = clen + llen;
  }
  else
    lim = rawlen;

  for(i = clen; i < lim; ++i)
    (void) printf("%02.2x", (ub4) raw[i] & 0xFF);

  (void) printf("\n");

  return;
}

/* ----------------------------------------------------------------- */
/*  Free the specified handles                                       */
/* ----------------------------------------------------------------- */
void free_handles(KCIEnv *envhp, KCISvcCtx *svchp, KCIServer *srvhp,KCIError *errhp, KCISession *authp, KCIStmt *stmthp)
{
  (void) printf("Freeing handles ...\n");

  if (srvhp)
    (void) KCIHandleFree((dvoid *) srvhp, (ub4) KCI_HTYPE_SERVER);
  if (svchp)
    (void) KCIHandleFree((dvoid *) svchp, (ub4) KCI_HTYPE_SVCCTX);
  if (errhp)
    (void) KCIHandleFree((dvoid *) errhp, (ub4) KCI_HTYPE_ERROR);
  if (authp)
    (void) KCIHandleFree((dvoid *) authp, (ub4) KCI_HTYPE_SESSION);
  if (stmthp)
    (void) KCIHandleFree((dvoid *) stmthp, (ub4) KCI_HTYPE_STMT);
  if (envhp)
    (void) KCIHandleFree((dvoid *) envhp, (ub4) KCI_HTYPE_ENV);

  return;
}

/* ----------------------------------------------------------------- */
/* Print the error message                                           */
/* ----------------------------------------------------------------- */
void report_error(KCIError *errhp)
{
  text  msgbuf[512];
  sb4   errcode = 0;

  (void) KCIErrorGet((dvoid *) errhp, (ub4) 1, (text *) NULL, &errcode,
                       msgbuf, (ub4) sizeof(msgbuf), (ub4) KCI_HTYPE_ERROR);
  (void) printf("ERROR CODE = %d\n", errcode);
  (void) printf("%.*s\n", 512, msgbuf);
  return;
}

/*-------------------------------------------------------------------*/
/* Logout and detach from the server                                 */
/*-------------------------------------------------------------------*/
void logout_detach_server(KCISvcCtx *svchp, KCIServer *srvhp,KCIError *errhp, KCISession *authp,text *userid)
{
  if (KCISessionEnd(svchp, errhp, authp, (ub4) 0))
  {
    (void) printf("FAILED: KCISessionEnd()\n");
    report_error(errhp);
  }

  (void) printf("%s Logged off.\n", userid);

  if (KCIServerDetach(srvhp, errhp, (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: KCISessionEnd()\n");
    report_error(errhp);
  }

  (void) printf("Detached from server.\n");

  return;
}

/*---------------------------------------------------------------------*/
/* Finish demo and clean up                                            */
/*---------------------------------------------------------------------*/
sword finish_demo(boolean loggedon, KCIEnv *envhp, KCISvcCtx *svchp,KCIServer *srvhp, KCIError *errhp, KCISession *authp,KCIStmt *stmthp, text *userid)
{

  if (loggedon)
    logout_detach_server(svchp, srvhp, errhp, authp, userid);

  free_handles(envhp, svchp, srvhp, errhp, authp, stmthp);

  return KCI_SUCCESS;
}

/*===================== END OF UTILITY FUNCTIONS ======================*/

/*========================= MAIN ======================================*/

int main(int argc, char *argv[])
{
  text *username = (text *)"SYSDBA";
  text *password = (text *)"SYSDBA";

  KCIEnv *envhp;
  KCIServer *srvhp;
  KCISvcCtx *svchp;
  KCISession *authp;
  KCIStmt *stmthp = (KCIStmt *) NULL;
  KCIBind *bndhp[MAXBINDS];
  int i;


  printf("初始化KCI句柄...\r\n");
  /* Initialize the Environment and allocate handles */
  if (init_handles(&envhp, &svchp, &errhp, &srvhp, &authp, (ub4)KCI_DEFAULT))
  {
    (void) printf("FAILED: init_handles()\n");
    return finish_demo(logged_on, envhp, svchp, srvhp, errhp, authp,
                       stmthp, username);
  }

  printf("attach_server...\r\n");
  /* Attach to the database server */
  if (attach_server((ub4) KCI_DEFAULT, srvhp, errhp, svchp))
  {
    (void) printf("FAILED: attach_server()\n");
    return finish_demo(logged_on, envhp, svchp, srvhp, errhp, authp,
                       stmthp, username);
  }

  printf("log_on...\r\n");
  /* Logon to the server and begin a session */
  if (log_on(authp, errhp, svchp, username, password,
             (ub4) KCI_CRED_RDBMS, (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: log_on()\n");
    return finish_demo(logged_on, envhp, svchp, srvhp, errhp, authp,
                       stmthp, username);
  }
  logged_on = TRUE;

  /* Allocate a statement handle */
  if (KCIHandleAlloc((dvoid *)envhp, (dvoid **) &stmthp,
                     (ub4)KCI_HTYPE_STMT, (CONST size_t) 0, (dvoid **) 0))
  {
    (void) printf("FAILED: alloc statement handle\n");
    return finish_demo(logged_on, envhp, svchp, srvhp, errhp, authp,
                       stmthp, username);
  }

  /* bind handles will be implicitly allocated in the bind calls */
  /* need to initialize them to null prior to first usage in bind calls */

  for (i = 0; i < MAXBINDS; i++)
    bndhp[i] = (KCIBind *) 0;

  printf("按任意键进行插入操作...\r\n");
  getchar();

  /* Demonstrate INSERT with RETURNING clause */
  if (demo_insert(svchp, stmthp, bndhp, errhp))
    (void) printf("FAILED: demo_insert()\n");
  else
    (void) printf("SUCCESS: demo_insert()\n");

  printf("按任意键进行更改操作...\r\n");
  getchar();

  /* Demonstrate UPDATE with RETURNING clause */
  if (demo_update(svchp, stmthp, bndhp, errhp))
    (void) printf("FAILED: demo_update()\n");
  else
    (void) printf("SUCCESS: demo_update()\n");

  printf("按任意键进行删除操作...\r\n");
  getchar();

  /* Demonstrate DELETE with RETURNING clause */
  if (demo_delete(svchp, stmthp, bndhp, errhp))
    (void) printf("FAILED: demo_delete()\n");
  else
    (void) printf("SUCCESS: demo_delete()\n");

  printf("按任意键进行退出程序...\r\n");
  getchar();

  /* clean up */
  return finish_demo(logged_on, envhp, svchp, srvhp, errhp, authp,
                     stmthp, username);
}

/* =================== End Main =====================================*/

/* ===================== Local Functions ============================*/
/* ----------------------------------------------------------------- */
/* bind all the columns of TAB1 by positions.                        */
/* ----------------------------------------------------------------- */
static sword bind_pos(KCIStmt *stmthp, KCIBind *bndhp[], KCIError *errhp)
{

  if (KCIBindByPos(stmthp, &bndhp[0], errhp, (ub4) 1,
                      (dvoid *) &in1[0], (sb4) sizeof(in1[0]), SQLT_INT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByPos(stmthp, &bndhp[1], errhp, (ub4) 2,
                      (dvoid *) in2[0], (sb4) sizeof(in2[0]), SQLT_AFC,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByPos(stmthp, &bndhp[2], errhp, (ub4) 3,
                      (dvoid *) in3[0], (sb4) sizeof(in3[0]), SQLT_CHR,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByPos(stmthp, &bndhp[3], errhp, (ub4) 4,
                      (dvoid *) &in4[0], (sb4) sizeof(in4[0]), SQLT_FLT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByPos(stmthp, &bndhp[4], errhp, (ub4) 5,
                      (dvoid *) &in5[0], (sb4) sizeof(in5[0]), SQLT_INT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByPos(stmthp, &bndhp[5], errhp, (ub4) 6,
                      (dvoid *) &in6[0], (sb4) sizeof(in6[0]), SQLT_FLT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByPos(stmthp, &bndhp[6], errhp, (ub4) 7,
                      (dvoid *) &in7[0], (sb4) sizeof(in7[0]), SQLT_INT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByPos(stmthp, &bndhp[7], errhp, (ub4) 8,
                      (dvoid *) &in8[0], (sb4) sizeof(in8[0]), SQLT_FLT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByPos(stmthp, &bndhp[8], errhp, (ub4) 9,
                      (dvoid *) in9[0], (sb4) sizeof(in9[0]), SQLT_DAT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByPos(stmthp, &bndhp[9], errhp, (ub4) 10,
                      (dvoid *) in10[0], (sb4) sizeof(in10[0]), SQLT_BIN,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: KCIBindByPos()\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  return KCI_SUCCESS;
}


/* ----------------------------------------------------------------- */
/* bind all the columns of TAB1 by name.                             */
/* ----------------------------------------------------------------- */
static sword bind_name(KCIStmt *stmthp, KCIBind *bndhp[], KCIError *errhp)
{

  if (KCIBindByName(stmthp, &bndhp[10], errhp,
                      (text *) ":out1", (sb4) strlen((char *) ":out1"),
                      (dvoid *) 0, (sb4) sizeof(int), SQLT_INT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC)
   || KCIBindByName(stmthp, &bndhp[11], errhp,
                      (text *) ":out2", (sb4) strlen((char *) ":out2"),
                      (dvoid *) 0, (sb4) MAXCOLLEN, SQLT_AFC,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC)
   || KCIBindByName(stmthp, &bndhp[12], errhp,
                      (text *) ":out3", (sb4) strlen((char *) ":out3"),
                      (dvoid *) 0, (sb4) MAXCOLLEN, SQLT_CHR,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC)
   || KCIBindByName(stmthp, &bndhp[13], errhp,
                      (text *) ":out4", (sb4) strlen((char *) ":out4"),
                      (dvoid *) 0, (sb4) sizeof(float), SQLT_FLT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC)
   || KCIBindByName(stmthp, &bndhp[14], errhp,
                      (text *) ":out5", (sb4) strlen((char *) ":out5"),
                      (dvoid *) 0, (sb4) sizeof(int), SQLT_INT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC)
   || KCIBindByName(stmthp, &bndhp[15], errhp,
                      (text *) ":out6", (sb4) strlen((char *) ":out6"),
                      (dvoid *) 0, (sb4) sizeof(float), SQLT_FLT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC)
   || KCIBindByName(stmthp, &bndhp[16], errhp,
                      (text *) ":out7", (sb4) strlen((char *) ":out7"),
                      (dvoid *) 0, (sb4) sizeof(int), SQLT_INT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC)
   || KCIBindByName(stmthp, &bndhp[17], errhp,
                      (text *) ":out8", (sb4) strlen((char *) ":out8"),
                      (dvoid *) 0, (sb4) sizeof(float), SQLT_FLT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC)
   || KCIBindByName(stmthp, &bndhp[18], errhp,
                      (text *) ":out9", (sb4) strlen((char *) ":out9"),
                      (dvoid *) 0, (sb4) DATBUFLEN, SQLT_DAT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC)
   || KCIBindByName(stmthp, &bndhp[19], errhp,
                      (text *) ":out10", (sb4) strlen((char *) ":out10"),
                      (dvoid *) 0, (sb4) MAXCOLLEN, SQLT_BIN,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DATA_AT_EXEC))
  {
    (void) printf("FAILED: KCIBindByName()\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  return KCI_SUCCESS;
}


/* ----------------------------------------------------------------- */
/* bind array structs for TAB1 columns.                              */
/* ----------------------------------------------------------------- */
static sword bind_array(KCIBind *bndhp[], KCIError *errhp)
{
  if (KCIBindArrayOfStruct(bndhp[0], errhp, s1, indsk[0], rlsk[0], rcsk[0])
   || KCIBindArrayOfStruct(bndhp[1], errhp, s2, indsk[1], rlsk[1], rcsk[1])
   || KCIBindArrayOfStruct(bndhp[2], errhp, s3, indsk[2], rlsk[2], rcsk[2])
   || KCIBindArrayOfStruct(bndhp[3], errhp, s4, indsk[3], rlsk[3], rcsk[3])
   || KCIBindArrayOfStruct(bndhp[4], errhp, s5, indsk[4], rlsk[4], rcsk[4])
   || KCIBindArrayOfStruct(bndhp[5], errhp, s6, indsk[5], rlsk[5], rcsk[5])
   || KCIBindArrayOfStruct(bndhp[6], errhp, s7, indsk[6], rlsk[6], rcsk[6])
   || KCIBindArrayOfStruct(bndhp[7], errhp, s8, indsk[7], rlsk[7], rcsk[7])
   || KCIBindArrayOfStruct(bndhp[8], errhp, s9, indsk[8], rlsk[8], rcsk[8])
   || KCIBindArrayOfStruct(bndhp[9], errhp, s10, indsk[9], rlsk[9], rcsk[9]))
  {
    (void) printf("FAILED: KCIBindArrayOfStruct()\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  return KCI_SUCCESS;
}


/* ----------------------------------------------------------------- */
/* bind dynamic for returning TAB1 columns.                          */
/* ----------------------------------------------------------------- */
static sword bind_dynamic(KCIBind *bndhp[], KCIError *errhp)
{
  /*
   * Note here that both IN & OUT BIND callback functions have to be
   * provided.  However, since the bind variables in the RETURNING
   * clause are pure OUT Binds the IN callback fuctions (cbf_no_data)
   * is essentially a "do-nothing" function.
   *
   * Also note here that although in this demonstration the IN and OUT
   * callback functions are same, in practice you can have a different
   * callback function for each bind handle.
   */

  ub4 i;

  for (i = 0; i < MAXCOLS; i++)
    pos[i] = i;

  if (KCIBindDynamic(bndhp[10], errhp, (dvoid *) &pos[0], cbf_no_data,
                    (dvoid *) &pos[0], cbf_get_data)
  ||  KCIBindDynamic(bndhp[11], errhp, (dvoid *) &pos[1], cbf_no_data,
                    (dvoid *) &pos[1], cbf_get_data)
  ||  KCIBindDynamic(bndhp[12], errhp, (dvoid *) &pos[2], cbf_no_data,
                    (dvoid *) &pos[2], cbf_get_data)
  ||  KCIBindDynamic(bndhp[13], errhp, (dvoid *) &pos[3], cbf_no_data,
                    (dvoid *) &pos[3], cbf_get_data)
  ||  KCIBindDynamic(bndhp[14], errhp, (dvoid *) &pos[4], cbf_no_data,
                    (dvoid *) &pos[4], cbf_get_data)
  ||  KCIBindDynamic(bndhp[15], errhp, (dvoid *) &pos[5], cbf_no_data,
                    (dvoid *) &pos[5], cbf_get_data)
  ||  KCIBindDynamic(bndhp[16], errhp, (dvoid *) &pos[6], cbf_no_data,
                    (dvoid *) &pos[6], cbf_get_data)
  ||  KCIBindDynamic(bndhp[17], errhp, (dvoid *) &pos[7], cbf_no_data,
                    (dvoid *) &pos[7], cbf_get_data)
  ||  KCIBindDynamic(bndhp[18], errhp, (dvoid *) &pos[8], cbf_no_data,
                    (dvoid *) &pos[8], cbf_get_data)
  ||  KCIBindDynamic(bndhp[19], errhp, (dvoid *) &pos[9], cbf_no_data,
                    (dvoid *) &pos[9], cbf_get_data))
  {
    (void) printf("FAILED: KCIBindDynamic()\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  return KCI_SUCCESS;
}



/* ----------------------------------------------------------------- */
/* bind input variables.                                             */
/* ----------------------------------------------------------------- */
static sword bind_input(KCIStmt *stmthp, KCIBind *bndhp[], KCIError *errhp)
{
  /* bind the input data by positions */
  if (bind_pos(stmthp, bndhp, errhp))
    return KCI_ERROR;

  /* bind input array attributes*/
  return (bind_array(bndhp, errhp));
}



/* ----------------------------------------------------------------- */
/* bind output variables.                                            */
/* ----------------------------------------------------------------- */
static sword bind_output(KCIStmt *stmthp, KCIBind *bndhp[], KCIError *errhp)
{

  /* bind the returning bind buffers by names */
  if (bind_name(stmthp, bndhp, errhp))
    return KCI_ERROR;

  /* bind the returning bind buffers dynamically */
  return (bind_dynamic(bndhp, errhp));
}


/* ----------------------------------------------------------------- */
/* bind row indicator variables.                                     */
/* ----------------------------------------------------------------- */
static sword bind_low_high(KCIStmt *stmthp, KCIBind *bndhp[], KCIError *errhp)
{
  if (KCIBindByName(stmthp, &bndhp[23], errhp,
                      (text *) ":low", (sb4) strlen((char *) ":low"),
                      (dvoid *) &lowc1[0], (sb4) sizeof(lowc1[0]), SQLT_INT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT)
   || KCIBindByName(stmthp, &bndhp[24], errhp,
                      (text *) ":high", (sb4) strlen((char *) ":high"),
                      (dvoid *) &highc1[0], (sb4) sizeof(highc1[0]), SQLT_INT,
                      (dvoid *) 0, (ub2 *)0, (ub2 *)0,
                      (ub4) 0, (ub4 *) 0, (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: KCIBindByName()\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  if (KCIBindArrayOfStruct(bndhp[23], errhp, s1, indsk[0], rlsk[0], rcsk[0])
   || KCIBindArrayOfStruct(bndhp[24], errhp, s1, indsk[0], rlsk[0], rcsk[0]))
  {
    (void) printf("FAILED: KCIBindArrayOfStruct()\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  return KCI_SUCCESS;
}


/* ----------------------------------------------------------------- */
/* Demontrate  INSERT with RETURNING clause.                         */
/* ----------------------------------------------------------------- */
static sword demo_insert(KCISvcCtx *svchp, KCIStmt *stmthp,
                            KCIBind *bndhp[], KCIError *errhp)
{
  int   i, j;

  /*
   * This function inserts values for 10 columns in table TAB1 and
   * uses the RETURN clause to get back the inserted column values.
   * It inserts  MAXITER (10) such rows.  Thus it expects MAXITER values
   * for each column to be returned.
   */
  /* The Insert SQLSTMT with RETURNING clause */
  text *sqlstmt = (text *)
        "INSERT INTO TAB1 VALUES (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10) \
                     RETURNING C1, C2, C3, C4, C5, C6, C7, C8, C9, C10 \
                     INTO :out1, :out2, :out3, :out4, :out5, :out6, \
                          :out7, :out8, :out9, :out10";

  /* Prepare the statement */
  if (KCIStmtPrepare(stmthp, errhp, sqlstmt, (ub4)strlen((char *)sqlstmt),
                    (ub4) KCI_NTV_SYNTAX, (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: KCIStmtPrepare() insert\n");
    report_error(errhp);
    return KCI_ERROR;
  }


  /* Initialise the buffers for update */
  for (i = 0; i < MAXITER; i++)
  {
    in1[i] = i + 101;
    memset((void *)in2[i], (int) 'A'+i%26, (size_t) 40);
    memset((void *)in3[i], (int) 'a'+i%26, (size_t) 40);
    in4[i] = 400.555 + (float) i;
    in5[i] = 500 + i;
    in6[i] = 600.250 + (float) i;
    in7[i] = 700 + i;
    in8[i] = 800.350 + (float) i;
    in9[i][0] = 119;
    in9[i][1] = 185 + (ub1)i%10;
	in9[i][2] = (ub1)i%12 + 1;
    in9[i][3] = (ub1)i%25 + 1;
    in9[i][4] = 0;
    in9[i][5] = 0;
    in9[i][6] = 0;
    for (j = 0; j < 40; j++)
      in10[i][j] = (ub1) (i%0x10);

    rowsret[i] = 0;
  }

  /* Bind all the input buffers to place holders (:1, :2. :3, etc ) */
  if (bind_input(stmthp, bndhp, errhp))
    return KCI_ERROR;

  /* Bind all the output buffers to place holders (:out1, :out2 etc */
  if (bind_output(stmthp, bndhp, errhp))
    return KCI_ERROR;

  /* Execute the Insert statement */
  if (KCIStmtExecute(svchp, stmthp, errhp, (ub4) MAXITER, (ub4) 0,
                    (CONST KCISnapshot*) 0, (KCISnapshot*) 0,
                    (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: KCIStmtExecute() insert\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  /* Commit the changes */
  (void) KCITransCommit(svchp, errhp, (ub4) 0);

  /* Print out the values in the return rows */
  (void) printf("\n\n DEMONSTRATING INSERT....RETURNING \n");
  (void) print_return_data((int)MAXITER);

  return KCI_SUCCESS;
}



/* ----------------------------------------------------------------- */
/* Demonstrate  UPDATE with RETURNING clause.                        */
/* ----------------------------------------------------------------- */
static sword demo_update(KCISvcCtx *svchp, KCIStmt *stmthp,
                          KCIBind *bndhp[], KCIError *errhp)
{
  int   i, j;
  int   range_size = 3;                                       /* iterations */


  /*
   * This function updates columns in table TAB1, for certain rows
   * depending on the values of the :low and :high values in
   * in the WHERE clause. It executes this statement 3 times, (3 iterations)
   * each time with a different set of values for :low and :high
   * Thus for each iteration, multiple rows are returned depending
   * on the number of rows that matched the WHERE clause.
   *
   * The rows it updates here are the rows that were inserted by the
   * cdemodr1.sql script.
   */

  /* The Update SQLSTMT with RETURNING clause */
  text *sqlstmt = (text *)
                   "UPDATE TAB1 SET C1 = C1 + :1, C2 = :2, C3 = :3, \
                           C4 = C4 + :4, C5 = C5 + :5, C6 = C6 + :6, \
                           C7 = C7 + :7, C8 = C8 + :8, C9 = :9, C10 = :10 \
                           WHERE C1 >= :low AND C1 <= :high \
                           RETURNING C1, C2, C3, C4, C5, C6, C7, C8, C9, C10 \
                           INTO :out1, :out2, :out3, :out4, :out5, :out6, \
                           :out7, :out8, :out9, :out10";

  /* Prepare the statement */
  if (KCIStmtPrepare(stmthp, errhp, sqlstmt, (ub4)strlen((char *)sqlstmt),
                    (ub4) KCI_NTV_SYNTAX, (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: KCIStmtPrepare() update\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  /* Initialise the buffers for insertion */
  for (i = 0; i < MAXITER; i++)
  {
    in1[i] = 100 + i;
    memset((void *)in2[i], (int) 'a'+i%26, (size_t) 40);
    memset((void *)in3[i], (int) 'A'+i%26, (size_t) 40);
    in4[i] = 400.555 + (float)i;
    in5[i] = 500 + i;
    in6[i] = 600.280 + (float)i;
    in7[i] = 700 + i;
    in8[i] = 800.620 + (float)i;
    in9[i][0] = 119;
    in9[i][1] = 185 - (ub1)i%10;
    in9[i][2] = (ub1)i%12 + 1;
    in9[i][3] = (ub1)i%25 + 1;
    in9[i][4] = 0;
    in9[i][5] = 0;
    in9[i][6] = 0;
    for (j = 0; j < 40; j++)
      in10[i][j] = (ub1) (i%0x08);

    rowsret[i] =0;
  }

  /* Bind all the input buffers to place holders (:1, :2. :3, etc ) */
  if (bind_input(stmthp, bndhp, errhp))
    return KCI_ERROR;

  /* Bind all the output buffers to place holders (:out1, :out2 etc */
  if (bind_output(stmthp, bndhp, errhp))
    return KCI_ERROR;

  /* bind row indicator low, high */
  if (bind_low_high(stmthp, bndhp, errhp))
    return KCI_ERROR;

  /* update rows
         between 101 and 103;  --  expecting 3 rows returned (update 3 rows)
         between 105 and 106;  --  expecting 2 rows returned (update 2 rows)
         between 109 and 113;  --  expecting 5 rows returned (update 5 rows)
  */
  lowc1[0] = 101;
  highc1[0] = 103;

  lowc1[1] = 105;
  highc1[1] = 106;

  lowc1[2] = 109;
  highc1[2] = 113;

  (void) printf("\n\n DEMONSTRATING UPDATE....RETURNING \n");
  if (KCIStmtExecute(svchp, stmthp, errhp, (ub4) range_size, (ub4) 0,
                    (CONST KCISnapshot*) 0, (KCISnapshot*) 0,
                    (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: KCIStmtExecute() update\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  /* Commit the changes */
  (void) KCITransCommit(svchp, errhp, (ub4) 0);

  /* Print out the values in the return rows */
  (void) print_return_data(range_size);

  return KCI_SUCCESS;
}



/* ----------------------------------------------------------------- */
/* Demonstrate  DELETE with RETURNING clause.                        */
/* ----------------------------------------------------------------- */
static sword demo_delete(KCISvcCtx *svchp, KCIStmt *stmthp,
                          KCIBind *bndhp[], KCIError *errhp)
{
  int   i, range_size = 3;                                    /* iterations */
  sword  retval;

  /*
   * This function deletes certain rows from table TAB1
   * depending on the values of the :low and :high values in
   * the WHERE clause. It executes this statement 3 times, (3 iterations)
   * each time with a different set of values for :low and :high
   * Thus for each iteration, multiples rows are returned depending
   * on the number of rows that matched the WHERE clause.
   *
   * The rows it deletes here are the rows that were inserted by the
   * cdemodr1.sql script.
   */

  /* The Delete SQLSTMT with RETURNING clause */
  text *sqlstmt = (text *)
          "DELETE FROM TAB1 WHERE C1 >= :low AND C1 <= :high \
                     RETURNING C1, C2, C3, C4, C5, C6, C7, C8, C9, C10 \
                     INTO :out1, :out2, :out3, :out4, :out5, :out6, \
                          :out7, :out8, :out9, :out10";

  /* Prepare the statement */
  if (KCIStmtPrepare(stmthp, errhp, sqlstmt, (ub4)strlen((char *)sqlstmt),
                    (ub4) KCI_NTV_SYNTAX, (ub4) KCI_DEFAULT))
  {
    (void) printf("FAILED: KCIStmtPrepare() delete\n");
    report_error(errhp);
    return KCI_ERROR;
  }

  /* Bind all the output buffers to place holders (:out1, :out2 etc */
  if (bind_output(stmthp, bndhp, errhp))
    return KCI_ERROR;

  /* bind row indicator low, high */
  if (bind_low_high(stmthp, bndhp, errhp))
    return KCI_ERROR;

  /* delete rows
         between 201 and 203;  --  expecting 3 rows returned (3 rows deleted)
         between 205 and 209;  --  expecting 5 rows returned (2 rows deleted)
         between 211 and 213;  --  expecting 3 rows returned (5 rows deleted)
  */
  lowc1[0] = 201;
  highc1[0] = 203;

  lowc1[1] = 205;
  highc1[1] = 209;

  lowc1[2] = 211;
  highc1[2] = 213;


  for (i=0; i<MAXITER; i++)
    rowsret[i] = 0;

  (void) printf("\n\n Demonstrating DETELE....RETURNING \n");
  if ((retval = KCIStmtExecute(svchp, stmthp, errhp, (ub4) range_size, (ub4) 0,
                             (CONST KCISnapshot*) 0, (KCISnapshot*) 0,
                             (ub4) KCI_DEFAULT)) != KCI_SUCCESS  &&
                              retval != KCI_SUCCESS_WITH_INFO)
  {
    (void) printf("FAILED: KCIStmtExecute() delete, retval = %d\n", retval);
    report_error(errhp);
  }

  /* Commit the changes */
  (void) KCITransCommit(svchp, errhp, (ub4) 0);

  /* Print out the values in the return rows */
  (void) print_return_data(range_size);

  return KCI_SUCCESS;
}



/* ----------------------------------------------------------------- */
/* IN bind callback that does not do any data input.                 */
/* ----------------------------------------------------------------- */
static sb4 cbf_no_data(dvoid *ctxp, KCIBind *bindp, ub4 iter, ub4 index,
                      dvoid **bufpp, ub4 *alenpp, ub1 *piecep, dvoid **indpp)
{
  /*
   * This is a dummy input callback function that provides input data
   * for the bind variables in the RETURNING clause.
   */
  *bufpp = NULL;
  *alenpp = 0;
  *indpp = NULL;
  *piecep = KCI_ONE_PIECE;

  return KCI_CONTINUE;
}

/* ----------------------------------------------------------------- */
/* Outbind callback for returning data.                              */
/* ----------------------------------------------------------------- */
static sb4 cbf_get_data(dvoid *ctxp, KCIBind *bindp, ub4 iter, ub4 index,
                         dvoid **bufpp, ub4 **alenp, ub1 *piecep,
                         dvoid **indpp, ub2 **rcodepp)
{
  /*
   * This is the callback function that is called to receive the OUT
   * bind values for the bind variables in the RETURNING clause
   */

if (iter > 20)
		printf("iter error(%d)\r\n", iter);


  static ub4  rows = 0;
  ub4    pos = *((ub4 *)ctxp);

  /* For each iteration the KCI_ATTR_ROWS_RETURNED tells us the number
   * of rows returned in that iteration.  So we can use this information
   * to dynamically allocate storage for all the returned rows for that
   * bind.
   */
  if (index == 0)
  {
    (void) KCIAttrGet((void *)bindp, KCI_HTYPE_BIND, (dvoid *)&rows,
                       (ub4 *) sizeof(ub4), KCI_ATTR_ROWS_RETURNED, errhp);
    rowsret[iter] = (ub2)rows;

    /* Dynamically allocate storage */
    if (alloc_buffer(pos, iter, rows))
      return KCI_ERROR;
  }

  /* Provide the address of the storage where the data is to be returned */
  switch(pos)
  {
  case 0:
    rl[pos][iter][index] = sizeof(int);
    *bufpp =  (dvoid *) (p1[iter]+ index);
    break;
  case 1:
    rl[pos][iter][index] = (ub4) MAXCOLLEN;
    *bufpp =  (dvoid *) (p2[iter]+(index * MAXCOLLEN));
    break;
  case 2:
    rl[pos][iter][index] = (ub4) MAXCOLLEN;
    *bufpp =  (dvoid *) (p3[iter]+(index * MAXCOLLEN));
    break;
  case 3:
    rl[pos][iter][index] = sizeof(float);
    *bufpp =  (dvoid *) (p4[iter]+ index);
    break;
  case 4:
    rl[pos][iter][index] = sizeof(int);
    *bufpp =  (dvoid *) (p5[iter]+index);
    break;
  case 5:
    rl[pos][iter][index] = sizeof(float);
    *bufpp =  (dvoid *) (p6[iter]+index );
    break;
  case 6:
    rl[pos][iter][index] = sizeof(int);
    *bufpp =  (dvoid *) (p7[iter]+ index);
    break;
  case 7:
    rl[pos][iter][index] = sizeof(float);
    *bufpp =  (dvoid *) (p8[iter]+index);
    break;
  case 8:
    rl[pos][iter][index] = DATBUFLEN;
    *bufpp =  (dvoid *) (p9[iter]+(index * DATBUFLEN));
    break;
  case 9:
    rl[pos][iter][index] = (ub4) MAXCOLLEN;
    *bufpp =  (dvoid *) (p10[iter]+(index * MAXCOLLEN));
    break;
  default:
    *bufpp =  (dvoid *) 0;
    *alenp =  (ub4 *) 0;
    (void) printf("ERROR: invalid position number: %d\n", *((ub2 *)ctxp));
  }

  *piecep = KCI_ONE_PIECE;

  /* provide address of the storage where the indicator will be returned */
  ind[pos][iter][index] = 0;
  *indpp = (dvoid *) &ind[pos][iter][index];


  /* provide address of the storage where the return code  will be returned */
  rc[pos][iter][index] = 0;
  *rcodepp = &rc[pos][iter][index];

  /*
   * provide address of the storage where the actual length  will be
   * returned
   */
  *alenp = &rl[pos][iter][index];

  return KCI_CONTINUE;
}



/* ----------------------------------------------------------------- */
/* allocate buffers for callback.                                    */
/* ----------------------------------------------------------------- */
static sword alloc_buffer(ub4 pos, ub4 iter, ub4 rows)
{
  switch(pos)
  {
  case 0:
    p1[iter] = (int *) malloc(sizeof(int) * rows);
    break;
  case 1:
    p2[iter] = (text *) malloc(rows * MAXCOLLEN);
    break;
  case 2:
    p3[iter] = (text *) malloc(rows * MAXCOLLEN);
    break;
  case 3:
    p4[iter] = (float *) malloc(sizeof(float) * rows);
    break;
  case 4:
    p5[iter] = (int *) malloc(sizeof(int) * rows);
    break;
  case 5:
    p6[iter] = (float *) malloc(sizeof(float) * rows);
    break;
  case 6:
    p7[iter] = (int *) malloc(sizeof(int) * rows);
    break;
  case 7:
    p8[iter] = (float *) malloc(sizeof(float) * rows);
    break;
  case 8:
    p9[iter] = (ub1 *) malloc(rows * DATBUFLEN);
    break;
  case 9:
    p10[iter] = (ub1 *) malloc(rows * MAXCOLLEN);
    break;
  default:
    (void) printf("ERROR: invalid position number: %d\n", pos);
    return KCI_ERROR;
  }

  ind[pos][iter] = (short *) malloc(rows * sizeof(short));
  rc[pos][iter] = (ub2 *) malloc(rows * sizeof(ub2));
  rl[pos][iter] = (ub4 *) malloc(rows * sizeof(ub4));

  return KCI_SUCCESS;
}



/* ----------------------------------------------------------------- */
/* print the returned data.                                          */
/* ----------------------------------------------------------------- */
static sword print_return_data(int iters)
{
  int i, j;

  for (i = 0; i < iters; i++)
  {
    (void) printf("\n*** ITERATION *** : %d\n", i);
    (void) printf("(...returning %d rows)\n",  rowsret[i]);

    for (j = 0; j < rowsret[i]  ; j++)
    {
      /* Column 1 */
      (void) printf("COL1 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[0][i][j], rc[0][i][j], rl[0][i][j]);
      if (ind[0][i][j] == -1)
        (void) printf("COL1 [%d]: NULL\n", j);
      else
        (void) printf("COL1 [%d]: %d\n", j, *(p1[i]+j) );

      /* Column 2 */
      (void) printf("COL2 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[1][i][j], rc[1][i][j], rl[1][i][j]);
      if (ind[1][i][j] == -1)
        (void) printf("COL2 [%d]: NULL\n", j);
      else
        (void) printf("COL2 [%d]: %.*s\n",j, rl[1][i][j], p2[i]+(j*MAXCOLLEN));

      /* Column 3 */
      (void) printf("COL3 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[2][i][j], rc[2][i][j], rl[2][i][j]);
      if (ind[2][i][j] == -1)
        (void) printf("COL3 [%d]: NULL\n", j);
      else
        (void) printf("COL3 [%d]: %.*s\n",j, rl[2][i][j], p3[i]+(j*MAXCOLLEN));
      /* Column 4 */
      (void) printf("COL4 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[3][i][j], rc[3][i][j], rl[3][i][j]);
      if (ind[3][i][j] == -1)
        (void) printf("COL4 [%d]: NULL\n", j);
      else
        (void) printf("COL4 [%d]: %8.3f\n", j, *(p4[i]+j) );

      /* Column 5 */
      (void) printf("COL5 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[4][i][j], rc[4][i][j], rl[4][i][j]);
      if (ind[4][i][j] == -1)
        (void) printf("COL5 [%d]: NULL\n", j);
      else
        (void) printf("COL5 [%d]: %d\n", j, *(p5[i]+j) );

      /* Column 6 */
      (void) printf("COL6 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[5][i][j], rc[5][i][j], rl[5][i][j]);
      if (ind[5][i][j] == -1)
        (void) printf("COL6 [%d]: NULL\n", j);
      else
        (void) printf("COL6 [%d]: %8.3f\n", j, *(p6[i]+j) );

      /* Column 7 */
      (void) printf("COL7 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[6][i][j], rc[6][i][j], rl[6][i][j]);
      if (ind[6][i][j] == -1)
        (void) printf("COL7 [%d]: NULL\n", j);
      else
        (void) printf("COL7 [%d]: %d\n", j, *(p7[i]+j) );

      /* Column 8 */
      (void) printf("COL8 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[7][i][j], rc[7][i][j], rl[7][i][j]);
      if (ind[7][i][j] == -1)
        (void) printf("COL8 [%d]: NULL\n", j);
      else
        (void) printf("COL8 [%d]: %8.3f\n", j, *(p8[i]+j) );

      /* Column 9 */
      (void) printf("COL9 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[8][i][j], rc[8][i][j], rl[8][i][j]);
      if (ind[8][i][j] == -1)
        (void) printf("COL9 [%d]: NULL\n", j);
      else
        (void) printf("COL9 [%d]: %u-%u-%u%u\n", j,
                                     *(p9[i]+(j*DATBUFLEN+3)),
                                     *(p9[i]+(j*DATBUFLEN+2)),
                                     *(p9[i]+(j*DATBUFLEN+0)) - 100,
                                     *(p9[i]+(j*DATBUFLEN+1)) - 100 );

      /* Column 10 */
      (void) printf("COL10 [%d]: ind = %d, rc = %d, retl = %d\n",
                            j, ind[9][i][j], rc[9][i][j], rl[9][i][j]);
      if (ind[9][i][j] == -1)
        (void) printf("COL10 [%d]: NULL\n", j);
      else
      {
        (void) printf("COL10 [%d]: ", j);
        print_raw(p10[i]+(j*MAXCOLLEN), rl[9][i][j]);
      }
      (void) printf("\n");
    }
  }

  return KCI_SUCCESS;
}


/* end of file cdemodr1.c */

