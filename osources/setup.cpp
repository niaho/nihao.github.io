#include  "odbc.h"
#include  "odbcinst.h"
#undef	 VOID         /* Becasue of ctl3d.h */
#define	 VOID void
#include <string.h>
#include "dialogs.h"
#include <winuser.h>
#include <windowsx.h>

extern HINSTANCE NEAR s_hModule;

/* Constants */
const char EMPTYSTR  []= "";

/* ODBC.INI keywords */
const char ODBC_INI    []="ODBC.INI";    /* ODBC initialization file */
const char INI_KDESC   []="Description"; /* Data source description */
const char INI_KDB     []="Database";
const char INI_KSERVER []="Server";
const char INI_KUSER   []="User";   /* Second option */
const char INI_KPASSWORD[] = "Password";
const char INI_KPORT   []  = "Port";
const char INI_AUTO_COMMIT[]  = "AutoCommit";
const char INI_USE_SSL []  = "UseSSL";
const char INI_ISO_LEVEL[]  = "IsoLevel";
const char INI_CHAR_SET[]  = "CharSet";
const char INI_TIME_ZONE[]  = "TimeZone";
const char INI_SDEFAULT[]= "Default"; 
const char szTranslateName[] = "TranslationName";
const char szTranslateDLL[] = "TranslationDLL";
const char szTranslateOption[] = "TranslationOption";
const char szUnkTrans[] = "Unknown Translator";
const char szDriverIniKey[] = "SOFTWARE\\ODBC\\ODBCINST.INI\\XuguSQL Server ODBC 2.0 Driver";
const char szHelpFileKey[] = "DSNHelpFile";

/* Attribute string look-up table (maps keys to associated indexes) */
static struct {
	char  szKey[MAXKEYLEN];
	int  iKey;
} s_aLookup[] = { "DSN",  KEY_DSN,
"DESC", KEY_DESC,
"Description",KEY_DESC,
"Database", KEY_DB,
"DB",   KEY_DB,
"Server", KEY_SERVER,
"UID",  KEY_USER,
"User", KEY_USER,
"Password", KEY_PASSWORD,
"PWD",  KEY_PASSWORD,
"Port", KEY_PORT,
"AutoCommit",KEY_AUTO_COMMIT,
"UseSSL",KEY_USE_SSL,
"CharSet",KEY_CHAR_SET,
"TimeZone",KEY_TIME_ZONE,
"IsoLevel",KEY_ISO_LEVEL,
"",0
};

static void INTFUNC CenterDialog (HWND   hdlg);
int  CALLBACK ConfigDlgProc (HWND hdlg,WORD wMsg,WPARAM wParam, 
							 LPARAM lParam);
static int CALLBACK SetDSNAttributes(HWND hwnd, LPSETUPDLG lpsetupdlg);
static void INTFUNC CopyDSNAttributes(HWND hwnd, LPSETUPDLG lpsetupdlg, 
									  bool flag_option);

#define  IsTrue(s)  (!strcmp(s,"true")||!strcmp(s,"TRUE"))  

/*
@type    : myodbc3 internal
@purpose : sets the connection dialog attributes
*/
void set_attributes(LPSETUPDLG lpsetupdlg)
{
	/* set the default configuration for new DSN */
	if(lpsetupdlg->fNewDSN)
	{	
		if (!lpsetupdlg->aAttr[KEY_DESC].fSupplied)
			strcpy(lpsetupdlg->aAttr[KEY_DESC].szAttr,"XuguSQL Server ODBC 2.0 Driver DSN");
		if (!lpsetupdlg->aAttr[KEY_DB].fSupplied)
			strcpy(lpsetupdlg->aAttr[KEY_DB].szAttr,"test");
		if (!lpsetupdlg->aAttr[KEY_SERVER].fSupplied)
			strcpy(lpsetupdlg->aAttr[KEY_SERVER].szAttr,"localhost");
		if (!lpsetupdlg->aAttr[KEY_PORT].fSupplied)
			strcpy(lpsetupdlg->aAttr[KEY_PORT].szAttr,"5138");
		if (!lpsetupdlg->aAttr[KEY_AUTO_COMMIT].fSupplied)
					strcpy(lpsetupdlg->aAttr[KEY_AUTO_COMMIT].szAttr,"TRUE");
		if (!lpsetupdlg->aAttr[KEY_USE_SSL].fSupplied)
					strcpy(lpsetupdlg->aAttr[KEY_USE_SSL].szAttr,"TRUE");
		if (!lpsetupdlg->aAttr[KEY_CHAR_SET].fSupplied)
					strcpy(lpsetupdlg->aAttr[KEY_CHAR_SET].szAttr,"GBK");
		if (!lpsetupdlg->aAttr[KEY_TIME_ZONE].fSupplied)
			strcpy(lpsetupdlg->aAttr[KEY_TIME_ZONE].szAttr,"GMT+08:00");
		if (!lpsetupdlg->aAttr[KEY_ISO_LEVEL].fSupplied)
			strcpy(lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr,"2");
	}
	else
	{
		LPCSTR lpszDSN=lpsetupdlg->aAttr[KEY_DSN].szAttr;
	
		if (!lpsetupdlg->aAttr[KEY_DESC].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_KDESC,
			EMPTYSTR,
			lpsetupdlg->aAttr[KEY_DESC].szAttr,
			sizeof(lpsetupdlg->aAttr[KEY_DESC].szAttr),
			ODBC_INI);
		if (!lpsetupdlg->aAttr[KEY_DB].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_KDB,
			lpszDSN,
			lpsetupdlg->aAttr[KEY_DB].szAttr,
			sizeof(lpsetupdlg->aAttr[KEY_DB].szAttr),
			ODBC_INI);
		if (!lpsetupdlg->aAttr[KEY_SERVER].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_KSERVER,
			EMPTYSTR,
			lpsetupdlg->aAttr[KEY_SERVER].szAttr,
			sizeof(lpsetupdlg->aAttr[KEY_SERVER].szAttr),
			ODBC_INI);
		if (!lpsetupdlg->aAttr[KEY_USER].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_KUSER,
			EMPTYSTR,
			lpsetupdlg->aAttr[KEY_USER].szAttr,
			sizeof(lpsetupdlg->aAttr[KEY_USER].szAttr),
			ODBC_INI);
		if (!lpsetupdlg->aAttr[KEY_PASSWORD].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_KPASSWORD,
			EMPTYSTR,
			lpsetupdlg->aAttr[KEY_PASSWORD].szAttr,
			sizeof(lpsetupdlg->aAttr[KEY_PASSWORD].
			szAttr),
			ODBC_INI);
		if (!lpsetupdlg->aAttr[KEY_PORT].fSupplied)
		{
			char buff[10];
			_itoa(RHSQL_PORT,buff,10);
			SQLGetPrivateProfileString(lpszDSN, INI_KPORT,
				buff,
			lpsetupdlg->aAttr[KEY_PORT].szAttr,
				sizeof(lpsetupdlg->aAttr[KEY_PORT].
				szAttr),
				ODBC_INI);
		}
		if (!lpsetupdlg->aAttr[KEY_AUTO_COMMIT].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_AUTO_COMMIT,
				"TRUE",
				lpsetupdlg->aAttr[KEY_AUTO_COMMIT].szAttr,
				sizeof(lpsetupdlg->aAttr[KEY_AUTO_COMMIT].
				szAttr),
				ODBC_INI);
		if (!lpsetupdlg->aAttr[KEY_USE_SSL].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_USE_SSL,
				"TRUE",
				lpsetupdlg->aAttr[KEY_USE_SSL].szAttr,
				sizeof(lpsetupdlg->aAttr[KEY_USE_SSL].
				szAttr),
				ODBC_INI);

		if (!lpsetupdlg->aAttr[KEY_CHAR_SET].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_CHAR_SET,
				"GBK",
				lpsetupdlg->aAttr[KEY_CHAR_SET].szAttr,
				sizeof(lpsetupdlg->aAttr[KEY_CHAR_SET].
				szAttr),
				ODBC_INI);

		if (!lpsetupdlg->aAttr[KEY_TIME_ZONE].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_TIME_ZONE,
			"GMT+8:00",
				lpsetupdlg->aAttr[KEY_TIME_ZONE].szAttr,
				sizeof(lpsetupdlg->aAttr[KEY_TIME_ZONE].
				szAttr),
				ODBC_INI);

		if (!lpsetupdlg->aAttr[KEY_ISO_LEVEL].fSupplied)
			SQLGetPrivateProfileString(lpszDSN, INI_ISO_LEVEL,
				"TRUE",
				lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr,
				sizeof(lpsetupdlg->aAttr[KEY_ISO_LEVEL].
				szAttr),
				ODBC_INI);

	}	
}

/*
@type    : myodbc3 internal
@purpose : sets the connection dialog items
*/

static void set_dlg_items(HWND hdlg ,LPSETUPDLG lpsetupdlg)
{
	set_attributes(lpsetupdlg);  
	
	SetDlgItemText(hdlg, IDC_DSN,	lpsetupdlg->aAttr[KEY_DSN].szAttr);        
	SetDlgItemText(hdlg, IDC_DESC,lpsetupdlg->aAttr[KEY_DESC].szAttr);
	SetDlgItemText(hdlg, IDC_DB_NAME,lpsetupdlg->aAttr[KEY_DB].szAttr);
	SetDlgItemText(hdlg, IDC_URL,  lpsetupdlg->aAttr[KEY_SERVER].szAttr);
	SetDlgItemText(hdlg, IDC_USER,  lpsetupdlg->aAttr[KEY_USER].szAttr);
	SetDlgItemText(hdlg, IDC_PASSWORD,  lpsetupdlg->aAttr[KEY_PASSWORD].szAttr);
	SetDlgItemText(hdlg, IDC_PORT,  lpsetupdlg->aAttr[KEY_PORT].szAttr);
	SetDlgItemText(hdlg, IDC_CHARSET,  lpsetupdlg->aAttr[KEY_CHAR_SET].szAttr);
	SetDlgItemText(hdlg, IDC_TIMEZONE,  lpsetupdlg->aAttr[KEY_TIME_ZONE].szAttr);
	CheckDlgButton(hdlg, IDC_AUTO_COMMIT,IsTrue(lpsetupdlg->aAttr[KEY_AUTO_COMMIT].szAttr));
	CheckDlgButton(hdlg, IDC_SSL,IsTrue(lpsetupdlg->aAttr[KEY_USE_SSL].szAttr));
	if(!strcmp(lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr,"2"))
		CheckDlgButton(hdlg, IDC_READ_COMMITED,true);
	else if(!strcmp(lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr,"3"))
		CheckDlgButton(hdlg, IDC_REPEAT,true);
	else if(!strcmp(lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr,"4"))
		CheckDlgButton(hdlg, IDC_SERIAL,true);
}

/*
@type    : myodbc3 internal
@purpose : sets the connection dialog items
*/

static void set_dlg_option_items(HWND hdlg ,LPSETUPDLG lpsetupdlg)
{
	//int i;
	//CheckDlgButton(hdlg, IDC_CHECK1+i,
    //   flag & (1 << i) ? BST_CHECKED : BST_UNCHECKED); 
}

/*
@type    : ODBC 1.0 API
@purpose : adds, modifies, or deletes data sources from the system
information. It may prompt the user for connection
information. It can be in the driver DLL or a separate
setup DLL
*/

int EXPFUNC 
ConfigDSN (HWND hwnd, 
		   WORD fRequest, 
		   LPCSTR lpszDriver,
		   LPCSTR lpszAttributes)
{
	bool  fSuccess; /* Success/fail flag */
	GLOBALHANDLE hglbAttr;
	LPSETUPDLG lpsetupdlg;
	
	/* Allocate attribute array */
	hglbAttr = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(SETUPDLG));
	if (!hglbAttr)
		return FALSE;
	lpsetupdlg = (LPSETUPDLG)GlobalLock(hglbAttr);
	
	/* Parse attribute string */
	if (lpszAttributes)
		ParseAttributes(lpszAttributes, lpsetupdlg);
	
	/* Save original data source name */
	if (lpsetupdlg->aAttr[KEY_DSN].fSupplied)
		lstrcpy(lpsetupdlg->szDSN, lpsetupdlg->aAttr[KEY_DSN].szAttr);
	else
		lpsetupdlg->szDSN[0] = '\0';
	
	/* Remove data source */
	if (ODBC_REMOVE_DSN == fRequest) {
		/* Fail if no data source name was supplied */
		if (!lpsetupdlg->aAttr[KEY_DSN].fSupplied)
			fSuccess = FALSE;
		
		/* Otherwise remove data source from ODBC.INI */
		else
			fSuccess = SQLRemoveDSNFromIni(lpsetupdlg->aAttr[KEY_DSN].szAttr);
	}
	
	/* Add or Configure data source */
	else
	{
		/* Save passed variables for global access (e.g., dialog access) */
		lpsetupdlg->hwndParent = hwnd;
		lpsetupdlg->lpszDrvr   = lpszDriver;
		lpsetupdlg->fNewDSN    = (ODBC_ADD_DSN == fRequest);
		lpsetupdlg->fDefault   = !lstrcmpi(lpsetupdlg->aAttr[KEY_DSN].szAttr,
			INI_SDEFAULT);
		
		/* Display the appropriate dialog (if parent window handle supplied) */
		if (hwnd)
		{
			/* Display dialog(s) */
			fSuccess = (IDOK == DialogBoxParam(s_hModule,
				MAKEINTRESOURCE(IDD_SETUP), 
				hwnd,
				(DLGPROC) ConfigDlgProc,
				(LONG)(LPSTR)lpsetupdlg));
		}
		else if (lpsetupdlg->aAttr[KEY_DSN].fSupplied)
			fSuccess = SetDSNAttributes(hwnd, lpsetupdlg);
		else
			fSuccess = FALSE;
	}
	
	GlobalUnlock(hglbAttr);
	GlobalFree(hglbAttr);
	return fSuccess;
}

/*
@type    : internal
@purpose : Center the dialog over the frame window
*/

static void INTFUNC CenterDialog(HWND hdlg)
{
	HWND  hwndFrame;
	RECT  rcDlg, rcScr, rcFrame;
	int cx, cy;
	
	hwndFrame = GetParent(hdlg);
	
	GetWindowRect(hdlg, &rcDlg);
	cx = rcDlg.right  - rcDlg.left;
	cy = rcDlg.bottom - rcDlg.top;
	
	GetClientRect(hwndFrame, &rcFrame);
	ClientToScreen(hwndFrame, (LPPOINT)(&rcFrame.left));
	ClientToScreen(hwndFrame, (LPPOINT)(&rcFrame.right));
	rcDlg.top    = rcFrame.top  + (((rcFrame.bottom - rcFrame.top) - cy) >> 1);
	rcDlg.left   = rcFrame.left + (((rcFrame.right - rcFrame.left) - cx) >> 1);
	rcDlg.bottom = rcDlg.top  + cy;
	rcDlg.right  = rcDlg.left + cx;
	
	GetWindowRect(GetDesktopWindow(), &rcScr);
	if (rcDlg.bottom > rcScr.bottom)
	{
		rcDlg.bottom = rcScr.bottom;
		rcDlg.top  = rcDlg.bottom - cy;
	}
	if (rcDlg.right  > rcScr.right)
	{
		rcDlg.right = rcScr.right;
		rcDlg.left  = rcDlg.right - cx;
	}
	
	if (rcDlg.left < 0) rcDlg.left = 0;
	if (rcDlg.top  < 0) rcDlg.top  = 0;
	
	MoveWindow(hdlg, rcDlg.left, rcDlg.top, cx, cy, TRUE);
	return;
}

/*
@purpose : test DSN configuration parameters
*/
static void testDataSource(HWND hdlg,LPSETUPDLG lpsetupdlg)
{
	char	*url,*port,*db_name,*user,*password;
	bool	auto_commit=true,is_ssl = true;
	char    conn_str[512];

	db_name = lpsetupdlg->aAttr[KEY_DB].szAttr;
	url = lpsetupdlg->aAttr[KEY_SERVER].szAttr;
	user = lpsetupdlg->aAttr[KEY_USER].szAttr;
	password = lpsetupdlg->aAttr[KEY_PASSWORD].szAttr;
	port =  lpsetupdlg->aAttr[KEY_PORT].szAttr;	
	
	sprintf(conn_str, "%s:%s/%s", url, port, db_name);
	if(!testConnect(conn_str,user,password))
		MessageBox(hdlg, "连接失败.", "连接测试", MB_ICONINFORMATION | MB_OK);
	else
		MessageBox(hdlg, "连接成功.", "连接测试", MB_ICONINFORMATION | MB_OK);	
}

/*
@type    : myodbc3 internal
@purpose : Copy DSN attributes from dialog to structure
*/

static void INTFUNC CopyDSNAttributes(HWND hdlg, LPSETUPDLG lpsetupdlg,
                                      bool flag_option)
{  
	HWND hwndDSN = lpsetupdlg->hwndDSN;
    
	/* Retrieve dialog values */
	if (!lpsetupdlg->fDefault)
		GetDlgItemText(hwndDSN, IDC_DSN,
		lpsetupdlg->aAttr[KEY_DSN].szAttr,
		sizeof(lpsetupdlg->aAttr[KEY_DSN].szAttr));
	GetDlgItemText(hwndDSN, IDC_DESC,
		lpsetupdlg->aAttr[KEY_DESC].szAttr,
		sizeof(lpsetupdlg->aAttr[KEY_DESC].szAttr));
	GetDlgItemText(hwndDSN, IDC_DB_NAME,
		lpsetupdlg->aAttr[KEY_DB].szAttr,
		sizeof(lpsetupdlg->aAttr[KEY_DB].szAttr));
	GetDlgItemText(hwndDSN, IDC_URL,
		lpsetupdlg->aAttr[KEY_SERVER].szAttr,
		sizeof(lpsetupdlg->aAttr[KEY_SERVER].szAttr));
	GetDlgItemText(hwndDSN, IDC_USER,
		lpsetupdlg->aAttr[KEY_USER].szAttr,
		sizeof(lpsetupdlg->aAttr[KEY_USER].szAttr));
	GetDlgItemText(hwndDSN, IDC_PASSWORD,
		lpsetupdlg->aAttr[KEY_PASSWORD].szAttr,
		sizeof(lpsetupdlg->aAttr[KEY_PASSWORD].szAttr));
	GetDlgItemText(hwndDSN, IDC_PORT,
		lpsetupdlg->aAttr[KEY_PORT].szAttr,
		sizeof(lpsetupdlg->aAttr[KEY_PORT].szAttr));
	GetDlgItemText(hwndDSN, IDC_CHARSET,
		lpsetupdlg->aAttr[KEY_CHAR_SET].szAttr,
		sizeof(lpsetupdlg->aAttr[KEY_CHAR_SET].szAttr));
	GetDlgItemText(hwndDSN, IDC_TIMEZONE,
		lpsetupdlg->aAttr[KEY_TIME_ZONE].szAttr,
		sizeof(lpsetupdlg->aAttr[KEY_TIME_ZONE].szAttr));
	if(BST_CHECKED==IsDlgButtonChecked(hwndDSN,IDC_AUTO_COMMIT))
		strcpy(lpsetupdlg->aAttr[KEY_AUTO_COMMIT].szAttr,
			"TRUE");
	else
		strcpy(lpsetupdlg->aAttr[KEY_AUTO_COMMIT].szAttr,
			"FALSE");

	if(BST_CHECKED==IsDlgButtonChecked(hwndDSN,IDC_SSL))
		strcpy(lpsetupdlg->aAttr[KEY_USE_SSL].szAttr,
			"TRUE");
	else
		strcpy(lpsetupdlg->aAttr[KEY_USE_SSL].szAttr,
			"FALSE");

	if(BST_CHECKED==IsDlgButtonChecked(hwndDSN,IDC_READ_COMMITED))
		strcpy(lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr,
			"2");
	else if(BST_CHECKED==IsDlgButtonChecked(hwndDSN,IDC_REPEAT))
		strcpy(lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr,
			"3");
	else
		strcpy(lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr,
			"4");
}

/*
@type    : myodbc3 internal
@purpose : Create and initialize Options dialog
*/

int CALLBACK 
ConfigOptionDlgProc (HWND hdlg,WORD wMsg,WPARAM wParam,LPARAM lParam)
{  
	switch (wMsg) {
		/* Initialize the dialog */
	case WM_INITDIALOG:
		{
			LPSETUPDLG lpsetupdlg;
			
			SetWindowLong(hdlg, DWL_USER, lParam);
			CenterDialog(hdlg);        
			lpsetupdlg = (LPSETUPDLG)lParam;
			lpsetupdlg->hwndOption = hdlg;
			set_dlg_option_items(hdlg,lpsetupdlg);
			return TRUE;  /* Focus was not set */
		}
		
		/* Process buttons */
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam)) {      
			
		case IDHELP:
			{
				HKEY  hKey;
				char  szHelpFileName[255];
				DWORD dwBufLen;
				
				RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					szDriverIniKey,
					0, KEY_QUERY_VALUE, &hKey);
				RegQueryValueEx(hKey, szHelpFileKey, NULL, NULL,
					(LPBYTE) szHelpFileName, &dwBufLen);
				RegCloseKey(hKey);
				WinHelp(hdlg, szHelpFileName, HELP_INDEX, 0L);
			}
			break;
			
		case ID_TEST:
			{
				LPSETUPDLG lpsetupdlg;
				
				lpsetupdlg = (LPSETUPDLG)GetWindowLong(hdlg, DWL_USER);      
				CopyDSNAttributes(hdlg, lpsetupdlg,0);
				testDataSource(hdlg,lpsetupdlg);
				break;
			}
			
		case IDOK:
			{
				LPSETUPDLG lpsetupdlg;
				lpsetupdlg = (LPSETUPDLG)GetWindowLong(hdlg, DWL_USER);
				CopyDSNAttributes(hdlg, lpsetupdlg,1);
			}
			
			/* Return to caller */
		case IDCANCEL:
			EndDialog(hdlg, wParam);
			return TRUE;
		}
		break;
	}
	
	/* Message not processed */
	return FALSE;
}

/*
@type    : internal
@purpose : Manage add data source name dialog
*/

int CALLBACK ConfigDlgProc (HWND hdlg,WORD wMsg,WPARAM wParam,LPARAM lParam)
{
	switch (wMsg) {
		/* Initialize the dialog */
	case WM_INITDIALOG:
		{
			LPSETUPDLG lpsetupdlg;
			
			SetWindowLong(hdlg, DWL_USER, lParam);
			CenterDialog(hdlg);
			lpsetupdlg = (LPSETUPDLG) lParam;
			lpsetupdlg->hwndDSN = hdlg;
			set_dlg_items(hdlg,lpsetupdlg);
			
			SendDlgItemMessage(hdlg, IDC_DSN,
				EM_LIMITTEXT, (WPARAM)(MAXDSNAME-1), 0L);
			SendDlgItemMessage(hdlg, IDC_DESC,
				EM_LIMITTEXT, (WPARAM)(MAXDESC-1), 0L);
			SendDlgItemMessage(hdlg, IDC_DB_NAME,
				EM_LIMITTEXT, (WORD)(MAX_NAME_LEN), 0L);
			SendDlgItemMessage(hdlg, IDC_URL,
				EM_LIMITTEXT, (WORD)(MAXSERVER-1), 0L);
			SendDlgItemMessage(hdlg, IDC_USER,
				EM_LIMITTEXT, (WORD)(MAXUSER-1), 0L);
			SendDlgItemMessage(hdlg, IDC_PASSWORD,
				EM_LIMITTEXT, (WORD)(MAXPASSWORD-1), 0L);
			SendDlgItemMessage(hdlg, IDC_PORT,
				EM_LIMITTEXT, (WORD) 5, 0L);
			return TRUE;  /* Focus was not set */
		}
		/* Process buttons */
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam)) {
		/*
		Ensure the OK button is enabled only when a data source name
		is entered
			*/
		case IDC_DSN:
			if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
			{
				char  szItem[MAXDSNAME];    /* Edit control text */
				
				/* Enable/disable the OK button */
				EnableWindow(GetDlgItem(hdlg, IDOK),
					GetDlgItemText(hdlg, IDC_DSN,
					szItem, sizeof(szItem)));
				return TRUE;
			}
			break;
			
			/*  case IDHELP:
			{
			HKEY  hKey;
			char  szHelpFileName[255];
			DWORD dwBufLen;
			
			 RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			 szDriverIniKey, 0,
			 KEY_QUERY_VALUE, &hKey );
			 RegQueryValueEx(hKey, szHelpFileKey, NULL, NULL,
			 (LPBYTE) szHelpFileName, &dwBufLen);
			 RegCloseKey(hKey);
			 WinHelp(hdlg, szHelpFileName, HELP_INDEX, 0L);
			 }
			 break;
			*/
			
		case ID_TEST:
			{
				LPSETUPDLG lpsetupdlg;
				
				lpsetupdlg = (LPSETUPDLG)GetWindowLong(hdlg, DWL_USER);
				CopyDSNAttributes(hdlg, lpsetupdlg,0);
				testDataSource(hdlg,lpsetupdlg);
				break;
			}
			/* Accept results */
		case IDOK:
			{
				LPSETUPDLG lpsetupdlg;
				lpsetupdlg = (LPSETUPDLG)GetWindowLong(hdlg, DWL_USER);
				CopyDSNAttributes(hdlg, lpsetupdlg,0);
				SetDSNAttributes(hdlg, lpsetupdlg);
			}
			
			/* Return to caller */
		case IDCANCEL:
			EndDialog(hdlg, wParam);
			return TRUE;
		}
		
		break;
	}
	
	/* Message not processed */
	return FALSE;
}

/*
@type    : internal
@purpose : Parse attribute string moving values into the aAttr array
*/

void INTFUNC ParseAttributes(LPCSTR lpszAttributes, LPSETUPDLG lpsetupdlg)
{
	LPCSTR  lpsz;
	LPCSTR  lpszStart;
	char  aszKey[MAXKEYLEN];
	int iElement;
	int cbKey;
	
	for (lpsz=lpszAttributes; lpsz!=NULL && (*lpsz);)
	{
		/*  Extract key name (e.g., DSN), it must be terminated by an equals */
		lpszStart = lpsz;
		if (!(lpsz=strchr(lpsz,'=')))
			return;
		/* Determine the keys index in the key table (-1 if not found) */
		iElement = -1;
		cbKey  = lpsz - lpszStart;
		if (cbKey < sizeof(aszKey))
		{
			register int j;
			
			_fmemcpy(aszKey, lpszStart, cbKey);
			aszKey[cbKey] = '\0';
			for (j = 0; *s_aLookup[j].szKey; j++)
			{
				if (!lstrcmpi(s_aLookup[j].szKey, aszKey))
				{
					iElement = s_aLookup[j].iKey;
					break;
				}
			}
		}
		
		/* Locate end of key value */
		lpszStart = ++lpsz;
		lpsz=strchr(lpsz,';');
		
		/*
		Save value if key is known
		NOTE: This code assumes the szAttr buffers in aAttr have been
		zero initialized
		*/
		if (iElement >= 0)
		{
			lpsetupdlg->aAttr[iElement].fSupplied = TRUE;
			strncpy1((char*)lpsetupdlg->aAttr[iElement].szAttr,(char*)lpszStart,
				min(lpsz-lpszStart, sizeof(lpsetupdlg->aAttr[0].szAttr)-1));
		}
		if(lpsz)
			lpsz++;     /* Skipp ';' or end 0 */
	}
	return;
}

/*
@type    : internal
@purpose : Write data source attributes to ODBC.INI
*/

int INTFUNC SetDSNAttributes(HWND hwndParent, LPSETUPDLG lpsetupdlg)
{
	LPCSTR lpszDSN=lpsetupdlg->aAttr[KEY_DSN].szAttr;  /* Pointer to DSN */
	
	/* Validate arguments */
	if (lpsetupdlg->fNewDSN && !*lpsetupdlg->aAttr[KEY_DSN].szAttr)
		return FALSE;
	
	/* Write the data source name */
	if (!SQLWriteDSNToIni(lpszDSN, lpsetupdlg->lpszDrvr))
	{
		if (hwndParent)
		{
			MessageBox(hwndParent, "保存DSN信息失败.", "操作失败", MB_ICONEXCLAMATION | MB_OK);
		}
		return FALSE;
	}
	
	/*
    Update ODBC.INI
    Save the value if the data source is new, if it was edited, or if
    it was explicitly supplied
	*/
	if (hwndParent || lpsetupdlg->aAttr[KEY_DESC].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_KDESC,
		lpsetupdlg->aAttr[KEY_DESC].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_DB].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_KDB,
		lpsetupdlg->aAttr[KEY_DB].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_SERVER].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_KSERVER,
		lpsetupdlg->aAttr[KEY_SERVER].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_USER].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_KUSER,
		lpsetupdlg->aAttr[KEY_USER].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_PASSWORD].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_KPASSWORD,
		lpsetupdlg->aAttr[KEY_PASSWORD].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_PORT].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
			INI_KPORT,
			lpsetupdlg->aAttr[KEY_PORT].szAttr,
			ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_PASSWORD].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_KPASSWORD,
		lpsetupdlg->aAttr[KEY_PASSWORD].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_AUTO_COMMIT].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_AUTO_COMMIT,
		lpsetupdlg->aAttr[KEY_AUTO_COMMIT].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_USE_SSL].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_USE_SSL,
		lpsetupdlg->aAttr[KEY_USE_SSL].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_CHAR_SET].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_CHAR_SET,
		lpsetupdlg->aAttr[KEY_CHAR_SET].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_TIME_ZONE].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_TIME_ZONE,
		lpsetupdlg->aAttr[KEY_TIME_ZONE].szAttr,
		ODBC_INI);
	if (hwndParent || lpsetupdlg->aAttr[KEY_ISO_LEVEL].fSupplied )
		SQLWritePrivateProfileString(lpszDSN,
		INI_ISO_LEVEL,
		lpsetupdlg->aAttr[KEY_ISO_LEVEL].szAttr,
		ODBC_INI);

	/* If the data source name has changed, remove the old name */
	if (lpsetupdlg->aAttr[KEY_DSN].fSupplied &&
		lstrcmpi(lpsetupdlg->szDSN, lpsetupdlg->aAttr[KEY_DSN].szAttr))
	{
		SQLRemoveDSNFromIni(lpsetupdlg->szDSN);
	}
	return TRUE;
}

/*
@type    : internal
@purpose : SQLDBC dialog for driver connect
*/

int FAR PASCAL
DriverConnectProc(HWND hdlg, WORD wMsg, WPARAM wParam, LPARAM lParam)
{
	LPSETUPDLG lpsetupdlg;
	switch (wMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hdlg, DWL_USER, lParam);
		CenterDialog(hdlg);
		lpsetupdlg = (LPSETUPDLG) lParam;
		lpsetupdlg->hwndDSN = hdlg;
		set_dlg_items(hdlg,lpsetupdlg);    
		
		SendDlgItemMessage(hdlg, IDC_DSN,
			EM_LIMITTEXT, (WPARAM)(MAXDSNAME-1), 0L);
		SendDlgItemMessage(hdlg, IDC_DESC,
			EM_LIMITTEXT, (WPARAM)(MAXDESC-1), 0L);
		SendDlgItemMessage(hdlg, IDC_DB_NAME,
			EM_LIMITTEXT, (WORD)(MAX_NAME_LEN), 0L);
		SendDlgItemMessage(hdlg, IDC_URL,
			EM_LIMITTEXT, (WORD)(MAXSERVER-1), 0L);
		SendDlgItemMessage(hdlg, IDC_USER,
			EM_LIMITTEXT, (WORD)(MAXUSER-1), 0L);
		SendDlgItemMessage(hdlg, IDC_PASSWORD,
			EM_LIMITTEXT, (WORD)(MAXPASSWORD-1), 0L);
		SendDlgItemMessage(hdlg, IDC_PORT,
			EM_LIMITTEXT, (WORD)5, 0L);
			/* 
			Disable DSN change for DriverConnect related ...
			so that the FileDSN will be validated correctly, 
			as user can't set/change the DSN 
		*/
		EnableWindow(GetDlgItem(hdlg, IDC_DSN), FALSE);
		EnableWindow(GetDlgItem(hdlg, IDC_DESC), FALSE);
		return TRUE;
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam)) {
			
		case IDHELP:
			{
				HKEY hKey;
				char szHelpFileName[255];
				DWORD dwBufLen;
				
				RegOpenKeyEx( HKEY_LOCAL_MACHINE,
					"SOFTWARE\\ODBC\\ODBCINST.INI\\XuguSQL Server ODBC 2.0 Driver",
					0, KEY_QUERY_VALUE, &hKey );
				RegQueryValueEx( hKey, "DSNHelpFile", NULL, NULL,
					(LPBYTE) szHelpFileName, &dwBufLen);
				RegCloseKey( hKey );
				
				WinHelp(hdlg, szHelpFileName, HELP_INDEX, 0L);
			}
			break;
			
		case ID_TEST:
			{
				LPSETUPDLG lpsetupdlg;
				
				lpsetupdlg = (LPSETUPDLG)GetWindowLong(hdlg, DWL_USER);
				CopyDSNAttributes(hdlg, lpsetupdlg,0);
				testDataSource(hdlg,lpsetupdlg);
				break;
			}
			
			/*  case IDOPTIONS:
			{
			bool  fSuccess;
			LPSETUPDLG lpsetupdlg = (LPSETUPDLG)GetWindowLong(hdlg, DWL_USER);
			if (fSuccess = (IDOK != DialogBoxParam(s_hModule,
			MAKEINTRESOURCE(MYOPTIONDLG),
			hdlg,
			(DLGPROC) ConfigOptionDlgProc,
			(LONG)(LPSTR)lpsetupdlg)))
			return fSuccess;
			}
			*/
			
			/* Accept results */
		case IDOK:
			{
				LPSETUPDLG lpsetupdlg;
				lpsetupdlg = (LPSETUPDLG)GetWindowLong(hdlg, DWL_USER);
				CopyDSNAttributes(hdlg, lpsetupdlg,0);
			}
			
			/* Return to caller */
		case IDCANCEL:
			EndDialog(hdlg, wParam);
			return TRUE;
		}
	}
	return FALSE;
}
