#include "sched.h"

#include "buddy.h"
#include "put.h"
#include "rand.h"
#include "slub.h"

#define offset (0xffffffe000000000 - 0x80000000)

struct task_struct *current;
struct task_struct *task[NR_TASKS];
long PRIORITY_INIT_COUNTER[NR_TASKS] = {0, 1, 2, 3, 4};
long COUNTER_INIT_COUNTER[NR_TASKS] = {0, 1, 2, 3, 4};

int task_init_done = 0;
extern int page_id;
extern unsigned long long _end;
extern unsigned long long rodata_start;
extern unsigned long long data_start;

extern void init_epc(void);
extern void __switch_to(struct task_struct *current, struct task_struct *next);

void create_task_mapping(void *pgtbl)
{
  // TODO:
  /*
  再 alloc_pages 一个页，用来做该进程的根页表地址。
  需要为该进程创建和之前一模一样的页表。
  （基本可以理解为直接复制 paging_init 的代码，
  但要此时程序已经运行在 0xfff.... 的虚拟内存地址空间下了，代码需要做些调整，
  create_mapping 的时候要注意转换为 0x80000000 的物理内存空间，
  也就是说，虚拟内存页表里面不存在虚拟内存地址，每一级转换得到的都是实际的物理内存地址）
  */
  create_mapping(pgtbl, 0xffffffe000000000, 0x80000000, 0x1000000, 7);
  create_mapping(pgtbl, 0x80000000, 0x80000000, 0x1000000, 7);
  create_mapping(pgtbl, 0x10000000, 0x10000000, 0x1000, 0x3);
}

void *page_ptr;
void task_init(void)
{
  // for (int i = 0; i < NR_TASKS; i++)
  for (int i = 0; i < 5; i++)
  {
    struct task_struct *new_ts = alloc_pages(1);
    new_ts->blocked = 0;
    new_ts->counter = COUNTER_INIT_COUNTER[i];
    new_ts->pid = i;
    new_ts->priority = PRIORITY_INIT_COUNTER[i];
    new_ts->state = TASK_RUNNING;

    new_ts->thread.sp = new_ts + 0x1000; // PAGE_SIZE = 4096
    new_ts->thread.ra = &init_epc;

    void *pgtbl = alloc_pages(1);
    pgtbl -= offset;
    // printf("pgtbl = %lx\n", pgtbl);
    ((void (*)(void *))((unsigned long long)create_task_mapping - offset))(pgtbl);

    // load satp to enable paging
    // satp[63:60] = 8: SV39 paging mode
    // satp[59:44] = 0: no use here
    // satp[43: 0] = addr of PPN[0] >> 12
    unsigned long long satp = 8;
    satp = satp << 60;
    satp += ((unsigned long long)pgtbl >> 12);
    new_ts->mm.satp = satp;
    // INIT_LIST_HEAD(&(new_ts->mm.vm->vm_list));

    task[i] = new_ts;
  }
  current = task[0];
  task_init_done = 1;
  puts("task_init done\n");
}

void do_timer(void)
{
  // TODO: Copy from Lab4 untested

  // puts("do_timer start");
  if (!task_init_done)
    return;
  if (current == NULL)
    return;
  printf("[*PID = %d] Context Calculation: counter = %d,priority = %d\n",
         current->pid, current->counter, current->priority);
  // current process's counter -1, judge whether to schedule or go on.
  /* your code */

  current->counter--;
  if (current->counter <= 0)
  {
    schedule();
  }
}

void schedule(void)
{

  // puts("schedule start\n");
  unsigned char next = 0;
  /* your code */
  long min_counter = -1;
  for (int i = NR_TASKS; i > 0; i--)
  {
    if (task[i] == NULL)
    {
      continue;
    }
    else if (task[i]->state == TASK_RUNNING && task[i]->counter > 0)
    {
      if (min_counter == -1 || task[i]->counter < min_counter)
      {
        min_counter = task[i]->counter;
        next = i;
      }
    }
  }

  // no task remains counter
  if (next == 0)
  {

    while(1);
    // task[next]->counter = 1;
    // for (int i = 0; i < 5; i++)
    // {
    //   task[i]->counter = rand();
    //   task[i]->priority = rand();
    // }
    // schedule();
  }
  else if (current->pid != task[next]->pid)
  {
    printf(
        "[ %d -> %d ] Switch from task %d[%lx] to task %d[%lx], prio: %d, "
        "counter: %d\n",
        current->pid, task[next]->pid, current->pid,
        (unsigned long)current->thread.sp, task[next]->pid,
        (unsigned long)task[next], task[next]->priority, task[next]->counter);
  }
  switch_to(task[next]);
}

void switch_to(struct task_struct *next)
{
  // puts("switch_to start\n");
  struct task_struct *prev = NULL;
  if (next != current)
  {
    prev = current;
    current = next;

    // refactor satp
    printf("current->mm.satp = %lx\n", current->mm.satp);
    __asm__ volatile(
        "csrw satp, %[satp]\n"
        "sfence.vma\n"
        :
        : [satp] "r"(current->mm.satp)
        :);

    // puts("__switch_to start\n");
    __switch_to(prev, current);
  }
}

void dead_loop(void)
{
  // puts("dead_loop start\n");
  // printf("%d\n", current->pid);
  
  // int i = 0;
  // while (1)
  // {
  //   printf("%d ", i++);
  //   if (i % 10 == 0)
  //   {
  //     printf("\n");
  //   }
  // }

  while (1) {
    if (current->pid) {
      mmap(0, 0x1000, 7, 0, 0, 0);
      int *a = (int *)(0x0000);
      *a = 1;
      printf("\033[32m[CHECK] page store OK\033[0m\n\n");

      mmap(0x1000, 0x9000, 7, 0, 0, 0);
      a = (int *)(0x3000);
      int b = (*a);
      printf("\033[32m[CHECK] page load OK\033[0m\n\n");

      while (1)
        ;
    }
    continue;
  }
}

void *mmap(void *__addr, size_t __len, int __prot, int __flags, int __fd,
           int __offset)
{
  // puts("mmap start\n");
  struct vm_area_struct *new_vm = kmalloc(sizeof(struct vm_area_struct));
  new_vm->vm_start = (uint64_t)__addr;
  new_vm->vm_end = (uint64_t)__addr + __len;
  // new_vm->vm_flags = __prot;
  new_vm->vm_flags = __flags;
  new_vm->vm_page_prot.pgprot = __prot;

  if(current->mm.vm == 0){
    struct vm_area_struct *head = kmalloc(sizeof(struct vm_area_struct));
    current->mm.vm = head;
    INIT_LIST_HEAD(&(head->vm_list));
  }
  // insertion into the linked list
  list_add(&new_vm->vm_list, &(current->mm.vm->vm_list));
  return __addr;
}