/* copyright_notice.c */

/******************* SHORT COPYRIGHT EX_NOTICE*************************
This source code is part of the BigDigits multiple-precision
arithmetic library Version 1.0 originally written by David Ireland,
copyright (c) 2001 D.I. Management Services Pty Limited, all rights
reserved. It is provided "as is" with no warranties. You may use
this software under the terms of the full copyright notice
"bigdigitsCopyright.txt" that should have been included with
this library. To obtain a copy send an email to
<code@di-mgt.com.au> or visit <www.di-mgt.com.au/crypto.html>.
This notice must be retained in any copy.
****************** END OF COPYRIGHT EX_NOTICE*************************/

/* Force linker to include copyright notice in executable object image */

char *copyright_notice(void)
{
	return 
"Contains multiple-precision arithmetic code originally written by David Ireland, copyright (c) 2001 by D.I. Management Services Pty Limited <www.di-mgt.com.au>, and is used with permission.";
}

/* To use, include this statement somewhere in the final code:

	copyright_notice();	
	
It has no real effect at run time. 
Thanks to Phil Zimmerman for this idea.
*/

