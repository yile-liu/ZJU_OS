#include "defs.h"
#include "riscv.h"
volatile unsigned long long ticks;

static uint64_t timebase = 100000;

#define sbi_set_timer(target_time) sbi_call(0, 0, target_time, 0, 0, 0, 0, 0);

// 使用 rdtime 汇编指令获得当前 mtime 中的值并返回
// 你并不需要修改该函数
uint64_t get_cycles(void)
{
#if __riscv_xlen == 64
  uint64_t n;
  __asm__ __volatile__("rdtime %0"
                       : "=r"(n));
  return n;
#else
  uint32_t lo, hi, tmp;
  __asm__ __volatile__(
      "1:\n"
      "rdtimeh %0\n"
      "rdtime %1\n"
      "rdtimeh %2\n"
      "bne %0, %2, 1b"
      : "=&r"(hi), "=&r"(lo), "=&r"(tmp));
  return ((uint64_t)hi << 32) | lo;
#endif
}

// void clock_init(void)
// {
//    puts("ZJU OS LAB 2    Student_ID:3200102708, 3200105724\n");

//     // 对 sie 寄存器中的时钟中断位设置（ sie[stie] = 1 ）以启用时钟中断
//
//  put_hex(read_csr(sie));
//    puts(" is sie before setting\n");
//   //  set_csr(sie, 5);
//    write_csr(sie, read_csr(sie) | 0x0000000000000020);
//    put_hex(read_csr(sie));
//    puts(" is sie after setting\n");
//     // 设置第一个时钟中断
//  sbi_set_timer(get_cycles() + timebase);
//    ticks = 0;
//     // your code
// }

void clock_init(void)
{
  // your code
  // 对 sie 寄存器中的时钟中断位设置（ sie[stie] = 1 ）以启用时钟中断
  write_csr(sie, read_csr(sie) | 0x0000000000000020);
  // 设置第一个时钟中断
  sbi_set_timer(get_cycles() + timebase);

  ticks = 0;
}

void clock_set_next_event(void)
{
  // your code
  // 获取当前 cpu cycles 数并计算下一个时钟中断的发生时刻
  // 通过调用 OpenSBI 提供的函数触发时钟中断
  sbi_set_timer(get_cycles() + timebase);
  // puts("\n\n!!!!!!!!Interrupt!!!!!!!!\n\n");
  ticks++;
}
