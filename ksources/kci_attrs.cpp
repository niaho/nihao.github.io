#include <malloc.h>
#include <memory.h>
#include <string.h>
#include "kci_defs.h"

extern sword setSvcAttr(KCISvcCtx *p_hdl, void *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp);

extern sword getDescAttr(KCIParam *p_hdl, void *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp);

extern sword getStmtAttr(KCIStmt *p_hdl, void *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp);


extern sword setSessAttr(KCISession *p_hdl,void  *attributep, ub4 size,
	ub4   attrtype, KCIError *errhp);

extern sword setStmtAttr(KCIStmt *p_hdl, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp);
extern sword setTransAttr(KCITrans *p_hdl, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp);
extern sword setSPoolAttr(KCISPool *p_hdl, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp);

extern sword setServerAttr(KCIServer *p_hdl, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp);

extern sword getBindAttr(KCIBind *p_hdl, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp);

/*设置句柄属性*/
sword  KCIAttrSet(void  *trgthndlp, ub4 trghndltyp,
				  void  *attributep, ub4 size,
				  ub4 attrtype, KCIError *errhp)
{
	//step1:分配句柄
	switch (trghndltyp)
	{
	case KCI_HTYPE_SVCCTX:
		return setSvcAttr((KCISvcCtx*)trgthndlp, &attributep, size, attrtype, errhp);
	case KCI_HTYPE_SESSION:
		return setSessAttr((KCISession*)trgthndlp, attributep, size, attrtype, errhp);
	case KCI_HTYPE_STMT:
		return setStmtAttr((KCIStmt*)trgthndlp, attributep, size, attrtype, errhp);
	case KCI_HTYPE_TRANS:
		return setTransAttr((KCITrans*)trgthndlp, &attributep, size, attrtype, errhp);
	case KCI_HTYPE_SPOOL:
		return setSPoolAttr((KCISPool*)trgthndlp, attributep, size, attrtype, errhp);
	case KCI_HTYPE_SERVER:
		return setServerAttr((KCIServer*)trgthndlp, attributep, size, attrtype, errhp);
	default:
		setError(errhp, 111, "Invalid handler type");
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
}

sword   KCIAttrGet(void  *trgthndlp, ub4 trghndltyp,
	void  *attributep, ub4 *sizep, ub4 attrtype,
	KCIError *errhp)
{
	//step1:分配句柄
	switch (trghndltyp)
	{
		/*
	case KCI_DTYPE_PARAM: // 53 a parameter descriptor obtained from ocigparm 
		return getDescAttr((KCIParam*)trgthndlp, attributep, *sizep, attrtype, errhp);
		break;
		*/	
	case KCI_HTYPE_STMT:
		return getStmtAttr((KCIStmt*)trgthndlp, attributep, *sizep, attrtype, errhp);
		break;
	case KCI_HTYPE_BIND:
        return getBindAttr((KCIBind*)trgthndlp, attributep, 0, attrtype, errhp);
		break;		
	default:
		setError(errhp, 111, "Invalid handler type");
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
}

