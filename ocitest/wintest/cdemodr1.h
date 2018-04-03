/*
 * $Header: cdemodr1.h 14-jul-99.12:43:57 mjaeger Exp $
 */

/* Copyright (c) 1997, 1999, Oracle Corporation.  All rights reserved.
*/

/* NOTE:  See 'header_template.doc' in the 'doc' dve under the 'forms'
      directory for the header file template that includes instructions.
*/

/*
   NAME
     cdemodr1.h - DML RETURNING demo program.

   DESCRIPTION
     Demonstrate INSERT/UPDATE/DELETE statements with RETURNING clause

   RELATED DOCUMENTS

   INSPECTION STATUS
     Inspection date:
     Inspection status:
     Estimated increasing cost defects per page:
     Rule sets:

   ACCEPTANCE REVIEW STATUS
     Review date:
     Review status:
     Reviewers:

   PUBLIC FUNCTION(S)
     <list of external functions declared/defined - with one-line descriptions>

   PRIVATE FUNCTION(S)
     <list of static functions defined in .c file - with one-line descriptions>

   EXAMPLES

   NOTES
     <other useful comments, qualifications, etc.>

   MODIFIED   (MM/DD/YY)
   mjaeger     07/14/99 - bug 808870: OCCS: convert tabs, no long lines
   svedala     10/01/98 - include stdlib.h - bug 714175
   svedala     09/09/98 - lines longer than 79 chars reformatted - bug 722491
   echen       07/30/97 - fix bug 516406
   azhao       05/30/97 - Creation

*/

/*------------------------------------------------------------------------
 * Include Files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kci_api.h"

/*
#include <orastd.h>
*/

/*------------------------------------------------------------------------
 * Define Constants
 */

#define MAXBINDS       25
#define MAXROWS         5           /* max no of rows returned per iter */
#define MAXCOLS        10
#define MAXITER        10           /* max no of iters in execute */
#define MAXCOLLEN      40           /* if changed, update cdemodr1.sql */
#define DATBUFLEN       7

int main(int argc, char *argv[]);
static sword init_handles( KCIEnv **envhp, KCISvcCtx **svchp,
                               KCIError **errhp, KCIServer **svrhp,
                               KCISession **authp, ub4 mode );

static sword attach_server( ub4 mode, KCIServer *srvhp,
                        KCIError *errhp, KCISvcCtx *svchp);
static sword log_on(KCISession *authp, KCIError *errhp, KCISvcCtx *svchp,
                 text *uid, text *pwd, ub4 credt, ub4 mode);
static sword init_bind_handle(KCIStmt *stmthp, KCIBind *bndhp[],    int nbinds );
static void print_raw(ub1 *raw, ub4 rawlen );

static void free_handles(KCIEnv *envhp, KCISvcCtx *svchp, KCIServer *srvhp,
                      KCIError *errhp, KCISession *authp, KCIStmt *stmthp );
void report_error(KCIError *errhp );
void logout_detach_server(KCISvcCtx *svchp, KCIServer *srvhp,
                              KCIError *errhp, KCISession *authp,
                              text *userid );
sword finish_demo(boolean loggedon, KCIEnv *envhp, KCISvcCtx *svchp,
                      KCIServer *srvhp, KCIError *errhp, KCISession *authp,
                      KCIStmt *stmthp, text *userid );
static sword demo_insert(KCISvcCtx *svchp, KCIStmt *stmthp,
                              KCIBind *bndhp[], KCIError *errhp);
static sword demo_update( KCISvcCtx *svchp, KCIStmt *stmthp,
                              KCIBind *bndhp[], KCIError *errhp );
static sword demo_delete(KCISvcCtx *svchp, KCIStmt *stmthp,
                              KCIBind *bndhp[], KCIError *errhp );
static sword bind_name(KCIStmt *stmthp, KCIBind *bndhp[],
                            KCIError *errhp );
static sword bind_pos(KCIStmt *stmthp, KCIBind *bndhp[],
                           KCIError *errhp);
static sword bind_input(KCIStmt *stmthp, KCIBind *bndhp[],
                             KCIError *errhp);
static sword bind_output( KCIStmt *stmthp, KCIBind *bndhp[],
                              KCIError *errhp );
static sword bind_array( KCIBind *bndhp[], KCIError *errhp );
static sword bind_dynamic( KCIBind *bndhp[], KCIError *errhp );
static sb4 cbf_no_data(dvoid *ctxp, KCIBind *bindp, ub4 iter, ub4 index,
                 dvoid **bufpp, ub4 *alenpp, ub1 *piecep, dvoid **indpp );
static sb4 cbf_get_data( dvoid *ctxp, KCIBind *bindp, ub4 iter, ub4 index,
                             dvoid **bufpp, ub4 **alenpp, ub1 *piecep,
                             dvoid **indpp, ub2 **rcodepp );
static sword alloc_buffer(ub4 pos, ub4 iter, ub4 rows );
static sword print_return_data( int iter);

