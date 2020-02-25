#include "types.h"
#include "string.h"
#include "Screen.h"

#define abs(a) (a < 0) ? -a : a

size_t strlen(char *s) {
  int count = 0;
  while (*s++) {
    count++;
  }
  return count;
}

void reverse(char *dst, char *src) {
  int l = strlen(src);
  char *begin_ptr = src,
       *end_ptr = &src[l-1];

  for (int i=0; i<l-1; i++) {
    char ch = *end_ptr;
    *end_ptr = *begin_ptr;
    *begin_ptr = ch;

    begin_ptr++;
    end_ptr--;
  }
}

static const char *digits = "0123456789abcdef";
char *itoa(int value, char *result, int base) {
  // check that the base if valid

//  screen->puts("itoa ");
//  screen->hexlong(value);
//  screen->puts(" ");
//  screen->hexlong(base);
//  screen->newline();

  if (base < 2 || base > 16) {
    *result = 0;
    return result;
  }
  if (value == 0) {
    result[0] = '0';
    result[1] = '\0';
    return result;
  }
  else {
    char *out = result;
    int quotient = abs(value);
//    screen->puts("quotient ");
//    screen->hexlong(quotient);
//    screen->newline();

    do {
      const int tmp = quotient / base;
//      screen->puts(" tmp ");
//      screen->hexlong(tmp);
      int ndx = quotient - (tmp * base);
      char c = digits[ndx];
      *out++ = c;
      *out = 0;
      quotient = tmp;
//      screen->puts(" quotient ");
//      screen->hexlong(quotient);
//      screen->puts(" ndx ");
//      screen->hexlong(ndx);
//      screen->puts(" c ");
//      screen->putc(c);
//      screen->puts(" out ");
//      screen->puts(out);
//      screen->newline();
    } while (quotient);

    // Apply negative sign
    if (value < 0) {
      *out++ = '-';
    }

    *out = 0;
    //    screen->puts("out ");
    //    screen->puts(out);
    //    screen->newline();
    reverse(result, result);
  }
  return result;
}
#if 0
char *itoa(int value, char *result, int base) {
  // check that the base if valid
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char *ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
  } while (value);

  // Apply negative sign
  if (tmp_value < 0)
    *ptr++ = '-';
  *ptr-- = '\0';
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }
  return result;
}
#endif
