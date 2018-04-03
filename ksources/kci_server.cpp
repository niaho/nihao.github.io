#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#if defined(LINUX)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include "kci_defs.h"
#include "kci_net.h"
#include "kci_err.h"


sword setServerAttr(KCIServer *p_hdl, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp)
{
	switch (attrtype)
	{
	default:
		setError(errhp, 111, "Invalid attribute type");
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
};

/*给定服务名（连接字串或连接池名）,设置服务器控制结构属性*/
sword   KCIServerAttach(KCIServer *p_srv,
						KCIError *errhp,
						const OraText *dbname,
						sb4 dbname_len,
						ub4 mode)
{
	if (mode & KCI_CPOOL)
	{
		KCIEnv *p_env = p_srv->p_env;
		MemCtx *p_mem = &(p_env->mem);		
		wait_mutex(p_env->op_lock);
		Foreach(l, p_env->cp_list)
		{
			CPool *p_pool = EntryPtr(l,CPool,cp_link);
			if (!kstrcmp(p_pool->name, p_pool->name_len, dbname, dbname_len))
			{
				p_srv->conn_pool = p_pool;
				break;
			}
		}
		free_mutex(p_env->op_lock);
		if (!p_srv->conn_pool)
		{
			setError(errhp, 1, "Connection pool %s not exists", dbname);
			return KCI_ERROR;
		}
		//在server中记载服务器名（通常是连接字串）
		p_srv->conn_str = (char*)mc_alloc(p_mem, dbname_len + 2);
		memcpy(p_srv->conn_str, dbname, dbname_len);
		p_srv->conn_str[dbname_len] = 0x0;
		p_srv->mode = 1;
	}
	else
	{
		KCIEnv *p_env = p_srv->p_env;
		MemCtx *p_mem = &(p_env->mem);
		//在server中记载服务器名（通常是连接字串）
		p_srv->conn_str = (char*)mc_alloc(p_mem,dbname_len + 2);
		memcpy(p_srv->conn_str,dbname,dbname_len);
		p_srv->conn_str[dbname_len] = 0x0;
		//创建空的物理连接结构（未实际连接）
		p_srv->phy_conn = mcNew(p_mem,PhyConn);
		p_srv->mode = 0;
	}
	return KCI_SUCCESS;
}

/*创建物理连接连接*/
sword buildServer(KCIServer *p_srv, Auth *p_auth, KCIError *p_err)
{
	//虚拟连接方式（连接池方式）
	if (p_srv->conn_pool)
	{
		char  sql_str[256];
		sprintf(sql_str, "LOGIN USER=%s PASSWORD='%s'",p_auth->uname,p_auth->passw);
		//绑定一个物理连接
		bindPhyConn(p_srv);
		//执行login命令,目的是得到用户的会话环境变量
		CHECK(execCommand(p_srv->phy_conn, sql_str, p_err));
		p_srv->curr_did = p_srv->phy_conn->curr_did;
		p_srv->curr_uid = p_srv->phy_conn->curr_uid;
		p_srv->curr_sid = p_srv->phy_conn->curr_sid;
		p_srv->mode = 1;
		return KCI_SUCCESS;
	}
	//直接物理连接方式
	else
	{
		p_srv->phy_conn->p_env = p_srv->p_env;
		return buildPhyConn(p_srv->phy_conn, p_srv->conn_str, p_auth, p_err);
	}
}

/*断开与服务器的连接*/
sword   KCIServerDetach(KCIServer *p_srv,
						KCIError *errhp,
						ub4 mode)
{
	if (p_srv->conn_pool)
	{
		if (p_srv->phy_conn)
		{
			releasePhyConn(p_srv);
			p_srv->conn_pool = NULL;
		}
	}
	else
	{
		PConn *p_conn = p_srv->phy_conn;
		CHECK(closePhyConn(p_conn,errhp));
		mfree(p_conn);
	}
	return KCI_SUCCESS;
}

/*释放服务器控制结构*/
void freeServer(KCIServer *p_hdl)
{
	if (p_hdl)
	{
		mfree(p_hdl);
	}	
}
