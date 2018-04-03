#include "kci_mem.h"

static void *blk_mem_alloc(BlkMem *p_mem, SizeT si)
{
	si = (si + 8) & 0xfffffff8UL;
	//当前分配区已不能满足分配?
	if (p_mem->off + si > BLK_SIZE-8)
	{
		//若所需内存大于单块内存尺度?(单独分配)
		if (si >= BLK_SIZE - 8)
		{
			MemBlk *p_blk = p_mem->parent ?
				(MemBlk*)IAlloc(p_mem->parent, si+8)
				: (MemBlk*)top_mem_alloc(si+8);
			if (p_blk)
			{
				p_blk->next = p_mem->mem_blks;
				p_mem->mem_blks = p_blk;
				return p_blk->mem;
			}
			else
				return NULL;
		}
		//否则,从上层内存分配器分配一块内存
		else
		{
			MemBlk *p_blk = p_mem->parent ?
				(MemBlk*)IAlloc(p_mem->parent, BLK_SIZE)
				: (MemBlk*)top_mem_alloc(BLK_SIZE);
			if (p_blk)
			{
				p_blk->next = p_mem->mem_blks;
				p_mem->mem_blks = p_blk;
				p_mem->buff = p_blk->mem;
				p_mem->off = 0;
			}
			else
				return NULL;
		}
	}
	//从当前内存分配区分配si大小的内内存
	char *ret_ptr = p_mem->buff + p_mem->off;
	p_mem->off += si;
	return ret_ptr;
}

static void *blk_mem_realloc(BlkMem *p_mem, void *old_p, SizeT si)
{
	return NULL;
}

static void blk_mem_free(BlkMem *p_mem, void *p)
{
	
}

static void *blk_mem_alloc_l(BlkMem *p_mem, SizeT si)
{
	void *p;
	wait_mutex(p_mem->op_lock);
	p = blk_mem_alloc(p_mem, si);
	free_mutex(p_mem->op_lock);
	return p;
}

static void *blk_mem_realloc_l(BlkMem *p_mem, void *old_p, SizeT si)
{
	return NULL;
}

static void blk_mem_free_l(BlkMem *p_mem, void *p)
{
}

static void reset_blk_mem(BlkMem *p_mem)
{
	p_mem->off = 0;
	p_mem->buff = p_mem->mem0;
	while (p_mem->mem_blks)
	{
		MemBlk *p_blk = p_mem->mem_blks;
		p_mem->mem_blks = p_blk->next;
		if (p_mem->parent)
			IFree(p_mem->parent, p_blk);
		else
			top_mem_free(p_blk);
	}
}

static void clear_blk_mem(BlkMem *p_mem)
{
	if (p_mem->__alloc == (__ALloc)blk_mem_alloc_l)
		close_mutex(p_mem->op_lock);
	reset_blk_mem(p_mem);
}

void init_blk_mem(BlkMem *p_mem, bool b_lock, MemCtx *parent)
{
	MemSet(p_mem, 0x0, sizeof(BlkMem));
	p_mem->parent = parent;
	p_mem->buff = p_mem->mem0;
	if (b_lock)
	{
		init_mutex(p_mem->op_lock);
		p_mem->__alloc = (__ALloc)blk_mem_alloc_l;
		p_mem->__realloc = (__REalloc)blk_mem_realloc_l;
		p_mem->__free = (__FRee)blk_mem_free_l;
	}
	else
	{
		p_mem->__alloc = (__ALloc)blk_mem_alloc;
		p_mem->__realloc = (__REalloc)blk_mem_realloc;
		p_mem->__free = (__FRee)blk_mem_free;
	}
	p_mem->__alloc_nl = (__ALloc)blk_mem_alloc;
	p_mem->__reset = (__REset)reset_blk_mem;
	p_mem->__clear = (__CLear)clear_blk_mem;
}

MemCtx *create_blk_mem(MemCtx *parent, bool b_lock)
{
	BlkMem *p_mem;
	if (parent)
		p_mem = mcNew(parent, BlkMem);
	else
		p_mem = gNew(BlkMem);
	if (p_mem)
		init_blk_mem(p_mem, b_lock, parent);
	return (MemCtx*)p_mem;
}
