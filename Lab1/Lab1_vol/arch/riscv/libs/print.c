#include "defs.h"
extern struct sbiret sbi_call(uint64_t ext, uint64_t fid, uint64_t arg0,
                              uint64_t arg1, uint64_t arg2, uint64_t arg3,
                              uint64_t arg4, uint64_t arg5);

#define PUT_NUM_STR_LEN 12

int puts(char *str)
{
  // your code
  int index = 0;
  char c = str[index];
  while (c != '\0')
  {
    sbi_call(0x01, 0, c, 0, 0, 0, 0, 0);
    index++;
    c = str[index];
  }
  return 0;
}

int put_num(uint64_t n)
{
  // your code
  int index = PUT_NUM_STR_LEN - 1;
  char str[PUT_NUM_STR_LEN] = "";
  str[PUT_NUM_STR_LEN - 1] = 0;
  while (n > 0)
  {
    index--;
    str[index] = n % 10 + '0';
    n = n / 10;
  }
  char *newStr = str + index;
  puts(newStr);
  return 0;
}