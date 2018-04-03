
/*************************************************************************** 
 * DIALOGS.H                                                               *
 *                                                                         *
 * @description: Definations needed for dialogboxes                        *
 *                                                                         *
 * @author     : MySQL AB(monty@mysql.com, venu@mysql.com)                 *
 * @date       : 2001-Aug-15                                               *
 * @product    : myodbc3                                                   *
 *                                                                         *
****************************************************************************/

#include "resource.h"
#include "odbc.h"

#define MAXPATHLEN  (255+1)    /* Max path length */
#define MAXKEYLEN   (15+1)     /* Max keyword length */
#define MAXDESC     (255+1)    /* Max description length */
#define MAXDSNAME   (64+1)     /* Max data source name length */
#define MAXSERVER   (64+1)     /* Max server name length */         
#define MAXUSER     (32+1)     /* Max user name length */
#define MAXPASSWORD (32+1)     /* Max password length */
#define MAXSTMT     (255+1)    /* Max initial statement length */

/* Attribute key indexes (into an array of Attr structs, see below */

#define KEY_DSN      0
#define KEY_DESC     1
#define KEY_DB       2
#define KEY_SERVER   3
#define KEY_USER     4
#define KEY_PASSWORD 5
#define KEY_PORT     6
#define KEY_AUTO_COMMIT 7
#define KEY_USE_SSL  8
#define KEY_CHAR_SET 9
#define KEY_TIME_ZONE 10
#define KEY_ISO_LEVEL 11
#define NUMOFKEYS    12
#define NUMOFBITS    22  /* Number of different options */


typedef struct tagAttr {
  bool fSupplied;
  char  szAttr[MAXPATHLEN];
} Attr, FAR * LPAttr;


typedef struct tagSETUPDLG {
  SQLHWND hwndParent;       /* Parent window handle */
  SQLHWND hwndDSN;          /* DSN window handle */
  SQLHWND hwndOption;       /* Options windows handle */
  LPCSTR  lpszDrvr;         /* Driver description */
  Attr  aAttr[NUMOFKEYS];   /* Attribute array */
  char  szDSN[MAXDSNAME];   /* Original data source name */
  bool  fNewDSN;            /* New data source flag */
  bool  fDefault;           /* Default data source flag */
  uint32 config_flags;       /* Flags to affect myodbc behavour */
} SETUPDLG, FAR *LPSETUPDLG;


int FAR PASCAL DriverConnectProc(HWND hdlg,WORD wMsg,WPARAM wParam,
				 LPARAM lParam);
int FAR PASCAL  HelpDlgProc(HWND, WORD, WPARAM, LPARAM);
void INTFUNC ParseAttributes (LPCSTR lpszAttributes, LPSETUPDLG lpsetupdlg);
void set_attributes(LPSETUPDLG lpsetupdlg);


