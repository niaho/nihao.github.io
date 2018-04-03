#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "kci_defs.h"

/*按位置对输出域进行定义*/
sword   KCIDefineByPos(KCIStmt *stmtp, KCIDefine **defnp, KCIError *errhp,
	ub4 position, void  *valuep, sb4 value_sz, ub2 dty,
	void  *indp, ub2 *rlenp, ub2 *rcodep, ub4 mode)
{
//	if (position<1 || position>stmtp->fld_num)
	{
//		setError(errhp, 111, "Position range error");
//		return KCI_ERROR;
	}
	if (mode != KCI_DEFAULT && mode != KCI_DYNAMIC_FETCH)
	{
		setError(errhp, 111, "Invalid mode");
		return KCI_ERROR;
	}
	KCIDefine *p_defi = stmtp->defines;
	ub4 i = 1;
	if (position == 1)
	{
		////第一个
		if (!p_defi) 			//创建输出
		{
			stmtp->defines = mcNew(&(stmtp->p_env->mem), KCIDefine);			
			p_defi = stmtp->defines;
			memset(p_defi, 0, sizeof(KCIDefine));
		}
			

	}else while (i<position )
	{		
		if (p_defi->Next)
		{
			i++;
			p_defi = p_defi->Next;
			continue;
		}
		else
		{
			//创建输出
			p_defi->Next = mcNew(&(stmtp->p_env->mem), KCIDefine);
			p_defi = p_defi->Next;
			memset(p_defi, 0, sizeof(KCIDefine));
			break;
		}
	}
//	KCIDefine *p_defi = stmtp->defines + (position - 1);
	p_defi->col_index = (position - 1);
	p_defi->val_ptr = (char*)valuep;
	p_defi->val_siz = value_sz;
	p_defi->val_dt = dty;
	p_defi->ind_ptr = (char*)indp;
	p_defi->alen_ptr = (char*)rlenp;
	p_defi->ret_ptr = (char*)rcodep;
	p_defi->exe_mode = mode;
	
	if (defnp)
		*defnp = p_defi;
	stmtp->defi_num++;
	return KCI_SUCCESS;
}

/*按位置对输出域进行定义*/
sword   KCIDefineByPos2(KCIStmt *stmtp, KCIDefine **defnp, KCIError *errhp,
	ub4   position, void  *valuep, sb8 value_sz, ub2 dty,
	void  *indp, ub4 *rlenp, ub2 *rcodep, ub4 mode)
{
	if (mode != KCI_DEFAULT && mode != KCI_DYNAMIC_FETCH)
	{
		setError(errhp, 111, "Invalid mode");
		return KCI_ERROR;
	}
	KCIDefine *p_defi = stmtp->defines;
	int i = 1;
	if (position == 1)
	{
		////第一个
		if (!p_defi) 			//创建输出
			p_defi = mcNew(&(stmtp->p_env->mem), KCIDefine);

	}
	else while (i<position)
	{
		if (p_defi->Next)
		{
			i++;
			p_defi = p_defi->Next;
			continue;
		}
		else
		{
			//创建输出
			p_defi->Next = mcNew(&(stmtp->p_env->mem), KCIDefine);
			break;
		}
	}	
	p_defi->col_index = (position - 1);
	p_defi->val_ptr = (char*)valuep;
	p_defi->val_siz = value_sz;
	p_defi->val_dt = dty;
	p_defi->ind_ptr = (char*)indp;
	p_defi->alen_ptr = (char*)rlenp;
	p_defi->ret_ptr = (char*)rcodep;
	p_defi->exe_mode = mode;
	p_defi->big_mode = true;
	if (defnp)
		*defnp = p_defi;
	stmtp->defi_num++;
	return KCI_SUCCESS;

}

sword   KCIDefineObject(KCIDefine *defnp, KCIError *errhp,
	const KCIType *type, void  **pgvpp,ub4 *pvszsp, 
	void  **indpp, ub4 *indszp)
{
	setError(errhp, 111, "Not supported");
	return KCI_ERROR;
}

/*动态输出模式定义*/
sword   KCIDefineDynamic(KCIDefine *defnp, KCIError *errhp, 
	void  *octxp,KCICallbackDefine ocbfp)
{
	defnp->out_ctxp = octxp;
	defnp->out_func = ocbfp;
	return KCI_SUCCESS;
}

/*结构化输出模式定义*/
sword   KCIDefineArrayOfStruct(KCIDefine *defnp, KCIError *errhp, ub4 pvskip,
	ub4 indskip, ub4 rlskip, ub4 rcskip)
{
	defnp->val_skip = pvskip;
	defnp->ind_skip = indskip;
	defnp->alen_skip = rlskip;
	defnp->ret_skip = rcskip;
	return KCI_SUCCESS;
}

