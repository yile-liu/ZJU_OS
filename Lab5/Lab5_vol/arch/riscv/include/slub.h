#pragma once

#include <list.h>

#include "vm.h"

#define NR_PARTIAL 9
#define PAGE_SHIFT 12
#define PPN_SHIFT 10
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (~((1UL << PAGE_SHIFT) - 1))
#define MEM_SHIFT (24 - PAGE_SHIFT)
#define STRUCTURE_SIZE 16UL

struct page {
  unsigned long flags;
  int count;
  struct page *header;
  struct page *next;
  struct list_head slub_list;
  struct kmem_cache *slub; /* Pointer to slab */
  void *freelist;
};

struct cache_area {
  void *base;
  size_t size;
  void *freelist;
};

struct kmem_cache {
  /* kmem_cache_cpu */
  void **freelist;   /* Pointer to next available object */
  unsigned long tid; /* Globally unique transaction id */
  struct page *page; /* The slab from which we are allocating */

  /* Used for retrieving partial slabs, etc. */
  int refcount;
  unsigned long min_partial;
  size_t size;         /* The size of an object including metadata */
  size_t object_size;  /* The size of an object without metadata */
  unsigned int offset; /* Free pointer offset */
  unsigned long nr_page_per_slub;

  void (*init_func)(void *);
  unsigned int inuse;        /* Offset to metadata */
  unsigned int align;        /* Alignment */
  unsigned int red_left_pad; /* Left redzone padding size */
  const char *name;          /* Name (only for display!) */
  struct list_head list;     /* List of slab caches */

  /* kmem_cache_node */
  unsigned long nr_partial;
  struct list_head partial;
#ifdef CONFIG_SLUB_DEBUG
  unsigned long nr_slabs;
  unsigned long total_objects;
  struct list_head full;
#endif
};

void slub_init();
struct kmem_cache *kmem_cache_create(const char *, size_t, unsigned int, int,
                                     void *(void *));

void *kmem_cache_alloc(struct kmem_cache *);
void kmem_cache_free(void *);

void *kmalloc(size_t);
void kfree(const void *);
