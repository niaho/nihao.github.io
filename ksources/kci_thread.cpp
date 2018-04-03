#include <malloc.h>
#include <memory.h>
#include <string.h>
#if defined(LINUX)
#include <pthread.h>
#endif
#include "kci_defs.h"

void KCIThreadProcessInit()
{
	return;
};

sword KCIThreadInit(void  *hndl, KCIError *err)
{
	KCIEnv *p_env = (KCIEnv *)hndl;
	if (!p_env)
		return KCI_ERROR;
	return KCI_SUCCESS;
};

sword KCIThreadIdInit(void  *hndl, KCIError *err, KCIThreadId **tid)
{
	KCIHdl *p_hdl = NULL;
	KCIEnv *p_env = (KCIEnv*)hndl;
	if (!p_env)
		return KCI_ERROR;
	p_hdl = mcNew(&(p_env->mem), KCIThreadId);
	*tid = (KCIThreadId *)p_hdl;
	return KCI_SUCCESS;
};

sword KCIThreadIdDestroy(void  *hndl, KCIError *err, KCIThreadId **tid)
{
	KCIHdl *p_hdl = (KCIHdl *)tid;
	KCIEnv *p_env = (KCIEnv *)hndl;
	if (!p_env)
		return KCI_ERROR;
	if (!p_hdl)
		return KCI_ERROR;
	
	mfree(*tid);
	*tid = (KCIThreadId *)0;
	return KCI_SUCCESS;
};

sword KCIThreadHndInit(void  *hndl, KCIError *err, KCIThreadHandle **thnd)
{
	KCIHdl *p_hdl = NULL;
	KCIEnv *p_env = (KCIEnv*)hndl;
	if (!p_env)
		return KCI_ERROR;
	p_hdl = mcNew(&(p_env->mem), KCIThreadHandle);
	*thnd = (KCIThreadHandle *)p_hdl;
	return KCI_SUCCESS;
};

sword KCIThreadHndDestroy(void  *hndl, KCIError *err, KCIThreadHandle **thnd)
{
	KCIHdl *p_hdl = (KCIHdl *)thnd;
	KCIEnv *p_env = (KCIEnv *)hndl;
	if (!p_env)
		return KCI_ERROR;
	if (!p_hdl)
		return KCI_ERROR;
	mfree(*thnd);
	*thnd = (KCIThreadHandle *)0;
	return KCI_SUCCESS;
};

sword KCIThreadCreate(void  *hndl, KCIError *err,
	void(*start)(void  *), void  *arg,
	KCIThreadId *tid, KCIThreadHandle *tHnd)
{
	int lpthreadhandle = 0;
	DWORD lpThreadId = 0;	
        #if defined(LINUX)
	lpthreadhandle = pthread_create(&lpThreadId, NULL, start, (void *)arg);	
        #else 
	lpthreadhandle =(int) CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start, (void *)arg, 0, &lpThreadId);
        #endif
	if (tid)
		tid->threadId = lpThreadId;
	if (tHnd)
		tHnd->threadHdl = lpthreadhandle;
	return KCI_SUCCESS;
};

sword KCIThreadJoin(void  *hndl, KCIError *err, KCIThreadHandle *tHnd)
{
	
	return KCI_SUCCESS;
};

sword KCIThreadClose(void  *hndl, KCIError *err, KCIThreadHandle *tHnd)
{
	if (tHnd)
	{
                ////linux 
		if (tHnd->threadHdl>0)
#if defined(LINUX)
			pthread_join(tHnd->threadHdl, NULL);    //等待线程执行完毕
			free(tHnd->threadHdl);
#else
		   WaitForSingleObject((HANDLE)tHnd->threadHdl, INFINITE); //等待一个进程结束
#endif
		tHnd->threadHdl = 0;
	}
	return KCI_SUCCESS;
};



sword KCIThreadTerm(void  *hndl, KCIError *err)
{
	KCIEnv *p_env = (KCIEnv *)hndl;
	if (!p_env)
		return KCI_ERROR;
	return KCI_SUCCESS;
};

