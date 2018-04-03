/*
 * $Header: cdemo82.h 07-jun-2005.11:41:35 aliu Exp $
 */

/* Copyright (c) 1996, 2005, Oracle. All rights reserved.  
*/

/*
   NAME
     cdemo82.h - header file for oci adt sample program

   MODIFIED   (MM/DD/YY)
   aliu        06/06/05 - fix lrg 1844051 
   mjaeger     07/14/99 - bug 808870: OCCS: convert tabs, no long lines
   echen       06/03/97 - fix name resolution problem
   azhao       01/30/97 - fix lint error
   echen       01/03/97 - remove obsoleve type
   azhao       07/18/96 - not to include s.h
   dchatter    07/18/96 - delete spurious .h files
   slari       07/15/96 - Creation

*/


#ifndef CDEMO82_ORACLE
# define CDEMO82_ORACLE

#ifndef OCI_ORACLE
#include <oci.h>
#endif

/*---------------------------------------------------------------------------
                     PUBLIC TYPES AND CONSTANTS
  ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                     PRIVATE TYPES AND CONSTANTS
  ---------------------------------------------------------------------------*/
#define SERVER "ORACLE"
#define ADDRESS_TYPE_NAME "ADDRESS_OBJECT"
#define EMB_ADDRESS_TYPE_NAME "EMBEDDED_ADDRESS"
#define ADDREXT "ADDREXT"
#define EMBADDREXT "EMBADDREXT"
#define RETURN_ON_ERROR(error) if (error) return (error)
#define BIG_RECORD_SIZE 1000



struct address
{
  OCIString   *state;
  OCIString   *zip;
};
typedef struct address address;

struct null_address
{
  sb4    null_adt;
  sb4    null_state;
  sb4    null_zip;
};
typedef struct null_address null_address;

struct embaddress
{
  OCIString   *state;
  OCIString   *zip;
  OCIRef  *preaddrref;
};
typedef struct embaddress embaddress;


struct null_embaddress
{
  sb4     null_state;
  sb4     null_zip;
  sb4     null_preaddrref;
};
typedef struct null_embaddress null_embaddress;

struct person
{
  OCIString        *name;
  OCINumber           age;
  address          addr;
};
typedef struct person person;

struct null_person
{
  sb4              null_name;
  sb4              null_age;
  null_address     null_addr;
};

typedef struct null_person null_person;

static  text *  names[] =
{(text *) "CUSTOMERVAL", (text *) "ADDRESS", (text *) "STATE"};

static  text *  selvalstmt = (text *)
                     "SELECT custno, addr FROM customerval order by custno, addr.zip";

static  text *  selobjstmt = (text *)
                     "SELECT custno, addr FROM customerobj order by custno, addr.zip";

static  text *  selref = (text *)
                     "SELECT REF(e) from extaddr e";

static  text *  deleteref = (text *)
                     "DELETE extaddr";

static  text *  insertref = (text *)
"insert into extaddr values(address_object('CA', '98765'))";

static  text *  modifyref = (text *)
"update extaddr set object_column = address_object('TX', '61111')";

static  text *  selembref = (text *)
                     "SELECT REF(emb) from embextaddr emb";

static  text *  bndref = (text *)
"update extaddr set object_column.state = 'GA' where object_column = :addrref";

static  text *  insstmt =
(text *)"INSERT INTO customerval (custno, addr) values (:custno, :addr)";

dvoid *tmp;


/*---------------------------------------------------------------------------
                           PUBLIC FUNCTIONS
  ---------------------------------------------------------------------------*/
OCIRef *cbfunc(/*_ dvoid *context _*/);

/*---------------------------------------------------------------------------
                          PRIVATE FUNCTIONS
  ---------------------------------------------------------------------------*/
static void pin_display_addr(OCIEnv *envhp, OCIError *errhp, OCIRef *addrref);
static void checkerr(OCIError *errhp, sword status);
static void selectval(OCIEnv *envhp, OCISvcCtx *svchp, OCIStmt *stmthp, OCIError *errhp);
static void selectobj(OCIEnv *envhp, OCISvcCtx *svchp, OCIStmt *stmthp, OCIError *errhp);
static void insert(OCIEnv *envhp, OCISvcCtx *svchp, OCIStmt *stmthp, OCIError *errhp, text *insstmt, ub2   nrows);
int main();

int main();



#endif                                              /* cdemo82 */
