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


typedef struct ConnOpt {

	char   url[256];
	char   db_name[128];
	char   char_set[32];
	char   time_zone[32];
	int    srv_ip;
	int    srv_port;
	bool   use_ssl;
}ConnOpt;

/*分析服务名，将其作为连接串进行解析，格式: domain_name_or_ip:port/db_name */
static  sword parseURL(cstr url_str0, int url_len, ConnOpt &conn_opt, KCIError *errhp)
{
	char *p_ch, *db_url, *db_port, *db_name, *url_str;

	//todo  加多种选项的破词 格式兼容 ip:port/db_name?foo1=val1&foo2=val2......    ip:port/db_name foo1=val1 foo2=.....
	if (url_len < 0)
		url_len = (sb4)strlen((char*)url_str0);
	url_str = (char*)alloca(url_len + 2);
	memcpy(url_str, url_str0, url_len);
	url_str[url_len] = 0x0;
	//查找':'号的位置,分离出url
	db_url = url_str;
	p_ch = strchr(url_str, ':');
	if (!p_ch)
	{
		setError(errhp, 111, "Invalid dblink string.");
		return KCI_ERROR;
	}
	*p_ch = 0x0;
	db_port = p_ch + 1;
	//查找'/'号的位置,分离端口及库名
	p_ch = strchr(db_port, '/');
	if (!p_ch)
	{
		setError(errhp, 111, "Invalid dblink string.");
		return KCI_ERROR;
	}
	*p_ch = 0x0;
	db_name = p_ch + 1;
	strcpy(conn_opt.db_name, db_name);
	//将db_url转换成ip
	conn_opt.srv_ip = inet_addr(db_url);
	if (conn_opt.srv_ip == -1)
	{
#if defined(LINUX)
		struct hostent  *p_hostent;
#else
		hostent *p_hostent;
#endif  
		p_hostent = gethostbyname(db_url);
		if (p_hostent)
		{
			conn_opt.srv_ip = *(int32*)(p_hostent->h_addr);
			if (conn_opt.srv_ip == -1)
			{
				setError(errhp, 111, "Invalid IP.");
				return KCI_ERROR;
			}
		}
		else
		{
			setError(errhp, 111, "Invalid IP.");
			return KCI_ERROR;
		}
	}
	//将db_port转换成port
	conn_opt.srv_port = atoi(db_port);
	if (conn_opt.srv_port <= 0)
	{
		setError(errhp, 111, "Invalid port.");
		return KCI_ERROR;
	}
	return KCI_SUCCESS;
}

/*初始化turing加密机*/
static  void init_turing_key(PConn *p_conn, char *key, int n)
{
	p_conn->turing_data_r = mcNew(&(p_conn->p_env->mem),TuringData);
	p_conn->turing_data_s = mcNew(&(p_conn->p_env->mem), TuringData);

	turing_key(p_conn->turing_data_r, (BYTE*)key, 32);
	turing_IV(p_conn->turing_data_r, (BYTE*)NULL, 0);
	turing_gen(p_conn->turing_data_r, (BYTE*)(p_conn->turing_data_r->key_stream));

	turing_key(p_conn->turing_data_s, (BYTE*)key, 32);
	turing_IV(p_conn->turing_data_s, (BYTE*)NULL, 0);
	turing_gen(p_conn->turing_data_s, (BYTE*)(p_conn->turing_data_s->key_stream));
	p_conn->turing_data_s->pos = 0;
	p_conn->turing_data_r->pos = 0;
}


/*建立到数据库服务器的实际连接*/
sword  buildPhyConn(PConn *p_conn, cstr db_url, Auth *auth,  KCIError *p_err)
{
	char     byte, buff[256];
	char     *link_str;
	int      one = 1;
	sockaddr_in  addr;
	ConnOpt  conn_opt;
	SOCKET   sock;
	
	CHECK(parseURL(db_url, -1, conn_opt, p_err));
	cstr   charset = NULL;
	size_t cslen = 0;
	if (p_conn->p_env)
	{
		switch (p_conn->p_env->char_set)
		{
		case KCI_GBK:
			cslen = 3;
			charset = "GBK";
			break;
		case KCI_GB18030:
			cslen = 7;
			charset = "GB18030";
			break;
		case KCI_UTF16ID:
		case KCI_UTF8:
		default:
			cslen = 4;
			charset = "UTF8";
			break;
		}
	}
	else
	{
		cslen = 4;
		charset = "UTF8";
	}
	conn_opt.use_ssl = false;// true;
	link_str = conn_opt.use_ssl ? buff + 32 : buff;
	sprintf(link_str, "LOGIN DATABASE=%s USER=%s PASSWORD=%s "
		"CHAR_SET=%s RETURN_SCENE=ON FETCH_SIZE=2000",
		conn_opt.db_name, auth->uname, auth->passw,charset);
	p_conn->b_encry = conn_opt.use_ssl;

	/**********step1:创建socket*******/
	memset(&addr, 0x0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = conn_opt.srv_ip;
	addr.sin_port = htons(conn_opt.srv_port);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0)
	{
		setError(p_err, 111, "Create socket failed");
		return KCI_ERROR;
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&one,
		sizeof(one));
	setsockopt(sock, IPPROTO_TCP, 0x0001,
		(char *)&one, sizeof(one));
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE,
		(char *)&one, sizeof(one));

	/*********step2:创建到服务器的连接******/
	if (connect(sock, (sockaddr*)&addr, sizeof(addr)) != 0)
	{
		setError(p_err, 111, "SQLDBC server failed");
		return KCI_ERROR;
	}
	/*******step3:进行登录验证***********/
	if (p_conn->b_encry)  //若是加密模式，则与数据库建立加密通道
	{
		DIGIT_T public_key[8];
		DIGIT_T trail_key[8];
		DIGIT_T encry_key[8];

		/*发送"~ssl~" */
		send(sock, "~ssl~", 5, 0);
		/*接收公钥 */
		recv(sock, (char*)public_key, 32,0);
		recv(sock, (char*)trail_key, 32,0);

		/*生成32字节随机数用作流加密器的键 */
		for (int j = 0; j<8; j++)
			encry_key[j] = spPseudoRand(0, MAX_DIGIT);

		/*将password 异或进key */
		char *p_ch = auth->passw;
		char *p_ch1 = (char*)encry_key;
		for (int j = 0; j<32; j++)
		{	
			*p_ch1++ ^= *p_ch++;
			if (!*p_ch)
				p_ch = auth->passw;
		}
		encry_key[7] &= ~HIBITMASK;
		memcpy(buff, encry_key, 32);
		int len = (int)strlen(link_str) + 1;
		len = rsa_encrypt(public_key, trail_key,buff,len + 32);
		send(sock, buff, len, 0);
		init_turing_key(p_conn, (char*)encry_key, 32);	
	}
	else
	{
		if (send(sock, link_str, (int)strlen(link_str) + 1, 0) <= 0)
		{
			setError(p_err, 111, "SQLDBC server failed");
			return KCI_ERROR;
		}
	}

	try {
		p_conn->srv_sock = sock;
		byte = readByte(p_conn);
		if (byte == '*') //有连接的场景信息
		{
			p_conn->curr_did = readI4(p_conn);
			p_conn->curr_sui = readI4(p_conn);
			p_conn->curr_uid = readI4(p_conn);
			p_conn->curr_sid = readI4(p_conn);
			byte = readByte(p_conn);
		}
		if (byte != 'K')
		{
			char message[256];
			readStr(p_conn, message, 255);
			setError(p_err, 111, message);
			return KCI_ERROR;
		}
	}
	catch (IOEx)
	{
		setError(p_err, 111, "Net error when recv");
		return KCI_ERROR;
	}
	p_conn->conn_status = 1;
	return KCI_SUCCESS;
}

/*对断了的物理连接进行重键*/
sword  reConnect(PConn *p_conn, Svctx *p_svctx, KCIError *p_err)
{
	Serv *p_srv = p_svctx->p_server;
	Sess *p_sess = p_svctx->p_sess;
	if (p_srv->conn_pool)
	{
		CPool *p_pool = p_srv->conn_pool;
		return buildPhyConn(p_conn, p_pool->conn_str, p_sess, p_err);
	}
	else
		return buildPhyConn(p_conn, p_srv->conn_str, p_sess, p_err);
	return KCI_SUCCESS;
}

/*执行一个简单命令(仅返回正确或错误信息)*/
sword  execCommand(PConn *p_conn, char *cmd_str, KCIError *p_err)
{
	int cmd_len = (int)strlen(cmd_str);
	try {
		sendByte(p_conn, '?');
		sendI4(p_conn, cmd_len);
		sendData(p_conn, cmd_str, cmd_len + 1);
		sendI4(p_conn, 0);
		netFlush(p_conn);
		for (;;)
		{
			byte b = readByte(p_conn);
			if (b == '*')
			{
				p_conn->curr_did = readI4(p_conn);
				p_conn->curr_sui = readI4(p_conn);
				p_conn->curr_uid = readI4(p_conn);
				p_conn->curr_sid = readI4(p_conn);
				continue;
			}
			if (b == '@')
			{
				p_conn->curr_did = readI4(p_conn);
				skipStr(p_conn);
				continue;
			}
			if (b == '#')
			{
				p_conn->curr_uid = readI4(p_conn);
				skipStr(p_conn);
				continue;
			}
			if (b == '$')
			{
				p_conn->curr_sid = readI4(p_conn);
				skipStr(p_conn);
				continue;
			}
			if (b == 'K')
			{
				p_conn->tran_status = readByte(p_conn);
				return KCI_SUCCESS;
			}
			else
			{
				char error[256];
				readStr(p_conn, error, 256);
				setError(p_err, 111, error);
				return KCI_ERROR;
			}
		}
	}
	catch (IOEx)
	{
		setError(p_err, 111, "Net error");
		return KCI_ERROR;
	}
}

sword closePhyConn(PConn *p_conn,KCIError *p_err)
{
	if (p_conn->tran_status & 0x1)
		execCommand(p_conn, "ROLLBACK", p_err);
	closesocket(p_conn->srv_sock);
	if (p_conn->b_encry)
	{
		mfree(p_conn->turing_data_r);
		mfree(p_conn->turing_data_s);
		p_conn->turing_data_r = NULL;
		p_conn->turing_data_s = NULL;
		p_conn->b_encry = false;
	}
	return KCI_SUCCESS;
}

/*测试是否可以成功地建立连接*/
sword testConnect(cstr url, cstr user, cstr passwd)
{
	PConn     conn;
	Auth      auth;
	KCIError  err;

	memset(&conn,0x0,sizeof(conn));
	memset(&err, 0x0, sizeof(err));
	setAuthInfo(&auth, user, astrlen(user), passwd, astrlen(passwd));
	if (buildPhyConn(&conn, url, &auth, &err) == KCI_SUCCESS)
	{
		closePhyConn(&conn, &err);
		return KCI_SUCCESS;
	}
	else
		return KCI_ERROR;
}
