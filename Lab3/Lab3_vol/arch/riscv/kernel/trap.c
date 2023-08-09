#include "defs.h"

extern main(), puts(), put_num(), ticks, do_timer();

extern void clock_set_next_event(void);

void handler_s(uint64_t cause, uint64_t epc)
{
  // interrupt
  if (cause >> 63)
  {
    // supervisor timer interrupt
    if (((cause << 1) >> 1) == 5)
    {
      // run do_timer() per 10 ticks, which means 0.1s.
      if (ticks % 10 == 0)
      {
        do_timer();
      }
      clock_set_next_event();
    }
  }
  return;
}

// void handler_s(uint64_t cause)
// {
//   // interrupt
//   if (cause >> 63)
//   {
//     // supervisor timer interrupt
//     if (((cause << 1) >> 1) == 5)
//     {

//       // 设置下一个时钟中断，打印当前的中断数目。
//       clock_set_next_event();
//       puts("Supervisor Time Interrupt. Cnt=");
//       put_num(ticks);
//       puts("\n");
//     }
//   }
// }