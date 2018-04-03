#include "kci_mem.h"

#define SAFE_FREE
#define TRACE_MALLOC

#define Pice2Mem(p) (void*)( ((char*)p)+sizeof(MemPice))
#define Mem2Pice(p) (MemPice*)( ((char*)p)-sizeof(MemPice))
static void *link_mem_alloc(LinkMem *p_mem,SizeT si)
{
	MemPice *p_pice = p_mem->parent? 
						(MemPice*)IAlloc(p_mem->parent,sizeof(MemPice)+si) 
						: (MemPice*)top_mem_alloc(sizeof(MemPice)+si);
	if(p_pice)
	{
		ListAdd(p_mem->stub,p_pice);
		return Pice2Mem(p_pice);
	}
	else
		return NULL;
}

static void *link_mem_realloc(LinkMem *p_mem,void *old_p,SizeT si)
{
	MemPice *p_pice;
	if(old_p)
	{
		p_pice= Mem2Pice(old_p);
		ListRmv(p_pice);
		p_pice = p_mem->parent? 
					(MemPice*)Realloc(p_mem->parent,p_pice,sizeof(MemPice)+si) 
					: (MemPice*)top_mem_realloc(p_pice,sizeof(MemPice)+si);
		if(p_pice)
		{
			ListAdd(p_mem->stub,p_pice);
			return Pice2Mem(p_pice);
		}
		else
			return NULL;
	}
	else
		return link_mem_alloc(p_mem,si);
}

static void link_mem_free(LinkMem *p_mem,void *p)
{
	if(p)
	{
		MemPice *p_pice = Mem2Pice(p);
		ListRmv(p_pice);
		if(p_mem->parent)
		{
			IFree(p_mem->parent,p_pice);
		}
		else
			 top_mem_free(p_pice);
	}
}

static void *link_mem_alloc_l(LinkMem *p_mem,SizeT si)
{
	void *p;
	wait_mutex(p_mem->op_lock);
	p = link_mem_alloc(p_mem,si);
	free_mutex(p_mem->op_lock);
	return p;
}

static void *link_mem_realloc_l(LinkMem *p_mem,void *old_p,SizeT si)
{
	void *p;
	wait_mutex(p_mem->op_lock);
	p = link_mem_realloc(p_mem,old_p,si);
	free_mutex(p_mem->op_lock);
	return p;
}

static void link_mem_free_l(LinkMem *p_mem,void *p)
{
	if(p)
	{
		wait_mutex(p_mem->op_lock);
		link_mem_free(p_mem,p);
		free_mutex(p_mem->op_lock);
	}
}

static void reset_link_mem(LinkMem *p_mem)
{
	MemPice *p_pice ; 
	while((p_pice=p_mem->stub.next) != &(p_mem->stub))
	{
		ListRmv(p_pice);
		if(p_mem->parent)
			IFree(p_mem->parent,p_pice);
		else
			top_mem_free(p_pice);
	}
}

static void clear_link_mem(LinkMem *p_mem)
{
	if(p_mem->__alloc == (__ALloc)link_mem_alloc_l)
		close_mutex(p_mem->op_lock);
	reset_link_mem(p_mem);
}

void init_link_mem(LinkMem *p_mem,bool b_lock,MemCtx *parent)
{
	MemSet(p_mem,0x0,sizeof(LinkMem));
	p_mem->stub.next = p_mem->stub.prev = &(p_mem->stub);
	p_mem->parent = parent;
	if(b_lock)
	{
		init_mutex(p_mem->op_lock);
		p_mem->__alloc = (__ALloc)link_mem_alloc_l;
		p_mem->__realloc = (__REalloc)link_mem_realloc_l;
		p_mem->__free = (__FRee)link_mem_free_l;
	}
	else
	{
		p_mem->__alloc = (__ALloc)link_mem_alloc;
		p_mem->__realloc = (__REalloc)link_mem_realloc;
		p_mem->__free = (__FRee)link_mem_free;
	}
	p_mem->__alloc_nl = (__ALloc)link_mem_alloc;
	p_mem->__reset = (__REset)reset_link_mem;
	p_mem->__clear = (__CLear)clear_link_mem;
}

MemCtx *create_link_mem(MemCtx *parent,bool b_lock)
{
	LinkMem *p_mem;

	if(parent)
		p_mem = mcNew(parent,LinkMem);
	else
		p_mem = gNew(LinkMem);
	if(p_mem)
		init_link_mem(p_mem,b_lock,parent);
	return (MemCtx*)p_mem;
}
