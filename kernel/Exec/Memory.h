#ifndef EXEC_MEMORY_H
#define EXEC_MEMORY_H

enum EMemoryFlags {
  MEMF_ANY,
  MEMF_PUBLIC,
  MEMF_CHIP,
  MEMF_FAST
};

TAny *AllocMem(TInt64 size, enum EMemoryFlags flags);

#endif
