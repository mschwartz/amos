#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

#include "types.h"

const uint64_t PAGE_SIZE = 4096;
const uint64_t KILOBYTE = 1024;
const uint64_t MEGABYTE = 1024 * KILOBYTE;
const uint64_t GIGABYTE = 1024 * MEGABYTE;
//const uint64_t GIGABYTE = 0x40000000;

extern uint64_t init_memory();

extern void memcpy(void *dst, void *src, size_t size);
extern void memset(void *dst, uint8_t v, size_t size);

#endif
