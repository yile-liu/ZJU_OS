#include "rand.h"

#include "put.h"

unsigned int rand() {
  static unsigned int r[344];
  static int init = 0;
  static int n = 0;

  if (!init) {
    r[0] = SEED;
    for (int i = 1; i < 31; i++) r[i] = (16807 * r[i - 1]) & 0x7fffffff;
    for (int i = 31; i < 34; i++) r[i] = r[i - 31];
    for (int i = 34; i < 344; i++) r[i] = r[i - 31] + r[i - 3];
    init = 1;
  }

  unsigned int ret = r[n % 344] = r[(n + 313) % 344] + r[(n + 341) % 344];
  n = (n + 1) % 344;
  return ret % 5;
}
