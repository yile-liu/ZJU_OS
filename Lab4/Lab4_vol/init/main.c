#include "put.h"
#include "sched.h"

int start_kernel() {
  const char *msg = "ZJU OS LAB 4\n"
                    "Student1:3200102708 刘思锐 Student2:3200105724 黄亦霄\n";
  puts(msg);

  // 设置第一次时钟中断
  asm volatile("ecall");

  task_init();
  dead_loop();

  return 0;
}
