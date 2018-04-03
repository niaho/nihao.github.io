/*
 * $Header: cdemodr2.h 14-jul-99.12:44:25 mjaeger Exp $
 */

/* Copyright (c) 1997, 1999, Oracle Corporation.  All rights reserved.
*/

/* NOTE:  See 'header_template.doc' in the 'doc' dve under the 'forms'
      directory for the header file template that includes instructions.
*/

/*
   NAME
     tkp8rv2.c - DML Returning Test 2.

   DESCRIPTION
     Demonstrate INSERT/UPDATE/DELETE statements RETURNING LOBS.

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
   azhao       06/03/97 - new file
   azhao       06/03/97 - Creation

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kci_api.h.h"

#define MAXBINDS       25
#define MAXROWS         5           /* max no of rows returned per iter */
#define MAXCOLS         4
#define MAXITER        10           /* max no of iters in execute */
#define MAXBUFLEN      40

int main(int argc, char *argv[]);
sword init_handles(KCIEnv **envhp, KCISvcCtx **svchp,
                               KCIError **errhp, KCIServer **svrhp,
                               KCISession **authp, ub4 mode);

sword attach_server( ub4 mode, KCIServer *srvhp,
                        KCIError *errhp, KCISvcCtx *svchp );
sword log_on( KCISession *authp, KCIError *errhp, KCISvcCtx *svchp,
                        text *uid, text *pwd, ub4 credt, ub4 mode );
sword alloc_bind_handle( KCIStmt *stmthp, KCIBind *bndhp[],
                                   int nbinds );
void print_raw( ub1 *raw, ub4 rawlen);

void free_handles( KCIEnv *envhp, KCISvcCtx *svchp, KCIServer *srvhp,
                      KCIError *errhp, KCISession *authp, KCIStmt *stmthp );
void report_error(KCIError *errhp );
void logout_detach_server( KCISvcCtx *svchp, KCIServer *srvhp,
                              KCIError *errhp, KCISession *authp,
                              text *userid );
sword finish_demo( boolean loggedon, KCIEnv *envhp, KCISvcCtx *svchp,
                      KCIServer *srvhp, KCIError *errhp, KCISession *authp,
                      KCIStmt *stmthp, text *userid );
sword demo_insert( KCISvcCtx *svchp, KCIStmt *stmthp,
                              KCIBind *bndhp[], KCIError *errhp );
sword demo_update( KCISvcCtx *svchp, KCIStmt *stmthp,
                              KCIBind *bndhp[], KCIError *errhp );
sword demo_delete( KCISvcCtx *svchp, KCIStmt *stmthp,
                              KCIBind *bndhp[], KCIError *errhp );
sword bind_name( KCIStmt *stmthp, KCIBind *bndhp[], KCIError *errhp );
sword bind_in_name( KCIStmt *stmthp, KCIBind *bndhp[], KCIError *errhp );
sword bind_low_high( KCIStmt *stmthp, KCIBind *bndhp[],
                        KCIError *errhp );
sword bind_input( KCIStmt *stmthp, KCIBind *bndhp[], KCIError *errhp );
sword bind_output( KCIStmt *stmthp, KCIBind *bndhp[], KCIError *errhp );
sword bind_array( KCIBind *bndhp[], KCIError *errhp );
sword bind_dynamic( KCIBind *bndhp[], KCIError *errhp );
sb4 cbf_no_data(dvoid *ctxp, KCIBind *bindp, ub4 iter, ub4 index,
                 dvoid **bufpp, ub4 *alenpp, ub1 *piecep, dvoid **indpp );
sb4 cbf_get_data( dvoid *ctxp, KCIBind *bindp, ub4 iter, ub4 index,
                             dvoid **bufpp, ub4 **alenpp, ub1 *piecep,
                             dvoid **indpp, ub2 **rcodepp );
sword alloc_buffer( ub4 pos, ub4 iter, ub4 rows );
sword read_piece( KCISvcCtx *svchp, KCILobLocator *lob, ub2 lobtype );
sword write_piece( KCISvcCtx *svchp, KCILobLocator *lob, ub2 lobtype );
sword check_lob(int iter, KCISvcCtx *svchp );
sword locator_alloc( KCILobLocator *lob[], ub4 nlobs );
sword locator_free(KCILobLocator *lob[], ub4 nlobs );
sword select_locator( KCISvcCtx *svchp, KCIStmt *stmthp,
                                 KCIBind *bndhp[], KCIError *errhp );

