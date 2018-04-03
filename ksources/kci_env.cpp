#include <malloc.h>
#include <memory.h>
#include <string.h>
#include "kci_defs.h"

#if defined(LINUX)
extern int IsBadWritePtr(void * ptr_buffer, unsigned long buffer_size);
#endif
ub4  kci_mode = 0;

#ifdef __WIN__
void startNetEnv()
{
	static    bool init_afx = false;
	unsigned  short wVersionRequested;
	WSADATA wsaData;
	if (!init_afx)
	{
		int err;
		init_afx = true;
		wVersionRequested = MAKEWORD(2, 2);
		err = WSAStartup(wVersionRequested, &wsaData);
	}
}
#else
#define startNetEnv() 
#endif

/*初始化驱动程序内存分配器*/
sword   KCIInitialize(ub4 mode, void  *ctxp,
	void *(*malocfp)(void  *ctxp, size_t size),
	void *(*ralocfp)(void  *ctxp, void  *memptr, size_t newsize),
	void(*mfreefp)(void  *ctxp, void  *memptr))
{
	kci_mode = mode;
	//若有一个指针非空,则要求所有指针非空
	if (ctxp || malocfp || ralocfp || mfreefp)
	{
		if (!ctxp || !malocfp || !ralocfp || !mfreefp)
			return KCI_ERROR;
	}
	kci_ctx = ctxp;
	kci_alloc = malocfp;
	kci_realloc = ralocfp;
	kci_free = mfreefp;
	return KCI_SUCCESS;
}

/*初始化驱动程序环境*/
sword   KCIEnvInit(KCIEnv **envp, ub4 mode,
	size_t mem_size, void  **usrmempp)
{
	startNetEnv();
	KCIEnv *p_env = gNew(KCIEnv);
	p_env->htype = KCI_HTYPE_ENV;
	init_mutex(p_env->op_lock);
	InitList(p_env->hdl_list);
	init_bin_mem(&p_env->mem, _1M, true, NULL);
	if (mem_size && usrmempp)
		*usrmempp = mc_alloc0(&p_env->mem,(SizeT)mem_size);
	p_env->char_set = KCI_GBK;
	p_env->nchar_set = KCI_GBK;
	*envp = p_env;
	return KCI_SUCCESS;
}

/*创建驱动程序环境*/
sword   KCIEnvCreate(KCIEnv **envp, ub4 mode, void  *ctxp,
	void  *(*malocfp)(void  *ctxp, size_t size),
	void  *(*ralocfp)(void  *ctxp, void  *memptr, size_t newsize),
	void  (*mfreefp)(void  *ctxp, void  *memptr),
	size_t mem_size, void  **usrmempp)
{
	startNetEnv();
	kci_mode = mode;
	//若有一个指针非空,则要求所有指针非空
	if (ctxp || malocfp || ralocfp || mfreefp)
	{
		if (!ctxp || !malocfp || !ralocfp || !mfreefp)
			return KCI_ERROR;
	}
	kci_ctx = ctxp;
	kci_alloc = malocfp;
	kci_realloc = ralocfp;
	kci_free = mfreefp;
	KCIEnv *p_env = gNew(KCIEnv);
	p_env->htype = KCI_HTYPE_ENV;
	init_mutex(p_env->op_lock);
	InitList(p_env->hdl_list);
	init_bin_mem(&p_env->mem, _1M, true, NULL);
	if (mem_size && usrmempp)
		*usrmempp = mc_alloc0(&p_env->mem, (SizeT)mem_size);
	p_env->char_set = KCI_GBK;
	p_env->nchar_set = KCI_GBK;
	*envp = p_env;
	return KCI_SUCCESS;
}

sword   KCIEnvNlsCreate(KCIEnv **envp, ub4 mode, void  *ctxp,
	void  *(*malocfp)(void  *ctxp, size_t size),
	void  *(*ralocfp)(void  *ctxp, void  *memptr, size_t newsize),
	void  (*mfreefp)(void  *ctxp, void  *memptr),
	size_t mem_size, void  **usrmempp,
	ub2 charset, ub2 ncharset)
{
	startNetEnv();
	kci_mode = mode;
	//若有一个指针非空,则要求所有指针非空
	if (ctxp || malocfp || ralocfp || mfreefp)
	{
		if (!ctxp || !malocfp || !ralocfp || !mfreefp)
			return KCI_ERROR;
	}
	kci_ctx = ctxp;
	kci_alloc = malocfp;
	kci_realloc = ralocfp;
	kci_free = mfreefp;
	KCIEnv *p_env = gNew(KCIEnv);
	p_env->htype = KCI_HTYPE_ENV;
	init_mutex(p_env->op_lock);
	InitList(p_env->hdl_list);
	init_bin_mem(&p_env->mem, _1M, true, NULL);
	if (mem_size && usrmempp)
		*usrmempp = mc_alloc0(&p_env->mem, (SizeT)mem_size);
	p_env->char_set = charset;
	p_env->nchar_set = ncharset;
	*envp = p_env;
	return KCI_SUCCESS;
}

sword   KCIFEnvCreate(KCIEnv **envp, ub4 mode, void  *ctxp,
	void  *(*malocfp)(void  *ctxp, size_t size),
	void  *(*ralocfp)(void  *ctxp, void  *memptr, size_t newsize),
	void(*mfreefp)(void  *ctxp, void  *memptr),
	size_t mem_size, void  **usrmempp, void  *fupg)
{
	startNetEnv();
	kci_mode = mode;
	//若有一个指针非空,则要求所有指针非空
	if (ctxp || malocfp || ralocfp || mfreefp)
	{
		if (!ctxp || !malocfp || !ralocfp || !mfreefp)
			return KCI_ERROR;
	}
	kci_ctx = ctxp;
	kci_alloc = malocfp;
	kci_realloc = ralocfp;
	kci_free = mfreefp;
	KCIEnv *p_env = gNew(KCIEnv);
	p_env->htype = KCI_HTYPE_ENV;
	init_mutex(p_env->op_lock);
	InitList(p_env->hdl_list);
	init_bin_mem(&p_env->mem, _1M, true, NULL);
	if (mem_size && usrmempp)
		*usrmempp = mc_alloc0(&p_env->mem, (SizeT)mem_size);
	p_env->char_set = KCI_GBK;
	p_env->nchar_set = KCI_GBK;
	*envp = p_env;
	return KCI_SUCCESS;
}

/*结束驱动环境*/
sword   KCITerminate(ub4 mode)
{
	kci_mode = 0;
	kci_ctx = NULL;
	kci_alloc = NULL;
	kci_realloc = NULL;
	kci_free = NULL;
	return KCI_SUCCESS;
}

/*分配句柄*/
sword   KCIHandleAlloc(const void  *parenth, 
	void  **hndlpp, const ub4 type,
	const size_t xtramem_sz, 
	void  **usrmempp)
{
	KCIHdl *p_hdl = NULL;
	//step1:得到KCIEnv指针
	KCIEnv *p_env = (KCIEnv*)parenth;
	if (p_env->htype != KCI_HTYPE_ENV)
	{
		if (p_env->htype != KCI_HTYPE_SVCCTX)
			return KCI_ERROR;
		p_env = ((KCISvcCtx*)parenth)->p_env;
	}
	//step2:分配句柄
	switch (type)
	{
	case KCI_HTYPE_SVCCTX:
		p_hdl = mcNew(&(p_env->mem), KCISvcCtx);
		memset(p_hdl, 0, sizeof(KCISvcCtx));
		break;
	case KCI_HTYPE_ERROR:
		p_hdl = mcNew(&(p_env->mem), KCIError);		
		memset(p_hdl, 0, sizeof(KCIError));
		break;
	case KCI_HTYPE_SERVER:
		p_hdl = mcNew(&(p_env->mem), KCIServer);
		memset(p_hdl, 0, sizeof(KCIServer));
		break;
	case KCI_HTYPE_SESSION:
		p_hdl = mcNew(&(p_env->mem), KCISession);
		memset(p_hdl, 0, sizeof(KCISession));
		((KCISession *)p_hdl)->isBegin =false;
		break;
	case KCI_HTYPE_STMT:
		p_hdl = mcNew(&(p_env->mem), KCIStmt);
		memset(p_hdl, 0, sizeof(KCIStmt));
		init_link_mem(&(((KCIStmt*)p_hdl)->mem),0, &(p_env->mem));
		init_bin_mem(&(((KCIStmt*)p_hdl)->mem2),_64K,0, &(p_env->mem));
		break;
	case KCI_HTYPE_TRANS:
		p_hdl = mcNew(&(p_env->mem), KCITrans);		
		memset(p_hdl, 0, sizeof(KCITrans));
		((KCITrans *)p_hdl)->isautobigdata = 0;
		break;
	case KCI_HTYPE_SPOOL:
		p_hdl = mcNew(&(p_env->mem), KCISPool);	
		memset(p_hdl, 0, sizeof(KCISPool));
		break;		
	case KCI_HTYPE_CPOOL:
		p_hdl = mcNew(&(p_env->mem), KCICPool);
		memset(p_hdl, 0, sizeof(KCICPool));
		break;
	default:
		return KCI_ERROR;
	}
	p_hdl->p_env = p_env;
	p_hdl->htype = type;
	wait_mutex(p_env->op_lock);
	ListAdd(p_env->hdl_list, &(p_hdl->hdl_link));
	free_mutex(p_env->op_lock);	
	*hndlpp = (dvoid *)p_hdl;
	if (xtramem_sz && usrmempp)
		*usrmempp = mc_alloc0(&(p_env->mem), (SizeT)xtramem_sz);
	return KCI_SUCCESS;
}

/*释放句柄*/
sword   KCIHandleFree(void  *hndlp, const ub4 type)
{
    if (IsBadWritePtr(hndlp, sizeof(hndlp)))
		return KCI_SUCCESS;
    if (type == KCI_HTYPE_ENV)
	{
		KCIEnv *p_env = (KCIEnv *)hndlp;
		while (!LEmpty(p_env->hdl_list))
		{
			KCIHdl *p_hdl = RmvFirst(p_env->hdl_list,KCIHdl,hdl_link);
			KCIHandleFree(p_hdl, p_hdl->htype);
		}	
		reset_mem(&(p_env->mem));
		return KCI_SUCCESS;
	}
	KCIHdl *p_hdl = (KCIHdl *)hndlp;
	KCIEnv *p_env =  p_hdl->p_env;
	wait_mutex(p_env->op_lock);
	if (InList(&(p_hdl->hdl_link)))
		ListRmv(&(p_hdl->hdl_link));
	free_mutex(p_env->op_lock);

	switch (p_hdl->htype)
	{
	case KCI_HTYPE_SVCCTX:
		freeSvcCtx((KCISvcCtx*)p_hdl);
		break;
	case KCI_HTYPE_ERROR:
		freeError((KCIError*)p_hdl);
		break;
	case KCI_HTYPE_SERVER:
		freeServer((KCIServer*)p_hdl);
		break;
	case KCI_HTYPE_SESSION:
		freeSession((KCISession*)p_hdl);
		break;
	case KCI_HTYPE_STMT:
		freeStmt((KCIStmt*)p_hdl);
		break;
	case KCI_HTYPE_TRANS:
		freeTrans((KCITrans*)p_hdl);
		break;
	case KCI_HTYPE_SPOOL:
		freeSPool((KCISPool*)p_hdl);
		break;
	case KCI_HTYPE_CPOOL:
		freeCPool((KCICPool*)p_hdl);
		break;
	}
	p_hdl = NULL;
	return KCI_SUCCESS;
}

sword   KCIDescriptorAlloc(const void  *parenth, void  **descpp,
	const ub4 type, const size_t xtramem_sz,
	void  **usrmempp)
{
	KCIHdl *p_hdl = NULL;
	//step1:得到KCIEnv指针
	KCIEnv *p_env = (KCIEnv*)parenth;
	if (p_env->htype != KCI_HTYPE_ENV)
	{
		if (p_env->htype != KCI_HTYPE_SVCCTX)
			return KCI_ERROR;
		p_env = ((KCISvcCtx*)parenth)->p_env;
	}
	switch (type)
	{
	case KCI_DTYPE_LOB:
		p_hdl = mcNew(&(p_env->mem), KCILobLocator);
		memset((KCILobLocator *)p_hdl, 0, sizeof(KCILobLocator));		
		break;
	case KCI_DTYPE_TIMESTAMP:
	case KCI_DTYPE_TIMESTAMP_LTZ:
	case KCI_DTYPE_TIMESTAMP_TZ:   ///Timestamp with timezone
		p_hdl = mcNew(&(p_env->mem), KCIDateTime);
		memset((KCIDateTime *)p_hdl, 0, sizeof(KCIDateTime));
		break;
	case KCI_DTYPE_INTERVAL_DS:   ///Interval day second
		p_hdl = mcNew(&(p_env->mem), KCIInterval);		
		memset((KCIInterval *)p_hdl, 0, sizeof(KCIInterval));
		break;		
	default:
		p_hdl = NULL;
		break;
	}
	if (!p_hdl)
		return KCI_ERROR;
	p_hdl->p_env = p_env;
	wait_mutex(p_env->op_lock);
	ListAdd(p_env->hdl_list, &(p_hdl->hdl_link));
	free_mutex(p_env->op_lock);
	*descpp = (void*)p_hdl;
	if (xtramem_sz && usrmempp)
		*usrmempp = mc_alloc0(&(p_env->mem), (SizeT)xtramem_sz);

	return KCI_SUCCESS;

};

sword   KCIArrayDescriptorAlloc(const void  *parenth, void  **descpp,
	const ub4 type, ub4 array_size,
	const size_t xtramem_sz, void  **usrmempp)
{
	KCIHdl *p_hdl = NULL;
	//step1:得到KCIEnv指针
	KCIEnv *p_env = (KCIEnv*)parenth;
	if (p_env->htype != KCI_HTYPE_ENV)
	{
		if (p_env->htype != KCI_HTYPE_SVCCTX)
			return KCI_ERROR;
		p_env = ((KCISvcCtx*)parenth)->p_env;
	}
	if ( array_size < 1 )
		return KCI_ERROR;

	if (type != KCI_DTYPE_LOB)
	{
		return KCI_ERROR;
	}
//	p_hdl = (KCIHdl *)mc_alloc0(&(p_env->mem), sizeof(KCILobLocator)*array_size);
//	p_hdl->p_env = p_env;
//	wait_mutex(p_env->op_lock);
//	ListAdd(p_env->hdl_list, &(p_hdl->link));
//	free_mutex(p_env->op_lock);
//	*descpp = (void*)p_hdl;
	if (xtramem_sz && usrmempp)
		*usrmempp = mc_alloc0(&(p_env->mem), (SizeT)xtramem_sz);
	return KCI_SUCCESS;
};

sword   KCIDescriptorFree(void  *descp, const ub4 type)
{
    if (IsBadWritePtr(descp, sizeof(descp)))
		return KCI_SUCCESS;	
    KCIHdl *p_hdl = (KCIHdl *)descp;
	KCIEnv *p_env = p_hdl->p_env;
	//{
	//	wait_mutex(p_env->op_lock);
	//	if (InList(&(p_hdl->link)))
	//		ListRmv(&(p_hdl->link));
	//	free_mutex(p_env->op_lock);
	//	mc_free(&(p_hdl->p_env->mem), p_hdl);
	//}
	
	return KCI_SUCCESS;
}


sword   KCIArrayDescriptorFree(void  **descp, const ub4 type)
{
	return KCI_SUCCESS;
};

sword KCIObjectNew(KCIEnv *env, KCIError *err, const KCISvcCtx *svc,
	KCITypeCode typecode, KCIType *tdo, void  *table,
	KCIDuration duration, boolean value,
	void  **instance)
{
	return KCI_SUCCESS;
};

sword KCIIterCreate(KCIEnv *env, KCIError *err, const KCIColl *coll,
	KCIIter **itr)
{
	return KCI_SUCCESS;
};

sword KCIIterNext(KCIEnv *env, KCIError *err, KCIIter *itr,
	void  **elem, void  **elemind, boolean *eoc)
{
	return KCI_SUCCESS;
};
oratext *KCIStringPtr(KCIEnv *env, const KCIString *vs)
{
	return NULL;
};

sword KCITypeByName(KCIEnv *env, KCIError *err, const KCISvcCtx *svc,
	const oratext *schema_name, ub4 s_length,
	const oratext *type_name, ub4 t_length,
	const oratext *version_name, ub4 v_length,
	KCIDuration pin_duration, KCITypeGetOpt get_option,
	KCIType **tdo)
{
	return KCI_SUCCESS;
};
