#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_net.h"
#include "kci_time.h"
#include "kci_err.h"

static sword resetStmt(KCIStmt *stmtp, KCIError *errhp);

/*准备一个SQL语句*/
sword   KCIStmtPrepare(KCIStmt *stmtp, KCIError *errhp,
					   const OraText *stmt,ub4 stmt_len, 
					   ub4 language, ub4 mode)
{
	Param    *p_para;
	XList    para_list;
	int      sql_type, para_num;
	MemCtx   *p_mem = &(stmtp->mem);
	
	//对语句句柄进行清0
	mfree(stmtp->sql_str);
	reset_mem(&(stmtp->mem));
	resetStmt(stmtp, errhp);
	/****step1:分离参变量,拷贝语句*****/
	InitList(para_list);
	CHECK(parseSQL(stmt, (sb4)stmt_len, p_mem, sql_type, para_list, errhp));
	stmtp->sql_type = sql_type;
	stmtp->sql_str = (char*)mc_alloc(p_mem, stmt_len + 2);
	memcpy(stmtp->sql_str, stmt, stmt_len);
	stmtp->sql_str[stmt_len] = 0x0;
	stmtp->sql_len = stmt_len;
	/****step2:创建KCIBind数组*********/
	para_num = LLength(para_list);
	if (para_num > 0)
	{
		int     idx = 0;
		char    *sql = stmtp->sql_str;
		KCIBind *binds = mcNewN(p_mem, KCIBind, para_num);
		memset(binds, 0, sizeof(KCIBind));
		Foreach(l, para_list)
		{
			binds[idx].htype = KCI_HTYPE_BIND;
			p_para = EntryPtr(l, Param, link);
			//拷贝变量名字
			memcpy(binds[idx].name, stmt + p_para->off, p_para->len);
			binds[idx].name[p_para->len] = 0x0;
			binds[idx].ret_mode = 0;
			idx++;
		}
		stmtp->bind_num = para_num;
		stmtp->binds = binds;
	}
	return KCI_SUCCESS;
}

/*用Prepare st2 as select ...方式预定义一个SQL语句*/
sword KCIStmtPrepare2(KCISvcCtx *p_svctx, KCIStmt **stmtpp,
	KCIError *errhp,const OraText *stmt, ub4 stmt_len, 
	const OraText *key,ub4 key_len, ub4 language, ub4 mode)
{

	KCIEnv   *p_env = p_svctx->p_env;
	KCIStmt  *stmtp = NULL;
	MemCtx   *p_mem = NULL;

	if (!stmt || stmt_len == 0)
	{
		setError(errhp, 111, "Search prepared statement by key failed");
		return KCI_ERROR;
	}
	CHECK(KCIHandleAlloc(p_env, (void**)&stmtp, KCI_HTYPE_STMT, 0, 0));
	CHECK(KCIStmtPrepare(stmtp, errhp, stmt, stmt_len, language, mode));   //2016-08-08
	stmtp->p_svctx = p_svctx;
	if (stmtpp) 
		*stmtpp = stmtp;
	return KCI_SUCCESS;
}

/*对语句进行复位
	若语句在服务器上打开的游标未关闭,则关闭之
	释放掉结果集内存,并对结果集变量清0
*/
static sword resetStmt(KCIStmt *stmtp, KCIError *errhp)
{
	stmtp->binds = NULL;
	stmtp->defines = NULL;
	stmtp->fld_num = 0;
	stmtp->bind_num = 0;
	stmtp->defi_num = 0;
	reset_mem(&(stmtp->mem2));
	stmtp->curr_row_num = 0;
	stmtp->max_row_idx = 0;
	stmtp->row_tab = NULL;
	return KCI_SUCCESS;
}

/*释放预定义SQL语句*/
sword KCIStmtRelease(KCIStmt *stmtp, KCIError *errhp, 
			const OraText *key,ub4 key_len, ub4 mode)
{
	resetStmt(stmtp, errhp);
	return KCI_SUCCESS;
}

/*释放KCIStmt中的全部或部分成员(用于支持ODBC)*/
sword KCIStmtPartFree(KCIStmt *stmtp, ub2 option)
{
	#define SQL_CLOSE           0
	#define SQL_DROP            1
	#define SQL_UNBIND          2
	#define SQL_RESET_PARAMS    3
	switch (option)
	{
	case 0:  //关闭游标
		//todo 
		break;
	case 1: //释放全部资源
		//todo
		break;
	case 2: //释放stmtp->defines
		//todo
		break;
	case 3: //释放stmtp->binds
		//todo
		break;
	}
	return KCI_SUCCESS;
}

/*获取Bind信息*/
sword   KCIStmtGetBindInfo(KCIStmt *stmtp, KCIError *errhp,
		ub4 size,ub4 startloc, sb4 *found, OraText *bvnp[],
		ub1 bvnl[],OraText *invp[], ub1 inpl[], ub1 dupl[],
		KCIBind **hndl)
{
	//todo
	return KCI_SUCCESS;
}

/*取当前执行阶段需要进行设置的分片信息*/
sword   KCIStmtGetPieceInfo(KCIStmt *stmtp, KCIError *errhp,
		void  **hndlpp, ub4 *typep, ub1 *in_outp, ub4 *iterp,
		ub4 *idxp, ub1 *piecep)
{
	if (stmtp->piece_mode == 1)
	{
		KCIBind      *p_bind = stmtp->binds + stmtp->curr_bind_n;
		if (hndlpp)  *hndlpp = p_bind;
		if (typep)   *typep = KCI_HTYPE_BIND;
		if (in_outp) *in_outp = KCI_PARAM_IN;
		if (iterp)   *iterp = stmtp->curr_iter_n;
		if (piecep)  *piecep = p_bind->piece_ty;
		return KCI_SUCCESS;
	}
	else if (stmtp->piece_mode == 2)
	{
		KCIDefine    *p_defi = stmtp->defines + stmtp->curr_defi_n;
		if (hndlpp)  *hndlpp = p_defi;
		if (typep)   *typep = KCI_HTYPE_DEFINE;
		if (in_outp) *in_outp = KCI_PARAM_OUT;
		if (iterp)   *iterp = stmtp->curr_iter_n;
		if (piecep)  *piecep = p_defi->piece_ty;
		return KCI_SUCCESS;
	}
	else
	{
		setError(errhp, 1111, "无分片信息");
		return KCI_ERROR;
	}
}

/*设置当前执行阶段需要的分片信息*/
sword   KCIStmtSetPieceInfo(void  *hndlp, ub4 type,
	KCIError *errhp, const void  *bufp, ub4 *alenp,
	ub1 piece, const void  *indp, ub2 *rcodep)
{
	if (type == KCI_HTYPE_BIND)
	{
		KCIBind *p_bind = (KCIBind*)hndlp;
		p_bind->val_ptr = (char*)bufp;
		p_bind->alen_ptr = (char*)alenp;
		p_bind->piece_ty = piece;
		p_bind->ind_ptr = (char*)indp;
		p_bind->ret_ptr = (char*)rcodep;
		return KCI_SUCCESS;
	}
	else if(type == KCI_HTYPE_DEFINE)
	{
		KCIDefine *p_defi = (KCIDefine*)hndlp;
		p_defi->val_ptr = (char*)bufp;
		p_defi->alen_ptr = (char*)alenp;
		p_defi->piece_ty = piece;
		p_defi->ind_ptr = (char*)indp;
		p_defi->ret_ptr = (char*)rcodep;
		return KCI_SUCCESS;
	}
	else
	{
		setError(errhp, 1111, "错误的句柄类型");
		return KCI_ERROR;
	}
}

/*检查是否需要分片输入（即需要多次Execute）*/
inline static sword checkBinds(KCIStmt *p_stmt,int iters,KCIError *errhp)
{
	KCIBind *binds = p_stmt->binds;

	for (int i = 0; i < p_stmt->bind_num; i++)
	{
		if ((binds[i].bind_mode == 0) && (binds[i].val_dt == 0))
		{
			setError(errhp, 1111, "参数(%d)未绑定", i + 1);
			return KCI_ERROR;
		}
		if (binds[i].exe_mode == KCI_DATA_AT_EXEC
			&& binds[i].in_func == NULL && iters > 1)
		{
			setError(errhp, 1111, "多行执行不支持动态参数");
			return KCI_ERROR;
		}
	}
	return KCI_SUCCESS;
}

/*检查是否需要分片输出*/
inline static sword checkDefines(KCIStmt *p_stmt,int iters,KCIError *errhp)
{
	KCIDefine *defis = p_stmt->defines;
	for (int i = 0; i < p_stmt->defi_num; i++)
	{
		if (defis[i].val_dt == 0)
		{
			setError(errhp, 1111, "输出域(%d)未定义", i + 1);
			return KCI_ERROR;
		}
		if (defis[i].exe_mode == KCI_DATA_AT_EXEC && !defis[i].out_func)
		{
			setError(errhp, 1111, "动态输出需使用fetch或fetch2");
			return KCI_ERROR;
		}
	}
	return KCI_SUCCESS;
}

/*执行语句*/
sword   KCIStmtExecute(KCISvcCtx *p_svctx, KCIStmt *stmtp,
						KCIError *errhp,ub4 iters, ub4 rowoff, 
						const KCISnapshot *snap_in,
						KCISnapshot *snap_out, ub4 mode)
{
	KCIStmt *const p_stmt = stmtp;
	PhyConn *p_conn = getPhyConn(p_svctx->p_server);
	
	p_stmt->p_svctx = p_svctx;  //设置语句与上下文的关联	
	try{
		//语句未处于分片输入模式？
		if (!p_stmt->piece_mode)
		{
			p_stmt->returning = false;
			p_stmt->is_over = false;
			if (!stmtp->sql_str)
			{
				setError(errhp, 111, "SQLSTMT not prepared");
				return KCI_ERROR;
			}
			CHECK(checkBinds(stmtp, iters, errhp));
			CHECK(checkDefines(stmtp, iters, errhp));			
			if (p_stmt->sql_type == KCI_STMT_SELECT || iters == 1)
			{
				//发送SQL语句及SQL参数
				CHECK(sendQryHead(p_stmt, 1, errhp));
				CHECK(sendQryParams(p_stmt, 0, errhp));
				netFlush(p_conn);
				//接收SQL返回
				CHECK(recvResult(p_stmt, 0, errhp));
				//检查是否需要分片输入数据？
				if (p_stmt->piece_mode)
					return KCI_NEED_DATA;
				//若执行前,已进行了define,则执行fetch操作
				if (p_stmt->defi_num > 0)
				{
					CHECK(KCIStmtFetch(p_stmt, errhp, iters,
						KCI_FETCH_CURRENT, KCI_DEFAULT));
				}
				//尝试将物理连接归还到连接池
				putPhyConn(p_svctx->p_server);
			}
			else
			{
				//发送SQL语句
				CHECK(sendQryHead(p_stmt, iters, errhp));
				//发送多行SQL参数
				for(int it=0; it<iters; it++)
					sendQryParams(p_stmt, it, errhp);
				netFlush(p_conn);
				//接收SQL返回
				CHECK(recvResult(p_stmt, 0, errhp));
			}
		}
		//语句处于分片输入模式
		else
		{
			KCIBind *p_bind = &(p_stmt->binds[p_stmt->curr_bind_n]);
			void  *piece_data = p_bind->val_ptr;
			int    piece_len = *(ub4*)p_bind->alen_ptr;
			
			sendI4(p_conn, piece_len);
			sendData(p_conn, piece_data, piece_len);
			//若是最后一片数据,则补发送结尾符
			if (p_bind->piece_ty == KCI_LAST_PIECE)
				sendI4(p_conn, 0);
			netFlush(p_conn);
			//接收SQL返回
			CHECK(recvResult(p_stmt, 0, errhp));
			//检查是否需要分片输入数据？
			if (p_stmt->piece_mode)
				return KCI_NEED_DATA;
			//若执行前,已进行了define,则执行fetch操作
			if (p_stmt->defi_num > 0)
			{
				CHECK(KCIStmtFetch(p_stmt, errhp, iters,
					KCI_FETCH_CURRENT, KCI_DEFAULT));
			}
			//尝试将物理连接归还到连接池
			putPhyConn(p_svctx->p_server);
		}
	}
	catch (IOEx)
	{
		//清理语句
		mfree(p_stmt->sql_str);
		reset_mem(&(p_stmt->mem));
		//尝试修复连接
		reConnect(p_conn, p_svctx, errhp);
		setError(errhp, 111, "Net error");
		return KCI_ERROR;
	};
	return KCI_SUCCESS; //2018-04-02
}

/*抓取数据*/
/*
orientation (IN) -  The acceptable values are as follows, with
KCI_FETCH_NEXT being the default value.
KCI_FETCH_CURRENT gets the current row,
KCI_FETCH_NEXT gets the next row from the current position,
KCI_FETCH_FIRST gets the first row in the result set,
KCI_FETCH_LAST gets the last row in the result set,
KCI_FETCH_PRIOR gets the previous row from the current row in the result set,
KCI_FETCH_ABSOLUTE will fetch the row number (specified by fetchOffset
parameter) in the result set using absolute positioning,
KCI_FETCH_RELATIVE will fetch the row number (specified by fetchOffset
parameter) in the result set using relative positioning.
scrollOffset(IN) - offset used with the KCI_FETCH_ABSOLUTE and
KCI_FETCH_RELATIVE orientation parameters only. It specify
the new current position for scrollable result set. It is
ignored for non-scrollable result sets.
*/
sword   KCIStmtFetch2(KCIStmt *stmtp, KCIError *errhp, ub4 nrows,
	ub2 orientation, sb4 scrollOffset, ub4 mode)
{
	int ret=-1;
	switch (scrollOffset)
	{
	case KCI_FETCH_NEXT :  ///being the default value.
	case KCI_FETCH_CURRENT:  /// gets the next row from the current position,
		ret= KCIStmtFetch(stmtp, errhp, nrows, orientation, mode);
		break;
	case KCI_FETCH_FIRST:/// gets the first row in the result set,
		break;
	case KCI_FETCH_LAST:/// gets the last row in the result set,
		break;
	case KCI_FETCH_PRIOR:  /// gets the previous row from the current row in the result set,
		break;
	case KCI_FETCH_ABSOLUTE: /// will fetch the row number(specified by fetchOffset parameter) in the result set using absolute positioning,
		break;
	case KCI_FETCH_RELATIVE: // will fetch the row number(specified by fetchOffset parameter) in the result set using relative positioning.
		break;
	default:
		ret = -1;
	}
	return ret;	
}

/*抓取数据*/
sword   KCIStmtFetch(KCIStmt *stmtp, KCIError *errhp, ub4 nrows,
					ub2 orientation, ub4 mode)
{
	int    idx =-1;
	if (stmtp->max_row_idx < 1)
	{
		return KCI_NO_DATA;
	}
	for (ub4 row = 0; row < nrows ; row++)
	{
		if (stmtp->curr_fetch_num > stmtp->curr_row_num)
		{
			if (fetchRows(stmtp, errhp)<1)
				return KCI_NO_DATA;
		}
		switch (orientation)
		{
		case KCI_FETCH_CURRENT:      /* refetching current position  */
			idx = stmtp->curr_fetch_num  % stmtp->max_row_num;
			if (idx == 0)
				idx = 1;
			break;
		case KCI_FETCH_NEXT:                     /* next row */
			stmtp->curr_fetch_num++;
			if (stmtp->curr_fetch_num > stmtp->curr_row_num)
			{
				return KCI_NO_DATA;
			}
			idx = stmtp->curr_fetch_num  % stmtp->max_row_num;
			break;
		case KCI_FETCH_FIRST:  /* first row of the result set */	
			idx = (stmtp->curr_fetch_num - 1) % stmtp->max_row_num;
			if (idx == 0)
				idx = 1;
			break;
		case KCI_FETCH_LAST: /* the last row of the result set */
		case KCI_FETCH_PRIOR:/* previous row relative to current */
		case KCI_FETCH_ABSOLUTE: /* absolute offset from first */
		case KCI_FETCH_RELATIVE:  /* offset relative to current */
			idx = -1;
			break;
		}
		if (idx < 0)
		{
			setError(errhp, 111, "Not support orientation");
			return KCI_ERROR;
		}		

		KCIRow cols = stmtp->row_tab[idx-1];
		KCIDefine *def = stmtp->defines;
		for (uint i = 0; i < stmtp->fld_num; i++)
		{
			if (!def)
				break;
			KCIFld *fld = &(stmtp->fields[i]);
			memset(def->val_ptr + def->val_siz *row, 0, def->val_siz);
			void * p_data = cols[i].size <= 8 ? cols[i].data : cols[i].p_dat; //2018-04-02
			toSysData(fld->type_id, p_data, cols[i].size,
				def->val_dt,def->val_ptr + def->val_siz *row,def->val_siz);
			def =def->Next;			
		}		
	}
	return KCI_SUCCESS;
}

/*取多结果集的下一结果集(存贮过程返回引用游标时)*/
sword KCIStmtGetNextResult(KCIStmt *stmthp, KCIError *errhp,
						void **result,ub4 *rtype, ub4 mode)
{
	return KCI_SUCCESS;
}

/*设置语句属性*/
sword setStmtAttr(KCIStmt *p_hdl, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp)
{
	return KCI_SUCCESS;
}

/*取语句属性*/
sword getStmtAttr(KCIStmt *stmthp, void  *attributep,
	ub4 size, ub4 attrtype, KCIError *errhp)
{
	switch (attrtype)
	{
		case KCI_ATTR_STMT_TYPE:
			*(sb4*)attributep = stmthp->sql_type;
			break;
		case KCI_ATTR_ROWS_FETCHED:  /* get count of the data */
			*(sb4*)attributep = stmthp->curr_fetch_num;
			break;
		default:
			return KCI_ERROR;
	}
	return KCI_SUCCESS;
}


void freeStmt(KCIStmt *p_hdl)
{
	if (p_hdl)
	{
		mfree(p_hdl->sql_str);
		clear_mem(&(p_hdl->mem));
		mfree(p_hdl);
	}	
}

////Gets the length of a LOB/FILE. If the LOB/FILE is NULL, the length is  undefined.
sword KCILobGetLength(KCISvcCtx      *p_svctx,
	KCIError       *errhp,
	KCILobLocator  *locp,
	ub4            *lenp)
{
	KCIServer *p_server = p_svctx->p_server;

	/****step1:检查属性及参数***/
	if (!p_server)
	{
		setError(errhp, 111, "Lake server infomation");
		return KCI_ERROR;
	}

	if (!locp)
	{
		setError(errhp, 111, "Lake locp infomation");
		return KCI_ERROR;
	}
	*lenp = locp->datLen;
	return KCI_SUCCESS;
};
/*
Comments
Writes a buffer into a LOB as specified. If LOB data already exists
it is overwritten with the data stored in the buffer.
The buffer can be written to the LOB in a single piece with this call, or
it can be provided piecewise using callbacks or a standard polling method.
If this value of the piece parameter is KCI_FIRST_PIECE, data must be
provided through callbacks or polling.
If a callback function is defined in the cbfp parameter, then this callback
function will be invoked to get the next piece after a piece is written to
the pipe. Each piece will be written from bufp.
If no callback function is defined, then KCILobWrite() returns the
KCI_NEED_DATA error code. The application must all KCILobWrite() again
to write more pieces of the LOB. In this mode, the buffer pointer and the
length can be different in each call if the pieces are of different sizes and
from different locations. A piece value of KCI_LAST_PIECE terminates the
piecewise write.

Parameters
p_svctx (IN/OUT) - the service context handle.
errhp (IN/OUT) - an error handle which can be passed to KCIErrorGet() for
diagnostic information in the event of an error.
locp (IN/OUT) - a LOB locator that uniquely references a LOB.
offset (IN) - On input, it is the absolute offset, for character LOBs in
the number of characters from the beginning of the LOB, for binary LOBs it
is the number of bytes. Starts at 1.
bufp (IN) - the pointer to a buffer from which the piece will be written. The
length of the allocated memory is assumed to be the value passed in bufl.
Even if the data is being written in pieces, bufp must contain the first
piece of the LOB when this call is invoked.
bufl (IN) - the length of the buffer in bytes.
Note: This parameter assumes an 8-bit byte. If your platform uses a
longer byte, the value of bufl must be adjusted accordingly.
piece (IN) - which piece of the buffer is being written. The default value for
this parameter is KCI_ONE_PIECE, indicating the buffer will be written in a
single piece.
The following other values are also possible for piecewise or callback mode:
KCI_FIRST_PIECE, KCI_NEXT_PIECE and KCI_LAST_PIECE.
amtp (IN/OUT) - On input, takes the number of character or bytes to be
written. On output, returns the actual number of bytes or characters written.
If the data is written in pieces, *amtp will contain the total length of the
pieces written at the end of the call (last piece written) and is undefined in
between.
(Note it is different from the piecewise read case)
ctxp (IN) - the context for the call back function. Can be NULL.
cbfp (IN) - a callback that may be registered to be called for each piece in
a piecewise write. If this is NULL, the standard polling method will be used.
The callback function must return KCI_CONTINUE for the write to continue.
If any other error code is returned, the LOB write is aborted. The
callback takes the following parameters:
ctxp (IN) - the context for the call back function. Can be NULL.
bufp (IN/OUT) - a buffer pointer for the piece.
lenp (IN/OUT) - the length of the buffer (in octets) and the length of
current piece in bufp (out octets).
piecep (OUT) - which piece - KCI_NEXT_PIECE or KCI_LAST_PIECE.
csid - the character set ID of the buffer data
csfrm - the character set form of the buffer data
*/
sword   KCILobWrite(KCISvcCtx *p_svctx, KCIError *errhp, 
	KCILobLocator *locp,ub4 *amtp, ub4 offset, void  *bufp, 
	ub4 buflen,ub1 piece, void  *ctxp, KCICallbackLobWrite cbfp,
	ub2 csid, ub1 csfrm)
{
	KCIServer *p_server = p_svctx->p_server;
	KCITrans  *p_trans = p_svctx->p_trans;
	PhyConn   *p_conn = getPhyConn(p_server);

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
	sendI4(p_conn, buflen);
	//发数据内容
	sendData(p_conn, bufp, buflen);
	locp->datLen += buflen;
	return KCI_SUCCESS;
};

/*
Comments
Reads a portion of a LOB/FILE as specified by the call into a buffer. Data
read from a hole is returned as 0s. It is an error to try to read from a NULL
LOB/FILE. The OS FILE must already exist on the server and must have been
opened using the input locator. Oracle must hav epermission to read the OS
file and user must have read permission on the directory object.

Parameters
p_svctx (IN/OUT) - the service context handle.
errhp (IN/OUT) - an error handle which can be passed to KCIErrorGet() for
diagnostic information in the event of an error.
locp (IN/OUT) - a LOB locator that uniquely references a LOB.
offset (IN) - On input, it is the absolute offset, for character LOBs in the
number of characters from the beginning of the LOB, for binary LOBs it is the
number of bytes. Starts from 1.
amtp (IN/OUT) - On input, the number of character or bytes to be read. On
output, the actual number of bytes or characters read.
If the amount of bytes to be read is larger than the buffer length it is
assumed that the LOB is being read in a streamed mode. On input if this value
is 0, then the data shall be read in streamed mode from the LOB until the end
of LOB. If the data is read in pieces, *amtp always contains the length of
the last piece read.  If a callback function is defined, then this callback
function will be invoked each time bufl bytes are read off the pipe. Each
piece will be written into bufp.
If the callback function is not defined, then KCI_NEED_DATA error code will
be returned. The application must invoke the LOB read over and over again to
read more pieces of the LOB until the KCI_NEED_DATA error code is not
returned. The buffer pointer and the length can be different in each call
if the pieces are being read into different sizes and location.
bufp (IN) - the pointer to a buffer into which the piece will be read. The
length of the allocated memory is assumed to be bufl.
bufl (IN) - the length of the buffer in octets.
ctxp (IN) - the context for the call back function. Can be NULL.
cbfp (IN) - a callback that may be registered to be called for each piece. If
this is NULL, then KCI_NEED_DATA will be returned for each piece.
The callback function must return KCI_CONTINUE for the read to continue.
If any other error code is returned, the LOB read is aborted.
ctxp (IN) - the context for the call back function. Can be NULL.
bufp (IN) - a buffer pointer for the piece.
len (IN) - the length of length of current piece in bufp.
piece (IN) - which piece - KCI_FIRST_PIECE, KCI_NEXT_PIECE or
KCI_LAST_PIECE.
csid - the character set ID of the buffer data
csfrm - the character set form of the buffer data
*/
sword   KCILobRead(KCISvcCtx *p_svctx, KCIError *errhp, KCILobLocator *locp,
	ub4 *amtp, ub4 offset, void  *bufp, ub4 bufl, void  *ctxp,
	KCICallbackLobRead cbfp, ub2 csid, ub1 csfrm)
{
	KCIServer *p_server = p_svctx->p_server;
	PhyConn   *p_conn = getPhyConn(p_server);


	/****step1:检查属性及参数***/
	if (!p_server)
	{
		setError(errhp, 111, "Lake server infomation");
		return KCI_ERROR;
	}
	/////当大对象字段返回的值为描述符时，实际数据需要在使用时读取，读取的方法为：驱动向服务器发【get lob  descriptor  offset xx limit xx】
	////命令，服务器将向驱动返回一系列块数据，其格式为：
	////	块长１(I4)　块数据１　... 块长n(I4)  块数据n  0(I4)
	if (locp->flags)   /////描述符
	{
		sb4 num=0;
		char sql[128];
		sprintf(sql, "get lob  %s  offset %d limit %d", locp->dat, offset, bufl);
		//发数据内容
		sendStr(p_conn, sql);
		readI4(num);
		*amtp = 0;
		while (num>0)
		{	
			readData(p_conn, (char *)bufp+(*amtp), num);
			*amtp += num;
			readI4(num);
		};
	}
	else
	{
		ub4 num = locp->datLen-offset;
		if (num < 1)
		{
			return KCI_NO_DATA;
			char byte = readByte(p_conn);
			readI4(num);
			readData(p_conn, (char *)bufp , num);
			*amtp = num;
			return KCI_SUCCESS;
			switch (byte)
			{
			case 'E':  //错误信息
			case 'F':  //严重错误
				return KCI_ERROR;
			case 'K':  //结束标志
				p_conn->tran_status = readByte(p_conn);
				return KCI_NO_DATA;
			}			
			return KCI_NO_DATA;
		}
			
		*amtp = 0;
		if (bufl < num)
		{
			memcpy(bufp, locp->dat + offset, bufl);
			*amtp = bufl;
		}
		else
		{
			memcpy(bufp, locp->dat + offset, num);
			*amtp = num;
		};
	}
	return KCI_SUCCESS;
};
