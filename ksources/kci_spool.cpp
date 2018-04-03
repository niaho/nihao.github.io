#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_net.h"

/*����һ���»Ự��SvcCtx��,��������������*/
static sword spCreateSvctx(KCIEnv *envhp,
	SPool *p_spool, KCIError *p_err,
	OraText *userid, ub4 uidLen,
	OraText *password, ub4 passwordLen,
	Svctx   **pp_svctx)
{
	Svctx   *p_svctx = NULL;
	Serv    *p_serv = NULL;
	Sess    *p_sess = NULL;

	/*��������ʼ��SvcCtx*/
	KCIHandleAlloc(envhp, (void**)&p_svctx, KCI_HTYPE_SVCCTX, 0, 0);
	KCIHandleAlloc(envhp, (void**)&p_serv, KCI_HTYPE_SERVER, 0, 0);
	KCIHandleAlloc(envhp, (void**)&p_sess, KCI_HTYPE_SESSION, 0, 0);
	KCIServerAttach(p_serv, p_err, (oratext*)p_spool->conn_str, p_spool->conn_slen, KCI_DEFAULT);
	KCIAttrSet(p_sess, KCI_HTYPE_SESSION, userid, uidLen, KCI_ATTR_USERNAME, p_err);
	KCIAttrSet(p_sess, KCI_HTYPE_SESSION, password, passwordLen, KCI_ATTR_PASSWORD, p_err);
	KCIAttrSet(p_svctx, KCI_HTYPE_SVCCTX, p_serv, 0, KCI_ATTR_SERVER, p_err);
	KCIAttrSet(p_svctx, KCI_HTYPE_SVCCTX, p_sess, 0, KCI_ATTR_SESSION, p_err);
	CHECK(buildServer(p_serv, p_sess, p_err));
	*pp_svctx = p_svctx;
	return KCI_SUCCESS;
}

/*�ӻỰ���Ƴ����ͷ�һ���Ự��SvcCtx),��������������*/
static sword spFreeSvctx(KCIEnv *envhp, Svctx *p_svctx)
{
	//step1:��tag_grps�����Ƴ�
	if (InList(&(p_svctx->tag_link)))
		ListRmv(&(p_svctx->tag_link));
	//step2:��usr_grps�����Ƴ�
	if (InList(&(p_svctx->idle_link)))
		ListRmv(&(p_svctx->idle_link));
	//step3:��all_dbcs���Ƴ�
	if (InList(&(p_svctx->pool_link)))
		ListRmv(&(p_svctx->pool_link));
	//step4:�ͷ�p_item->p_svctx
	CHECK(KCIHandleFree(p_svctx, KCI_HTYPE_SVCCTX));
	return KCI_SUCCESS;
}

/*�����Ự��*/
sword KCISessionPoolCreate(KCIEnv *envhp,KCIError *p_err, 
	KCISPool *p_spool,OraText **name_out, ub4 *nlen_out,
	const OraText *connStr, ub4 connSlen,ub4 sessMin, 
	ub4 sessMax, ub4 sessIncr,OraText *userid, ub4 uidLen,
	OraText *password, ub4 passwordLen,ub4 mode)
{
	if (!envhp)
		return KCI_ERROR;
	if (!p_spool)
		return KCI_ERROR;
	
	uint  pool_no = (uint)atom_inc(envhp->id_gen);
	sprintf((char*)p_spool->name, "KCICPool-%u", pool_no);
	p_spool->name_len = astrlen((char*)p_spool->name);
	*nlen_out = p_spool->name_len;
	*name_out = p_spool->name;
	p_spool->conn_str = (uchar*)mc_alloc(&envhp->mem, connSlen + 2);
	memcpy(p_spool->conn_str, connStr, connSlen);
	p_spool->conn_str[connSlen] = 0;
	p_spool->conn_slen = connSlen;

	memcpy(p_spool->user_name, userid, uidLen);
	p_spool->user_name[uidLen] = 0;
	p_spool->uname_len = connSlen;

	memcpy(p_spool->password, password, passwordLen);
	p_spool->password[passwordLen] = 0;
	p_spool->pass_len = passwordLen;

	p_spool->sessMin = sessMin;
	p_spool->sessMax = sessMax;

	InitList(p_spool->all_dbcs);
	InitList(p_spool->idle_list);
	InitList(p_spool->ntag_list);
	InitList(p_spool->tag_grps);
	
	//��ʼ��������
	init_mutex(p_spool->op_lock);
	//������ӳ�������ͬ��ģʽ������ͬһ�û�������
	if (mode & KCI_SPC_HOMOGENEOUS)
	{
		//�½�sessMin��Svctx���������������������ޱ�ǩ��
		for (uint i = 0; i < sessMin; i++)
		{
			Svctx *p_svctx = NULL;
			CHECK(spCreateSvctx(envhp, p_spool, p_err, userid, 
				uidLen, password, passwordLen, &p_svctx));
			ListAdd(p_spool->all_dbcs, &(p_svctx->pool_link));
			ListAdd(p_spool->idle_list, &(p_svctx->idle_link));
			ListAdd(p_spool->ntag_list, &(p_svctx->tag_link));
			p_svctx->p_spool = p_spool;
			p_svctx->cre_mode = 3; //��ʽ3ֻ����KCISessionRelease�����ͷ�
			p_spool->sessNum++;
		}
	}
	//���Ự�ؼ��뵽������ 
	wait_mutex(envhp->op_lock);
	ListAdd(envhp->sp_list, &(p_spool->sp_link));
	free_mutex(envhp->op_lock);
	return KCI_SUCCESS;
}

/*�ͷŻỰ�����������Ӷ���*/
sword KCISessionPoolDestroy(KCISPool *p_spool, KCIError *p_err, ub4 mode)
{
	KCIEnv *p_env = p_spool->p_env;
	wait_mutex(p_env->op_lock);
	ListRmv(&(p_spool->sp_link));
	free_mutex(p_env->op_lock);
	while (!LEmpty(p_spool->all_dbcs))
	{
		Svctx *p_svctx = RmvFirst(p_spool->all_dbcs, Svctx, pool_link);
		//��tag_grps�����Ƴ�
		ListRmv(&(p_svctx->tag_link));
		//��usr_grps�����Ƴ�
		if (InList(&(p_svctx->idle_link)))
			ListRmv(&(p_svctx->idle_link));
		//��all_dbcs���Ƴ�
		if (InList(&(p_svctx->pool_link)))
			ListRmv(&(p_svctx->pool_link));
		//�ͷŻỰ����
		KCIServerDetach(p_svctx->p_server, p_err, 0);
		KCIHandleFree(p_svctx->p_server, KCI_HTYPE_SERVER);
		KCIHandleFree(p_svctx->p_sess, KCI_HTYPE_SESSION);
		if (p_svctx->p_trans)
			KCIHandleFree(p_svctx->p_trans, KCI_HTYPE_TRANS);
		KCIHandleFree(p_svctx, KCI_HTYPE_SVCCTX);
	}
	close_mutex(p_spool->op_lock);
	close_event(p_spool->op_event);
	return KCI_SUCCESS;
}

/*��չ�Ự�أ����´���һЩ�Ự���䵽�Ự����*/
static  sword appendSPool(SPool *p_spool)
{
	KCIError error;
	Svctx *p_svctx = NULL;
	KCIEnv *envhp = p_spool->p_env;

	memset(&error, 0x0, sizeof(error));
	CHECK(spCreateSvctx(envhp, p_spool, &error, 
		p_spool->user_name,p_spool->uname_len, 
		p_spool->password, p_spool->pass_len, &p_svctx));
	ListAdd(p_spool->all_dbcs, &(p_svctx->pool_link));
	ListAdd(p_spool->idle_list, &(p_svctx->idle_link));
	ListAdd(p_spool->ntag_list, &(p_svctx->tag_link));
	p_svctx->p_spool = p_spool;
	p_svctx->cre_mode = 3; //��ʽ3ֻ����KCISessionRelease�����ͷ�
	p_spool->sessNum++;
	return KCI_SUCCESS;
}

/*��session pool��ȡ���򴴽���Svctx����*/
sword spSessionGet(SPool *p_spool, KCIError *p_err,
	KCISvcCtx **pp_svctx,KCIAuthInfo *p_auth,
	const OraText *tag_str, ub4 tag_slen,
	OraText **ret_tag, ub4 *ret_tag_len,
	boolean *found, ub4 mode)
{
	KCISvcCtx  *p_svctx = NULL;
	int        new_num = 0;
	bool       find = false;

	wait_mutex(p_spool->op_lock);
retry:
	//�б����
	if (tag_str != NULL)
	{
		//��ѯ���б�ǩ��
		Foreach(l, p_spool->tag_grps)
		{
			TagGrp *p_grp = GetFirst(p_spool->tag_grps, TagGrp, tag_link);
			//�ҵ�ƥ��ı�ǩ��
			if (!kstrcmp(p_grp->tag_str, p_grp->tag_len,tag_str, tag_slen))
			{
				//����ȡ������Ŀ
				if (!LEmpty(p_grp->dbc_list))
				{
					p_svctx = RmvFirst(p_grp->dbc_list, Svctx, tag_link);
					ListRmv(&(p_svctx->idle_link));
					find = true;
					break;
				}
			}
		}
	}
	if (p_svctx == NULL)
	{
		//ntag_list���ջ���չ�Ự�سɹ���
		if (!LEmpty(p_spool->ntag_list) || appendSPool(p_spool)==KCI_SUCCESS)
		{
			p_svctx = RmvFirst(p_spool->ntag_list, Svctx, tag_link);
			ListRmv(&(p_svctx->idle_link));
		}
		//����ƥ�����⣿
		else if (mode & KCI_SESSGET_SPOOL_MATCHANY && !LEmpty(p_spool->idle_list))
		{
			p_svctx = RmvFirst(p_spool->idle_list, Svctx, tag_link);
			ListRmv(&(p_svctx->tag_link));
		}
	}
	if (p_svctx != NULL)
	{
		free_mutex(p_spool->op_lock);
		if (ret_tag)
			*ret_tag = p_svctx->p_tag;
		if (ret_tag_len)
			*ret_tag_len = p_svctx->tag_len;
		if (found) *found = find;
		*pp_svctx = p_svctx;  //add by zhoufawei 2018-03-29
		return KCI_SUCCESS;
	}
	else
	{
		p_spool->waitNum++;
		reset_event(p_spool->op_event);
		free_mutex(p_spool->op_lock);
		if (p_spool->timeout > 0)
		{
			if (wait_event_t(p_spool->op_event, p_spool->timeout) == WAIT_TIMEOUT)
			{
				wait_mutex(p_spool->op_lock);
				p_spool->waitNum--;
				free_mutex(p_spool->op_lock);
				setError(p_err, 1111, "no enough idle sessions");
				return KCI_ERROR;
			}
			else
			{
				wait_mutex(p_spool->op_lock);
				p_spool->waitNum--;
				goto retry;
			}
		}
		else
		{
			wait_event(p_spool->op_event);
			wait_mutex(p_spool->op_lock);
			p_spool->waitNum--;
			goto retry;
		}
	}
}

/*��Svctx�����ͷ����Ự����*/
sword spSessionRelease(SPool *p_spool,Svctx *p_svctx, KErr *p_err,
					   OraText *tag, ub4 tag_len, ub4 mode)
{
	//Ҫɾ���ûỰ��
	if (mode & KCI_SESSRLS_DROPSESS)
	{
		wait_mutex(p_spool->op_lock);
		if (InList(&p_svctx->pool_link))
			ListRmv(&(p_svctx->pool_link));
		p_spool->sessNum--;
		free_mutex(p_spool->op_lock);
		KCIServerDetach(p_svctx->p_server, p_err, 0);
		KCIHandleFree(p_svctx->p_server, KCI_HTYPE_SERVER);
		KCIHandleFree(p_svctx->p_sess, KCI_HTYPE_SESSION);
		if (p_svctx->p_trans)
			KCIHandleFree(p_svctx->p_trans, KCI_HTYPE_TRANS);
		KCIHandleFree(p_svctx, KCI_HTYPE_SVCCTX);
	}
	//���±�ǸûỰ��
	else if(p_svctx->p_tag || (mode & KCI_SESSRLS_RETAG))
	{
		bool find = false;
		//���ñ��
		if (mode & KCI_SESSRLS_RETAG)
		{
			if (p_svctx->p_tag)
				mfree(p_svctx->p_tag);
			p_svctx->p_tag = (uchar*)mc_alloc0(&(p_spool->p_env->mem), tag_len + 1);
			p_svctx->tag_len = tag_len;
			memcpy(p_svctx->p_tag, tag, tag_len);
			p_svctx->p_tag[tag_len] = 0x0;
		}
		//���뵽��Ӧ�ı����
		wait_mutex(p_spool->op_lock);
		Foreach(l, p_spool->tag_grps)
		{
			TagGrp *p_grp = EntryPtr(l, TagGrp, tag_link);
			if (!strcmp(p_grp->tag_str, (char*)p_svctx->p_tag))
			{
				ListAdd(p_grp->dbc_list, &(p_svctx->tag_link));
				find = true;
				break;
			}
		}
		//����һ����ǩ��
		if (!find)
		{
			TagGrp *p_grp = mcNew(&(p_spool->p_env->mem), TagGrp);
			memcpy(p_grp->tag_str, tag, tag_len);
			p_grp->tag_len = tag_len;
			InitList(p_grp->dbc_list);
			ListAdd(p_spool->tag_grps, &(p_grp->tag_link));
			//��dbc�����ǩ��
			ListAdd(p_grp->dbc_list, &(p_svctx->tag_link));
		}
		//������ж���
		ListAdd(p_spool->idle_list, &(p_svctx->idle_link));
		if (p_spool->waitNum)
			set_event(p_spool->op_event);
		free_mutex(p_spool->op_lock);
	}
	//�ޱ��ģʽ�������ޱ�ǩ�飩
	else
	{
		wait_mutex(p_spool->op_lock);
		//�����ޱ����
		ListAdd(p_spool->ntag_list, &(p_svctx->tag_link));
		//������ж���
		ListAdd(p_spool->idle_list, &(p_svctx->idle_link));
		if (p_spool->waitNum)
			set_event(p_spool->op_event);
		free_mutex(p_spool->op_lock);
	}
	return KCI_SUCCESS;
}

/*������������*/
sword setSPoolAttr(KCISPool *p_hdl, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *p_err)
{
	switch (attrtype)
	{
	case KCI_ATTR_SPOOL_TIMEOUT:
		p_hdl->timeout = *((ub4 *)attributep);
		break;
	case KCI_ATTR_SPOOL_OPEN_COUNT:
		break;
	case  KCI_ATTR_SPOOL_MIN:      /* min session count */
	{
		ub4 min = *((ub4 *)attributep);
		if (min > p_hdl->sessMax)
		{
			setError(p_err, 111, "min is over Max");
			return KCI_ERROR;
		}
		p_hdl->sessMin = min;
	}
	break;
	case KCI_ATTR_SPOOL_MAX:      /* max session count */
	{
		ub4 max = *((ub4 *)attributep);
		if (max < p_hdl->sessMin)
		{
			setError(p_err, 111, "Max is less Min");
			return KCI_ERROR;
		}
		p_hdl->sessMax = max;
	}
	break;
	case KCI_ATTR_SPOOL_INCR:   /* session increment count */
		break;
	case KCI_ATTR_SPOOL_STMTCACHESIZE: /*Stmt cache size of pool  */
		break;
	default:
		setError(p_err, 111, "Invalid parameter type");
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
}

/*�ͷŻỰ�ؾ��*/
void freeSPool(KCISPool *p_hdl)
{
	mfree(p_hdl);
}
