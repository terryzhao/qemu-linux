#ifndef _LINUX_SLUB_DEF_H
#define _LINUX_SLUB_DEF_H

/*
 * SLUB : A Slab allocator without object queues.
 *
 * (C) 2007 SGI, Christoph Lameter
 */
#include <linux/kobject.h>

enum stat_item {
	ALLOC_FASTPATH,		/* Allocation from cpu slab */
	ALLOC_SLOWPATH,		/* Allocation by getting a new cpu slab */
	FREE_FASTPATH,		/* Free to cpu slab */
	FREE_SLOWPATH,		/* Freeing not to cpu slab */
	FREE_FROZEN,		/* Freeing to frozen slab */
	FREE_ADD_PARTIAL,	/* Freeing moves slab to partial list */
	FREE_REMOVE_PARTIAL,	/* Freeing removes last object */
	ALLOC_FROM_PARTIAL,	/* Cpu slab acquired from node partial list */
	ALLOC_SLAB,		/* Cpu slab acquired from page allocator */
	ALLOC_REFILL,		/* Refill cpu slab from slab freelist */
	ALLOC_NODE_MISMATCH,	/* Switching cpu slab */
	FREE_SLAB,		/* Slab freed to the page allocator */
	CPUSLAB_FLUSH,		/* Abandoning of the cpu slab */
	DEACTIVATE_FULL,	/* Cpu slab was full when deactivated */
	DEACTIVATE_EMPTY,	/* Cpu slab was empty when deactivated */
	DEACTIVATE_TO_HEAD,	/* Cpu slab was moved to the head of partials */
	DEACTIVATE_TO_TAIL,	/* Cpu slab was moved to the tail of partials */
	DEACTIVATE_REMOTE_FREES,/* Slab contained remotely freed objects */
	DEACTIVATE_BYPASS,	/* Implicit deactivation */
	ORDER_FALLBACK,		/* Number of times fallback was necessary */
	CMPXCHG_DOUBLE_CPU_FAIL,/* Failure of this_cpu_cmpxchg_double */
	CMPXCHG_DOUBLE_FAIL,	/* Number of times that cmpxchg double did not match */
	CPU_PARTIAL_ALLOC,	/* Used cpu partial on alloc */
	CPU_PARTIAL_FREE,	/* Refill cpu partial on free */
	CPU_PARTIAL_NODE,	/* Refill cpu partial from node partial */
	CPU_PARTIAL_DRAIN,	/* Drain cpu partial to node partial */
	NR_SLUB_STAT_ITEMS };

//各个CPU都有自己独立的一个结构，用于管理本地的对象缓存
struct kmem_cache_cpu {
	void **freelist;	/* Pointer to next available object */ /* 空闲对象队列的指针 */
	unsigned long tid;	/* Globally unique transaction id */ /* 用于保证cmpxchg_double计算发生在正确的CPU上，并且可作为一个锁保证不会同时申请这个kmem_cache_cpu的对象 */
	struct page *page;	/* The slab from which we are allocating */ /* 指向slab对象来源的内存页面 */
	struct page *partial;	/* Partially allocated frozen slabs */ /* 指向曾分配完所有的对象，但当前已回收至少一个对象的page */
#ifdef CONFIG_SLUB_STATS
	unsigned stat[NR_SLUB_STAT_ITEMS];
#endif
};

/*
 * Word size structure that can be atomically updated or read and that
 * contains both the order and the number of objects that a slab of the
 * given order would contain.
 */
struct kmem_cache_order_objects {
	unsigned long x;
};

/*
 * Slab cache management.
 */
struct kmem_cache {
	struct kmem_cache_cpu __percpu *cpu_slab; /*每CPU的结构，用于各个CPU的缓存管理*/
	/* Used for retriving partial slabs etc */
	unsigned long flags; /*描述标识，描述缓冲区属性的标识*/
	unsigned long min_partial;
	int size;		/* The size of an object including meta data */ /*对象大小，包括元数据大小*/
	int object_size;	/* The size of an object without meta data */ /*slab对象纯大小*/
	int offset;		/* Free pointer offset. */ /*空闲对象的指针偏移*/
	int cpu_partial;	/* Number of per cpu partial objects to keep around */ /*每CPU持有量*/
	struct kmem_cache_order_objects oo;  /*存放分配给slab的页框的阶数(高16位) slab中的对象数量(低16位)*/

	/* Allocation and freeing of slabs */
	struct kmem_cache_order_objects max;
	struct kmem_cache_order_objects min; /*存储单对象所需的页框阶数及该阶数下可容纳的对象数*/
	gfp_t allocflags;	/* gfp flags to use on each alloc */ /*申请页面时使用的GFP标识*/
	int refcount;		/* Refcount for slab cache destroy */ /*缓冲区计数器，当用户请求创建新的缓冲区时，SLUB分配器重用已创建的相似大小的缓冲区，从而减少缓冲区个数*/
	void (*ctor)(void *); /*创建对象的回调函数*/
	int inuse;		/* Offset to metadata */ /*元数据偏移量*/
	int align;		/* Alignment */ /*对齐值*/
	int reserved;		/* Reserved bytes at the end of slabs */
	const char *name;	/* Name (only for display!) */ /*slab缓存名称*/
	struct list_head list;	/* List of slab caches */ /*用于slab cache管理的链表*/
#ifdef CONFIG_SYSFS
	struct kobject kobj;	/* For sysfs */
#endif
#ifdef CONFIG_MEMCG_KMEM
	struct memcg_cache_params memcg_params;
	int max_attr_size; /* for propagation, maximum size of a stored attr */
#ifdef CONFIG_SYSFS
	struct kset *memcg_kset;
#endif
#endif

#ifdef CONFIG_NUMA
	/*
	 * Defragmentation by allocating from a remote node.
	 */
	int remote_node_defrag_ratio; /*远端节点的反碎片率，值越小则越倾向从本节点中分配对象*/
#endif
	struct kmem_cache_node *node[MAX_NUMNODES]; /*各个内存管理节点的slub信息*/
};

#ifdef CONFIG_SYSFS
#define SLAB_SUPPORTS_SYSFS
void sysfs_slab_remove(struct kmem_cache *);
#else
static inline void sysfs_slab_remove(struct kmem_cache *s)
{
}
#endif


/**
 * virt_to_obj - returns address of the beginning of object.
 * @s: object's kmem_cache
 * @slab_page: address of slab page
 * @x: address within object memory range
 *
 * Returns address of the beginning of object
 */
static inline void *virt_to_obj(struct kmem_cache *s,
				const void *slab_page,
				const void *x)
{
	return (void *)x - ((x - slab_page) % s->size);
}

void object_err(struct kmem_cache *s, struct page *page,
		u8 *object, char *reason);

#endif /* _LINUX_SLUB_DEF_H */
