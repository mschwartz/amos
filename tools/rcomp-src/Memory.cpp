#include "Memory.h"
#include <stdlib.h>


extern "C" void *AllocMem(unsigned long size, int flags) { return malloc(size); }
extern "C" void FreeMem(void *ptr) { free(ptr ); }
