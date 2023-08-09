#include "put.h"
#include "sched.h"

int start_kernel() {
  const char *msg =
      "ZJU OS LAB 5\n"
      "Student1:3200102708 刘思锐 Student2:3200105724 黄亦霄\n";
  puts(msg);

  slub_init();
  task_init();

  // 设置第一次时钟中断
  asm volatile("ecall");

  dead_loop();

  return 0;
}
