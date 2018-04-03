#ifndef __kci_types_h__
#define __kci_types_h__
#define NULL 0
#define _1K  1024
#define _2K  2048
#define _4K  4096
#define _8K  8192
#define _16K (_1K*16)
#define _32K (_1K*32)
#define _64K (_1K*64)
#define _1M  (_1K*_1K)
#define _2M  (2*_1M)
#define _4M  (4*_1M)
#define _8M  (8*_1M)

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
#include <sys/locking.h>
#include <winsock2.h>
#include <Mswsock.h>
#include <windows.h>
#include <math.h>				
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <time.h>
#include <direct.h>

typedef unsigned int        SizeT;
typedef unsigned char       byte;
typedef signed	char		int8;		/* Signed integer >= 8	bits */
typedef signed	short		int16;		/* Signed integer >= 16 bits */
typedef signed	int			int32;
typedef volatile char		vint8;
typedef volatile short		vint16;
typedef volatile int        vint32;
typedef volatile int        vint;
typedef volatile bool		vbool;
typedef unsigned char	    uchar;		/* Short for unsigned char */
typedef unsigned char	    uint8;		/* Short for unsigned integer >= 8  bits */
typedef unsigned short		ushort;
typedef unsigned short	    uint16;		/* Short for unsigned integer >= 16 bits */
typedef unsigned int		uint;
typedef unsigned int		uint32;		/* Short for unsigned integer >= 32 bits */
typedef unsigned long	    ulong;		/* Short for unsigned long */
typedef const char *        cstr;
#define __WIN__

typedef __int64				longlong;
typedef __int64				int64;
typedef unsigned __int64    ulonglong;
typedef unsigned __int64	uint64;

typedef		void *HANDLE;
#define		INFINITE            0xFFFFFFFF
#define     atom_inc(i)			InterlockedIncrement(&(i))
#define     atom_dec(i)		    InterlockedDecrement(&(i))

typedef     SRWLOCK             rwlock;
#define     init_rwlock(h)      InitializeSRWLock(&(h))
#define     wait_rlock(h)       AcquireSRWLockShared(&(h))
#define     free_rlock(h)       ReleaseSRWLockShared(&(h))
#define     wait_wlock(h)       AcquireSRWLockExclusive(&(h))
#define     free_wlock(h)       ReleaseSRWLockExclusive(&(h))

typedef		CRITICAL_SECTION	mutex;
#define		init_mutex(h)		InitializeCriticalSection(&(h))
#define		close_mutex(h)		DeleteCriticalSection(&(h))
#define     wait_mutex(h)       EnterCriticalSection(&(h))
#define     free_mutex(h)       LeaveCriticalSection(&(h))

#define		sema				HANDLE
#define		init_sema(h)		h=CreateSemaphore(NULL,0,2000000000,NULL)
#define   	init_sema_n(h,n)	h=CreateSemaphore(NULL,n,2000000000,NULL)
#define		dec_sema(h)			WaitForSingleObject(h,INFINITE)
#define		inc_sema(h)			ReleaseSemaphore(h,1,NULL)
#define		wait_sema(h)		WaitForSingleObject(h,INFINITE)
#define		wait_sema_t(h,t)	WaitForSingleObjectEx(h,(DWORD)(t),true)
#define		clr_sema(h)			while (WaitForSingleObject((h),0)!=WAIT_TIMEOUT)
#define		close_sema(h)		CloseHandle(h)

#define		event				HANDLE
#define		init_event(h)		h=CreateEvent(NULL,false,false,NULL)
#define		wait_event(h)		WaitForSingleObject(h,INFINITE)
#define		wait_event_t(h,t)	WaitForSingleObject(h,(DWORD)(t))
#define		set_event(h)		SetEvent(h)
#define		reset_event(h)		ResetEvent(h)
#define		close_event(h)		CloseHandle(h)

#define    __sleep(t)          Sleep(t)
#define    strdup(s)		   _strdup(s)
#else

typedef		void *HANDLE;
#define		INFINITE            0xFFFFFFFF

typedef int SOCKET;
typedef unsigned int        SizeT;
typedef unsigned char       byte;
typedef signed	char		int8;		/* Signed integer >= 8	bits */
typedef signed	short		int16;		/* Signed integer >= 16 bits */
typedef signed	int			int32;
typedef volatile char		vint8;
typedef volatile short		vint16;
typedef volatile int        vint32;
typedef volatile int        vint;
typedef volatile bool		vbool;
typedef unsigned char	    uchar;		/* Short for unsigned char */
typedef unsigned char	    uint8;		/* Short for unsigned integer >= 8  bits */
typedef unsigned short		ushort;
typedef unsigned short	    uint16;		/* Short for unsigned integer >= 16 bits */
typedef unsigned int		uint;
typedef unsigned int		uint32;		/* Short for unsigned integer >= 32 bits */
typedef unsigned long	    ulong;		/* Short for unsigned long */
typedef const char *        cstr;
typedef signed	long long	longlong;
typedef signed	long long	int64;
typedef unsigned long long  ulonglong;	/* ulong or unsigned long long */
typedef unsigned long long	uint64;
typedef unsigned long DWORD;

#include <unistd.h> 
#include <linux/unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/fcntl.h>
#include <fcntl.h> 
#include <string.h> 
#include <sys/mman.h> 
#include <sys/file.h> 
#include <syscall.h> 
#include <errno.h> 
#include <pthread.h>		/* AIX must have this included first */
#include <aio.h>
#include <libio.h>
#include <stdio.h> 
#include <stdlib.h>
#include <dirent.h>
#include <asm/ioctls.h>
#include <signal.h>	
#include <sys/ioctl.h> 
#include <ctype.h>      
#include <math.h>       
#include <sys/time.h>   

typedef     pthread_rwlock_t    rwlock;
#define     init_rwlock(h)		pthread_rwlock_init(&(h),NULL)
#define     wait_rlock(h)       pthread_rwlock_rdlock(&(h))
#define     free_rlock(h)       pthread_rwlock_unlock(&(h))
#define     wait_wlock(h)       pthread_rwlock_wrlock(&(h))
#define     free_wlock(h)       pthread_rwlock_unlock(&(h))

typedef		pthread_mutex_t 	mutex;
#define		init_mutex(h)		pthread_mutex_init(&(h),NULL)
#define		close_mutex(h)		pthread_mutex_destroy(&(h))	
#define		wait_mutex(h)		pthread_mutex_lock(&(h))	
#define		free_mutex(h)	    pthread_mutex_unlock(&(h))

typedef struct event{
	pthread_mutex_t   _mutex;
	pthread_cond_t    _cond;
	int				  _val;
}event;

#define init_event(h)  __init_event(&(h))
#define wait_event(h)  __wait_event(&(h))
#define set_event(h)   __set_event(&(h))
#define reset_event(h) __reset_event(&(h))
#define close_event(h) __close_event(&(h))

inline static void __init_event(event *p_event)
{
	pthread_mutex_init(&(p_event->_mutex),NULL);
	pthread_cond_init(&(p_event->_cond),NULL);
	p_event->_val = 0;
}

inline static void __wait_event(event *p_event)
{
	pthread_mutex_lock(&(p_event->_mutex));
	while (!p_event->_val)
		pthread_cond_wait(&(p_event->_cond), &(p_event->_mutex));
	pthread_mutex_unlock(&(p_event->_mutex));
}

inline static int __wait_event_t(event *p_event, int ms)
{
	struct  timespec  ts;
	int     ret_val = 0;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += ms / 1000;
	ts.tv_nsec += (ms % 1000) * 1000000;
	if (ts.tv_nsec>1000000000)
	{
		ts.tv_sec += ts.tv_nsec / 1000000000;
		ts.tv_nsec %= 1000000000;
	}
	pthread_mutex_lock(&(p_event->_mutex));
	while (!p_event->_val)
	{
		if (pthread_cond_timedwait(&(p_event->_cond),
			&(p_event->_mutex), &ts) < 0)
		{
			ret_val = WAIT_TIMEOUT;
			break;
		}
	}
	pthread_mutex_unlock(&(p_event->_mutex));
	return ret_val;
}

inline static void __set_event(event *p_event)
{
	pthread_mutex_lock(&(p_event->_mutex));
	p_event->_val = 1;
	pthread_cond_broadcast(&(p_event->_cond));
	pthread_mutex_unlock(&(p_event->_mutex));
}

inline static void __reset_event(event *p_event)
{
	pthread_mutex_lock(&(p_event->_mutex));
	p_event->_val = 0;
	pthread_mutex_unlock(&(p_event->_mutex));
}

inline static void __close_event(event *p_event)
{
	pthread_mutex_destroy(&(p_event->_mutex));
	pthread_cond_destroy(&(p_event->_cond));
}

#endif

#define set_if_bigger(a,b)  { if ((a) < (b)) (a)=(b); }
#define set_if_smaller(a,b) { if ((a) > (b)) (a)=(b); }
#define test_all_bits(a,b) (((a) & (b)) == (b))
#define array_elements(A) ((uint) (sizeof(A)/sizeof(A[0])))

#define PARAM_IN    1     //输入型参数
#define PARAM_OUT   2     //输出型参数
#define PARAM_INOUT 3     //输入输出型
#define PARAM_NVL   4     //参数为空值
#define PARAM_IS    8     //参数为流式
#define PARAM_STR   16    //参数以串的形式给值

/*------------------字符集定义----------------------------*/
#define KCI_GBK      110
#define KCI_GB18030  120
#define KCI_UTF8     130 


extern uint32 mode;
extern void *kci_ctx;
extern void *(*kci_alloc)(void *ctxp, size_t siz);
extern void *(*kci_realloc)(void *ctxp, void *ptr, size_t siz);
extern void(*kci_free)(void *ctxp, void *ptr);

#define astrlen(S) ((SizeT)__strlen((const char*)(S)))
#define kalloc(siz) kci_alloc(kci_ctx, siz)
#define krealloc(ptr,siz) kci_realloc(kci_ctx,p,siz)
#define kfree(ptr)  kci_free(kci_ctx,ptr)
#define New(T)  (T*)(kalloc0(sizeof(T)))
#define MemSet(p,c,s) memset(p,c,s)

inline static void *kalloc0(size_t siz)
{
	void *ptr = kci_alloc(kci_ctx, siz);
	memset(ptr, 0x0, siz);
	return ptr;
}

inline static int kstrcmp(const void *s1, int l1, const void *s2, int l2)
{
	int  ret = strncmp((char*)s1, (char*)s2, min(l1, l2));
	if (ret != 0) return ret;
	return l1==l2? 0: (l1 > l2 ? 1 : -1);
}

typedef struct XList{
	struct XList *volatile prev;
	struct XList *volatile next;
}XList;

#define InitList(_l_) (_l_).prev=(_l_).next=&(_l_)
#define ZeroList(_l_) (_l_).prev=(_l_).next=NULL 
/*根据结构或类的成员地址计算结构或类的入口地址*/
#define EntryPtr(ptr,cla,nam) ((cla*)(((char*)ptr)-(size_t)(&(((cla*)0)->nam)))) 

/*将XList元素加入到链尾*/
#define ListAdd(_h_,_l_) do{\
	(_l_)->next = &(_h_);\
	(_l_)->prev=(_h_).prev;\
	(_h_).prev->next=(_l_);\
	(_h_).prev = (_l_);}while(0)

/*将XList元素插入到指定元素前面*/
#define ListInsB(_l0_,_l_)   do{\
	(_l_)->prev = (_l0_)->prev;\
	(_l_)->next = (_l0_);\
	(_l0_)->prev->next = (_l_);\
	(_l0_)->prev = (_l_);}while(0)

/*将XList元素插入到指定元素后面*/
#define ListInsA(_l0_,_l_)   do{\
	(_l_)->next = (_l0_)->next;\
	(_l_)->prev = (_l0_);\
	(_l0_)->next->prev=(_l_);\
	(_l0_)->next = (_l_);}while(0)

/*将XList元素从链表中移除*/
#define ListRmv(_l_) \
	do{ if((_l_)->next){\
	(_l_)->prev->next = (_l_)->next;\
	(_l_)->next->prev = (_l_)->prev;\
	(_l_)->prev=(_l_)->next = NULL;}}while(0)

/*将XList元素从源链转移至目标链*/
#define ListMov(_d_,_s_) do{\
	if(!LEmpty(_s_)){\
	(_d_).next = (_s_).next;\
	(_d_).prev = (_s_).prev;\
	(_s_).next->prev = &(_d_);\
	(_s_).prev->next = &(_d_);\
	(_s_).prev = (_s_).next = &(_s_);\
			}else{InitList(_d_);}}while(0)

/*XList链表遍历头部*/
#define Foreach(_l_,_h_)	\
	for(XList *_l_=(_h_.next); _l_!=&(_h_); _l_=_l_->next)

#define LEmpty(_h_) ((_h_).next==&(_h_)&&(_h_).next!=NULL) 
#define InList(_l_) ((_l_)->next!=NULL)
#define LFirst(_h_) ((_h_).next)
#define LLast(_h_)  ((_h_).prev)

#if defined(__cplusplus)
inline static XList *RmvFirstL(XList &l)
{
	XList *l1 = l.next;
	ListRmv(l1);
	return l1;
}
inline static XList *RmvLastL(XList &l)
{
	XList *l1 = l.prev;
	ListRmv(l1);
	return l1;
}
inline static int LLength(XList &xlist)
{
	int num = 0;
	Foreach(l, xlist)
		num++;
	return num;
}
#endif

#define RmvFirst(l,t,m) EntryPtr(RmvFirstL(l),t,m)
#define GetFirst(l,t,m) EntryPtr((l).next,t,m)
#define RmvLast(l,t,m)  EntryPtr(RmvLastL(l),t,m)
#define GetLast(l,t,m)  EntryPtr((l).prev,t,m)

#define lengthof(array) (sizeof (array) / sizeof ((array)[0]))

#define clock_diff(t1,t2)  (t1>=t2?\
	(t1-t2)<_1G? (int)(t1-t2)\
	: -(int)(0xffffffffU-t1+t2+1) \
	:(t2-t1)<_1G? -(int)(t2-t1)\
	:(int)(0xffffffffU-t2+t1+1))

#define clock_cmp(t1,t2) (t1>=t2?\
	(t1-t2<=_1G? (int)(t1-t2): -1)\
	 : ((t2-t1)<_1G? -1 : 1))

#define clock_sub(t1,t2)  (t1>=t2?\
	 (t1-t2):(0xffffffffU-t2+t1+1))

#endif
