
//内联汇编中输入输出限制符"i"表示立即数

#define write_csr(reg, val)                                     \
  ({                                                            \
    if (__builtin_constant_p(val) && (unsigned long)(val) < 32) \
      asm volatile("csrwi " #reg ", %0" ::"i"(val));            \
    else                                                        \
      asm volatile("csrw " #reg ", %0" ::"r"(val));             \
  })

#define read_csr(reg)                             \
  ({                                              \
    unsigned long __tmp;                          \
    asm volatile("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp;                                        \
  })

#define set_csr(reg, bit)                                               \
  ({                                                                    \
    unsigned long __tmp;                                                \
    if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32)         \
      asm volatile("csrrsi %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
    else                                                                \
      asm volatile("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit));  \
    __tmp;                                                              \
  })

#define clear_csr(reg, bit)                                             \
  ({                                                                    \
    unsigned long __tmp;                                                \
    if (__builtin_constant_p(bit) && (unsigned long)(bit) < 32)         \
      asm volatile("csrrci %0, " #reg ", %1" : "=r"(__tmp) : "i"(bit)); \
    else                                                                \
      asm volatile("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "r"(bit));  \
    __tmp;                                                              \
  })

#define rdtime() read_csr(time)
#define rdcycle() read_csr(cycle)
#define rdinstret() read_csr(instret)