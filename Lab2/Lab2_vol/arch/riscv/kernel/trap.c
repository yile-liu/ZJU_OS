#include "defs.h"

extern main(), puts(), put_num(), ticks;
extern void clock_set_next_event(void);

void handler_s(uint64_t cause)
{
  // interrupt
  if (cause >> 63)
  {
    // supervisor timer interrupt
    if (((cause << 1) >> 1) == 5)
    {

      // 设置下一个时钟中断，打印当前的中断数目。
      clock_set_next_event();
      puts("Supervisor Time Interrupt. Cnt=");
      put_num(ticks);
      puts("\n");
    }
  }
}