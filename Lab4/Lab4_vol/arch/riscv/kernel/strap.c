#include "put.h"
#include "sched.h"

int count = 0;
void handler_s(uint64_t cause) {
  if (cause >> 63) {                // interrupt
    if (((cause << 1) >> 1) == 5) { // supervisor timer interrupt
      asm volatile("ecall");
      do_timer();
      count++;
    }
  }
  return;
}
