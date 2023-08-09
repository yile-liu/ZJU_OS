#pragma once

#include "stdint.h"

#define UART16550A_DR (volatile unsigned char *)0x10000000

int putchar(const char c);
int putint(int num, int base);
int putuint(uint num, int base);
int printf(const char *s, ...);