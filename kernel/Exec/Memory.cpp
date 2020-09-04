#define DEBUGME
#undef DEBUGME

#include <Types.hpp>
#include <Exec/Memory.hpp>
#include <Exec/BBase.hpp>

// #include <posix/malloc.h>

#ifdef KERNEL
#include <Exec/x86/bochs.hpp>
#include <Exec/x86/kernel_memory.hpp>
#else
#include <stdlib.h>
#endif

#define LOCKMEM
//#undef LOCKMEM

#ifdef KERNEL
#ifdef LOCKMEM
#include <Exec/ExecBase.hpp>
#endif
#endif

#ifdef KERNEL

static TUint64 sUsedMem = 0,
               sTotalMem = 0;

static TAny *ExtendChipRam(TUint64 aIncrement) {
  extern void *chip_memory;
  extern void *chip_memory_end;
  static TUint8 *sChipBreak = ENull;
  static void *end = &chip_memory_end;

  aIncrement = (aIncrement + 15) & ~0x0f;
  DLOG("ExtendChipRam(%d)\n", aIncrement);
  if (sChipBreak == ENull) {
    sChipBreak = (TUint8 *)&chip_memory;
    DLOG("ExtendChipRam(0x%x)\n", sChipBreak);
  }

  if (aIncrement == 0) {
    if (sChipBreak > end) {
      DLOG("AllocMem(%d) OUT OF CHIP MEMORY\n", aIncrement);
      bochs;
      return ENull;
    }
    return sChipBreak;
  }

  if (sChipBreak > end) {
    DLOG("OUT OF CHIP MEMORY\n", sChipBreak, end);
    bochs;
    return ENull;
  }

  void *ret = sChipBreak;
  sChipBreak = &sChipBreak[aIncrement];
  if (sChipBreak > end) {
    DLOG("OUT OF CHIP MEMORY\n", sChipBreak, end);
    bochs;
    return ENull;
  }

  return ret;
}

static TAny *ExtendDataSegment(TUint64 aIncrement) {
  extern void *kernel_end;
  static TUint8 *sProgramBreak = ENull;

  if (sProgramBreak == ENull) {
    sProgramBreak = (TUint8 *)&kernel_end;
  }

  if (aIncrement == 0) {
    return sProgramBreak;
  }

  aIncrement = (aIncrement + 15) & ~0x0f;
  void *ret = sProgramBreak;
  sProgramBreak = &sProgramBreak[aIncrement];
  return ret;
}

class Chunk : public BBase {
public:
  Chunk *mNext, *mPrev;
  TUint64 mSize; // high bit = 1 means MEMF_CHIP

public:
  void Dump() {
    dlog("Chunk(%x) mNext(%x) mPrev(%x) mSize(%016x)\n", this, mNext, mPrev, mSize);
  }

public:
  void InsertBeforeChunk(Chunk *aNextChunk) {
    Chunk *pnode = aNextChunk->mPrev;
    pnode->mNext = this;
    aNextChunk->mPrev = this;
    mNext = aNextChunk;
    mPrev = pnode;
  }

  // make this node first on the list, if node is key
  void InsertAfterChunk(Chunk *mPreviousChunk) {
    Chunk *nnode = mPreviousChunk->mNext;
    mPreviousChunk->mNext = this;
    nnode->mPrev = this;
    mNext = nnode;
    mPrev = mPreviousChunk;
  }

  void Remove() {
    mNext->mPrev = mPrev;
    mPrev->mNext = mNext;
  }
};

const TUint64 CHUNK_CHIP = 0x8000000000000000;
class ChunkList : public Chunk {
public:
  ChunkList() {
    DLOG("ChunkList constructorn");
    Init();
  }
  void Init() {
    DLOG("ChunkList Init\n");
    mNext = (Chunk *)this;
    mPrev = (Chunk *)this;
  }
  void AddHead(Chunk &aChunk) {
    aChunk.InsertAfterChunk(this);
  }
  Chunk *RemHead() {
    Chunk *n = mNext;
    if (n == (Chunk *)this)
      return ENull;
    n->Remove();
    return n;
  }
  void AddTail(Chunk &aChunk) {
    aChunk.InsertBeforeChunk(this);
  }
  Chunk *RemTail() {
    Chunk *n = mPrev;
    if (n == (Chunk *)this)
      return NULL;
    n->Remove();
    return n;
  }

  Chunk *First() { return mNext; }
  Chunk *Next(Chunk *aChunk) { return aChunk->mNext; }
  Chunk *Last() { return mPrev; }
  Chunk *Prev(Chunk *aChunk) { return aChunk->mPrev; }
  TBool End(Chunk *aChunk) { return aChunk == (Chunk *)this; }
};

static TUint8 sChunkListMem[sizeof(ChunkList)];
static ChunkList *sFreeChunks = ENull;

void InitAllocMem() {
  gSystemInfo.mRam = 0;
  TBiosMemory *m = (TBiosMemory *)BIOS_MEMORY;
  TInt32 count = m->mCount;
  for (TInt32 i = 0; i < count; i++) {
    TMemoryInfo *b = &m->mInfo[i]; // defined in memory.inc
    TInt type = b->type;
    if (type != 1) {
      continue;
    }
    // b->Dump();
    if (b->address + b->size > gSystemInfo.mRam) {
      gSystemInfo.mRam = b->address + b->size;
    }
  }

  extern void *kernel_end;
  if (!sFreeChunks) {
    sFreeChunks = (ChunkList *)&sChunkListMem;
    sFreeChunks->Init();
  }
  sUsedMem = 0;
  sTotalMem = gSystemInfo.mRam - (TUint64)&kernel_end;
  // dlog("InitAllocMem total(%d)\n", gSystemInfo.mRam);
}

static TAny *allocate(TInt64 aSize, TInt aFlags) {
  Chunk *ret = ENull;
  DLOG("AllocMem aSize(%d) aFlags(%x)\n", aSize, aFlags);

  // search for free chunk of desired size
  for (Chunk *c = sFreeChunks->First(); !sFreeChunks->End(c); c = c->mNext) {
    DLOG("Chunk(%x)\n", c);
    if (aFlags & MEMF_CHIP) {
      // mSize will have CHUNK_CHIP bit set if the Chunk is in Chip RAM
      if (c->mSize == (aSize | CHUNK_CHIP)) {
        c->Remove();
        DLOG("AllocMem CHIP freeList HIT\n");
        ret = c;
        break;
      }
    }
    else if (c->mSize == aSize) {
      c->Remove();
      DLOG("AllocMem freeList HIT\n");
      ret = c;
      sUsedMem += aSize;
      break;
    }
  }

  if (ret == ENull) {
    // no Chunk found
    DLOG("AllocMem freeList MISS\n");
    if (aFlags & MEMF_CHIP) {
      ret = (Chunk *)ExtendChipRam(sizeof(Chunk) + aSize);
      ret->mSize = aSize | CHUNK_CHIP;
      DLOG("New CHIP Chunk(%x)\n", ret);
      ret->Dump();
    }
    else {
      sUsedMem += aSize;
      ret = (Chunk *)ExtendDataSegment(sizeof(Chunk) + aSize);
      ret->mSize = aSize;
    }
  }

  return ret;
}

static SpinLock allocmem_mutex;

TAny *AllocMem(TInt64 aSize, TInt aFlags) {
  allocmem_mutex.Acquire();
  TAny *ret = allocate(aSize, aFlags);
  allocmem_mutex.Release();

  TUint8 *p = (TUint8 *)ret;
  TUint8 *mem = &p[sizeof(Chunk)];
  if (aFlags & MEMF_CLEAR) {
    SetMemory8(mem, 0, aSize);
  }

  DLOG("AllocMem returns(%x)\n", mem);
  return mem;
}
void FreeMem(TAny *aPtr) {
  // TODO combine this Chunk with any existing that are contiguous
  allocmem_mutex.Acquire();
  TUint8 *p = (TUint8 *)aPtr;
  Chunk *c = (Chunk *)(p - sizeof(Chunk));
  if ((c->mSize & CHUNK_CHIP) == 0) {
    sUsedMem -= c->mSize;
  }
  sFreeChunks->AddHead(*c);
  allocmem_mutex.Release();
}

TUint64 AvailMem() {
  return sTotalMem - sUsedMem;
}

TUint64 TotalMem() {
  return sTotalMem;
}

TUint64 UsedMem() {
  return sUsedMem;
}

#else
void *AllocMem(TInt64 aSize, TInt aFlags) {
  TAny *mem = malloc(aSize);
  if (aFlags & MEMF_CLEAR) {
    SetMemory8(mem, 0, aSize);
  }
  return mem;
}

void FreeMem(TAny *p) {
  free(p);
}
#endif

#if 0
void *AllocMem(TInt64 aSize, TInt aFlags) {
#ifdef KERNEL
#ifdef LOCKMEM
  DISABLE;
#endif
#endif

// #ifdef KERNEL
//   dlog("AllocMem(%d)\n", aSize);
// #endif

  TUint8 *mem = (TUint8 *)malloc(aSize);

#ifdef KERNEL
#ifdef LOCKMEM
  ENABLE;
#endif
#endif
  if (aFlags & MEMF_CLEAR) {
    SetMemory8(mem, 0, aSize);
  }
  return (TAny *)mem;
}

void FreeMem(TAny *memory) {
#ifdef KERNEL
#ifdef LOCKMEM
  TUint64 flags = GetFlags();
  cli();
#endif
#endif
  free(memory);
#ifdef KERNEL
#ifdef LOCKMEM
  SetFlags(flags);
#endif
#endif
}
#endif

void *operator new(unsigned long aSize) {
#ifdef KERNEL
  return AllocMem(aSize, MEMF_PUBLIC);
#else
  return malloc(aSize);
#endif
}

void operator delete(void *aMemory, unsigned long aSize) {
#ifdef KERNEL
  FreeMem(aMemory);
#else
  free(aMemory);
#endif
}

void operator delete(void *aMemory) {
#ifdef KERNEL
  FreeMem(aMemory);
#else
  free(aMemory);
#endif
}

void operator delete[](void *aMemory) {
#ifdef KERNEL
  FreeMem(aMemory);
#else
  free(aMemory);
#endif
}
TInt StringLength(const char *aString) {
  TInt len;
  for (len = 0; aString[len]; len++)
    ;
  return len;
}

char *DuplicateString(const char *aString, EMemoryFlags aMemoryType) {
  char *s = (char *)AllocMem((unsigned long)StringLength(aString) + 1, aMemoryType);
  CopyString(s, aString);
  return s;
}

void CopyString(char *aDestination, const char *aSource) {
  while ((*aDestination++ = *aSource++))
    ;
}

TInt CompareStrings(const char *aString1, const char *aString2) {
  while (*aString1) {
    if (*aString1 != *aString2) {
      break;
    }
    aString1++;
    aString2++;
  }
  return (*aString1 - *aString2);
}

TInt CompareMemory(const TAny *aMem1, const TAny *aMem2, TUint64 aCount) {
  char *aString1 = (char *)aMem1;
  char *aString2 = (char *)aMem2;

  while (*aString1 && --aCount > 0) {
    if (*aString1 != *aString2) {
      break;
    }
    aString1++;
    aString2++;
  }
  return (*aString1 - *aString2);
}

// these can be optimized by storing TUint64 until we need to store the remaining values
void SetMemory8(TAny *aDestination, TUint8 aValue, TInt64 aCount) {
  TUint8 *dst = (TUint8 *)aDestination;

  for (TInt64 i = 0; i < aCount; i++) {
    *dst++ = aValue;
  }
}

void SetMemory16(TAny *aDestination, TUint16 aValue, TInt64 aCount) {
  TUint16 *dst = (TUint16 *)aDestination;

  for (TInt64 i = 0; i < aCount; i++) {
    *dst++ = aValue;
  }
}

void SetMemory32(TAny *aDestination, TUint32 aValue, TInt64 aCount) {
  TUint32 *dst = (TUint32 *)aDestination;

  for (TInt64 i = 0; i < aCount; i++) {
    *dst++ = aValue;
  }
}

void SetMemory64(TAny *aDestination, TUint64 aValue, TInt64 aCount) {
  TUint64 *dst = (TUint64 *)aDestination;

  for (TInt64 i = 0; i < aCount; i++) {
    *dst++ = aValue;
  }
}

void CopyMemory(TAny *aDestination, TAny *aSource, TInt64 aCount) {
  TUint8 *src = (TUint8 *)aSource,
         *dst = (TUint8 *)aDestination;
  for (TInt64 i = 0; i < aCount; i++) {
    *dst++ = *src++;
  }
}

void CopyMemory32(TAny *aDestination, TAny *aSource, TInt64 aCount) {
  TUint32 *src = (TUint32 *)aSource,
          *dst = (TUint32 *)aDestination;
  for (TInt64 i = 0; i < aCount; i++) {
    *dst++ = *src++;
  }
}

void CopyMemory64(TAny *aDestination, TAny *aSource, TInt64 aCount) {
  TUint64 *src = (TUint64 *)aSource,
          *dst = (TUint64 *)aDestination;
  for (TInt64 i = 0; i < aCount; i++) {
    *dst++ = *src++;
  }
}

char *GetToken(char *aString, char *aToken, const char aDelim) {
  if (aString == ENull || *aString == '\0') {
    return ENull;
  }
  while (*aString && *aString != aDelim) {
    *aToken++ = *aString++;
  }
  *aToken = '\0';
  if (*aString != '\0') {
    aString++;
  }
  return aString;
}
