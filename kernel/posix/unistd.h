#ifndef POSIX_UNISTD_H
#define POSIX_UNISTD_H

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#if 0
#include <posix/stdint.h>
#include <posix/stddef.h>

extern int brk(void *aAddress);
extern void *sbrk(intptr_t aIncrement);
#endif

#endif
