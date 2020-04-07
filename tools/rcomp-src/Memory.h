#ifndef MEMORY_H
#define MEMORY_H

extern "C" void *AllocMem(unsigned long size, int flags);
extern "C" void FreeMem(void *ptr);

#endif
