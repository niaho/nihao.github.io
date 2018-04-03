#include "kci_mem.h"

#define SAFE_FREE
#define TRACE_MALLOC

/****************BIN内存管理*********************/
#define B_USED	0x4
#define B_NEXT	0x2
#define B_PREV	0x1
/*chunk头部大小*/
#define ChunkHeadSize	((int)(&(((Chunk*)0)->prev)))
/*检查是否用malloc分配的块*/
#define  IsGMem(t)		((t->prev_size)&0x1)
/*设置块为用malloc方式分配的*/
#define  SetGMem(t)		((t->prev_size)= 0x1) 
/*检查是否为TOP块*/
#define  NextOk(t)		((t->size)&B_NEXT)
/*设置下块有效*/
#define	 SetNextOk(t)	(t->size)|=B_NEXT
/*检查上块是否有效*/
#define  PrevOk(t)		((t->size)&B_PREV)
/*设置上块有效*/
#define  SetPrevOk(t)  (t->size)|=B_PREV 
/*检查块是否自由*/
#define  IsUse(t)		((t->size)&B_USED)
/*设置自由*/
#define  SetUse(t)		(t->size)|= B_USED
#define  ClrUse(t)		(t->size)&= (~B_USED)
/*取块大小*/
#define	ChunkSize(t)	((t->size)&0xfffffff8)
/*取上块的指针*/
#define PrevChunk(t)	((Chunk*) (((char*)t) - (t->prev_size)))
/*取下块指针*/
#define NextChunk(t)	((Chunk*) (((char*)t) + ChunkSize(t)))
/*设置块大小*/
#define SetHead(t,s)	t->size = s
#define SetFoot(t,s)	if(NextOk(t)) (NextChunk(t))->prev_size=(s&0xfffffff8)

#define MAX_SMALLBIN         63
#define MAX_SMALLBIN_SIZE    512
#define SMALLBIN_WIDTH       8
#define IsSmall(si)			(si<MAX_SMALLBIN_SIZE-SMALLBIN_WIDTH)

/*由给定大小计算其所在的bin的下标号*/
#define small_bin_index(sz)	(((uint32)(sz))>>3)  
#define bin_index(sz)																\
(((((uint32)(sz)) >> 9) ==    0) ?       (((uint32)(sz)) >>  3):\
((((uint32)(sz)) >> 9) <=    4) ?  56 + (((uint32)(sz)) >>  6):\
((((uint32)(sz)) >> 9) <=   20) ?  91 + (((uint32)(sz)) >>  9):\
((((uint32)(sz)) >> 9) <=   84) ? 110 + (((uint32)(sz)) >> 12):\
((((uint32)(sz)) >> 9) <=  340) ? 119 + (((uint32)(sz)) >> 15):\
((((uint32)(sz)) >> 9) <= 1364) ? 124 + (((uint32)(sz)) >> 18):\
											126) 
/* sizes, alignments */
#define INTERNAL_SIZE_T		sizeof(SizeT)  
#define SIZE_SZ             INTERNAL_SIZE_T
#define MALLOC_ALIGNMENT    (SIZE_SZ + SIZE_SZ)
#define MINSIZE             (sizeof(struct Chunk))
/* conversion from malloc headers to user pointers, and back */

#define chunk2mem(p)   ((char*)((char*)(p) + 2*SIZE_SZ))
#define mem2chunk(mem) ((Chunk *)((char*)(mem) - 2*SIZE_SZ))

#define insert_into_bin0(p_mem,p_chunk) do{\
	p_chunk->next = &(p_mem->bins[0]);\
	p_chunk->prev = p_mem->bins[0].prev;\
	p_mem->bins[0].prev->next = p_chunk;\
	p_mem->bins[0].prev = p_chunk;\
}while(0)

#define insert_into_bin(p_mem,p_chunk) do{\
	int b_i = bin_index(ChunkSize(p_chunk));\
	p_chunk->next = &(p_mem->bins[b_i]);\
	p_chunk->prev = p_mem->bins[b_i].prev;\
	p_mem->bins[b_i].prev->next = p_chunk;\
	p_mem->bins[b_i].prev = p_chunk;\
}while(0)

#define remove_from_bin(p_chunk) do{\
	p_chunk->prev->next = p_chunk->next;\
	p_chunk->next->prev = p_chunk->prev;\
}while(0)

#define split_chunk(p_mem,p_chunk,siz) do{\
	Chunk	*p_new = (Chunk*)( ((char*)p_chunk) + siz);\
	SizeT	more = ChunkSize(p_chunk)-siz;\
	SetHead(p_new,NextOk(p_chunk)|B_PREV|more);\
	SetHead(p_chunk,PrevOk(p_chunk)|B_USED|B_NEXT|siz);\
	p_new->prev_size = siz;\
	SetFoot(p_new,more);\
	insert_into_bin(p_mem,p_new);\
}while(0)

/*将多余部分放入bin[0]*/
#define split_chunk0(p_mem,p_chunk,siz) do{\
	Chunk	*p_new = (Chunk*)( ((char*)p_chunk) + siz);\
	SizeT	more = ChunkSize(p_chunk)-siz;\
	SetHead(p_new,NextOk(p_chunk)|B_PREV|more);\
	SetHead(p_chunk,PrevOk(p_chunk)|B_USED|B_NEXT|siz);\
	p_new->prev_size = siz;\
	SetFoot(p_new,more);\
	insert_into_bin0(p_mem,p_new);\
}while(0)


#define merge_chunk(p_chunk,p_next) do{\
	SizeT new_siz = ChunkSize(p_chunk)+ChunkSize(p_next);\
	SetHead(p_chunk,(NextOk(p_next)|PrevOk(p_chunk))|new_siz);\
	SetFoot(p_chunk,new_siz);\
}while(0)

static 	void *bin_mem_alloc(BinMem *p_mem, SizeT si);
static 	void *bin_mem_realloc(BinMem *p_mem, void *p_old, SizeT si);
static 	void bin_mem_free(BinMem *p_mem, void *p);
static 	void *bin_mem_alloc_l(BinMem *p_mem, SizeT si);
static 	void *bin_mem_realloc_l(BinMem *p_mem, void *p_old, SizeT si);
static 	void bin_mem_free_l(BinMem *p_mem, void *p);
static	void reset_bin_mem_context(BinMem *p_mem);
static	void clr_bin_mem_context(BinMem *p_mem);

void init_bin_mem(BinMem *p_mem, SizeT step, bool use_lock, MemCtx *pp_c)
{
	MemSet(p_mem, 0x0, sizeof(MemCtx));
	p_mem->step = step;
	p_mem->max_b_i = bin_index(step);
	p_mem->parent = pp_c;
	for (int i = 0; i<127; i++)
		p_mem->bins[i].next = p_mem->bins[i].prev = &(p_mem->bins[i]);
	p_mem->heap_stub.next = p_mem->heap_stub.prev = &(p_mem->heap_stub);
	p_mem->large_stub.next = p_mem->large_stub.prev = &(p_mem->large_stub);
	if (use_lock)
	{
		init_mutex(p_mem->op_lock);
		p_mem->__alloc = (__ALloc)bin_mem_alloc_l;
		p_mem->__realloc = (__REalloc)bin_mem_realloc_l;
		p_mem->__free = (__FRee)bin_mem_free_l;
	}
	else
	{
		p_mem->__alloc = (__ALloc)bin_mem_alloc;
		p_mem->__realloc = (__REalloc)bin_mem_realloc;
		p_mem->__free = (__FRee)bin_mem_free;
	}
	p_mem->__alloc_nl = (__ALloc)bin_mem_alloc;
	p_mem->__reset = (__REset)reset_bin_mem_context;
	p_mem->__clear = (__CLear)clr_bin_mem_context;
}

MemCtx *create_bin_mem(MemCtx *parent, SizeT si, bool use_lock)
{
	BinMem *p_mem;

	if (parent)
		p_mem = mcNew(parent, BinMem);
	else
		p_mem = gNew(BinMem);
	if (p_mem)
		init_bin_mem(p_mem, si, use_lock, parent);
	return (MemCtx*)p_mem;
}

static void reset_bin_mem_context(BinMem *p_mem)
{
	while (p_mem->heap_stub.next != &(p_mem->heap_stub))
	{
		MemHeap *p_heap = p_mem->heap_stub.next;
		p_heap->prev->next = p_heap->next;
		p_heap->next->prev = p_heap->prev;
		if (p_mem->parent)
			IFree(p_mem->parent, p_heap);
		else
			top_mem_free(p_heap);
	}

	while (p_mem->large_stub.next != &(p_mem->large_stub))
	{
		MemHeap *p_heap = p_mem->large_stub.next;
		p_heap->prev->next = p_heap->next;
		p_heap->next->prev = p_heap->prev;
		if (p_mem->parent)
			IFree(p_mem->parent, p_heap);
		else
			top_mem_free(p_heap);
	}
	for (int i = 0; i<127; i++)
		p_mem->bins[i].next = p_mem->bins[i].prev = &(p_mem->bins[i]);
	p_mem->heap_stub.next = p_mem->heap_stub.prev = &(p_mem->heap_stub);
	p_mem->large_stub.next = p_mem->large_stub.prev = &(p_mem->large_stub);
	p_mem->heap_size = 0;
}

static void clr_bin_mem_context(BinMem *p_mem)
{
	if (p_mem->__alloc == (__ALloc)bin_mem_alloc_l)
		close_mutex(p_mem->op_lock);

	while (p_mem->heap_stub.next != &(p_mem->heap_stub))
	{
		MemHeap *p_heap = p_mem->heap_stub.next;
		p_heap->prev->next = p_heap->next;
		p_heap->next->prev = p_heap->prev;
		if (p_mem->parent)
			IFree(p_mem->parent, p_heap);
		else
			top_mem_free(p_heap);
	}

	while (p_mem->large_stub.next != &(p_mem->large_stub))
	{
		MemHeap *p_heap = p_mem->large_stub.next;
		p_heap->prev->next = p_heap->next;
		p_heap->next->prev = p_heap->prev;
		if (p_mem->parent)
			IFree(p_mem->parent, p_heap);
		else
			top_mem_free(p_heap);
	}
	p_mem->heap_size = 0;
}

inline static Chunk *alloc_top_chunk(BinMem *p_mem)
{
	MemHeap *p_heap;
	SizeT	size;
	Chunk	*p_chunk;

	p_heap = p_mem->parent ?
		(MemHeap*)IAlloc(p_mem->parent, p_mem->step + sizeof(MemHeap)) :
		(MemHeap*)top_mem_alloc(p_mem->step + sizeof(MemHeap));
	if (p_heap)
	{
		p_heap->heap_size = p_mem->step;
		p_mem->heap_size += p_mem->step;
		p_heap->prev = p_mem->heap_stub.prev;
		p_heap->next = &(p_mem->heap_stub);
		p_mem->heap_stub.prev->next = p_heap;
		p_mem->heap_stub.prev = p_heap;
		size = p_mem->step;
		p_chunk = (Chunk*)(((char*)p_heap) + sizeof(MemHeap));
		p_chunk->prev_size = 0;
		SetHead(p_chunk, size);
		return p_chunk;
	}
	else
		return NULL;
}

static void *bin_mem_alloc(BinMem *p_mem, SizeT si)
{
	SizeT	siz;
	int		b_i;
	Chunk	*p_chunk;

	siz = ((si + MINSIZE - 1) / MALLOC_ALIGNMENT)*MALLOC_ALIGNMENT;
	if (siz >= p_mem->step - sizeof(Chunk))
	{
		MemHeap *p_heap = p_mem->parent ?
			(MemHeap *)IAlloc(p_mem->parent, siz + sizeof(MemHeap))
			: (MemHeap *)top_mem_alloc(siz + sizeof(MemHeap));
		if (p_heap)
		{
			p_heap->heap_size = siz;
			p_mem->heap_size += siz;
			p_heap->prev = p_mem->large_stub.prev;
			p_heap->next = &(p_mem->large_stub);
			p_chunk = (Chunk*)(((char*)p_heap) + sizeof(MemHeap));
			p_mem->large_stub.prev->next = p_heap;
			p_mem->large_stub.prev = p_heap;
			p_chunk->prev_size = 0;
			p_chunk->size = siz;
			SetGMem(p_chunk);
			return chunk2mem(p_chunk);
		}
		else
			return NULL;
	}

	/*检查对应的bin是否有自由块*/
	if (IsSmall(siz))
	{
		b_i = small_bin_index(siz);
		if (p_mem->bins[b_i].next == &(p_mem->bins[b_i]))
			b_i++;
		if (p_mem->bins[b_i].next != &(p_mem->bins[b_i]))
		{
			p_chunk = p_mem->bins[b_i].next;
			remove_from_bin(p_chunk);
			SetUse(p_chunk);
			return chunk2mem(p_chunk);
		}
		else
			b_i++;
	}
	else
	{
		b_i = bin_index(siz);
		if (p_mem->bins[b_i].next != &(p_mem->bins[b_i]))
		{
			for (p_chunk = p_mem->bins[b_i].next; p_chunk != &(p_mem->bins[b_i]); p_chunk = p_chunk->next)
			{
				if (ChunkSize(p_chunk) >= siz)
				{
					remove_from_bin(p_chunk);
					if (ChunkSize(p_chunk) >= siz + MINSIZE)
						split_chunk(p_mem, p_chunk, siz);
					else
						SetUse(p_chunk);
					return chunk2mem(p_chunk);
				}
			}
		}
		b_i++;
	}

	/*尝试从最近被分割的块分配*/
	if (p_mem->bins[0].next != &(p_mem->bins[0]))
	{
		p_chunk = p_mem->bins[0].next;
		remove_from_bin(p_chunk);
		if (ChunkSize(p_chunk) >= siz)
		{
			if (ChunkSize(p_chunk) >= siz + MINSIZE)
				split_chunk0(p_mem, p_chunk, siz);
			else
				SetUse(p_chunk);
			return chunk2mem(p_chunk);
		}
		else
			insert_into_bin(p_mem, p_chunk);
	}

	/*向上检查各个bin*/
	for (; b_i <= p_mem->max_b_i; b_i++)
	{
		if (p_mem->bins[b_i].next != &(p_mem->bins[b_i]))
		{
			for (p_chunk = p_mem->bins[b_i].next; p_chunk != &(p_mem->bins[b_i]); p_chunk = p_chunk->next)
			{
				if (ChunkSize(p_chunk) >= siz)
				{
					remove_from_bin(p_chunk);
					if (ChunkSize(p_chunk) >= siz + MINSIZE)
						split_chunk0(p_mem, p_chunk, siz);
					else
						SetUse(p_chunk);
					return  chunk2mem(p_chunk);
				}
			}
		}
	}

	if ((p_chunk = alloc_top_chunk(p_mem)) != NULL)
	{
		/*将p_chunk分割*/
		if (ChunkSize(p_chunk) >= siz + MINSIZE)
			split_chunk0(p_mem, p_chunk, siz);
		else
			SetUse(p_chunk);
		return chunk2mem(p_chunk);
	}
	else
		return NULL;
}

static void bin_mem_free(BinMem *p_mem, void *p)
{
	Chunk	*p_chunk;
	bool	is_bin0 = false;

	p_chunk = mem2chunk(p);
	if (IsGMem(p_chunk))
	{
		MemHeap *p_heap = (MemHeap*)(((char*)p_chunk) - sizeof(MemHeap));
		p_mem->heap_size -= p_heap->heap_size;
		p_heap->next->prev = p_heap->prev;
		p_heap->prev->next = p_heap->next;
		if (p_mem->parent)
			IFree(p_mem->parent, p_heap);
		else
			top_mem_free(p_heap);
		return;
	}
#ifdef SAFE_FREE
	//检查下一块的上一块是否与当前块吻合
	if (NextOk(p_chunk))
	{
		Chunk *p_next = NextChunk(p_chunk);
		if (!PrevOk(p_next) || PrevChunk(p_next) != p_chunk)
			raise_mem_err(65);
	}
	if (PrevOk(p_chunk))
	{
		Chunk *p_prev = PrevChunk(p_chunk);
		if (!NextOk(p_prev) || NextChunk(p_prev) != p_chunk)
			raise_mem_err(65);
	}
#endif
	if (NextOk(p_chunk))
	{
		Chunk *p_next = NextChunk(p_chunk);
		if (!IsUse(p_next))
		{
			if (p_mem->bins[0].next == p_next)
				is_bin0 = true;
			remove_from_bin(p_next);
			merge_chunk(p_chunk, p_next);
		}
	}
	if (PrevOk(p_chunk))
	{
		Chunk *p_prev = PrevChunk(p_chunk);
		if (!IsUse(p_prev))
		{
			if (p_mem->bins[0].next == p_prev)
				is_bin0 = true;
			remove_from_bin(p_prev);
			merge_chunk(p_prev, p_chunk);
			p_chunk = p_prev;
		}
	}
	ClrUse(p_chunk);
	if (ChunkSize(p_chunk) == p_mem->step && p_mem->heap_stub.next->next != &(p_mem->heap_stub))
	{
		MemHeap *p_heap = (MemHeap*)(((char*)p_chunk) - sizeof(MemHeap));
		p_mem->heap_size -= p_heap->heap_size;
		p_heap->prev->next = p_heap->next;
		p_heap->next->prev = p_heap->prev;
		if (p_mem->parent)
			IFree(p_mem->parent, p_heap);
		else
			top_mem_free(p_heap);
	}
	else
	{
		if (is_bin0)
			insert_into_bin0(p_mem, p_chunk);
		else
			insert_into_bin(p_mem, p_chunk);
	}
}

/*重新分配局部内存*/
static void *bin_mem_realloc(BinMem *p_mem, void *p_old, SizeT si)
{
	SizeT	siz;
	Chunk	*p_chunk;
	void	*ret_p;
	SizeT	old_size;
	bool	is_bin0 = false;

	if (!p_old)
		return bin_mem_alloc(p_mem, si);

	siz = (si + (MINSIZE - 1));
	siz -= (siz&(MALLOC_ALIGNMENT-1));
	p_chunk = mem2chunk(p_old);

	/*检查原块是否使用了上级内存*/
	if (IsGMem(p_chunk))
	{
		MemHeap *p_heap = (MemHeap*)(((char*)p_chunk) - sizeof(MemHeap));
		p_mem->heap_size -= p_heap->heap_size;
		p_heap->next->prev = p_heap->prev;
		p_heap->prev->next = p_heap->next;
		if (p_mem->parent)
			p_heap = (MemHeap*)Realloc((MemCtx*)(p_mem->parent), p_heap, siz + sizeof(MemHeap));
		else
			p_heap = (MemHeap*)top_mem_realloc(p_heap, siz + sizeof(MemHeap));
		if (p_heap)
		{
			p_heap->heap_size = siz;
			p_mem->heap_size += siz;
			p_heap->prev = p_mem->large_stub.prev;
			p_heap->next = &(p_mem->large_stub);
			p_mem->large_stub.prev->next = p_heap;
			p_mem->large_stub.prev = p_heap;
			p_chunk = (Chunk*)(((char*)p_heap) + sizeof(MemHeap));
			return chunk2mem(p_chunk);
		}
		else
			return NULL;
	}

	old_size = ChunkSize(p_chunk) - 2 * SIZE_SZ;
	/*若合并后块尺寸大于新尺寸,则当多余内存超过MINSIZE时,分割合并后的块,将多余部分重新插入自由表,否则将该块内存全部返回*/
	if (ChunkSize(p_chunk) >= siz)
	{
		if (ChunkSize(p_chunk) >= siz + MINSIZE)
		{
			if (is_bin0)
				split_chunk0(p_mem, p_chunk, siz);
			else
				split_chunk(p_mem, p_chunk, siz);
		}
		else
			SetUse(p_chunk);
		return chunk2mem(p_chunk);
	}

	/*尝试合并前块*/
	if (PrevOk(p_chunk))
	{
		Chunk *p_prev = PrevChunk(p_chunk);
		if ((!IsUse(p_prev)))
		{
			if (p_mem->bins[0].prev == p_prev)
				is_bin0 = true;
			remove_from_bin(p_prev);
			merge_chunk(p_prev, p_chunk);
			p_chunk = p_prev;
		}
	}
	/*尝试从合并后的块中重分配*/
	if (ChunkSize(p_chunk) >= siz)
	{
		ret_p = chunk2mem(p_chunk);
		if (si>old_size)
			memmove(ret_p, p_old, old_size);
		else
			memmove(ret_p, p_old, si);
		if (ChunkSize(p_chunk) >= siz + MINSIZE)
		{
			if (is_bin0)
				split_chunk0(p_mem, p_chunk, siz);
			else
				split_chunk(p_mem, p_chunk, siz);
		}
		else
			SetUse(p_chunk);
		return ret_p;
	}
	/*与前合块进行合并操作后,内存仍不够,则分配新块,拷贝内容,释放旧块*/
	else
	{
		ret_p = bin_mem_alloc((BinMem*)p_mem, si);
		if (ret_p)
		{
			if (si>old_size)
				memmove(ret_p, p_old, old_size);
			else
				memmove(ret_p, p_old, si);
			ClrUse(p_chunk);
			/*若前面操作涉及到bins[0]且local_alloc操作未涉及bin0[0],则将内存块挂入bins[0]*/
			if (is_bin0 && p_mem->bins[0].next != &(p_mem->bins[0]))
				insert_into_bin0(p_mem, p_chunk);
			else
				insert_into_bin(p_mem, p_chunk);
			return ret_p;
		}
		else
			return NULL;
	}
}

static void *bin_mem_alloc_l(BinMem *p_mem, SizeT si)
{
	void *p;
	wait_mutex(p_mem->op_lock);
	p = bin_mem_alloc(p_mem, si);
	free_mutex(p_mem->op_lock);
	return p;
}

static void *bin_mem_realloc_l(BinMem *p_mem, void *p_old, SizeT si)
{
	void *p;
	wait_mutex(p_mem->op_lock);
	p = bin_mem_realloc(p_mem, p_old, si);
	free_mutex(p_mem->op_lock);
	return p;
}

static void bin_mem_free_l(BinMem *p_mem, void *p)
{
	wait_mutex(p_mem->op_lock);
	bin_mem_free(p_mem, p);
	free_mutex(p_mem->op_lock);
}

