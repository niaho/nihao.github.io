#include <malloc.h>
#include <memory.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_err.h"
#include "kci_net.h"

/*设置连接属性*/
sword setTransAttr(KCITrans *p_hdl,void  *attributep,
				ub4 size,ub4 attrtype,KCIError *errhp)
{
	switch (attrtype)
	{
	case KCI_ATTR_XID:
	{
		p_hdl->p_xid = *((XID**)attributep);
		if (!p_hdl->p_xid)
		{
			setError(errhp, 111, "Lake XID infomation");
			return KCI_ERROR;
		}		
	}
		break;
	case KCI_ATTR_TRANS_NAME:
		if (size > 0 && size < 64)
		{
			//memcpy(p_hdl->passw, attributep, size);
			//p_hdl->passw[size] = 0x0;
			//p_hdl->pass_len = size;
		}
		else
		{
			setError(errhp, 111, "trans name to long");
			return KCI_ERROR;
		}
		break;
	default:
		setError(errhp, 111, "Invalid parameter type");
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
}

void freeTrans(KCITrans *p_hdl)
{
	if (p_hdl)
	{
		int32	si;
		void *p = ((char*)p_hdl->p_xid) - 8;
		si = *(int32*)p;
		if (ChkMask(p, si))
			mfree(p_hdl->p_xid);
		mfree(p_hdl);
	}	
}

/*
This function sets the beginning of a global or serializable transaction. The
transaction context currently associated with the service context handle is
initialized at the end of the call if the flags parameter specifies that a new
transaction should be started.
*/
sword   KCITransStart(KCISvcCtx *p_svctx, KCIError *errhp,
					  uword timeout, ub4 flags)
{
	sword	   ret;
	KCIServer  *p_server = p_svctx->p_server;
	PhyConn    *p_conn = getPhyConn(p_server);
	KCITrans   *p_trans = p_svctx->p_trans;


	/****step1:检查属性及参数***/
	if (!p_server)
	{
		setError(errhp, 111, "Lake server infomation");
		return KCI_ERROR;
	}
	if (!p_trans)
	{
		setError(errhp, 111, "Lake trans infomation");
		return KCI_ERROR;
	}

	/****step2:执行事务语句***/
	switch (flags)
	{
		case KCI_TRANS_NEW:
		{
			p_trans->timeout = timeout;
			p_trans->auto_commit = false;
			ret = execCommand(p_conn, "set  auto commit off", errhp);
			if (ret == KCI_ERROR)
			{
				p_trans->auto_commit = true;
				return KCI_ERROR;
			}
		}
			break;
		case KCI_TRANS_RESUME:
		{
			
		}
			break;
		default:
			break;
	}
	
	return KCI_SUCCESS;
}

/*
Detaches a global transaction from the service context handle. The transaction
currently attached to the service context handle becomes inactive at the end
of this call. The transaction may be resumed later by calling KCITransStart(),
specifying  a flags value of KCI_TRANS_RESUME.
When a transaction is detached, the value which was specified in the timeout
parameter of KCITransStart() when the transaction was started is used to
determine the amount of time the branch can remain inactive before being
deleted by the server's PMON process.
Note: The transaction can be resumed by a different process than the one
that detached it, provided that the transaction has the same
authorization.
*/
sword   KCITransDetach(KCISvcCtx *p_svctx, KCIError *errhp, ub4 flags)
{
//	Trans *p1;
	//Trans *p = p_svctx->p_trans;//
//	p = NULL;
	p_svctx->p_trans = (Trans *)0;
	return KCI_SUCCESS;
};

/*
The transaction currently associated with the service context is committed. If
it is a distributed transaction that the server cannot commit, this call
additionally retrieves the state of the transaction from the database to be
returned to the user in the error handle.
If the application has defined multiple transactions, this function operates
on the transaction currently associated with the service context. If the
application is working with only the implicit local transaction created when
database changes are made, that implicit transaction is committed.
If the application is running in the object mode, then the modified or updated
objects in the object cache for this transaction are also committed.
The flags parameter is used for one-phase commit optimization in distributed
transactions. If the transaction is non-distributed, the flags parameter is
ignored, and KCI_DEFAULT can be passed as its value. KCI applications
managing global transactions should pass a value of
KCI_TRANS_TWOPHASE to the flags parameter for a two-phase commit. The
default is one-phase commit.
Under normal circumstances, KCITransCommit() returns with a status
indicating that the transaction has either been committed or rolled back. With
distributed transactions, it is possible that the transaction is now in-doubt
(i.e., neither committed nor aborted). In this case, KCITransCommit()
attempts to retrieve the status of the transaction from the server.
The status is returned.
*/
sword   KCITransCommit(KCISvcCtx *p_svctx, KCIError *errhp, ub4 flags)
{
	KCIServer *p_server = p_svctx->p_server;
	PhyConn   *p_conn = getPhyConn(p_server);
	KCITrans  *p_trans = p_svctx->p_trans;
	sword     kci_ret;

	/****step1:检查属性及参数***/
	if (!p_server || !p_conn)
	{
		setError(errhp, 111, "Lake server infomation");
		return KCI_ERROR;
	}
	if (!(p_conn->tran_status & 0x1))
		return KCI_SUCCESS;
	kci_ret = execCommand(p_conn, "COMMIT", errhp);
	if (p_server->mode)
		unbindPhyConn(p_server);
	return kci_ret;
}

/*
The current transaction- defined as the set of statements executed since the
last KCITransCommit() or since KCISessionBegin()-is rolled back.
If the application is running under object mode then the modified or updated
objects in the object cache for this transaction are also rolled back.
An error is returned if an attempt is made to roll back a global transaction
that is not currently active.
*/
sword   KCITransRollback(KCISvcCtx *p_svctx, KCIError *errhp, ub4 flags)
{
	KCIServer *p_server = p_svctx->p_server;
	PhyConn   *p_conn = getPhyConn(p_server);
	KCITrans   *p_trans = p_svctx->p_trans;
	sword     kci_ret;

	/****step1:检查属性及参数***/
	if (!p_server || !p_conn)
	{
		setError(errhp, 111, "Lake server infomation");
		return KCI_ERROR;
	}
	if (!(p_conn->tran_status & 0x1))
		return KCI_SUCCESS;
	kci_ret = execCommand(p_conn, "ROLLBACK", errhp);
	if (p_server->mode)
		unbindPhyConn(p_server);
	return kci_ret;
}

/*
Prepares the specified global transaction for commit.
This call is valid only for distributed transactions.
The call returns KCI_SUCCESS_WITH_INFO if the transaction has not made
any changes. The error handle will indicate that the transaction is read-only.
The flag parameter is not currently used.
*/
sword   KCITransPrepare(KCISvcCtx *p_svctx, KCIError *errhp, ub4 flags)
{

	return KCI_SUCCESS;
};

/*
Prepares the specified global transaction for commit.
This call is valid only for distributed transactions.
This call is an advanced performance feature intended for use only in
situations where the caller is responsible for preparing all the branches
in a transaction.
*/
sword   KCITransMultiPrepare(KCISvcCtx *p_svctx, ub4 numBranches,
	KCITrans **txns, KCIError **errhp)
{
	return KCI_SUCCESS;
};

/*
Forgets a heuristically completed global transaction. The server deletes the
status of the transaction from the system's pending transaction table.
The XID of the transaction to be forgotten is set as an attribute of the
transaction handle (KCI_ATTR_XID).
*/
sword   KCITransForget(KCISvcCtx *p_svctx, KCIError *errhp, ub4 flags)
{
	return KCI_SUCCESS;
};
