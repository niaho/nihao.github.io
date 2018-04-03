#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "kci_defs.h"
#include "kci_err.h"
#include "kci_net.h"

/*直接从socket读取数据*/
int sockRecv(SOCKET sock, void *buff, int len)
{
	if (!(sock))
		return 0;
	int  ret, off = 0;
	while (len > 0)
	{
		ret = recv(sock, ((char*)buff) + off, len, 0);
		if (ret <= 0) 
			throw IOEx(1);
		off += ret;
		len -= ret;
	}
	return off;
}

/*直接向socket发送数据*/
int sockSend(SOCKET sock, void *buff, int len)
{
	if (!(sock))
		return 0;
	int  ret, off = 0;
	while (len > 0)
	{
		ret = send(sock, ((char*)buff) + off, len, 0);
		if (ret <= 0) 
			throw IOEx(2);
		off += ret;
		len -= ret;
	}
	return off;
}

/*从Sock读一字符串*/
int readStr(SOCKET sock, char *buff, int buf_len)
{
	int slen = readI4(sock);
	if (slen < buf_len)
	{
		sockRecv(sock, buff, slen);
		buff[slen] = 0x0;
		return slen;
	}
	else
	{
		/*超过buf_len长度的数据将丢弃*/
		void *buff1 = new char(slen);
		sockRecv(sock, buff1, slen);
		memcpy(buff, buff1, buf_len - 1);
		delete (char*)buff1;
		buff[buf_len - 1] = 0x0;
		return buf_len - 1;
	}
}

/*发送前对将要发送的数据进行加密*/
void encrypt_buff(PConn *p_conn, char *buff, int n)
{
	TuringData *p_td = p_conn->turing_data_s;
	for (int i = 0; i<n; i++)
	{
		*buff++ ^= p_td->key_stream[p_td->pos % 340];
		p_td->pos++;
		if (p_td->pos == 3400)
		{	/*重新生成加密流,加密流复用10次以节约计算*/
			turing_gen(p_td, p_td->key_stream);
			p_td->pos = 0;
		}
	}
}

/*接收后对刚接收的数据进行解密*/
void decrypt_buff(PConn *p_conn, char *buff, int n)
{
	TuringData *p_td = p_conn->turing_data_r;
	for (int i = 0; i<n; i++)
	{
		*buff++ ^= p_td->key_stream[p_td->pos % 340];
		p_td->pos++;
		if (p_td->pos == 3400)
		{	/*重新生成加密流,加密流复用10次以节约计算*/
			turing_gen(p_td, p_td->key_stream);
			p_td->pos = 0;
		}
	}
}

/*从SvcCtx中读取数据*/
void  readData(PConn *p_conn, void *buff, int len)
{
	if (!(p_conn->srv_sock))
		return;
	int  dat_siz, ret, off = 0;
	//读长数据
	if (len > _2K && !p_conn->b_encry)
	{
		//读完剩余数据
		if ((dat_siz = p_conn->recv_pos - p_conn->read_pos) > 0)
		{
			memcpy(((char*)buff) + off, p_conn->rbuff + p_conn->read_pos, dat_siz);
			len -= dat_siz;
			off += dat_siz;
		}
		p_conn->recv_pos = p_conn->read_pos = 0;
		//直接从socket读其余数据
		sockRecv(p_conn->srv_sock, ((char*)buff) + off, len);
	}
	else //读小数据
	{
		while (len > 0)
		{
			//计算可读的数据尺度
			dat_siz = p_conn->recv_pos - p_conn->read_pos;
			//可读数据超过要读的长度?
			if (dat_siz >= len)
			{
				memcpy(((char*)buff) + off, p_conn->rbuff + p_conn->read_pos, len);
				p_conn->read_pos += len;
				len = 0;
			}
			else //可读数据不足
			{
				//读完剩余数据
				if (dat_siz)
				{
					memcpy(((char*)buff) + off, p_conn->rbuff + p_conn->read_pos, dat_siz);
					p_conn->read_pos += dat_siz;
					len -= dat_siz;
					off += dat_siz;
				}
				//重新接收一批新数据
				p_conn->recv_pos = p_conn->read_pos = 0;
				memset(p_conn->rbuff, 0, sizeof(p_conn->rbuff));
				if ((ret = recv(p_conn->srv_sock, p_conn->rbuff, _2K, 0)) <= 0)
					throw IOEx(1);
				//对接收数据进行解密
				if (p_conn->b_encry)
					decrypt_buff(p_conn, p_conn->rbuff, ret);
				p_conn->recv_pos = ret;
			}
		}
	}
}

/*从SvcCtx读一字符串*/
int readStr(PConn *p_conn, char *buff, int buf_len)
{
	int slen = readI4(p_conn);
	if (slen < buf_len)
	{
		readData(p_conn, buff, slen);
		buff[slen] = 0x0;
		return slen;
	}
	else
	{
		void *buff1 = new char(slen);
		readData(p_conn, buff1, slen);
		memcpy(buff, buff1, buf_len - 1);
		delete (char*)buff1;
		buff[buf_len - 1] = 0x0;
		return buf_len - 1;
	}
}

/*向SvcCtx发送数据*/
void  sendData(PConn *p_conn, void *buff, int len)
{
	int  len1, mlen, off = 0;
	//发长数据(加密状态无效)
	if (len > _2K && !p_conn->b_encry)
	{
		//发完剩余数据
		if (p_conn->put_pos > 0)
		{
			sockSend(p_conn->srv_sock, p_conn->sbuff, p_conn->put_pos);
			off += p_conn->put_pos;
		}
		p_conn->put_pos = 0;
		//直接发送其余部分
		sockSend(p_conn->srv_sock, ((char*)buff) + off, len);
	}
	else //发小数据
	{
		while (len > 0)
		{
			//计算可用的发送缓冲区长度
			mlen = _2K-p_conn->put_pos;
			//缓冲区已满,则发送已有数据
			if (mlen == 0)
			{
				//对发送数据进行加密
				if (p_conn->b_encry)
					encrypt_buff(p_conn, p_conn->sbuff, p_conn->put_pos);
				sockSend(p_conn->srv_sock, p_conn->sbuff, p_conn->put_pos);
				p_conn->put_pos = 0;
				mlen = _2K;
			}
		    len1 = mlen>len?len:mlen;				
			memcpy(p_conn->sbuff+p_conn->put_pos,((char*)buff) + off, len1);
			p_conn->put_pos += len1;
			off += len1;
			len -= len1;
		}
	}
}

