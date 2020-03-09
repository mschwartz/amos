#ifndef STRING_H
#define STRING_H

#include "../x86/types.h"

extern size_t strlen(char *s);
extern void strcpy(char *dst, const char *src);
extern void reverse(char *s);

extern void memcpy(void *dst, void *src, size_t size);
extern void memset(void *dst, uint8_t v, size_t size);

#endif
