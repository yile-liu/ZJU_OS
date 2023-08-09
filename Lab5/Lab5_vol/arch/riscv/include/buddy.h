#pragma once

#include "stdint.h"

typedef struct {
  unsigned long size;
  unsigned bitmap[8192];
} buddy;

void init_buddy_system(void);
void *alloc_pages(int);
void free_pages(void *);
