#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__

#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include "kci_string.h"


//#define	DEBUG_MEM
#ifdef _DEBUG
#define	DEBUG_MEM
#endif



struct  MemCtx;
typedef void *(*__ALloc)(struct MemCtx *_this,SizeT si);
typedef	void *(*__REalloc)(struct MemCtx *_this,void *p_old,SizeT si);
typedef	void  (*__FRee)(struct MemCtx *_this,void *p);
typedef	void  (*__REset)(struct MemCtx *_this);
typedef	void  (*__CLear)(struct MemCtx *_this);
/*内存上下文的通用接口*/
typedef struct MemCtx
{
	void *(*__alloc)(struct MemCtx *_this,SizeT si);
	void *(*__realloc)(struct MemCtx *_this,void *p_old,SizeT si);
	void  (*__free)(struct MemCtx *_this,void *p);
	void  (*__reset)(struct MemCtx *_this);
	void  (*__clear)(struct MemCtx *_this);
	void *(*__alloc_nl)(struct MemCtx *_this,SizeT si);
	struct MemCtx *parent;
}MemCtx;

#define  IAlloc(m,s)    (m)->__alloc((m),s)
#define  LAlloc(m,s)    (m)->__alloc_nl((m),s)
#define  Realloc(m,o,s) (m)->__realloc((m),o,s) 
#define  IFree(m,p)	    (m)->__free((m),p)

void  *top_mem_alloc(SizeT size);
void  *top_mem_alloc0(SizeT size);
void  *top_mem_realloc(void *old_p, SizeT new_size);
void  top_mem_free(void *p);
void  raise_mem_err(int code);

#define  PtoM(p)	 (void*)(((char*)p)+sizeof(MemCtx*))
#define  MtoP(p)	 (void*)(((char*)p)-sizeof(MemCtx*))
#define  SetCtx(p,m) *((MemCtx**)p) = m
#ifdef	 DEBUG_MEM
#define  GetCtx(p)	 *(MemCtx**)(((char*)p)-sizeof(MemCtx*)-8)
#else
#define  GetCtx(p)	 *(MemCtx**)(((char*)p)-sizeof(MemCtx*))
#endif

#define SetMask(ptr,siz) do{ *(((char*)ptr)+4) = 'M';\
							*(((char*)ptr)+5) = 'a';\
							*(((char*)ptr)+6) = 'Z';\
							*(((char*)ptr)+7) = 'h';\
							*(((char*)ptr)+siz+8) = 'm';\
							*(((char*)ptr)+siz+9) = 'A';\
							*(((char*)ptr)+siz+10) = 'z';\
							*(((char*)ptr)+siz+11) = 'H';\
						}while(0)

#define ChkMask(ptr,siz)   (*(((char*)ptr)+4) == 'M' &&\
						    *(((char*)ptr)+5) == 'a' &&\
							*(((char*)ptr)+6) == 'Z' &&\
							*(((char*)ptr)+7) == 'h' &&\
							*(((char*)ptr)+siz+8) == 'm' &&\
							*(((char*)ptr)+siz+9) == 'A' &&\
							*(((char*)ptr)+siz+10) == 'z' &&\
							*(((char*)ptr)+siz+11) =='H' )



inline static void*
mc_alloc(MemCtx *p_mem,SizeT si)
{
#ifdef DEBUG_MEM
	char *p;
	p = (char*)IAlloc(p_mem,si+sizeof(MemCtx*)+12);
	if(p)
	{
		SetCtx(p,p_mem);
		p = (char*)PtoM(p);
		*((int32*)p)=si;
		SetMask(p,si);
		return (p+8);
	}
	else
	{
		raise_mem_err(64);
		return NULL;
	}
#else
	char *p;
	p = (char*)IAlloc(p_mem,si+sizeof(MemCtx*));
	if(p)
	{
		SetCtx(p,p_mem);
		p = (char*)PtoM(p);
		*((int32*)p)=0;
		return p;
	}
	else
	{
		raise_mem_err(64);
		return NULL;
	}
#endif
}

inline static void*
mc_alloc0(MemCtx *p_mem,SizeT si)
{
	void *p  = mc_alloc(p_mem,si);
	MemSet(p,0x0,si);
	return p;
}

inline static void*
mc_realloc(MemCtx *p_mem,void *old,SizeT si)
{
#ifdef DEBUG_MEM
	char	*p;
	int32	si0;
	if(si<4) si=4;
	if(old)
	{
		p = ((char*)old)-8;
		si0 = *(int32*)p;
		if(!ChkMask(p,si0))
			raise_mem_err(65);
		p = (char*)MtoP(p);
	}
	else
		p = NULL;
	
	p = (char*)Realloc(p_mem,p,si+sizeof(MemCtx*)+12);
	if(p)
	{
		SetCtx(p,p_mem);
		p = (char*)PtoM(p);
		*((int32*)p)=si;
		SetMask(p,si);;
		return (p+8);
	}
	else
	{
		raise_mem_err(64);
		return NULL;
	}
#else
	char *p;
	p = (char*)((old!=NULL) ? MtoP(old) :NULL);
	p = (char*)Realloc(p_mem,p,si+sizeof(MemCtx*));
	if(p)
	{
		SetCtx(p,p_mem);
		return PtoM(p);
	}
	else
	{
		raise_mem_err(64);
		return NULL;
	}
#endif
}

inline static void  
mc_free(MemCtx *p_mem,void *p)
{
#ifdef DEBUG_MEM
	if(p)
	{
		int32	si;
		p = ((char*)p)-8;
		si = *(int32*)p;
		if(!ChkMask(p,si))
			raise_mem_err(65);
		*(int64*)p = 0x0;
		IFree(p_mem,MtoP(p));	

	}
#else
	if(p){
		*(int32*)p = 0x0;
		IFree(p_mem,MtoP(p));
	}
#endif
}
inline  static char *mc_strdup(MemCtx *m, const char *s)
{
	return s ? strcpy((char*)mc_alloc((m), astrlen(s) + 2), (s)) : NULL;
}
inline  static char *mc_strndup(MemCtx *m, const char *s, int n)
{
	return s ? strncpy((char*)mc_alloc0((m), (n)+2), (s), (n)) : NULL;
}

#define mfree(p)  do{if(p) mc_free(GetCtx(p),(void*)(p));p=NULL;}while(0)

#ifdef DEBUG_MEM
#define CheckMem(p)    do{if(!p) break; char *_ptr = ((char*)(p))-8;\
	int32 si = *(int32*)_ptr;  if(!ChkMask(_ptr,si))\
    printf("memory error at %s(%d)\r\n",__FILE__, __LINE__);\
}while(0)
#else
#define CheckMem(p)
#endif

#define gNew(t)		    (t*)top_mem_alloc0(sizeof(t))
#define mcNew(m,t)		(t*)mc_alloc0((MemCtx*)(m),sizeof(t))
#define aNew(t)			(t*)alloca(sizeof(t))
#define mcNewN(m,t,n)	(t*)mc_alloc0((MemCtx*)(m),sizeof(t)*(n))
#define aNewN(t,n)		(t*)alloca(sizeof(t)*(n))

#define reset_mem(p_mem) if(p_mem) (p_mem)->__reset(p_mem)
#define clear_mem(p_mem) (p_mem)->__clear(p_mem)
#define free_mem(p_mem)  do{if(p_mem){ clear_mem(p_mem); mfree(p_mem);}}while(0)

#ifdef __cplusplus
typedef struct Chunk
{
	uint32	 prev_size;
	uint32	 size;
	struct	 Chunk *prev;
	struct	 Chunk *next;
}Chunk;

typedef struct MemHeap
{
	struct MemHeap	*prev;
	struct MemHeap	*next;
	int32  heap_size;
}MemHeap;

/*BIN内存分配器*/
typedef struct BinMem
: public MemCtx
{
	mutex	op_lock;
	SizeT	step;
	int		max_b_i;
	int64   heap_size;
	MemHeap	heap_stub;
	MemHeap large_stub;
	Chunk	bins[127];
}BinMem;

typedef struct MemBlk{
	union{
		struct  MemBlk	*next;
		int64   llptr;
	};
	char	mem[4];		
}MemBlk;

/*块式内存分配器*/
#define BLK_SIZE _4K
typedef struct BlkMem
	: public MemCtx
{
	mutex	op_lock;
	char    mem0[BLK_SIZE];
	MemBlk  *mem_blks;
	char    *buff;
	int     off;
}BlkMem;

typedef struct MemPice
{
	struct MemPice *volatile prev;
	struct MemPice *volatile next;
}MemPice;

/*链式内存分配器(将分配内存链接在一起,以便统一释放)*/
typedef struct LinkMem 
: public MemCtx
{
	mutex	op_lock;
	MemPice stub;
}LinkMem;

MemCtx *create_bin_mem(MemCtx *parant,SizeT,bool use_lock );
MemCtx *create_link_mem(MemCtx *parant, bool use_lock);
MemCtx *create_blk_mem(MemCtx *parant, bool use_lock);
void   init_bin_mem(BinMem *, SizeT, bool use_lock, MemCtx *parent);
void   init_link_mem(LinkMem *,bool use_lock,MemCtx *parent);
void   init_blk_mem(BlkMem *, bool use_lock, MemCtx *parent);

#endif	//__cplusplus 

#endif
