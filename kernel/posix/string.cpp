#include <posix.h>
#include <posix/string.h>

size_t strlen(char *s) {
  int count = 0;
  while (*s++) {
    count++;
  }
  return count;
}

void strcpy(char *dst, const char *src) {
  while ((*dst++ = *src++));
}

void reverse(char *src) {
  int l = strlen(src);
  char work[l+1];
  char *dst = work,
       *end_ptr = &src[l-1];

  for (int i=0; i<l; i++) {
    *dst++ = *end_ptr--;
  }
  *dst++ = '\0';
  strcpy(src, work);
}

void memcpy(void *dst, void *src, size_t size) {
  uint8_t *s = (uint8_t *)src,
          *d = (uint8_t *)dst;

  for (size_t i = 0; i < size; i++) {
    *d++ = *s++;
  }
}

void memset(void *dst, uint8_t v, size_t size) {
  uint8_t *d = (uint8_t *)dst;
  //  size = 4096;

  //  kprint("memset(%x, %d, %d)\n", d, v, 4096);
  for (size_t i = 0; i < size; i++) {
    *d++ = v;
  }
}

