#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include "kci_defs.h"
#include "kci_err.h"
#include "kci_net.h"

/*设置连接属性*/
sword setSvcAttr(KCISvcCtx *p_hdl,void *attributep,
			ub4 size,ub4 attrtype,KCIError *errhp)
{
	switch (attrtype)
	{
	case KCI_ATTR_SERVER:
		p_hdl->p_server = *((KCIServer**)attributep);
		break;
	case KCI_ATTR_SESSION:
		p_hdl->p_sess = *((KCISession**)attributep);
		p_hdl->p_sess->p_svctx = p_hdl;
		break;
	case KCI_ATTR_TRANS:
		p_hdl->p_trans = *((KCITrans**)attributep);
		break;
	default:
		setError(errhp, 111, "Invalid attribute type");
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
}

/*设置连接描述属性*/
sword getDescAttr(KCIParam *p_hdl, void *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp)
{
	switch (attrtype)
	{
	case KCI_ATTR_DATA_SIZE:      /* maximum size of the data */
		attributep =(void *) p_hdl->col_name_len;
		break;
	case KCI_ATTR_DATA_TYPE:      /* the SQL type of the column/argument */
		attributep = (void *)p_hdl->dtype;
		break;
	case KCI_ATTR_DISP_SIZE:     /* the display size */
		break;
	case  KCI_ATTR_DISP_NAME:    /* the display name */
		break;
	case KCI_ATTR_NAME:          /* the name of the column/argument */
		attributep = (void *)p_hdl->col_name;
		break;
	case KCI_ATTR_PRECISION:     /* precision if number type */
		break;
	case KCI_ATTR_SCALE:         /* scale if number type */
		break;
	case KCI_ATTR_IS_NULL:       /* is it null ? */
		break;
	case KCI_ATTR_TYPE_NAME:     /* name of the named data type or a package name for package private types */
		break;
	case KCI_ATTR_SCHEMA_NAME:   /* the schema name */
		break;
	case KCI_ATTR_SUB_NAME:      /* type name if package private type */
		break;
	case KCI_ATTR_POSITION:      /* relative position of col/arg in the list of cols/args */
		break;
	case KCI_ATTR_PACKAGE_NAME:  /* package name of package type */
		break;		
	default:
		setError(errhp, 111, "Invalid attribute type");
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
}


/*开始会话*/
sword   KCISessionBegin(KCISvcCtx  *p_svctx,KCIError   *errhp, 
						KCISession *p_sess,ub4 credt, ub4 mode)
{
	KCIServer *p_server = NULL;
	p_server = p_svctx->p_server;
	
	/****step1:检查属性及参数***/
	if (!p_server)
	{
		setError(errhp, 111, "Lake server infomation");
		return KCI_ERROR;
	}
	if (!p_sess)
	{
		setError(errhp, 111, "Lake session infomation");
		return KCI_ERROR;
	}
	if (credt != KCI_CRED_RDBMS)
	{
		setError(errhp, 111, "No supported credit type");
		return KCI_ERROR;
	}
	if (mode != KCI_DEFAULT && mode != KCI_SYSDBA && mode != KCI_SYSOPER)
	{
		setError(errhp, 111, "No supported auth type");
		return KCI_ERROR;
	}	
	/****step2:检查及创建到数据库服务器的连接***/
	return buildServer(p_server, p_sess, errhp);
}

/*结束会话*/
sword   KCISessionEnd(KCISvcCtx *p_svctx, KCIError *errhp, 
					  KCISession *p_sess,ub4 mode)
{
	p_svctx->p_sess = NULL;
	p_sess->p_svctx = NULL;
	//设置状态
	return KCI_SUCCESS;
}

/*单用户登录*/
sword   KCILogon(KCIEnv *envhp,KCIError *errhp, 
	KCISvcCtx **p_svctx,const OraText *username, 
	ub4 uname_len,const OraText *password, 
	ub4 passwd_len,const OraText *dbname, 
	ub4 dbname_len)
{
	KCISvcCtx  *ctxhp = NULL;
	KCIServer  *p_srv = NULL;
	KCISession *p_sess = NULL;

	if (KCIHandleAlloc(envhp, (void**)&ctxhp, KCI_HTYPE_SVCCTX, 0, 0) != KCI_SUCCESS)
	{
		setError(errhp, 111, "Alloc handle failed");
		return KCI_ERROR;
	}
	if (KCIHandleAlloc(envhp, (void**)&p_srv, KCI_HTYPE_SERVER, 0, 0) != KCI_SUCCESS)
	{
		KCIHandleFree(ctxhp, KCI_HTYPE_SVCCTX);
		setError(errhp, 111, "Alloc handle failed");
		return KCI_ERROR;
	}
	if (KCIHandleAlloc(envhp, (void**)&p_sess, KCI_HTYPE_SESSION, 0, 0) != KCI_SUCCESS)
	{
		KCIHandleFree(ctxhp, KCI_HTYPE_SVCCTX);
		KCIHandleFree(p_srv, KCI_HTYPE_SERVER);
		setError(errhp, 111, "Alloc handle failed");
		return KCI_ERROR;
	}
	if (KCIServerAttach(p_srv, errhp, dbname, dbname_len, KCI_DEFAULT) != KCI_SUCCESS)
	{
		KCIHandleFree(ctxhp, KCI_HTYPE_SVCCTX);
		KCIHandleFree(p_srv, KCI_HTYPE_SERVER);
		KCIHandleFree(p_sess, KCI_HTYPE_SESSION);
		setError(errhp, 111, "Attach server failed");
		return KCI_ERROR;
	}
	if (KCISessionBegin(ctxhp, errhp, p_sess, KCI_CRED_RDBMS, KCI_DEFAULT) != KCI_SUCCESS)
	{
		KCIHandleFree(ctxhp, KCI_HTYPE_SVCCTX);
		KCIHandleFree(p_srv, KCI_HTYPE_SERVER);
		KCIHandleFree(p_sess, KCI_HTYPE_SESSION);
		setError(errhp, 111, "Start session failed");
		return KCI_ERROR;
	}
	ctxhp->cre_mode = 1;
	if(p_svctx) *p_svctx = ctxhp;
	return KCI_SUCCESS;
}

sword   KCILogon2(KCIEnv *envhp, KCIError *errhp, KCISvcCtx **pp_svctx,
				const OraText *username, ub4 uname_len,
				const OraText *password, ub4 passwd_len,
				const OraText *dbname, ub4 dbname_len,
				ub4 mode)
{
	Svctx   *p_svctx = NULL;
	if (mode == KCI_DEFAULT)
	{//按照通常的session模式创建
		return KCILogon(envhp, errhp, pp_svctx, username, uname_len, 
						password, passwd_len, dbname, dbname_len);
	}
	if (mode & KCI_LOGON2_SPOOL)
	{
		Auth    auth;

		setAuthInfo(&auth, username, uname_len, password, passwd_len);
		Foreach(l, envhp->sp_list)
		{
			SPool *spool = EntryPtr(l, SPool, sp_link);
			if (!kstrcmp(spool->name, spool->name_len, dbname, dbname_len))
			{
				CHECK(spSessionGet(spool, errhp, &p_svctx, &auth, NULL,
					0, NULL, 0, NULL, mode));
				p_svctx->cre_mode = 3;
				*pp_svctx = p_svctx;
				return KCI_SUCCESS;
			}
		}
		setError(errhp, 1111, "Connection pool not exist.");
		return KCI_ERROR;
	}
	else
	{
		Serv    *p_serv = NULL;
		Sess    *p_sess = NULL;

		KCIHandleAlloc(envhp, (void**)&p_svctx, KCI_HTYPE_SVCCTX, 0, 0);
		KCIHandleAlloc(envhp, (void**)&p_serv, KCI_HTYPE_SERVER, 0, 0);
		KCIHandleAlloc(envhp, (void**)&p_sess, KCI_HTYPE_SESSION, 0, 0);
		setAuthInfo(p_sess, username, uname_len, password, passwd_len);
		if (mode & KCI_SESSGET_CPOOL)
			KCIServerAttach(p_serv, errhp, dbname, dbname_len, KCI_CPOOL);
		else
			KCIServerAttach(p_serv, errhp, dbname, dbname_len, KCI_DEFAULT);
		KCIAttrSet(p_svctx, KCI_HTYPE_SVCCTX, p_serv, 0, KCI_ATTR_SERVER, errhp);
		KCIAttrSet(p_svctx, KCI_HTYPE_SVCCTX, &p_sess, 0, KCI_ATTR_SESSION, errhp);
		CHECK(buildServer(p_serv, p_sess, errhp));
		p_svctx->cre_mode = 1; //创建方式
		*pp_svctx = p_svctx;
	}
	return KCI_SUCCESS;
}


/*用户登出*/
sword   KCILogoff(KCISvcCtx *p_svctx, KCIError *errhp)
{
	if (p_svctx->cre_mode != 1)
	{
		setError(errhp, 1111, "This session not created by KCILogon.");
		return KCI_ERROR;
	}
	//该会话使用了会话池吗？
	if (p_svctx->p_spool)
	{
		return spSessionRelease(p_svctx->p_spool, p_svctx, errhp, 0, 0, 0);
	}
	else
	{
		KCIServerDetach(p_svctx->p_server, errhp, 0);
		KCIHandleFree(p_svctx->p_server, KCI_HTYPE_SERVER);
		KCIHandleFree(p_svctx->p_sess, KCI_HTYPE_SESSION);
		if (p_svctx->p_trans)
			KCIHandleFree(p_svctx->p_trans, KCI_HTYPE_TRANS);
		KCIHandleFree(p_svctx, KCI_HTYPE_SVCCTX);
	}
	return KCI_SUCCESS;
}

/*修改登录口令*/
sword   KCIPasswordChange(KCISvcCtx *p_svctx, 
	KCIError *errhp,const OraText *user_name, 
	ub4 usernm_len,const OraText *opasswd, 
	ub4 opasswd_len,const OraText *npasswd, 
	ub4 npasswd_len,ub4 mode)
{
	char   cmd_str[256];
	Serv   *p_srv = p_svctx->p_server;
	PConn  *p_conn = getPhyConn(p_srv);
	char   *user = (char*)alloca(usernm_len + 2);
	char   *pass = (char*)alloca(npasswd_len + 2);

	memcpy(user, user_name, usernm_len);
	user[usernm_len] = 0x0;
	memcpy(pass, npasswd, npasswd_len);
	user[npasswd_len] = 0x0;
	sprintf(cmd_str, "ALTER USER %s IDENTIFIED BY '%s'", user, pass);	
	sword kci_ret =  execCommand(p_conn, cmd_str, errhp);
	putPhyConn(p_srv);
	return kci_ret;
}

/*释放连接*/
void  freeSvcCtx(KCISvcCtx *p_hdl)
{
	//关闭掉所有缓存的语句
	if (p_hdl)
	{
		mfree(p_hdl);
	}	
}
