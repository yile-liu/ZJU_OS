// #include "riscv.h"

// extern void clock_init(void);
// extern void task_init(void);

// void intr_enable(void) {
//   // 设置 sstatus[sie] = 1, 打开 s 模式的中断开关
//   // your code
// }

// void intr_disable(void) {
//   // 设置 sstatus[sie] = 0, 关闭 s 模式的中断开关
//   // your code
// }

// void idt_init(void) {
//   extern void trap_s(void);
//   // 向 stvec 寄存器中写入中断处理后跳转函数的地址
//   // your code
// }

// void init(void) {
//   idt_init();
//   intr_enable();
//   clock_init();
//   task_init();
// }

#include "riscv.h"

extern void clock_init(void);
extern void task_init(void);

void intr_enable(void)
{
  // 设置 sstatus[sie] = 1, 打开 s 模式的中断开关
  // your code
  write_csr(sstatus, read_csr(sstatus) | 0x0000000000000002);
}

void intr_disable(void)
{
  // 设置 sstatus[sie] = 0, 关闭 s 模式的中断开关
  // your code
  write_csr(sstatus, read_csr(sstatus) & 0xFFFFFFFFFFFFFFFD);
}

void idt_init(void)
{
  extern void trap_s(void);
  // 向 stvec 寄存器中写入中断处理后跳转函数的地址
  write_csr(stvec, (unsigned long long)trap_s);
}

void init(void)
{
  idt_init();
  intr_enable();
  clock_init();
  task_init();
}
