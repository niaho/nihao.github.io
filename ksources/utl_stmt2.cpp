#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_net.h"
#include "kci_err.h"

/*跳过结果集返回流中的字段描述部分*/
static void  skipFields(KCIStmt *p_stmt)
{
	KCIServer *p_srv = p_stmt->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;
	//读字段个数
	int       fld_num = readI4(p_conn);
	char      buff[256];
	//从网络读取并丢弃各字段信息
	for (int i = 0; i < fld_num; i++)
	{
		//读取并丢弃字段全名
		readStr(p_conn, buff, 254);
		//读取并丢弃类型,精度及位标
		readData(p_conn, buff, 12);
	}
}

/*接收结果的字段描述部分*/
static void  recvFields(KCIStmt *p_stmt, bool for_returning = false)
{
	KCIServer *p_srv = p_stmt->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;
	MemCtx    *p_mem = &(p_stmt->mem);

	//读字段个数
	p_stmt->fld_num = readI4(p_conn);

	//创建字段数组
	p_stmt->fields = mcNewN(p_mem, KCIFld, p_stmt->fld_num);
	//读各个字段信息
	for (uint i = 0; i < p_stmt->fld_num; i++)
	{
		char *p_ch, *col_label = NULL,
			*sch_name = NULL, *tab_name = NULL;
		//读字段全名
		char *col_name = readStr(p_conn, p_mem);
		//分离出字段标签
		if ((p_ch = strchr(col_name, '%')) != NULL)
		{
			*p_ch = 0x0;
			col_label = p_ch + 1;
		}
		//分离模式名,表名,字段名
		if ((p_ch = strchr(col_name, '.')) != NULL)
		{
			*p_ch = 0x0;
			tab_name = col_name;
			col_name = p_ch + 1;

			if ((p_ch = strchr(col_name, '.')) != NULL)
			{
				*p_ch = 0x0;
				sch_name = tab_name;
				tab_name = col_name;
				col_name = p_ch + 1;
			}
		}
		p_stmt->fields[i].sch_name = sch_name;
		p_stmt->fields[i].tab_name = tab_name;
		p_stmt->fields[i].col_name = col_name;
		p_stmt->fields[i].col_label = col_label;

		/*********mazh 2016-10-20 add  begin**********/
		//读取的是returning字段吗？
		if (for_returning)
			p_stmt->fields[i].para_no = readI4(p_conn);
		/*********mazh 2016-10-20 add  end**********/

		//读字段数据类型号
		p_stmt->fields[i].type_id = readI4(p_conn);
		//读字段的精度标度
		p_stmt->fields[i].scale = readI4(p_conn);
		//读字段的属性位
		p_stmt->fields[i].flag = readI4(p_conn);
	}
}

/*接收消息并调用消息处理函数*/
static void  recvMsg(KCIStmt *p_stmt)
{
	char      *p_msg;
	KCISvcCtx *p_svctx = p_stmt->p_svctx;
	PhyConn   *p_conn = getPhyConn(p_svctx->p_server);

	if (p_svctx->msg_handler)
	{
		p_msg = readStr(p_conn);
		p_svctx->msg_handler(p_msg);
	}
	else
	{
		p_msg = readStr(p_conn);
		free(p_msg);
	}
}

/*接收警告并调用警告处理函数*/
static  void recvWrn(KCIStmt *p_stmt)
{
	char *p_wrn;
	KCISvcCtx *p_svctx = p_stmt->p_svctx;
	PhyConn   *p_conn = getPhyConn(p_svctx->p_server);

	if (p_svctx->wrn_handler)
	{
		p_wrn = readStr(p_conn);
		p_svctx->wrn_handler(p_wrn);
	}
	else
	{
		p_wrn = readStr(p_conn);
		free(p_wrn);
	}
}

/*接收错误信息*/
static void  recvErr(KCIStmt *p_stmt, KCIError *p_err)
{
	KCIServer *p_srv = p_stmt->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;

	p_err->err_len = readI4(p_conn);
	if (p_err->err_len < 256)
	{
		readData(p_conn, p_err->message, p_err->err_len);
		p_err->message[p_err->err_len] = 0x0;
	}
	else
	{
		char *p_str = (char*)malloc(p_err->err_len + 2);
		readData(p_conn, p_str, p_err->err_len);
		memcpy(p_err->message, p_str, 254);
		p_err->message[254] = 0x0;
	}
}

/*接收行数据*/
void  recvRows(KCIStmt *p_stmt)
{
	KCIServer *p_srv = p_stmt->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;
	MemCtx    *p_mem = &(p_stmt->mem2);

	if (!p_stmt->row_tab)
	{
		if (!p_stmt->fetch_size)
			p_stmt->fetch_size = 1000;
		if (p_stmt->max_row_num < p_stmt->fetch_size)
			p_stmt->max_row_num = p_stmt->fetch_size;
		p_stmt->row_tab = mcNewN(p_mem, KCIRow, p_stmt->max_row_num);
	}

	while (true)
	{
		byte bt = readByte(p_conn);
		if (bt == 'R')
		{
			p_stmt->max_row_idx++;
			int idx = p_stmt->curr_row_num %p_stmt->max_row_num;
			p_stmt->curr_row_num++;
			//对应行的行结构已存在(仅替换列值)?
			if (p_stmt->row_tab[idx])
			{
				KCIRow cols = p_stmt->row_tab[idx];
				for (uint i = 0; i < p_stmt->fld_num; i++)
				{
					int  siz = readI4(p_conn);
					if (cols[i].size > sizeof(int64))
						mfree(cols[i].p_dat);
					if (siz < sizeof(int64)) //2018-04-02
					{
						readData(p_conn, cols[i].data, siz);
					}
					else
					{
						cols[i].p_dat = mc_alloc(p_mem, siz);
						readData(p_conn, cols[i].p_dat, siz);
					}
					cols[i].size = siz;
				}
			}
			//否则,新建行结构并读入列值
			else
			{
				KCIRow cols = mcNewN(p_mem, KCICol, p_stmt->fld_num);
				for (uint i = 0; i < p_stmt->fld_num; i++)
				{
					int  siz = readI4(p_conn);
					if (siz <= sizeof(int64))
						readData(p_conn, cols[i].data, siz);
					else
					{
						cols[i].p_dat = mc_alloc(p_mem, siz);
						readData(p_conn, cols[i].p_dat, siz);
					}
					cols[i].size = siz;
				}
				p_stmt->row_tab[idx] = cols;
			}
		}
		else if (bt == 'C')
		{
			p_stmt->cursor = readStr(p_conn, &(p_stmt->mem));
			break;
		}
		else if (bt == 'M') //是通知吗?
			recvMsg(p_stmt);
		else if (bt == 'W')
			recvWrn(p_stmt);
		else
		{
			backByte(p_conn, bt);
			break;
		}
	}
}

void freeRows(KCIStmt *p_stmt)
{
	p_stmt->max_row_idx = 0;
	p_stmt->curr_row_num = 0;
	clear_mem(&(p_stmt->mem2));
	p_stmt->row_tab = NULL;
}

/*接收输出型参数*/
static sword recvOutPara(KCIStmt *p_stmt, int iters)
{
	//'P' 参数序号（0开始） 数据类型ID  值长度（I4) 参数数据
	KCIServer *p_srv = p_stmt->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;
	MemCtx    *p_mem = &(p_stmt->mem);
	ub4       para_no = readI4(p_conn);
	int       type_id = readI4(p_conn);
	ub4       val_len = readI4(p_conn);
	char      *buf = mcNewN(p_mem, char, val_len);

	readData(p_conn, buf, val_len);
	if (para_no < p_stmt->bind_num)
	{
		void    *val_ptr = NULL;
		void    *ind_ptr = NULL;;
		void    *alen_ptr = NULL;
		ub2     *ret_ptr = NULL;
		ub1     piecep = 0;
		KCIBind *p_bind = &(p_stmt->binds[para_no]);

		//若有回调函数，则由回调函数返回用于放置结果的地址
		if (p_bind->out_func)
		{
			p_bind->out_func(p_bind->out_ctxp, p_bind, 1,
				0, &val_ptr, (ub4**)&alen_ptr, &piecep,
				&ind_ptr, &ret_ptr);
		}
		else
		{
			val_ptr = p_bind->val_ptr;
			ind_ptr = p_bind->ind_ptr;
			alen_ptr = p_bind->alen_ptr;
			ret_ptr = (ub2*)p_bind->ret_ptr;
		}
		if (val_len == 0) //空值？
		{
			if (ind_ptr)
				*(sb2*)ind_ptr = -1;
			if (alen_ptr)
			{
				//是bind2模式吗（alen为 ub4类型）?
				if (p_bind->is_bind2)
					*(ub4*)alen_ptr = 0;
				else //alen为 ub2类型
					*(ub2*)alen_ptr = 0;
			}
		}
		else
		{
			int dlen = toSysData(type_id, buf, val_len, p_bind->val_dt,
				(char*)val_ptr, p_bind->val_siz);
			if (dlen < 0)
			{
				if (ind_ptr) *(sb2*)ind_ptr = -2;
				dlen = -dlen;
			}
			else
			{
				if (ind_ptr) *(sb2*)ind_ptr = 0;
			}
			if (alen_ptr)
			{
				//是bind2模式吗（alen为 ub4类型）?
				if (p_bind->is_bind2)
					*(ub4*)alen_ptr = (ub4)dlen;
				else //alen为 ub2类型
					*(ub2*)alen_ptr = (ub2)dlen;
			}
		}
	}
	return 0;
}

/*处理Returning返回数据*/   //add by mazh 2016-10-20
sword  handleReturning(KCIStmt *stmtp, int iters, KCIError *errhp)
{
	for (ub4 idx = 0; idx < stmtp->curr_row_num; idx++)
	{
		KCIRow cols = stmtp->row_tab[idx];

		for (uint i = 0; i < stmtp->fld_num; i++)
		{
			KCIFld  *fld = &(stmtp->fields[i]);
			KCIBind *p_bind = stmtp->binds + fld->para_no;
			void    *p_dat = cols[i].size > 8 ? cols[i].p_dat : cols[i].data;
			void    *val_ptr, *ind_ptr, *alen_ptr;
			void    *ret_ptr = NULL;
			ub1      piecep = 0;

			p_bind->rows = stmtp->curr_row_num;
			if (p_bind->out_func)
				p_bind->out_func(p_bind->out_ctxp, p_bind, iters, idx,
					&val_ptr, (ub4**)&alen_ptr, &piecep, &ind_ptr, (ub2**)&ret_ptr);
			else
				getBindPtr(p_bind, idx, val_ptr, ind_ptr, alen_ptr, ret_ptr);

			if (cols[i].size == 0) //空值？
			{
				if (ind_ptr)
					*(sb2*)ind_ptr = -1;
				if (alen_ptr)
				{
					//是bind2模式吗（alen为 ub4类型）?
					if (p_bind->is_bind2)
						*(ub4*)alen_ptr = 0;
					else //alen为 ub2类型
						*(ub2*)alen_ptr = 0;
				}
			}
			else
			{
				int dlen = toSysData(fld->type_id, p_dat, cols[i].size,
					p_bind->val_dt, (char*)val_ptr, p_bind->val_siz);
				if (dlen < 0)
				{
					if (ind_ptr) *(sb2*)ind_ptr = -2;
					dlen = -dlen;
				}
				else
				{
					if (ind_ptr) *(sb2*)ind_ptr = 0;
				}
				if (alen_ptr)
				{
					//是bind2模式吗（alen为 ub4类型）?
					if (p_bind->is_bind2)
						*(ub4*)alen_ptr = (ub4)dlen;
					else //alen为 ub2类型
						*(ub2*)alen_ptr = (ub2)dlen;
				}
			}
		}
	}
	return KCI_SUCCESS;
}

/*接收服务器执行语句后返回的结果集*/
sword recvResult(KCIStmt *const p_stmt, int iters, KCIError *p_err)
{
	KCIServer *p_srv = p_stmt->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;
	MemCtx    *p_mem = &(p_stmt->mem2);

	while (true)
	{
		switch (readByte(p_conn))
		{
		case 'A':  //结果集
			p_stmt->curr_row_num = 0;     ///当前缓存的行数
			p_stmt->curr_fetch_num = 0;   ///前一次读取到的行数
			recvFields(p_stmt, false);
			recvRows(p_stmt);
			break;
		case 'E':  //错误信息
		case 'F':  //严重错误
			recvErr(p_stmt, p_err);
			return KCI_ERROR;
		case 'M':  //消息
			recvMsg(p_stmt);
			break;
		case 'W':  //警告信息
			recvWrn(p_stmt);
			break;
		case '*': //连接的场景信息
			p_conn->curr_did = readI4(p_conn);
			p_conn->curr_sui = readI4(p_conn);
			p_conn->curr_uid = readI4(p_conn);
			p_conn->curr_sid = readI4(p_conn);
			break;
		case '@': //发生了库切换
			p_conn->curr_did = readI4(p_conn);
			p_srv->curr_did = p_conn->curr_did;
			skipStr(p_conn);
			break;
		case '#': //发生了用户切换
			p_conn->curr_uid = readI4(p_conn);
			p_srv->curr_uid = p_srv->curr_uid;
			skipStr(p_conn);
			break;
		case '$': //发生了模式切换
			p_conn->curr_sid = readI4(p_conn);
			p_srv->curr_sid = p_srv->curr_sid;
			skipStr(p_conn);
			break;
		case 'K':  //结束标志
			return KCI_SUCCESS;
		case 'k':
			p_conn->tran_status = readByte(p_conn);
			return KCI_SUCCESS;
		case 'T':  //带状态结束标志
			p_conn->tran_status = readByte(p_conn);
			return KCI_SUCCESS;
		case 'I':  //更改行数
			p_stmt->updt_num = readI4(p_conn);
			if (p_stmt->returning && p_stmt->curr_row_num)
			{
				handleReturning(p_stmt, iters, p_err);
				freeRows(p_stmt);
			}
			iters++;
			break;
		case 'U':  //更改行数
			p_stmt->updt_num = readI4(p_conn);
			if (p_stmt->returning && p_stmt->curr_row_num)
			{
				handleReturning(p_stmt, iters, p_err);
				freeRows(p_stmt);
			}
			iters++;
			break;
		case 'D':  //删除行数
			p_stmt->updt_num = readI4(p_conn);
			if (p_stmt->returning && p_stmt->curr_row_num)
			{
				handleReturning(p_stmt, iters, p_err);
				freeRows(p_stmt);
			}
			iters++;
			break;
		case 'P':  //输出参数
			recvOutPara(p_stmt, iters);
			break;
		case 'S':  //要求驱动传送流数据
			char streamLabel[8];
			readData(p_conn, streamLabel, 8);
			handleLobIStream(p_stmt, streamLabel, p_err);
			if (p_stmt->piece_mode)
				return KCI_NEED_DATA;
			break;
		case 'X':  //returning内容
			p_stmt->is_over = false;
			p_stmt->returning = true;
			recvFields(p_stmt, true);
			recvRows(p_stmt);
			break;
		case 'R':
			backByte(p_conn, 'R');
			recvRows(p_stmt);
			break;
		}
	}
	return KCI_SUCCESS;
}

/*从服务器端游标抓取下一批数据(返回实际行数,出错时返回-1)*/
int  fetchRows(KCIStmt *p_stmt, KCIError *p_err)
{
	KCIServer *p_srv = p_stmt->p_svctx->p_server;
	PhyConn   *p_conn = p_srv->phy_conn;
	MemCtx    *p_mem = &(p_stmt->mem2);
	char      cmd_str[32];
	byte      bt;

	if (!p_stmt->cursor)
		return 0;
	sprintf(cmd_str, "fetch %d from %s",
		p_stmt->fetch_size, p_stmt->cursor);
	sendCmdStr(p_conn, cmd_str);
	bt = readByte(p_conn);
	if (bt == 'E' || bt == 'F')
	{
		recvErr(p_stmt, p_err);
		mfree(p_stmt->cursor);
		return -1;
	}
	if (p_stmt->fields)
		skipFields(p_stmt);
	else
		recvFields(p_stmt);
	recvRows(p_stmt);
	//读结果集结束标志
	bt = readByte(p_conn);
	if (bt == 'K')
	{
		//读连接的事务状态
		p_conn->tran_status = readByte(p_conn);
		//此处为一个物理连接归还点
		if (p_srv->mode && !p_conn->tran_status)
			unbindPhyConn(p_srv);
	}
	return p_stmt->curr_row_num;
}

