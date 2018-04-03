#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_net.h"
#include "kci_err.h"

/*跳过语句中的字符串,返回跳过后的位置偏移(失败返回-1)*/
static int skipString(const OraText *stmt, int off, int stmt_len)
{
	off++;
	while (off < stmt_len)
	{
		if (stmt[off] == '\'')
		{
			off++;
			return off;
		}
		//是转义符?
		else if (stmt[off] == '\\')
			off += 2;
		else
			off++;
	}
	return -1;
}

/*跳过语句中的"标识符",返回跳过后的位置偏移(失败返回-1)*/
static int skipIdent(const OraText *stmt, int off, int stmt_len)
{
	off++;
	while (off < stmt_len)
	{
		if (stmt[off] == '"')
		{
			off++;
			return off;
		}
		//是转义符?
		else if (stmt[off] == '\\')
			off += 2;
		else
			off++;
	}
	return -1;
}

/*跳过语句中的块注释,返回跳过后的位置偏移(失败返回-1)*/
static int skipComment1(const OraText *stmt, int off, int stmt_len)
{
	off += 2;
	while (off < stmt_len)
	{
		if (stmt[off] == '*' && off < stmt_len - 1)
		{
			if (stmt[off + 1] == '/')
			{
				off += 2;
				return off;
			}
			else
				off++;
		}
		else
			off++;
	}
	return -1;
}

/*跳过语句中的行注释,返回跳过后的位置偏移(失败返回-1)*/
static int skipComment2(const OraText *stmt, int off, int stmt_len)
{
	off += 2;
	while (off < stmt_len)
	{
		if (stmt[off] == '\r')
		{
			if (off < stmt_len - 1 && stmt[off + 1] == '\n')
				off += 2;
			else
				off++;
			return off;
		}
		else if (stmt[off] == '\n')
		{
			off++;
			return off;
		}
		else
			off++;
	}
	return off;
}

/*检查第一个词得到SQL类型*/
inline static int  getSQLType(const OraText *stmt, int &off)
{
	int sql_type;
	if (!astrnicmp((char*)(stmt + off), "SELECT", 6))
	{
		off += 6;
		sql_type = KCI_STMT_SELECT;
	}
	else if (!astrnicmp((char*)(stmt + off), "INSERT", 6))
	{
		off += 6;
		sql_type = KCI_STMT_INSERT;
	}
	else if (!astrnicmp((char*)(stmt + off), "UPDATE", 6))
	{
		off += 6;
		sql_type = KCI_STMT_UPDATE;
	}
	else if (!astrnicmp((char*)(stmt + off), "DELETE", 6))
	{
		off += 6;
		sql_type = KCI_STMT_DELETE;
	}
	else if (!astrnicmp((char*)(stmt + off), "CREATE", 6))
	{
		off += 6;
		sql_type = KCI_STMT_CREATE;
	}
	else if (!astrnicmp((char*)(stmt + off), "DROP", 4))
	{
		off += 4;
		sql_type = KCI_STMT_DROP;
	}
	else if (!astrnicmp((char*)(stmt + off), "ALTER", 5))
	{
		off += 5;
		sql_type = KCI_STMT_ALTER;
	}
	else if (!astrnicmp((char*)(stmt + off), "EXEC", 5))
	{
		off += 5;
		sql_type = KCI_STMT_CALL;
	}
	else if (!astrnicmp((char*)(stmt + off), "BEGIN", 5))
	{
		off += 5;
		sql_type = KCI_STMT_BEGIN;
	}
	else if (!astrnicmp((char*)(stmt + off), "DECLARE", 7))
	{
		off += 7;
		sql_type = KCI_STMT_DECLARE;
	}
	else
		sql_type = KCI_STMT_UNKNOWN;
	return sql_type;
}

/*分析SQL语句,从中分离出占位符*/
sword parseSQL(const OraText *stmt, sb4 stmt_len, MemCtx *p_mem, 
			   int  &sql_type,XList &para_list, KCIError *errhp)
{
	Param  *p_para;
	sb4    off = 0;
	
	//跳过前面空白
	while (isspace(stmt[off]))
		off++;
	//跳过注释
	if (stmt[off] == '/' && stmt[off + 1] == '*')
	{
		if (skipComment1(stmt, off, stmt_len) < 0)
		{
			setError(errhp, 111, "Comment error");
			return KCI_ERROR;
		}
		while (isspace(stmt[off]))
			off++;
	}
	//是"//......"形式的注释?
	else if (stmt[off] == '/' && stmt[off + 1] == '/'
		|| stmt[off] == '-' && stmt[off + 1] == '-')
	{
		skipComment2(stmt, off, stmt_len);
		while (isspace(stmt[off]))
			off++;
	}
	sql_type = getSQLType(stmt, off);
	
	//扫描所有占位符循环
	while (off<stmt_len)
	{
		//当前字符是'?'?(无名占位符)
		if (stmt[off] == '?')
		{
			p_para = mcNew(p_mem, Param);
			p_para->off = off;
			p_para->len = 1;
			ListAdd(para_list, &(p_para->link));
			off++;
		}
		//当前字符是':'?(形如 :1或 :nam的占位符)
		else if (stmt[off] == ':')
		{
			p_para = mcNew(p_mem, Param);
			p_para->off = off;
			off++;
			while (isalnum(stmt[off]) && off < stmt_len)
				off++;
			p_para->len = off - p_para->off;
			if (p_para->len > 30)
			{
				setError(errhp, 111, "Parameter name too long");
				return KCI_ERROR;
			}
			ListAdd(para_list, &(p_para->link));
		}
		//是字符串?(跳过字符串)
		else if (stmt[off] == '\'')
		{
			off = skipString(stmt, off, stmt_len);
			if (off < 0)
			{
				setError(errhp, 111, "String error");
				return KCI_ERROR;
			}
		}
		else if (stmt[off] == '"')
		{
			off = skipIdent(stmt, off, stmt_len);
			if (off < 0)
			{
				setError(errhp, 111, "Ident error");
				return KCI_ERROR;
			}
		}
		else if (stmt[off] == '/' && off<stmt_len - 1)
		{
			//是"/*....*/"形式的注释?
			if (stmt[off + 1] == '*')
			{
				off = skipComment1(stmt, off, stmt_len);
				if (off < 0)
				{
					setError(errhp, 111, "Comment error");
					return KCI_ERROR;
				}
			}
			//是"//......"形式的注释?
			else if (stmt[off + 1] == '/')
				off = skipComment2(stmt, off, stmt_len);
			else
				off++;
		}
		//是"--......"形式的注释吗?
		else if (stmt[off] == '-' &&off < stmt_len - 1)
		{
			if (stmt[off + 1] == '-')
				off = skipComment2(stmt, off, stmt_len);
			else
				off++;
		}
		else
			off++;
	}
	return KCI_SUCCESS;
}

/*向服务器发送查询命令的头部*/
sword  sendQryHead(KCIStmt *p_stmt, int iters, KCIError *p_err)
{
	KCIServer *p_srv = p_stmt->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;

	// 需要发送带外数据？
	if (p_conn->scene_chged || iters > 1)
	{
		sendByte(p_conn, '*');
		//需要发送会话场景信息？
		if (p_conn->scene_chged)
		{
			sendByte(p_conn, 7);
			sendI4(p_conn, p_conn->curr_did);
			sendByte(p_conn, 8);
			sendI4(p_conn, p_conn->curr_sui);
			sendByte(p_conn, 9);
			sendI4(p_conn, p_conn->curr_sid);
		}
		if (iters > 1)
		{
			sendByte(p_conn, 6);
			sendI4(p_conn, iters);
		}
		sendByte(p_conn, 0);
	}
	else
		sendByte(p_conn, '?');
	sendI4(p_conn, p_stmt->sql_len);
	sendData(p_conn, p_stmt->sql_str, p_stmt->sql_len +1);
	return KCI_SUCCESS;
}

typedef struct StreamLabel
{
	ub4 bind_idx;
	ub4 param_nth;
}StreamLabel;

#define IsStrType(dt) (dt==TYPE_CHAR || dt==TYPE_CHR0)

/*向服务器发送查询参数*/
sword sendQryParams(KCIStmt *p_stmt, ub4 nth, KCIError *p_err)
{
	KCIServer *p_srv = p_stmt->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;
	MemCtx    *p_mem = &(p_stmt->mem);
	char      val_buff[512], *p_val = 0;

	sendI4(p_conn, p_stmt->bind_num);
	for (uint i = 0; i < p_stmt->bind_num; i++)
	{
		void    *val_ptr, *len_ptr, *ind_ptr, *alen_ptr,*ret_ptr;
		KCIBind *p_bind = p_stmt->binds + i;
		sb2     ind = 0, val_dt;
		int     val_len;

		val_dt = p_bind->val_dt; //源数据类型	
		getBindPtr(p_bind, nth, val_ptr, ind_ptr, alen_ptr, ret_ptr);
		if (ind_ptr)  ind = *(sb2*)ind_ptr;
		if (alen_ptr)
			val_len = p_bind->is_bind2 ? *(ub4*)alen_ptr : *(ub2*)alen_ptr;
		else
			val_len = p_bind->val_siz;
		//若参数非空,则检查参数类型并作必要转换,同时,修正长度
		if (ind != -1 && val_ptr)
		{
			CHECK(toKunData(p_mem, val_ptr, val_len, val_dt, val_buff, p_val));
		}
		else
			val_dt = sqlT2KunT(val_dt, val_len);
		//发参数名
		if (p_bind->name)
		{
			if (p_bind->name[0] == ':')
			{
				int name_len = strlen(p_bind->name + 1);
				sendI2(p_conn, name_len);
				sendData(p_conn, p_bind->name + 1, name_len);
			}
			else
			{
				int name_len = strlen(p_bind->name);
				sendI2(p_conn, name_len);
				sendData(p_conn, p_bind->name, name_len);
			}
		}
		else
			sendI2(p_conn, 0);
		//发标志位
		if (p_bind->exe_mode == KCI_DATA_AT_EXEC 
			&& p_bind->out_func == NULL)
		{
			ind = -1;
			sendI2(p_conn, PARAM_IS);
			//发参数类型ID
			if (val_dt == TYPE_BINARY)
				sendI2(p_conn, TYPE_BLOB);
			else if(IsStrType(val_dt))
				sendI2(p_conn, TYPE_CLOB);
			else
			{
				setError(p_err, 1111, "参数(%s)不支持分片输入",p_bind->name);
				return KCI_ERROR;
			}
		}
		else
		{
			sendI2(p_conn, 0);
			sendI2(p_conn, val_dt);
		}

		//是空值(发0)?
		if (ind == -1 || !val_ptr || p_bind->exe_mode == KCI_DATA_AT_EXEC)
		{
			//空值的时候，发送-1
			sendI4(p_conn, -1);
		}
		else
		{
			//发数据长度
			sendI4(p_conn, val_len);
			//发数据内容
			sendData(p_conn, p_val, val_len);
			//释放数据转换内存
			if (p_val != val_buff && p_val != val_ptr)
				mfree(p_val);
		}
	}
	return KCI_SUCCESS;
}

/*处理大对象输入流
  若绑定结构指定有回调函数，则从回调函数取数据
  否则，为语句对象设置分片控制
*/
sword handleLobIStream(KCIStmt *stmtp, char *streamLabel, KCIError *p_err)
{
	KCIServer *p_srv = stmtp->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;
	StreamLabel *label = (StreamLabel*)streamLabel;
	KCIBind   *p_bind = stmtp->binds + label->bind_idx;
	
	//若指定了回调函数,则调用回调函数取得输入数据
	if (p_bind->in_func)
	{
		void  *var_ptr = 0, *ind_ptr = 0;
		ub1   piece = KCI_FIRST_PIECE;
		ub4   alen = 0;

		while (piece != KCI_LAST_PIECE)
		{
			p_bind->in_func(p_bind->in_ctxp, p_bind, stmtp->curr_iter_n,
				label->bind_idx, &var_ptr, &alen, &piece, &ind_ptr);
			sendI4(p_conn, alen);
			sendData(p_conn, var_ptr, alen);
			if (piece == KCI_FIRST_PIECE)
				piece = KCI_NEXT_PIECE;
		}
		sendI4(p_conn, 0); //发结束符
		return KCI_SUCCESS;
	}
	//否则,设置分片控制信息,返回KCI_NEED_DATA
	else
	{
		stmtp->curr_bind_n = label->bind_idx;
		stmtp->piece_mode = 1;
		return KCI_NEED_DATA;
	}
}

