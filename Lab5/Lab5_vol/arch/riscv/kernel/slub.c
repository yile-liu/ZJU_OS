#include "slub.h"

#include <put.h>

#include "buddy.h"
#include "stddef.h"

enum
{
  PAGE_FREE,
  PAGE_BUDDY,
  PAGE_SLUB,
  PAGE_RESERVE
};

struct cache_area cache_region;
unsigned long cache_tid = 0;

struct kmem_cache *slub_allocator[NR_PARTIAL] = {};
void *page_base;

const size_t kmem_cache_objsize[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048};
const char *kmem_cache_name[] = {
    "slub-objectsize-8   ", "slub-objectsize-16  ", "slub-objectsize-32  ",
    "slub-objectsize-64  ", "slub-objectsize-128 ", "slub-objectsize-256 ",
    "slub-objectsize-512 ", "slub-objectsize-1024", "slub-objectsize-2048"};

#define IS_POWER_OF_2(x) (((x) & ((x)-1)))
#define ALIGN_SIZE(size, aligns) (((size - 1) / aligns + 1) * aligns)
#define STRUCT_PAGE_SIZE (ALIGN_SIZE(sizeof(struct page), 8))
#define GET_NR_PAGE_PER_SLUB(size) \
  (size < PAGE_SIZE ? 4 : ((size / PAGE_SIZE) + 1) * 4)
#define ADDR_TO_PAGE(addr)                                            \
  ((struct page *)(page_base +                                        \
                   ((((unsigned long)addr - VM_START) & PAGE_MASK) >> \
                    PAGE_SHIFT) *                                     \
                       STRUCT_PAGE_SIZE))
#define PAGE_TO_ADDR(page_addr)                                            \
  ((void *)((((page_addr - page_base) / STRUCT_PAGE_SIZE) << PAGE_SHIFT) + \
            VM_START))

void *memset(void *dst, int c, uint n)
{
  char *cdst = (char *)dst;
  for (int i = 0; i < n; i++)
  {
    cdst[i] = c;
  }
  return dst;
}

void set_page_attr(void *addr, int nr, int attr)
{
  struct page *page, *npage;
  if (addr == NULL)
    return;

  npage = ADDR_TO_PAGE(addr);
  page = npage;

  for (int i = 0; i < nr - 1; i++)
  {
    page->flags = attr;
    page->count = 0;
    page->header = npage;
    page->next = (struct page *)((unsigned long)page + STRUCT_PAGE_SIZE);
    page = page->next;
  }
  page->flags = attr;
  page->header = npage;
  page->next = NULL;
  page->count = 0;

  return;
}

void clear_page_attr(struct page *p)
{
  struct page *t;
  if (p->flags == PAGE_FREE)
    return;
  else if (p->flags == PAGE_SLUB)
  {
    list_del(&(p->slub_list));
  }
  while (p != NULL)
  {
    t = p;
    p = p->next;
    t->flags = PAGE_FREE;
    t->header = NULL;
    t->next = NULL;
    t->slub = NULL;
    t->count = 0;
    t->freelist = NULL;
  }
}

void *init_object_list(void *addr, size_t objsize, size_t size)
{
  unsigned long end = (unsigned long)addr + size;
  unsigned long this_obj = (unsigned long)addr;
  unsigned long next_obj = (unsigned long)addr + objsize;

  while (next_obj < end)
  {
    *(void **)(this_obj) = (void *)next_obj;
    this_obj += objsize;
    next_obj += objsize;
  }

  if (next_obj == end)
  {
    *(void **)(this_obj) = NULL;
    return (void *)this_obj;
  }
  else
  {
    *(void **)(this_obj - objsize) = NULL;
    return (void *)(this_obj - objsize);
  }
}

void page_init()
{
  size_t page_size;

  page_size = (1UL << (MEM_SHIFT - PAGE_SHIFT)) * STRUCT_PAGE_SIZE;

  page_base = alloc_pages(page_size);
  if (page_base == NULL)
    while (1)
      ;

  memset(page_base, 0, page_size << PAGE_SHIFT);

  set_page_attr(page_base, page_size, PAGE_RESERVE);
}

void slub_structure_init()
{
  void *structure_free_list = alloc_pages(STRUCTURE_SIZE);

  if (structure_free_list == NULL)
    while (1)
      ;

  set_page_attr(structure_free_list, STRUCTURE_SIZE, PAGE_RESERVE);

  cache_region.base = structure_free_list;
  init_object_list(structure_free_list,
                   ALIGN_SIZE(sizeof(struct kmem_cache), 8),
                   (STRUCTURE_SIZE << PAGE_SHIFT));
  cache_region.freelist = (void **)structure_free_list;

  return;
}

struct kmem_cache *alloc_slub_structure()
{
  struct kmem_cache *p;

  if (cache_region.freelist == NULL)
    return NULL;

  p = (struct kmem_cache *)(cache_region.freelist);
  cache_region.freelist = *(void **)(cache_region.freelist);
  memset(p, 0, sizeof(struct kmem_cache));

  return p;
}

void *cache_create(const char *name, size_t size, unsigned int aligns,
                   int flags, void *func(void *))
{
  struct kmem_cache *s = NULL;
  s = alloc_slub_structure();
  if (s == NULL)
  {
    while (1)
      ;
  }

  /* init kmem_cache */
  s->name = name;
  s->init_func = (void *)func;
  s->refcount = 1;

  s->min_partial = 4;
  s->size = ALIGN_SIZE(size, aligns);
  s->size = s->size < 8 ? 8 : s->size;
  s->object_size = size;
  s->offset = 0;
  s->inuse = 0;
  s->align = aligns;
  INIT_LIST_HEAD(&(s->list));
  s->nr_page_per_slub = GET_NR_PAGE_PER_SLUB(s->size);

  s->nr_partial = 0;

  s->tid = cache_tid++;
  return s;
}

void *cache_alloc_pages(struct kmem_cache *cache)
{
  void *p;
  void *tp;
  struct page *page;

  p = alloc_pages(cache->nr_page_per_slub);
  if (p == NULL)
    return NULL;

  memset(p, 0, (cache->nr_page_per_slub) << PAGE_SHIFT);
  set_page_attr(p, cache->nr_page_per_slub, PAGE_SLUB);
  tp = init_object_list(p, cache->size,
                        ((cache->nr_page_per_slub) << PAGE_SHIFT));
  cache->freelist = p;
  page = ADDR_TO_PAGE(p);
  page->slub = cache;
  INIT_LIST_HEAD(&(page->slub_list));
  list_add_tail(&(cache->list), &(page->slub_list));
  cache->nr_partial++;

  return p;
}

static void inline free_slub_structure(struct kmem_cache *cache)
{
  if (cache_region.freelist == NULL)
    cache_region.freelist = (void *)cache;
  else
  {
    *((void **)cache) = *(void **)(cache_region.freelist);
    *(void **)(cache_region.freelist) = (void *)cache;
  }

  return;
}

void slub_init()
{
  page_init();
  slub_structure_init();
  for (int i = 0; i < NR_PARTIAL; i++)
  {
    slub_allocator[i] = kmem_cache_create(kmem_cache_name[i],
                                          kmem_cache_objsize[i], 8, 0, NULL);
  }

  return;
}

struct kmem_cache *kmem_cache_create(const char *name, size_t size,
                                     unsigned int aligns, int flags,
                                     void *func(void *))
{
  struct kmem_cache *s = NULL;
  const char *cache_name;

  s = cache_create(name, size, aligns, flags, func);
  if (cache_alloc_pages(s) == NULL)
  {
    free_slub_structure(s);
    return NULL;
  }
  else
  {
    return s;
  }
}

int kmem_cache_destroy(struct kmem_cache *s)
{
  struct list_head *l;
  struct page *p;
  list_for_each(l, &(s->list))
  {
    if (list_entry(l, struct page, slub_list)->count != 0)
      return -1;
  }
  list_for_each(l, &(s->list))
  {
    p = list_entry(l, struct page, slub_list);
    free_pages(PAGE_TO_ADDR((void *)p));
    clear_page_attr(p);
  }
  free_slub_structure(s);
  return 0;
}

void *kmem_cache_alloc(struct kmem_cache *cache)
{
  void *object = NULL;
  struct list_head *l;
  struct page *p;

  if (cache->freelist == NULL)
  {
    list_for_each(l, &(cache->list))
    {
      p = list_entry(l, struct page, slub_list);
      if (p->freelist != NULL)
      {
        cache->freelist = p->freelist;
      }
    }
    if (cache->freelist == NULL && cache_alloc_pages(cache) == NULL)
      return NULL;
  }

  object = cache->freelist;
  cache->freelist = *(cache->freelist);
  (ADDR_TO_PAGE(object)->header)->count++;
  if (cache->init_func != NULL)
    cache->init_func(object);
  else
  {
    memset(object, 0, cache->size);
  }
  return object;
}

void kmem_cache_free(void *obj)
{
  struct page *page = ADDR_TO_PAGE(obj)->header;
  struct kmem_cache *s;
  void *p;

  s = page->slub;
  if (page->freelist == NULL)
  {
    page->freelist = obj;
  }
  else
  {
    *((void **)obj) = *(void **)(page->freelist);
    *(void **)(page->freelist) = (void *)obj;
  }
  page = page->header;
  page->count--;
  if (page->count == 0 && s->nr_partial > s->min_partial)
  {
    free_pages(obj);
    clear_page_attr(page);
    s->nr_partial--;
  }

  return;
}

void *kmalloc(size_t size)
{
  int objindex;
  void *p;

  if (size == 0)
    return NULL;

  // size 若在 kmem_cache_objsize 所提供的范围之内，则使用 slub allocator
  // 来分配内存
  for (objindex = 0; objindex < NR_PARTIAL; objindex++)
  {
    // YOUR CODE HERE
    // 1. 判断 size 是否处于 slub_allocator[objindex] 管理的范围内
    // 2. 如果是就使用 kmem_cache_alloc 接口分配
    if (size <= kmem_cache_objsize[objindex])
    {
      break;
    }
  }

  // size 若不在 kmem_cache_objsize 范围之内，则使用 buddy system 来分配内存
  if (objindex >= NR_PARTIAL)
  {
    // YOUR CODE HERE
    // 1. 使用 alloc_pages 接口分配

    p = alloc_pages(size / PAGE_SIZE);
    set_page_attr(p, (size - 1) / PAGE_SIZE, PAGE_BUDDY);
  }
  else
  {
    p = kmem_cache_alloc(slub_allocator[objindex]);
  }

  return p;
}

void kfree(const void *addr)
{
  struct page *page;

  if (addr == NULL)
    return;

  // 获得地址所在页的属性
  page = ADDR_TO_PAGE(addr);

  // 判断当前页面属性
  if (page->flags == PAGE_BUDDY)
  {
    // YOUR CODE HERE
    // 1. 使用 free_pages 接口回收
    free_pages(addr);
    clear_page_attr(ADDR_TO_PAGE(addr)->header);
  }
  else if (page->flags == PAGE_SLUB)
  {
    // YOUR CODE HERE
    // 1. 使用 kmem_cache_free 接口回收
    kmem_cache_free(addr);
  }

  return;
}
