#include "put.h"

#include "stddef.h"
#include "stdint.h"

// ─── Char And String ─────────────────────────────────────────────────────────

int putchar(const char c) {
  *UART16550A_DR = (unsigned char)(c);
  return (unsigned char)c;
}

int puts(const char *s) {
  const char *start = s;
  while (*s) {
    putchar(*(s++));
  }
  return s - start;
}

// ─── Int And Long Int And Long Long Int ──────────────────────────────────────

int putint(int num, int base) {
  if (num < 0) {
    putchar('-');
    num = -num;
  }
  return putuint(num, base);
}

int putlint(lint num, int base) {
  if (num < 0) {
    putchar('-');
    num = -num;
  }
  return putulint(num, base);
}

int putllint(llint num, int base) {
  if (num < 0) {
    putchar('-');
    num = -num;
  }
  return putullint(num, base);
}

// ─── Unsigned Int And Long Int And Long Long Int ─────────────────────────────

int putuint(uint num, int base) {
  char s[16], *p = s + 15;
  *(p--) = '\0';
  do {
    *(p--) = num % base < 10 ? num % base + '0' : num % base - 10 + 'a';
    num = num / base;
  } while (num);
  return puts(++p);
}

int putulint(ulint num, int base) {
  char s[16], *p = s + 15;
  *(p--) = '\0';
  do {
    *(p--) = num % base < 10 ? num % base + '0' : num % base - 10 + 'a';
    num = num / base;
  } while (num);
  return puts(++p);
}

int putullint(ullint num, int base) {
  char s[32], *p = s + 31;
  *(p--) = '\0';
  do {
    *(p--) = num % base < 10 ? num % base + '0' : num % base - 10 + 'a';
    num = num / base;
  } while (num);
  return puts(++p);
}

// ─── Printf Support %(ll)d, %(ll)u, %(ll)x, %s, %% ───────────────────────────

int printf(const char *fmt, ...) {
  int res = 0, longfmt = 0;
  int i;
  lint li;
  llint lli;
  uint ui;
  ulint uli;
  ullint ulli;
  char *s;

  va_list vl;
  va_start(vl, fmt);
  while (*fmt) {
    if (*fmt != '%') {
      putchar(*(fmt++));
      res++;
    } else {
      fmt++;
    control:
      switch (*(fmt++)) {
        case '%':
          putchar('%');
          res++;
          break;

        case 'd':
          if (longfmt == 2) {
            lli = va_arg(vl, lint);
            res += putllint(lli, 10);
          } else if (longfmt == 1) {
            li = va_arg(vl, lint);
            res += putlint(li, 10);
          } else {
            i = va_arg(vl, int);
            res += putint(i, 10);
          }
          break;

        case 'u':
          if (longfmt == 2) {
            ulli = va_arg(vl, ulint);
            res += putullint(ulli, 10);
          } else if (longfmt == 1) {
            uli = va_arg(vl, ulint);
            res += putulint(uli, 10);
          } else {
            ui = va_arg(vl, uint);
            res += putuint(ui, 10);
          }
          break;

        case 'x':
          res += puts("0x");
          if (longfmt == 2) {
            ulli = va_arg(vl, ulint);
            res += putullint(ulli, 16);
          } else if (longfmt == 1) {
            uli = va_arg(vl, ulint);
            res += putulint(uli, 16);
          } else {
            ui = va_arg(vl, uint);
            res += putuint(ui, 16);
          }
          break;

        case 's':
          s = va_arg(vl, char *);
          res += puts(s);
          break;

        case 'l':
          longfmt++;
          goto control;

        default:
          break;
      }
      longfmt = 0;
    }
  }
  va_end(vl);
  return res;
}