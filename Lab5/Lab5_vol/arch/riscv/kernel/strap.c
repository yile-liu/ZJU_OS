#include "put.h"
#include "riscv.h"
#include "sched.h"
#include "vm.h"
#include "slub.h"

#define offset (0xffffffe000000000 - 0x80000000)

#define CAUSE_FETCH_PAGE_FAULT 12
#define CAUSE_LOAD_PAGE_FAULT 13
#define CAUSE_STORE_PAGE_FAULT 15

void handler_s(uint64_t cause, uint64_t *stack)
{
  // puts("handler_s start\n");
  if (cause >> 63) // interrupt
  {
    if (((cause << 1) >> 1) == 5) // supervisor timer interrupt
    {
      asm volatile("ecall");
      do_timer();
    }
  }
  else // Exception
  {
    uint64_t bad_addr = read_csr(stval);
    if (cause == CAUSE_FETCH_PAGE_FAULT || cause == CAUSE_LOAD_PAGE_FAULT || cause == CAUSE_STORE_PAGE_FAULT) // page fault
    {

      if (cause == CAUSE_STORE_PAGE_FAULT)
      {
        printf("\033[33m[%x] Store Page Fault\033[0m\n", bad_addr);
      }
      else if (cause == CAUSE_LOAD_PAGE_FAULT)
      {
        printf("\033[33m[%x] Load Page Fault\033[0m\n", bad_addr);
      }
      else if (cause == CAUSE_FETCH_PAGE_FAULT)
      {
        printf("\033[33m[%x] FETCH Page Fault\033[0m\n", bad_addr);
      }
      // printf("page fault! va: %lx, type: %d\n", bad_addr, cause);

      struct vm_area_struct *pos;
      int not_found = 1;
      list_for_each_entry(pos, &(current->mm.vm->vm_list), vm_list)
      {
        // puts("\tlist_for_each_entry loop\n");
        if (bad_addr >= pos->vm_start && bad_addr < pos->vm_end)
        {
          // 在节点内
          not_found = 0;

          uint64_t size = pos->vm_end - pos->vm_start;
          uint64_t pa = kmalloc(size) - offset;
          
          // create_mapping((current->mm.satp & 0x00000fffffffffff) << 12, pos->vm_start, pa - offset, size, 0x7);
          void (*create_mapping_in_pa)(uint64_t *, uint64_t, uint64_t, uint64_t, int) = (uint64_t)&create_mapping - offset;
          uint64_t pgtbl_in_pa = (current->mm.satp & 0x00000fffffffffff) << 12;
          create_mapping_in_pa(pgtbl_in_pa, bad_addr, pa, size, pos->vm_page_prot.pgprot);
        }
      }
      if(not_found){
        printf("Invalid vm area in page fault.\n");
      }
      // puts("list_for_each_entry stop\n");
    }
    return;
  }
}