#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_net.h"


/*创建连接池对象*/
sword KCIConnectionPoolCreate(KCIEnv *envhp, KCIError *errhp, KCICPool *poolhp,
							OraText **poolName, sb4 *poolNameLen,
							const OraText *dblink, sb4 dblinkLen,
							ub4 connMin, ub4 connMax, ub4 connIncr,
							const OraText *poolUserName, sb4 poolUserLen,
							const OraText *poolPassword, sb4 poolPassLen,
							ub4 mode)
{
	if (!envhp)
		return KCI_ERROR;
	if (!poolhp)
		return KCI_ERROR;
	uint pool_no = (uint)atom_inc(envhp->id_gen);
	sprintf((char *)poolhp->name, "KCICPOOL-%u", pool_no);
	poolhp->name_len = astrlen((char *)poolhp->name);
	*poolName = (OraText *)poolhp->name;
	*poolNameLen = poolhp->name_len;
	poolhp->conn_str = (char *)mc_alloc(&envhp->mem, dblinkLen + 2);
	memcpy(poolhp->conn_str, dblink, dblinkLen);
	poolhp->conn_str[dblinkLen] = 0;
	poolhp->conn_slen = dblinkLen;
	memcpy(poolhp->uname, poolUserName, poolUserLen);
	poolhp->uname[poolUserLen] = 0;
	poolhp->uname_len = poolUserLen;

	memcpy(poolhp->passwd, poolPassword, poolPassLen);
	poolhp->passwd[poolPassLen] = 0;
	poolhp->pass_len = poolPassLen;

	poolhp->connMin = connMin;
	poolhp->connMax = connMax;

	InitList(poolhp->idle_conns);
	InitList(poolhp->phy_conns);
	init_mutex(poolhp->op_lock);
	init_event(poolhp->op_event);

	for (uint i = 0; i < connMin; i++)
	{
		PConn *p_conn = NULL;
		Auth auth;
		setAuthInfo(&auth, poolUserName, poolUserLen, poolPassword, poolPassLen);
		KCIError *p_err;
		CHECK(buildPhyConn(p_conn, poolhp->conn_str, &auth, p_err));
		ListAdd(poolhp->idle_conns, &(p_conn->link));
		ListAdd(poolhp->phy_conns, &(p_conn->link));
		poolhp->connNum++;
	}
	wait_mutex(envhp->op_lock);
	ListAdd(envhp->cp_list, &(poolhp->cp_link));
	free_mutex(envhp->op_lock);
	return KCI_SUCCESS;
}

/*销毁连接池对象*/
sword KCIConnectionPoolDestroy(KCICPool *poolhp,KCIError *errhp, ub4 mode)
{
	wait_mutex(poolhp->p_env->op_lock);
	ListRmv(&(poolhp->cp_link));
	free_mutex(poolhp->p_env->op_lock);
	mfree(poolhp->conn_str);
	close_mutex(poolhp->op_lock);
	close_event(poolhp->op_event);
	while(!LEmpty(poolhp->phy_conns))
	{
		PConn *p_conn = RmvFirst(poolhp->phy_conns, PConn, link);
		closePhyConn(p_conn,errhp);
	}
	return KCI_SUCCESS;
}

/*释放连接池对象*/
void freeCPool(KCICPool *p_hdl)
{
	mfree(p_hdl);
}

/*对虚连接进行物理连接绑定操作，返回绑定后的物理连接*/
PhyConn *bindPhyConn(KCIServer *p_server)
{
	PConn *p_conn = NULL;
	CPool *p_pool = p_server->conn_pool;

	wait_mutex(p_pool->op_lock);
retry:
	//逻辑与物理相互指向还存在？
	if (p_server->phy_conn)
	{
		p_conn = p_server->phy_conn;
		ListRmv(&(p_conn->idle_link));
		free_mutex(p_pool->op_lock);
	}
	//有空闲物理连接？
	else if (!(LEmpty(p_server->conn_pool->idle_conns)))
	{
		p_conn = RmvFirst(p_pool->idle_conns, PConn, idle_link);
		if (p_conn->log_conn)
			p_conn->log_conn->phy_conn = NULL;
		free_mutex(p_pool->op_lock);
		p_conn->log_conn = p_server;
		p_server->phy_conn = p_conn;
	}
	//连接池还可扩展？
	else if (p_pool->connNum < p_pool->connMax)
	{
		Auth auth;
		KCIError *p_err;
		p_pool->connNum++;
		free_mutex(p_pool->op_lock);
		setAuthInfo(&auth, p_pool->uname, p_pool->uname_len,
					p_pool->passwd, p_pool->pass_len);
		if (buildPhyConn(p_conn, p_pool->conn_str, &auth, p_err)==KCI_SUCCESS)
		{
			wait_mutex(p_pool->op_lock);
			ListAdd(p_pool->phy_conns, &(p_conn->link));
			free_mutex(p_pool->op_lock);
			p_conn->log_conn = p_server;
			p_server->phy_conn = p_conn;
		}
		else
		{
			wait_mutex(p_pool->op_lock);
			p_pool->connNum--;
			p_pool->waitNum++;
			free_mutex(p_pool->op_lock);
			wait_event(p_pool->op_event);
			wait_mutex(p_pool->op_lock);
			p_pool->waitNum--;
			goto retry;
		}
	}
	//只能重试
	else
	{
		p_pool->waitNum++;
		//清除条件值
		reset_event(p_pool->op_event);
		free_mutex(p_pool->op_lock);
		wait_event(p_pool->op_event);
		wait_mutex(p_pool->op_lock);
		p_pool->waitNum--;
		goto retry;
	}
	if (p_conn->curr_did != p_server->curr_did ||
		p_conn->curr_sid != p_server->curr_sid ||
		p_conn->curr_sui != p_server->curr_uid)
	{
		p_conn->curr_did = p_server->curr_did;
		p_conn->curr_sid = p_server->curr_sid;
		p_conn->curr_sui = p_server->curr_uid;
		p_conn->scene_chged = true;
	}
	else
		p_conn->scene_chged = false;
	return p_conn;
}

/*对虚连接进行物理连接解绑定操作，即将物理连接放回连接池*/
void  unbindPhyConn(KCIServer *p_server)
{
	CPool *p_pool = p_server->conn_pool;
	if (p_server->phy_conn)
	{
		p_server->state = 2;
		wait_mutex(p_pool->op_lock);
		ListAdd(p_pool->idle_conns, &(p_server->phy_conn->idle_link));
		if (p_pool->waitNum)
			set_event(p_pool->op_event);
		free_mutex(p_pool->op_lock);
	}
}

/*释放虚连接的物理连接，即将物理连接放回连接池并消除相互指向*/
void  releasePhyConn(KCIServer *p_server)
{
	CPool *p_pool = p_server->conn_pool;
	if (p_server->phy_conn)
	{
		PConn *p_conn = p_server->phy_conn;
		p_conn->log_conn = NULL;
		p_server->phy_conn = NULL;
		wait_mutex(p_pool->op_lock);
		ListAdd(p_pool->idle_conns, &(p_conn->idle_link));
		if (p_pool->waitNum)
			set_event(p_pool->op_event);
		free_mutex(p_pool->op_lock);
	}
	p_server->state = 0;
}

