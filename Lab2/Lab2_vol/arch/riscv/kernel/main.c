typedef unsigned long long uint64_t;
extern puts();
extern init();
extern get_cycles();
extern struct sbiret sbi_call(uint64_t ext, uint64_t fid, uint64_t arg0,
                              uint64_t arg1, uint64_t arg2, uint64_t arg3,
                              uint64_t arg4, uint64_t arg5);

#include "riscv.h"

int main() {
  int a = 0;
  init();
  while (1) {
    if (a++ > 10000) {
      a = 0;
        // put_hex(read_csr(sepc));
        // puts(" is the current sepc in 'main'\n");
        // put_hex(read_pc(0));
        // puts(" is the current pc in 'main'\n");
    }
  }
  return 0;
}