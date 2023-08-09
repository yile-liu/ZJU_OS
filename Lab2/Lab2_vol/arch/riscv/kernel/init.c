#include "riscv.h"

extern puts(), put_num(), put_hex();

extern void clock_init(void);

void intr_enable(void)
{
  // 设置 sstatus[sie] = 1, 打开 s 模式的中断开关
  // your code
  put_hex(read_csr(sstatus));
  puts(" is sstatus before setting\n");
  write_csr(sstatus, read_csr(sstatus)|0x0000000000000002);
//   set_csr(sstatus, 1);
  put_hex(read_csr(sstatus));
  puts(" is sstatus after setting\n");
}

void intr_disable(void)
{
  // 设置 sstatus[sie] = 0, 关闭 s 模式的中断开关
  // your code

//   clear_csr(sstatus, 1);
  write_csr(sstatus, read_csr(sstatus)&0xFFFFFFFFFFFFFFFD);
  put_hex(read_csr(sstatus));
  puts(" is sstatus after clearing\n");
}

void idt_init(void)
{
  extern void trap_s(void);
  // 向 stvec 寄存器中写入中断处理后跳转函数的地址
  write_csr(stvec, &trap_s);
}

void init(void)
{
  idt_init();
  intr_enable();
  clock_init();
}
