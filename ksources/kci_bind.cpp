#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_utils.h"


/*取属性*/
sword getBindAttr(KCIBind *bindp, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp)
{
	switch (attrtype)
	{
	case KCI_ATTR_ROWS_RETURNED:
		*(sb4 *)attributep = bindp->rows;
		break;
	case KCI_ATTR_ROW_COUNT:
		*(sb4 *)attributep = bindp->rows;
		break;
	default:
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
}

/*按位置进行参数绑定*/
sword   KCIBindByPos(KCIStmt *stmtp, KCIBind **bindp, 
	KCIError *errhp,ub4 position, void  *valuep, 
	sb4 value_sz,ub2 dty, void  *indp, ub2 *alenp, 
	ub2 *rcodep,ub4 maxarr_len, ub4 *curelep, ub4 mode)
{
       if (position<1 || position>stmtp->bind_num)
	{
		setError(errhp, 111, "Position range error");
		return KCI_ERROR;
	}
       if (mode != KCI_DEFAULT && mode != KCI_DATA_AT_EXEC)
	{
		setError(errhp, 111, "Invalid mode");
		return KCI_ERROR;
	}
       KCIBind *p_bind = stmtp->binds + (position - 1);
	p_bind->big_mode = false;
	p_bind->bind_mode = 0;
	if ((dty == SQLT_BLOB) || (dty == SQLT_CLOB)) 
	{
		KCITrans *p_trans;
		if (!stmtp->p_svctx->p_trans)
		{
			XID  gxid;

			/* 事务准备 */
			KCIHandleAlloc(stmtp->p_env, (void**)&p_trans, KCI_HTYPE_TRANS, 0, 0);
			p_trans->isautobigdata = 1;
			KCIAttrSet(stmtp->p_svctx, KCI_HTYPE_SVCCTX, p_trans, 0, KCI_ATTR_TRANS, errhp);
			gxid.formatID = 1015; 
			gxid.gtrid_length = 3; 
			gxid.data[0] = 1;
			gxid.data[1] = 7;
			gxid.data[2] = 7;
			gxid.bqual_length = 1;
			gxid.data[3] = 1;
			KCIAttrSet(stmtp->p_svctx->p_trans, KCI_HTYPE_TRANS, &gxid, sizeof(XID), KCI_ATTR_XID, errhp);
			p_trans->stmtp = stmtp;
			KCITransStart(stmtp->p_svctx, errhp, 60, KCI_TRANS_NEW);
		}
		else
			stmtp->p_svctx->p_trans->stmtp = stmtp;
		
		KCILobLocator   *v1 = (KCILobLocator *)valuep;
		if (v1 == NULL)
			v1 = mcNew(&(stmtp->p_env->mem), KCILobLocator);
		gen_randstr(v1->name,8);
		value_sz = 8;
		v1->datLen = 0;
		p_bind->big_mode = true;

	}
	p_bind->val_ptr = (char*)valuep;
	p_bind->val_siz = value_sz;
	p_bind->val_dt = dty;
	p_bind->ind_ptr = (char*)indp;
	p_bind->alen_ptr = (char*)alenp;
	p_bind->ret_ptr = (char*)rcodep;
	p_bind->max_alen = maxarr_len;
	p_bind->exe_mode = mode;
	if (bindp)
		*bindp = p_bind;
	return KCI_SUCCESS;
}

/*按位置进行参数绑定(支持8字节长度)*/
sword   KCIBindByPos2(KCIStmt *stmtp, KCIBind **bindp, 
	KCIError *errhp,ub4 position, void  *valuep, 
	sb8 value_sz,ub2 dty, void  *indp, ub4 *alenp, 
	ub2 *rcodep,ub4 maxarr_len, ub4 *curelep, ub4 mode)
{
	if (position<1 || position>stmtp->bind_num)
	{
		setError(errhp, 111, "Position range error");
		return KCI_ERROR;
	}
	if (mode != KCI_DEFAULT && mode != KCI_DATA_AT_EXEC)
	{
		setError(errhp, 111, "Invalid mode");
		return KCI_ERROR;
	}
	KCIBind *p_bind = stmtp->binds + (position - 1);
	p_bind->big_mode = false;
	p_bind->bind_mode = 0;
	if ((dty == SQLT_BLOB) || (dty == SQLT_CLOB))
	{
		KCITrans *p_trans;
		if (!stmtp->p_svctx->p_trans)
		{
			XID gxid;

			/* 事务准备 */
			KCIHandleAlloc(stmtp->p_env, (void**)&p_trans, KCI_HTYPE_TRANS, 0, 0);
			p_trans->isautobigdata = 1;
			KCIAttrSet(stmtp->p_svctx, KCI_HTYPE_SVCCTX, p_trans, 0, KCI_ATTR_TRANS, errhp);
			gxid.formatID = 1015;
			gxid.gtrid_length = 3;
			gxid.data[0] = 1;
			gxid.data[1] = 7;
			gxid.data[2] = 7;
			gxid.bqual_length = 1;
			gxid.data[3] = 1;
			KCIAttrSet(stmtp->p_svctx->p_trans, KCI_HTYPE_TRANS, &gxid, sizeof(XID), KCI_ATTR_XID, errhp);
			p_trans->stmtp = stmtp;
			KCITransStart(stmtp->p_svctx, errhp, 60, KCI_TRANS_NEW);
		}
		else
			stmtp->p_svctx->p_trans->stmtp = stmtp;

		KCILobLocator   *v1 = (KCILobLocator *)valuep;
		if (v1 == NULL)
			v1 = mcNew(&(stmtp->p_env->mem), KCILobLocator);
		gen_randstr(v1->name, 8);
		value_sz = 8;
		v1->datLen = 0;
		p_bind->big_mode = true;
	}

	p_bind->val_ptr = (char*)valuep;
	p_bind->val_siz = value_sz;
	p_bind->val_dt = dty;
	p_bind->ind_ptr = (char*)indp;
	p_bind->alen_ptr = (char*)alenp;
	p_bind->ret_ptr = (char*)rcodep;
	p_bind->max_alen = maxarr_len;
	p_bind->exe_mode = mode;
	*bindp = p_bind;
	return KCI_SUCCESS;
}

/*按名字进行参数绑定*/
sword   KCIBindByName(KCIStmt *stmtp, KCIBind **bindp, 
	KCIError *errhp,const OraText *placeholder, 
	sb4 placeh_len,void  *valuep, sb4 value_sz, 
	ub2 dty,void  *indp, ub2 *alenp, ub2 *rcodep,
	ub4 maxarr_len, ub4 *curelep, ub4 mode)
{
	uint  idx = -1;
	KCIBind *binds = stmtp->binds;
	if (mode != KCI_DEFAULT && mode != KCI_DATA_AT_EXEC)
	{
		setError(errhp, 111, "Invalid mode");
		return KCI_ERROR;
	}
	//在参数数组中按名查找
	if (placeh_len < 0)
		placeh_len = (sb4)strlen((char*)placeholder);
	for (uint i = 0; i < stmtp->bind_num; i++)
	{
		if (astrnicmp(binds[i].name, (char*)placeholder, placeh_len) == 0)
		{
			idx = i;
			break;
		}
	}
	if (idx < 0)
	{
		setError(errhp, 111, "Parameter not exists");
		return KCI_ERROR;
	}

	KCIBind *p_bind = stmtp->binds + idx;
	p_bind->big_mode = false;
	p_bind->bind_mode = 0;
	if ((dty == SQLT_BLOB) || (dty == SQLT_CLOB))
	{
		KCITrans *p_trans;
		if (!stmtp->p_svctx->p_trans)
		{
			XID gxid;

			/* 事务准备 */
			KCIHandleAlloc(stmtp->p_env, (void**)&p_trans, KCI_HTYPE_TRANS, 0, 0);
			p_trans->isautobigdata = 1;
			KCIAttrSet(stmtp->p_svctx, KCI_HTYPE_SVCCTX, p_trans, 0, KCI_ATTR_TRANS, errhp);
			gxid.formatID = 1015;
			gxid.gtrid_length = 3;
			gxid.data[0] = 1;
			gxid.data[1] = 7;
			gxid.data[2] = 7;
			gxid.bqual_length = 1;
			gxid.data[3] = 1;
			KCIAttrSet(stmtp->p_svctx->p_trans, KCI_HTYPE_TRANS, &gxid, sizeof(XID), KCI_ATTR_XID, errhp);
			p_trans->stmtp = stmtp;
			KCITransStart(stmtp->p_svctx, errhp, 60, KCI_TRANS_NEW);
		}
		else
			stmtp->p_svctx->p_trans->stmtp = stmtp;

		KCILobLocator   *v1 = (KCILobLocator *)valuep;
		if (v1 == NULL)
			v1 = mcNew(&(stmtp->p_env->mem), KCILobLocator);
		gen_randstr(v1->name, 8);
		value_sz = 8;
		v1->datLen = 0;
		p_bind->big_mode = true;

	}

	p_bind->val_ptr = (char*)valuep;
	p_bind->val_siz = value_sz;
	p_bind->val_dt = dty;
	p_bind->ind_ptr = (char*)indp;
	p_bind->alen_ptr = (char*)alenp;
	p_bind->ret_ptr = (char*)rcodep;
	p_bind->max_alen = maxarr_len;
	p_bind->exe_mode = mode;
	*bindp = p_bind;
	return KCI_SUCCESS;
}

/*按名字进行参数绑定(支持8字节长度)*/
sword   KCIBindByName2(KCIStmt *stmtp, KCIBind **bindp, 
	KCIError *errhp,const OraText *placeholder, 
	sb4 placeh_len,void  *valuep, sb8 value_sz, 
	ub2 dty,void  *indp, ub4 *alenp, ub2 *rcodep,
	ub4 maxarr_len, ub4 *curelep, ub4 mode)
{
	uint  idx = -1;
	KCIBind *binds = stmtp->binds;
	if (mode != KCI_DEFAULT && mode != KCI_DATA_AT_EXEC)
	{
		setError(errhp, 111, "Invalid mode");
		return KCI_ERROR;
	}
	//在参数数组中按名查找
	if (placeh_len < 0)
		placeh_len = (sb4)strlen((char*)placeholder);
	for (uint i = 0; i < stmtp->bind_num; i++)
	{
		if (astrnicmp(binds[i].name, (cstr)placeholder, placeh_len) == 0)
		{
			idx = i;
			break;
		}
	}
	if (idx < 0)
	{
		setError(errhp, 111, "Parameter not exists");
		return KCI_ERROR;
	}

	KCIBind *p_bind = stmtp->binds + idx;
	p_bind->big_mode = false;
	p_bind->bind_mode = 0;
	if ((dty == SQLT_BLOB) || (dty == SQLT_CLOB))
	{
		KCITrans *p_trans;
		if (!stmtp->p_svctx->p_trans)
		{
			XID gxid;

			/* 事务准备 */
			KCIHandleAlloc(stmtp->p_env, (void**)&p_trans, KCI_HTYPE_TRANS, 0, 0);
			p_trans->isautobigdata = 1;
			KCIAttrSet(stmtp->p_svctx, KCI_HTYPE_SVCCTX, p_trans, 0, KCI_ATTR_TRANS, errhp);
			gxid.formatID = 1015;
			gxid.gtrid_length = 3;
			gxid.data[0] = 1;
			gxid.data[1] = 7;
			gxid.data[2] = 7;
			gxid.bqual_length = 1;
			gxid.data[3] = 1;
			KCIAttrSet(stmtp->p_svctx->p_trans, KCI_HTYPE_TRANS, &gxid, sizeof(XID), KCI_ATTR_XID, errhp);
			p_trans->stmtp = stmtp;
			KCITransStart(stmtp->p_svctx, errhp, 60, KCI_TRANS_NEW);
		}
		else
			stmtp->p_svctx->p_trans->stmtp = stmtp;

		KCILobLocator   *v1 = (KCILobLocator *)valuep;
		if (v1 == NULL)
			v1 = mcNew(&(stmtp->p_env->mem), KCILobLocator);
		gen_randstr(v1->name, 8);
		value_sz = 8;
		v1->datLen = 0;
		p_bind->big_mode = true;

	}

	p_bind->val_ptr = (char*)valuep;
	p_bind->val_siz = value_sz;
	p_bind->val_dt = dty;
	p_bind->ind_ptr = (char*)indp;
	p_bind->alen_ptr = (char*)alenp;
	p_bind->ret_ptr = (char*)rcodep;
	p_bind->max_alen = maxarr_len;
	p_bind->exe_mode = mode;
	*bindp = p_bind;
	return KCI_SUCCESS;
}

/*绑定对象*/
sword   KCIBindObject(KCIBind *bindp, KCIError *errhp, const KCIType *type,
					 void  **pgvpp, ub4 *pvszsp, void  **indpp,ub4 *indszp)
{
	bindp->p_type = type;
	bindp->big_mode = false;
	bindp->bind_mode = 0;
	return KCI_SUCCESS;
}

/*动态绑定(执行期间调用回调函数动态获取数据)*/
sword   KCIBindDynamic(KCIBind *bindp, KCIError *errhp, void  *ictxp,
		KCICallbackInBind icbfp, void  *octxp,KCICallbackOutBind ocbfp)
{
	bindp->big_mode = false;
	bindp->bind_mode = 1;
	bindp->in_ctxp = ictxp;
	bindp->in_func = icbfp;
	bindp->out_ctxp = octxp;
	bindp->out_func = ocbfp;
	return KCI_SUCCESS;
}

/*结构方式绑定*/
sword   KCIBindArrayOfStruct(KCIBind *bindp, KCIError *errhp,
			  ub4 pvskip, ub4 indskip,ub4 alskip, ub4 rcskip)
{
        if (bindp==NULL)
   	    return KCI_ERROR;
	bindp->big_mode = false;
	bindp->bind_mode = 0;
	bindp->val_skip = pvskip;
	bindp->ind_skip = indskip;
	bindp->alen_skip = alskip;
	bindp->ret_skip = rcskip;	
	return KCI_SUCCESS;
}

void getBindPtr(KCIBind *p_bind, int nth,void *&val_ptr,
				void *&ind_ptr,void *&alen_ptr,void *&ret_ptr)
{
	//是结构型化方式绑定吗?
	if (p_bind->val_skip)
	{
		//计算变量地址
		if (p_bind->val_ptr)
			val_ptr = p_bind->val_ptr + p_bind->val_skip*nth;
		else
			val_ptr = NULL;
		//取指示器值
		if (p_bind->ind_ptr)
			ind_ptr = p_bind->ind_ptr + 2 * nth;
		else
			ind_ptr = NULL;
		//计算变量长度地址
		if (p_bind->alen_ptr)
		{
			if(p_bind->is_bind2)
				alen_ptr = p_bind->alen_ptr + 4 * nth;
			else
				alen_ptr = p_bind->alen_ptr + 2 * nth;
		}
		else
			alen_ptr = NULL;
		//计算返回码地址
		if (p_bind->ret_ptr)
			ret_ptr = p_bind->ret_ptr + 2 * nth;
		else
			ret_ptr = NULL;
	}
	else
	{
		//计算变量地址
		if (p_bind->val_ptr)
			val_ptr = p_bind->val_ptr + p_bind->val_siz*nth;
		else
			val_ptr = NULL;
		//取指示器值
		if (p_bind->ind_ptr)
			ind_ptr = p_bind->ind_ptr + p_bind->ind_skip * nth;
		else
			ind_ptr = NULL;
		//计算变量长度地址
		if (p_bind->alen_ptr)
			alen_ptr = p_bind->alen_ptr + p_bind->alen_skip * nth;
		else
			alen_ptr = NULL;
		//计算返回码地址
		if (p_bind->ret_ptr)
			ret_ptr = p_bind->ret_ptr + p_bind->ret_skip * nth;
		else
			ret_ptr = 0;
	}
}
