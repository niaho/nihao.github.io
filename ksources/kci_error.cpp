#include <malloc.h>
#include <memory.h>
#include "kci_string.h"
#include "kci_defs.h"

/*设置错误信息*/
void setError(KCIError *errhp, sb4 code, char *info, ...)
{
	int err_len;
	va_list	ap;
	if (!errhp) return;
	va_start(ap, info);
	err_len = kvsnprintf(errhp->message, 254, info, ap);
	va_end(ap);
	errhp->err_no = code;
	errhp->err_len = (ub4)err_len;
}

/*取错误信息*/
sword KCIErrorGet(void *hndlp,
	ub4        recordno,
	OraText    *sqlstate,
	sb4        *errcodep,
	OraText    *bufp,
	ub4        bufsiz,
	ub4        type)
{
	KCIError *errhp = (KCIError*)hndlp;
	if (type != KCI_HTYPE_ERROR)
		return KCI_NO_DATA;
	if (recordno != 1)
		return KCI_NO_DATA;
	if (errcodep)
		*errcodep = errhp->err_no;
	if (errhp->err_len >= bufsiz)
	{
		memcpy(bufp, errhp->message, bufsiz - 1);
		bufp[bufsiz - 1] = 0x0;
	}
	else
	{
		memcpy(bufp, errhp->message, errhp->err_len);
		bufp[errhp->err_len] = 0x0;
	}
	return KCI_SUCCESS;
}

/*释放错误句柄*/
void freeError(KCIError *p_hdl)
{
	if (p_hdl)
	   mfree(p_hdl);
}
