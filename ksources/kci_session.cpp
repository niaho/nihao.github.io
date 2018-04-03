#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_net.h"

/*KCISessionGet函数:新创建或从会话池取得会话*/
sword KCISessionGet(KCIEnv *envhp, KCIError *errhp, 
	KCISvcCtx **pp_svctx,KCIAuthInfo *authhp,
	OraText *srv_info, ub4 srv_len,
	const OraText *itag, ub4 itag_len,
	OraText **otag, ub4 *otag_len,
	boolean *found, ub4 mode)
{
	Svctx   *p_svctx = NULL;

	//从会话池取逻辑连接（Svctx）
	if (mode & KCI_SESSGET_SPOOL)
	{
		Foreach(l, envhp->sp_list)
		{
			SPool *spool = EntryPtr(l, SPool, sp_link);
			if (!kstrcmp(spool->name, spool->name_len, srv_info, srv_len))
			{
				CHECK(spSessionGet(spool, errhp, &p_svctx, authhp, itag,
					itag_len, otag, otag_len, found, mode));
				p_svctx->cre_mode = 3;
				*pp_svctx = p_svctx;
				return KCI_SUCCESS;
			}
		}
		setError(errhp, 1111, "Connection pool not exist.");
		return KCI_ERROR;
	}
	//建立物理级连接或使用连接池
	else
	{
		Serv    *p_serv = NULL;
		Sess    *p_sess = NULL;
	
		KCIHandleAlloc(envhp, (void**)&p_svctx, KCI_HTYPE_SVCCTX, 0, 0);
		KCIHandleAlloc(envhp, (void**)&p_serv, KCI_HTYPE_SERVER, 0, 0);
		KCIHandleAlloc(envhp, (void**)&p_sess, KCI_HTYPE_SESSION, 0, 0);
		if(mode & KCI_SESSGET_CPOOL)
			KCIServerAttach(p_serv, errhp, srv_info, srv_len, KCI_CPOOL);
		else
			KCIServerAttach(p_serv, errhp, srv_info, srv_len, KCI_DEFAULT);
		KCIAttrSet(p_sess, KCI_HTYPE_SESSION, authhp->uname, authhp->uname_len, KCI_ATTR_USERNAME, errhp);
		KCIAttrSet(p_sess, KCI_HTYPE_SESSION, authhp->passw,authhp->pass_len, KCI_ATTR_PASSWORD, errhp);
		KCIAttrSet(p_svctx, KCI_HTYPE_SVCCTX, p_serv, 0, KCI_ATTR_SERVER, errhp);
		KCIAttrSet(p_svctx, KCI_HTYPE_SVCCTX, &p_sess, 0, KCI_ATTR_SESSION, errhp);
		CHECK(buildServer(p_serv, p_sess, errhp));
		p_svctx->cre_mode = 3; //创建方式
		*pp_svctx = p_svctx;
	}
	return KCI_SUCCESS;
}

/*释放会话（若是会话池模式，则释放到会话池中，否则释放资源并释放句柄）*/
sword KCISessionRelease(KCISvcCtx *p_svctx, KCIError *errhp,
						OraText *tag, ub4 tag_len,ub4 mode)
{
	if (p_svctx->cre_mode != 3)
	{
		setError(errhp, 1111, "This session not created by KCISessionGet.");
		return KCI_ERROR;
	}
	//该会话使用了会话池吗？
	if (p_svctx->p_spool)
	{
		return spSessionRelease(p_svctx->p_spool, p_svctx, errhp, tag, tag_len, mode);
	}
	else
	{
		KCIServerDetach(p_svctx->p_server,errhp, 0);
		KCIHandleFree(p_svctx->p_server, KCI_HTYPE_SERVER);
		KCIHandleFree(p_svctx->p_sess, KCI_HTYPE_SESSION);
		if (p_svctx->p_trans)
			KCIHandleFree(p_svctx->p_trans, KCI_HTYPE_TRANS);
		KCIHandleFree(p_svctx, KCI_HTYPE_SVCCTX);
	}
	return KCI_SUCCESS;
}
/*设置连接属性*/
sword setSessAttr(KCISession *p_hdl,
	void  *attributep, ub4   size,
	ub4   attrtype, KCIError *errhp)
{
	switch (attrtype)
	{
	case KCI_ATTR_USERNAME:
		if (size > 0 && size < 64)
		{
			memcpy(p_hdl->uname, attributep, size);
			p_hdl->uname[size] = 0x0;
			p_hdl->uname_len = size;
		}
		else
		{
			setError(errhp, 111, "User name to long");
			return KCI_ERROR;
		}
		break;
	case KCI_ATTR_PASSWORD:
		if (size > 0 && size < 64)
		{
			memcpy(p_hdl->passw, attributep, size);
			p_hdl->passw[size] = 0x0;
			p_hdl->pass_len = size;
		}
		else
		{
			setError(errhp, 111, "Password to long");
			return KCI_ERROR;
		}
		break;
	default:
		setError(errhp, 111, "Invalid parameter type");
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
}

void freeSession(KCISession *p_hdl)
{
	mfree(p_hdl);
}
