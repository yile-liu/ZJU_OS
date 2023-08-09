#pragma once

#include "sched.h"

extern unsigned long long _end;

#define VM_START 0xffffffe000000000

void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz,
                    int perm);
void paging_init();
