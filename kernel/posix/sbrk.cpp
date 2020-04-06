#include <posix/unistd.h>
#include <posix/stdint.h>
#include <posix/stddef.h>

int brk(void *addr) {
  return 0;
}

void *sbrk(intptr_t increment) {
  return 0;
}

