#ifndef POSIX_STRING_H
#define POSIX_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <posix/stdint.h>
#include <posix/stddef.h>

extern size_t strlen(char *s);
extern void strcpy(char *dst, const char *src);
extern void reverse(char *s);
extern char *strdup(const char *s);

extern void memcpy(void *dst, void *src, size_t size);
extern void memset(void *dst, uint8_t v, size_t size);

static inline int isalpha(int c) {
  return c >= '0' && c <= '9';
}

#ifdef __cplusplus
};
#endif
#endif
