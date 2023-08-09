#include "defs.h"
extern struct sbiret sbi_call(uint64_t ext, uint64_t fid, uint64_t arg0,
                              uint64_t arg1, uint64_t arg2, uint64_t arg3,
                              uint64_t arg4, uint64_t arg5);

#define _dec_len_max_uint64 21
#define _hex_len_max_uint64 19

int puts(char *str) {
  while(*str){
    sbi_call(1, 0, *str, 0, 0, 0, 0, 0);
    str ++;
  }
  return 0;
}

int put_num(uint64_t n) {
    // uint64 has at most 20 digits
    char ret_str[_dec_len_max_uint64] = "";
    char* ret = ret_str + _dec_len_max_uint64 - 1;
    *ret = '\0';
    if(!n){
        ret--;
        *ret = '0';
    }
    while(n){
        ret--;
        *ret = n % 10 + '0';
        n /= 10;
    }
    puts(ret);
  return 0;
}

int put_hex(uint64_t n) {
    // uint64 has at most 20 digits
    char ret_str[_hex_len_max_uint64] = "";
    char* ret = ret_str + _hex_len_max_uint64 - 1;
    *ret = '\0';
    if(!n){
        ret--;
        *ret = '0';
    }
    while(n){
        ret--;
        *ret = (n%16 < 10)? n%16+'0': n%16-10+'A';
        n /= 16;
    }
    ret --;
    *ret = 'x';
    ret --;
    *ret = '0';
    puts(ret);
  return 0;
}