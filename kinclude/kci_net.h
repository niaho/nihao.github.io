#ifndef __KCI_NET_IO_H__
#define __KCI_NET_IO_H__
#include "kci_defs.h"


#if defined(LINUX)
#include <arpa/inet.h>
#endif


int    sockRecv(SOCKET sock, void *buff, int len);
int    sockSend(SOCKET sock, void *buff, int len);
int    readStr(SOCKET, char *buff, int buf_len);

void   encrypt_buff(PConn *p_conn, char *buff, int n);
void   decrypt_buff(PConn *p_conn, char *buff, int n);
void   readData(PConn *p_conn, void *buff, int len);
void   sendData(PConn *p_conn, void *buff, int len);
int    readStr(PConn *p_conn, char *buff, int buf_len);
PConn  *getPhyConn(KCIServer *p_server);
sword  reConnect(PConn *p_conn, Svctx *p_svctx, KCIError *p_err);
sword  buildPhyConn(PConn *p_conn, cstr db_url, Auth *auth, KCIError *p_err);

inline static char readByte(SOCKET sock)
{
	char byte;
	sockRecv(sock, &byte, 1);
	return byte;
}

inline static int readI2(SOCKET sock)
{
	sb2 i2;
	sockRecv(sock, &i2, 2);
	return ntohs(i2);
}

inline static int readI4(SOCKET sock)
{
	sb4 i4;
//	sockRecv(sock, &i4, 2);
	sockRecv(sock, &i4, 4);
	return ntohl(i4);

}

inline static void sendByte(SOCKET sock, char byte)
{
	sockSend(sock, &byte, 1);
}

inline static void sendI2(SOCKET sock, sb2 i2)
{
	sb2 _i2 = htons(i2);
	sockSend(sock, &_i2, 2);
}

inline static void sendI4(SOCKET sock, sb4 i4)
{
	sb4 _i4 = htonl(i4);
	sockSend(sock, &_i4, 4);
}

inline static void sendStr(SOCKET sock, char *buff, int slen)
{
	sendI4(sock, slen);
	sockSend(sock, buff, slen);
}

inline static char readByte(PConn *p_conn)
{
	if (!(p_conn))
		return 0;
	char byte;
	if (p_conn->back_byte)
	{
		byte = p_conn->back_byte;
		p_conn->back_byte = 0x0;
	}
	else
		readData(p_conn, &byte, 1);
	return byte;
}

inline static void backByte(PConn *p_conn,byte bt)
{
	p_conn->back_byte = bt;
}

inline static int readI2(PConn *p_conn)
{
	sb2 i2;
	readData(p_conn, &i2, 2);
	return ntohs(i2);
}

inline static int readI4(PConn *p_conn)
{
	sb4 i4;
//	readData(p_conn, &i4, 2);
	readData(p_conn, &i4, 4);
	return ntohl(i4);
}

inline static char *readStr(PConn *p_conn)
{
	int  slen = readI4(p_conn);
	char *str = (char*)malloc(slen + 1);
	readData(p_conn, str, slen);
	str[slen] = 0x0;
	return str;
}

/*将网流跳过一个字串*/
inline static void skipStr(PConn *p_conn)
{
	char buff[256];
	int  slen = readI4(p_conn);
	if (slen > 256)
	{
		char *str = (char*)malloc(slen);
		readData(p_conn, str, slen);
		free(str);
	}
	else
		readData(p_conn, buff, slen);
}

inline static char *readStr(PConn *p_conn, MemCtx *p_mem)
{
	int  slen = readI4(p_conn);
	char *str = (char*)mc_alloc(p_mem, slen + 1);
	readData(p_conn, str, slen);
	str[slen] = 0x0;
	return str;
}

inline static void sendByte(PConn *p_conn, char byte)
{
	sendData(p_conn, &byte, 1);
}

inline static void sendI2(PConn *p_conn, sb2 i2)
{
	sb2 _i2 = htons(i2);
	sendData(p_conn, &_i2, 2);
}

inline static void sendI4(PConn *p_conn, sb4 i4)
{
	sb4 _i4 = htonl(i4);
	sendData(p_conn, &_i4, 4);
}

inline static void sendStr(PConn *p_conn, char *buff, int slen)
{
	sendI4(p_conn, slen);
	sendData(p_conn, buff, slen);
}

inline static void sendStr(PConn *p_conn, char *buff)
{
	int slen = (int)strlen(buff);
	sendI4(p_conn, slen);
	sendData(p_conn, buff, slen);
}

/*将缓冲区内容刷出到网络*/
inline static void  netFlush(PConn *p_conn)
{
	if (!(p_conn->srv_sock))
		return;
	if (p_conn->put_pos)
	{
		//对发送数据进行加密
		if (p_conn->b_encry)
			encrypt_buff(p_conn, p_conn->sbuff, p_conn->put_pos);
		sockSend(p_conn->srv_sock, p_conn->sbuff, p_conn->put_pos);
		p_conn->put_pos = 0;
	}
}

/*向服务器发简单命令*/
inline static void sendCmdStr(PConn *p_conn, char *cmd_str)
{
	int  cmd_len = astrlen(cmd_str);
	sendByte(p_conn, '?');
	sendI4(p_conn, cmd_len);
	sendData(p_conn, cmd_str, cmd_len + 1);
	sendI4(p_conn, 0);
	netFlush(p_conn);
}

#endif
