#include <Exec/Types.h>
#include <Exec/Memory.h>
#include <Exec/BBase.h>

// #include <posix/malloc.h>

#ifdef KERNEL
#include <Exec/x86/bochs.h>
#else
#include <stdlib.h>
#endif

#define LOCKMEM
//#undef LOCKMEM

#ifdef KERNEL
#ifdef LOCKMEM
#include <Exec/ExecBase.h>
#endif
#endif

#ifdef KERNEL

extern "C" TAny *ExtendDataSegment(TUint64 aIncrement) {
  extern void *kernel_end;
  static TUint8 *sProgramBreak = ENull;

  if (sProgramBreak == ENull) {
    sProgramBreak = (TUint8 *)&kernel_end;
  }

  if (aIncrement == 0) {
    return sProgramBreak;
  }

  void *ret = sProgramBreak;
  sProgramBreak = &sProgramBreak[aIncrement];
  return ret;
}

class Chunk : public BBase {
public:
  Chunk *mNext, *mPrev;
  TUint64 mSize;

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

class ChunkList : public Chunk {
public:
  ChunkList() {
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

static ChunkList sFreeChunks;

TAny *AllocMem(TInt64 aSize, TInt aFlags) {
  Chunk *ret = ENull;
  for (Chunk *c = sFreeChunks.First(); !sFreeChunks.End(c); c = c->mNext) {
    if (c->mSize == aSize) {
      c->Remove();
      ret = c;
    }
  }
  if (ret == ENull) {
    ret = (Chunk *)ExtendDataSegment(sizeof(Chunk) + aSize);
  }

  ret->mSize = aSize;

  TUint8 *p = (TUint8 *)ret;
  TUint8 *mem = &p[sizeof(Chunk)];
  if (aFlags & MEMF_CLEAR) {
    SetMemory8(mem, 0, aSize);
  }
  return mem;
}

void FreeMem(TAny *aPtr) {
  // TODO combine this Chunk with any existing that are contiguous
  TUint8 *p = (TUint8 *) aPtr;
  Chunk *c = (Chunk *)(p - sizeof(Chunk));
  sFreeChunks.AddHead(*c);
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
