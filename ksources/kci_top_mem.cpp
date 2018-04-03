#include "stdio.h"
#include "kci_mem.h"

#define SAFE_FREE
#define TRACE_MALLOC

void   *kci_ctx = NULL;
void   *(*kci_alloc)(void *ctxp, size_t siz) = NULL;
void   *(*kci_realloc)(void *ctxp, void *ptr, size_t siz) = NULL;
void   (*kci_free)(void *ctxp, void *ptr) = NULL;

#ifdef  TRACE_MALLOC
static  int  alloced_num = 0;
typedef struct GMem{
	int	 siz;
	char label[4];
	char user_area[8];
}GMem;

#define GMemHSiz (size_t)(((GMem*)0)->user_area)

inline static void CheckGMem(GMem *p_gmem)
{
	int siz = p_gmem->siz;
	if (memcmp(p_gmem->label, "MaZh", 4))
		printf("GMemErr1[%p] ", p_gmem);
	else if (memcmp(p_gmem->user_area + siz, "mAzH", 4))
		printf("GMemErr1[%p] ", p_gmem);
}

void  *xg_malloc(size_t siz)
{
	GMem *p_gmem = (GMem*)malloc(GMemHSiz + siz + 4);
	if (p_gmem)
	{
		alloced_num++;
		p_gmem->siz = (int)siz;
		memcpy(p_gmem->label, "MaZh", 4);
		memcpy(p_gmem->user_area + siz, "mAzH", 4);
		return p_gmem->user_area;
	}
	else
		return NULL;
}

void  *xg_realloc(void *p_old, size_t new_siz)
{
	GMem *p_old_gmem;
	GMem *p_new_gmem;

	p_old_gmem = (GMem*)(((char*)p_old) - GMemHSiz);
	CheckGMem(p_old_gmem);
	p_new_gmem = (GMem*)realloc(p_old_gmem, GMemHSiz + new_siz + 4);
	if (p_new_gmem)
	{
		p_new_gmem->siz = (int)new_siz;
		memcpy(p_new_gmem->label, "MaZh", 4);
		memcpy(p_new_gmem->user_area + new_siz, "mAzH", 4);
		return p_new_gmem->user_area;
	}
	else
		return NULL;
}

void xg_free(void *p)
{
	GMem *p_gmem = (GMem*)(((char*)p) - GMemHSiz);
	CheckGMem(p_gmem);
	alloced_num--;
	free(p_gmem);
}

#else
#define xg_malloc(__SIZ__) malloc(__SIZ__)
#define xg_realloc(__OLD_P__,__NEE_SIZ__) realloc(__OLD_P__,__NEE_SIZ__) 
#define xg_free(__P__)	free(__P__)
#endif

/*********顶层内存申请、释放等**********/
void *top_mem_alloc(SizeT size)
{
	 if (kci_alloc)
		return kci_alloc(kci_ctx, (size_t)size);
	 else
		 return xg_malloc(size);
}

void *top_mem_alloc0(SizeT size)
{
	void *ptr;
	if (kci_alloc)
		ptr = kci_alloc(kci_ctx, (size_t)size);
	else
		ptr = xg_malloc(size);
	if (ptr)
		MemSet(ptr, 0x0, size);
	return ptr;
}
void *top_mem_realloc(void *old_p, SizeT new_size)
{
	if (kci_realloc)
		return kci_realloc(kci_ctx, old_p, (size_t)new_size);
	else
		return xg_realloc(old_p, new_size);
}

void top_mem_free(void *p)
{
	if (p)
	{
		if (kci_free)
			kci_free(kci_ctx,p);
		else
			xg_free(p);
	}
}

void raise_mem_err(int err_no)
{
}
