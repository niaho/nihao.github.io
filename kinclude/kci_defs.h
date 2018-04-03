#ifndef __kci_defs_h__
#define __kci_defs_h__
#include "kci_mem.h"
#include "kci_api.h"
#include "kci_bigint.h"
#include "rh_ssl.h"

/*
 一个进程中可以创建多个KCIEnv;
 在一个KCIEnv中，可以创建多个KCISvcCtx.
 在一个KCISvcCtx中，根据昆仑数据库特性，具体如下：
    1、只能创建一个KCIError;
	2、只能创建一个KCIServer;每个只能对应一个KCISession
	3、只能创建一个KCISession;每个KCISession只能对应一个KCITrans，多个KCIStmt（但多个KCIStmt顺序执行也可，不能交叉执行）
 基于以上原则，对整个驱动进行重新设计和修改	
*/
#define KCIEn struct KCIEnv
typedef struct KCIHdl{
	int     htype;    //句柄类型
	KCIEn   *p_env;   //环境指针
	XList   hdl_link; //连接至env的hdl_list中
}KCIHdl;

/*错误信息结构*/
struct KCIError : public KCIHdl
{
	ub4     err_no;
	ub4     err_len;
	char    sqlstat[32];
	char    message[256];
};

/*定义KCIEnv*/
struct  KCIEnv : public KCIHdl
{
	BinMem  mem;
	mutex   op_lock;
	XList   hdl_list;  //句柄列表
	XList   cp_list;   //连接池构成的链
	XList   sp_list;   //会话池构成的链
	ub2     char_set;  //char字符集编号
	ub2     nchar_set; //nchar字符集编号
	ub2     odbc_ver;  //支持odbc版本
	long    id_gen;    //id值产生器（atom_inc方式）
#ifdef __KUNLUN_ODBC__
	KCIError error;
#endif
};

/*定义KCIThreadId*/
struct  KCIThreadId : public KCIHdl
{
	BinMem  mem;	
	DWORD   threadId;
};

/*定义KCIThreadHandle*/
struct  KCIThreadHandle : public KCIHdl
{
	BinMem  mem;
	DWORD threadHdl;
};

#define Sess   struct KCISession
#define Serv   struct KCIServer
#define Svctx  struct KCISvcCtx
#define Trans  struct KCITrans
#define Result struct KCIResult
#define Stmt   struct KCIStmt
#define CPool  struct KCICPool
#define SPool  struct KCISPool
#define PConn  struct PhyConn
#define KErr   struct KCIError
#define Auth   struct KCIAuthInfo

typedef void(*MsgHdl)(char *msg);
typedef void(*WrnHdl)(char *msg);
/*KCISvcCtx相当于DBC*/
struct KCISvcCtx : public KCIHdl
{
	XList   pool_link;	 //连接至会话池
	XList   idle_link;	 //连接至闲置队列
	XList   tag_link;	 //连接至标记队列
	uchar   *p_tag;	     //标签字串	
	SPool   *p_spool;    //指向会话池
	Serv    *p_server;   //指向server
	Sess    *p_sess;     //指向session
	Trans   *p_trans;    //指向事务控制
	MsgHdl  msg_handler; //消息处理函数指针
	WrnHdl  wrn_handler; //警告处理函数指针
#ifdef __KUNLUN_ODBC__
	char    *dsn;
	char    *db_name;
	char    *server_version;
	KCIError error;
#endif
	ub4     flag;        //特征控制位
	ub2     tag_len;     //tag的长度
	ub1     cre_mode;    //创建方式 1:logon 2:KCISessionBegin 3:KCISessionGet
};

typedef struct SessAttr{
	bool   auto_commit; //是否自动提交
	bool   rollbk_all;  //出错时回滚全部(否则回滚命令)
	int8   iso_level;   //事务隔离级别
	int    max_rows;    //最大返回行数
	int    fetch_size;  //单次返回行数
}SessAttr;

/*到数据库的物理连接（当物理连接的未结束语句数为0且不在事务中时，此连接可切换给其它虚连接）*/
typedef struct PhyConn {
	XList   link;
	XList   idle_link;
	KCIEnv  *p_env;
	int     srv_ip;     //服务器ip
	int     srv_port;   //服务器端口
	SOCKET  srv_sock;   //套接字
	TuringData *turing_data_r;
	TuringData *turing_data_s;
	Serv    *log_conn;  //当前绑定的逻辑连接(KCIServer对象)
	//以下三个变量用于连接池模式下,虚连接与实连接绑定时,确定是否执行环境切换
	uint    curr_did;   //当前库ID(用于支持连接池)
	uint    curr_sui;   //创建用户ID(用于支持连接池)
	uint    curr_uid;   //会话用户ID(用于支持连接池)
	uint    curr_sid;   //当前模式ID(用于支持连接池) 
	bool    b_encry;    //是否加密
	bool    scene_chged;//会话场景是否发生了变化
	ub1     tran_status;//事务状态 bit0:事务中 bit1:有未关闭的cursor
	ub1     conn_status;//连接状态 0:未连接 1:连接 2:忙碌 3:断开
	ub2     put_pos;    //放置位置
	ub2     recv_pos;   //接收位置
	ub2     read_pos;   //读取位置
	byte    back_byte;  //退回字符
	char    sbuff[_2K]; //发送缓冲区
	char    rbuff[_4K]; //接收缓冲区
}Phyconn;

/*连接池描述体*/
struct KCICPool : public KCIHdl
{
	XList   cp_link;     //连接至evn的cp_list中
	char    name[64];
	int     name_len;    //用户名长度
	
	ub4		connMin;
	ub4		connMax;
	ub4     connNum;
	ub4     waitNum;    //等候线程数
	char	*conn_str;
	sb4		conn_slen;
	char	uname[64];
	sb4		uname_len;
	char	passwd[64];
	sb4		pass_len;
	event   op_event;
	mutex   op_lock;    //多线程获得CPool中的互斥锁 	
	XList   phy_conns;  //物理连接链
	XList   idle_conns; //当前空闲的连接
};

/*服务器描述体*/
struct KCIServer : public KCIHdl
{
	char    *conn_str;  //连接串（或配置文件名）
	char    *user_name; //用户名
	uint    curr_did;   //当前库ID
	uint    curr_uid;   //当前用户ID
	uint    curr_sid;   //当前模式ID 
	CPool   *conn_pool; //关联到的连接池
	PConn   *phy_conn;  //物理连接
	ub1     mode;       //0:物理连接 1:使用连接池
	ub1     state;      //状态 0:未连接 1:连接 2:虚连接 
};

struct KCIAuthInfo : public KCIHdl
{
	int     uname_len;   //用户长度
	int     pass_len;    //口令长度
	char    uname[64];   //用户名 
	char    passw[64];   //口令
};

/*会话描述体*/
struct KCISession : public KCIAuthInfo
{
	bool    auto_commit; //是否自动提交
	bool    rollbk_all;  //出错时回滚全部(否则回滚命令)
	int8    iso_level;   //事务隔离级别
	int     max_rows;    //最大返回行数
	int     fetch_size;  //单次返回行数
	Svctx   *p_svctx;    //关联服务上下文
	bool    isBegin;
};

/*标签会话组*/
typedef struct TagGrp {
	XList   tag_link;   //用于连接至用户组中的标签队列
	XList   dbc_list;	//此标签下的DBC双向链
	char    tag_str[64];//标签字串
	int     tag_len;    //标签长度
}TagGrp;

/*会话池描述体*/
struct KCISPool : public KCIHdl
{
	XList   sp_link;     //连接至环境的sp_list中
	mutex   op_lock;     //多线程获得SPool中的互斥锁 	
	event   op_event;
	uchar   name[64];    //会话池名称
	int     name_len;    //用户名长度
	uchar	*conn_str;
	sb4		conn_slen;
	uchar	user_name[64];
	sb4		uname_len;
	uchar	password[64];
	sb4		pass_len;
	ub4     sessNum;	 //当前会话数
	ub4		sessMin;	 //最少sess数
	ub4		sessMax;	 //最多sess数
	ub4     incrNum;	 //创建数步长
	ub4		timeout;	 //超时时间
	ub4     waitNum;     //等候线程数
	XList   all_dbcs;    //全部DBC成员
	XList   idle_list;   //闲置会话链
	XList   ntag_list;   //无标记组
	XList   tag_grps;    //标记组链
	ub4     mode;        //模式
};
/*
* Transaction branch identification: XID and NULLXID:
*/
#define XIDDATASIZE		128     /* size in bytes */
#define MAXGTRIDSIZE    64      /* maximum size in bytes of gtrid */
#define MAXBQUALSIZE    64      /* maximum size in bytes of bqual */
struct xid_t {
	long        formatID;       /* format identifier */
	long        gtrid_length;   /* value from 1 through 64 */
	long        bqual_length;   /* value from 1 through 64 */
	char        data[XIDDATASIZE];
};
typedef struct xid_t XID;


/*KCILobLocator(数据对象参数绑定)*/
struct KCILobLocator :public KCIHdl{
	char	 name[8];
	int		 flags;
	boolean	 lobEmpty;  //是否按照标签模式发送
	long	 datLen;
	char	 *dat;
	void     *in_ctxp;  //输入回调期间作为参数传给回调函数
};

////KCIDateTime
struct KCIDateTime :public KCIHdl{
	sb2 KCIDateYYYY;         /* gregorian year; range is -4712 <= year <= 9999 */
	ub1 KCIDateMM;           /* month; range is 1 <= month < 12 */
	ub1 KCIDateDD;           /* day; range is 1 <= day <= 31 */
	ub1 KCITimeHH;           /* hours; range is 0 <= hours <=23 */
	ub1 KCITimeMI;           /* minutes; range is 0 <= minutes <= 59 */
	ub1 KCITimeSS;           /* seconds; range is 0 <= seconds <= 59 */
	sb4 KCITimeUS;                   
	ub1 TZ;
};

////Interval day second
struct KCIInterval :public KCIHdl{
	int		s; /*秒值*/
	int		us;	/*微秒值*/
};

typedef enum FieldFlag {
	FIELD_BASE_TAB = 0x1,	  //表示字段是事实表字段(否则为逻辑字段) 
	FIELD_NOT_NULL = 0x2,	  //表示字段具有非空约束 
	FIELD_IS_PRIMARY = 0x4,   //表示字段是主键 
	FIELD_IS_SERIAL = 0x8,    //表示字段是序列值 
	FIELD_IS_TIMESTAMP = 0x10,//表示字段是timestamp 
	FIELD_IS_LOB = 0x20,	  //表示字段是大对象类型 
	FIELD_IS_UNIQUE = 0x40,	  //表示字段是唯一值类型 
	FIELD_IS_ROWID = 0x80,	  //表示字段是rowid 
	FIELD_IS_DUMMY = 0x100,   //表示字段不应输出 
	FIELD_IS_HIDE = 0x200,	  //表示字段是隐藏字段
	FIELD_IS_VARYING = 0x400, //表示字段是变长
}FieldFlag;


typedef enum KunTypeId
{
	TYPE_NONE = 0,
	TYPE_NULL = 1,
	TYPE_BOOL = 2,
	TYPE_I1 = 3,
	TYPE_I2 = 4,
	TYPE_I4 = 5,
	TYPE_I8 = 6,
	TYPE_NUM = 7,
	TYPE_R4 = 8,
	TYPE_R8 = 9,

	TYPE_DATE = 10,
	TYPE_TIME = 11,
	TYPE_TIMEZ = 12,
	TYPE_DATETIME = 13,
	TYPE_DATETIMEZ = 14,

	TYPE_INTERVAL_Y2M = 16,
	TYPE_INTERVAL_D2S = 24,

	TYPE_ROWVER = 28,
	TYPE_GUID = 29,
	TYPE_CHAR = 30,
	TYPE_CHR0 = 31,
	TYPE_CLOB = 32,

	TYPE_BINARY = 33,
	TYPE_BLOB = 34,

	TYPE_GEOM = 35,
	TYPE_POINT = 36,
	TYPE_BOX = 37,
	TYPE_POLYLINE = 38,
	TYPE_POLYGON = 39,

	TYPE_ROWID = 40,
	TYPE_SIBLING = 41,
	TYPE_MAX_SYS,

	TYPE_BLADE_BEGIN = 101,
	TYPE_BLADE_END = 1000,

	TYPE_OBJECT = 1001,		//对象类型 
	TYPE_REFROW,
	TYPE_RECORD,			//记录类型 
	TYPE_VARRAY,			//数组类型 
	TYPE_TABLE,				//表类型 
	TYPE_ITABLE,			//Idxby表
	TYPE_CURSOR,			//引用记录类型(不能更改字段) 
	TYPE_REFCUR,			//REF_CURSOR类型 
	TYPE_ROWTYPE,			//引用行类型 
	TYPE_COLTYPE,			//引用列类型
	TYPE_CUR_REC,
	TYPE_PARAM,
}KunTypeID;

/*事务控制*/
struct KCITrans :public KCIHdl
{
	bool    auto_commit;            
	int     iso_level;              
	XID     *p_xid;
	int     timeout;
	int     isautobigdata;
	KCIStmt *stmtp;
};

/*KCIBind(查询参数绑定)*/
struct KCIBind{
	int      htype;     //句柄类型
	int      val_dt;    //数据类型
	char     name[32];  //参数名
	ub8      val_siz;   //变量长度
	ub4      val_skip;  //值区步长
	ub4      ind_skip;  //指示器区步长
	ub4      alen_skip; //实际长度区步长
	ub4      ret_skip;  //返回码区步长 
	ub4      max_alen;  //最大长度
	ub2      exe_mode;  //执行模式
	ub1      piece_ty;  //分片状态（KCI_ONE_PIECE~KCI_LAST_PIECE)
	bool     big_mode;  //长数据?
	bool     is_bind2;  //true:alen_ptr为ub4指针，否则为ub2指针
	ub2      bind_mode;  //绑定模式  0:常规模式，1：动态模式
	ub2      ret_mode;  //返回模式  0:常用模式，1：returning模式
	ub2      rows;
	char     *val_ptr;  //参数数据指针
	char     *ind_ptr;  //指示数据指针
	char     *alen_ptr; //实际长度指针
	char     *ret_ptr;  //返回码指针 
	char     *cure_ptr; //实际元素个数指针
	const struct 
	KCIType  *p_type;   //对象类型
	void     *in_ctxp;  //输入回调期间作为参数传给回调函数
	void     *out_ctxp; //输出回调期间作为参数传给回调函数
	KCICallbackInBind in_func; //输入回调
	KCICallbackOutBind out_func; //输出回调
};

/*OCJDefine(查询结果字段)*/
struct KCIDefine{
	int      htype;     //句柄类型
	char     *tab_name; //表名
	char     *col_name; //字段名
	char     *col_label;//别名
	int      col_index; ///序号
	int      val_dt;    //类型ID
	ub8      val_siz;   //变量长度
	ub4	     val_skip;  //结构区步长 
	ub4      ind_skip;  //指示器区步长
	ub4      alen_skip; //实际长度区步长
	ub4      ret_skip;  //返回码区步长 
	ub4      max_alen;  //最大长度
	ub2      exe_mode;  //执行模式
	ub1      piece_ty;  //分片类型
	bool     big_mode;  //长数据模式 
	char     *val_ptr;  //参数数据指针
	char     *ind_ptr;  //指示数据指针
	char     *alen_ptr; //实际长度指针
	char     *ret_ptr;  //返回码指针 
	void     *out_ctxp; //输出回调期间作为参数传给回调函数
	KCICallbackDefine out_func; //输出回调
	struct KCIDefine *Next;
};

/*字段描述结构*/
typedef struct KCIFld{
	char     *sch_name;  //模式名
	char     *tab_name;  //表名
	char     *col_name;  //字段名
	char     *col_label; //字段标签
	int      type_id;    //字段类型ID
	int      scale;      //字段精度
	int      flag;       //标志位
	int      para_no;    //参数号(支持returning) add by mazh 2016-10-20
}KCIFld;


/*KCIParam(参数描述结构)*/
struct KCIParam{
	ub2   dtype;         //数据类型
	text  *col_name;     //参数名
	ub4   col_name_len;  //参数名长度
};

/*列值定义*/
typedef struct KCICol{
	ub2      size;		   //数据长度
	union{
		char     data[8];  //数据缓冲区(长度未超过8时) 
		void     *p_dat;   //数据指针(长度超过8时)
	};
}KCICol;

/*行值定义*/
typedef KCICol  *KCIRow;

/*创建一个记录行*/
KCIRow *createRow(int row_size);
/*第i个字段为空?*/
bool isNull(KCIRow *p_row, int n);
void setNull(KCIRow *p_row, int n);
void clrNull(KCIRow *p_row, int n);
/*第i个字段为指针?*/
bool isPtr(KCIRow *p_row, int n);
void setPtr(KCIRow *p_row, int n);
void clrPtr(KCIRow *p_row, int n);

/*结果集对象*/
class RhResult
{
	KCIStmt   *p_stmt;
	int       cur_type;    //游标方式(前滚式,双向式)
	ub4       fld_num;     //结果集字段个数
	BinMem    mem2;        //结果集内存 
};

/*语句对象*/
struct KCIStmt : public KCIHdl
{
	LinkMem   mem;         //私有内存(除结果集外)
	BinMem    mem2;        //结果集内存 
	int       sql_type;    //语句类型
	int       cur_type;    //游标方式(前滚式,双向式)
	cstr      tag;         //用于标记语句
	char      *sql_str;    //SQL语句
	int       sql_len;     //SQL长度
	ub4       max_row_num; //最大可接收行数
	ub4       fetch_size;  //从服务器每次抓取条数
	ub4       curr_row_num;//当前缓存的行数(不可超过max_row_num)
	ub4       curr_fetch_num;//当前获取的行数(不可超过max_row_num)
	ub4       max_row_idx; //当前已收到行的最大行号(当游标方式为forwod only时该值可超过缓存数)
	ub4       fld_num;     //结果集字段个数
	ub4       bind_num;    //参数个数
	ub4       defi_num;    //结果字段数
	ub4       updt_num;    //变更数
	ub1       piece_mode;  //处理分片模式(1:分片输入 2:分片输出)
	bool      is_over;     //是否接收完毕
	bool      returning;   //有returing?
	ub4       curr_iter_n;
	union {
		ub2   curr_bind_n; //当前分片数据对应的bind下标
		ub2   curr_defi_n; //当前分片数据对应的define下标
	};
	KCISvcCtx *p_svctx;    //服务器上下文	
	char      *cursor;     //服务器游标名
	KCIFld    *fields;     //结果集字段数组
	KCIBind   *binds;      //用于绑定参数
	KCIDefine *defines;    //用于定义输出 
	KCIRow    *row_tab;	   //缓冲的行数据表
#ifdef __KUNLUN_ODBC__
	int      state;
	KCIError error;
#endif
};

typedef struct Param{
	XList  link;
	int    off;
	int    len;
	int    paramType;   //参数类型，输入，输出，输入输出
}Param;

void  freeSvcCtx(KCISvcCtx *p_hdl);
void  freeError(KCIError *p_hdl);
void  freeServer(KCIServer *p_hdl);
void  freeSession(KCISession *p_hdl);
void  freeStmt(KCIStmt *p_hdl);
void  freeTrans(KCITrans *p_hdl);
void  freeSPool(KCISPool *p_hdl);
void  freeCPool(KCICPool *p_hdl);
bool  check_server_alive(KCISvcCtx *p_svctx);
void  setError(KCIError *p_err, sb4 code, char *info,...);
sword testConnect(cstr url, cstr user, cstr passwd);
sword closePhyConn(PConn *p_conn, KCIError *p_err);
sword buildServer(KCIServer *p_server, Auth *p_auth, KCIError *p_err);
sword execCommand(PhyConn *p_conn, char *cmd_str, KCIError *p_err);
sword parseSQL(const OraText *stmt, sb4 stmt_len, MemCtx *p_mem,
			   int &sql_type,XList &para_list, KCIError *errhp);
sword toKunData(MemCtx *p_mem, void *sdat, int &dlen, sb2 &dt, char buff[512], char *&ret_ptr);
sb2   sqlT2KunT(sb2 dt, sb2 val_len);
sword toSysData(int val_dt, int type_id, void *p_dat, int ap_size, int db_size, char *val_ptr);
sword sendQryHead(KCIStmt *p_stmt, int iters,KCIError *p_err);
sword sendQryParams(KCIStmt *p_stmt, ub4 nth, KCIError *p_err);
sword recvResult(KCIStmt *const p_stmt, int iters, KCIError *p_err);
sword handleReturning(KCIStmt *stmtp, int iters, KCIError *errhp);
sword handleLobIStream(KCIStmt *stmtp, char *streamLabel, KCIError *p_err);
int   fetchRows(KCIStmt *p_stmt, KCIError *p_err);
sword spSessionGet(SPool *spool, KCIError *errhp,KCISvcCtx **pp_svctx, KCIAuthInfo *authhp,
	const OraText *tagInfo, ub4 tagInfoLen,OraText **retTagInfo, ub4 *retTagInfoLen,
	boolean *found, ub4 mode);
sword spSessionRelease(SPool *p_spool, Svctx *p_svctx, KErr *p_err,
	OraText *tag, ub4 tag_len, ub4 mode);
#define CHECK(Ret)  if(Ret==KCI_ERROR) return KCI_ERROR
PhyConn *bindPhyConn(KCIServer *p_server);
void  unbindPhyConn(KCIServer *p_server);
void  releasePhyConn(KCIServer *p_server);

typedef struct KCIEnv           KCIEnv;            /* KCI environment handle */
typedef struct KCIError         KCIError;          /* KCI error handle */
typedef struct KCISvcCtx        KCISvcCtx;         /* KCI service handle */
typedef struct KCIStmt          KCIStmt;           /* KCI statement handle */
typedef struct KCIBind          KCIBind;           /* KCI bind handle */
typedef struct KCIDefine        KCIDefine;         /* KCI Define handle */
typedef struct KCIDescribe      KCIDescribe;       /* KCI Describe handle */
typedef struct KCIServer        KCIServer;         /* KCI Server handle */
typedef struct KCISession       KCISession;        /* KCI Authentication handle */
typedef struct KCIComplexObject KCIComplexObject;  /* KCI COR handle */
typedef struct KCITrans         KCITrans;          /* KCI Transaction handle */
typedef struct KCISecurity      KCISecurity;       /* KCI Security handle */
typedef struct KCISubscription  KCISubscription;   /* subscription handle */
typedef struct KCILobLocator    KCILobLocator;     /* KCI Lob Locator descriptor */


inline void setAuthInfo(Auth *p_auth, const void *user, int user_len, const void *pass, int pass_len)
{
	p_auth->uname_len = user_len;
	p_auth->pass_len = pass_len;
	memcpy(p_auth->uname,user,user_len);
	memcpy(p_auth->passw, pass, pass_len);
	p_auth->uname[user_len] = 0x0;
	p_auth->passw[pass_len] = 0x0;
}

/*取KCIServer对象的物理连接，若是直接连接，则返回其物理连接，否则为其绑定物理连接并返回此物理连接*/
inline static PhyConn *getPhyConn(KCIServer *p_server)
{
	if (!p_server->mode)
		return p_server->phy_conn;
	else if (p_server->state == 1)
		return p_server->phy_conn;
	else
		return bindPhyConn(p_server);
}

/*放回KCIServer对象的物理连接，若是直接连接，则不做任何操作，否则将绑定的物理连接放回连接池*/
inline static void putPhyConn(KCIServer *p_server)
{
	//虚连接只能在实连接未处于事务状态时才可解绑定
	if (p_server->mode && !p_server->phy_conn->tran_status)
		unbindPhyConn(p_server);
}

void rsa_encrypt(DIGIT_T *encrypt_key, DIGIT_T *trail_key,
	char *buff, int len, char *out, int &out_len);
void rsa_decrypt(DIGIT_T *decrypt_key, DIGIT_T *trail_key,
	char *buff, int len, char *out, int &out_len);
int  rsa_encrypt(DIGIT_T *encrypt_key, DIGIT_T *trail_key,
	char *buff, int len);
int  rsa_decrypt(DIGIT_T *decrypt_key, DIGIT_T *trail_key,
	char *buff, int len);

void getBindPtr(KCIBind *p_bind, int nth, void *&val_ptr,
	void *&ind_ptr, void *&alen_ptr, void *&ret_ptr);

#endif
