#include <Exec/Memory.h>
#include <string.h>
#include <posix/malloc.h>

void *AllocMem(TInt64 aSize, TInt aFlags) {
  TUint8 *mem = (TUint8 *)malloc(aSize);
  if (aFlags & MEMF_CLEAR) {
    memset(mem, 0, aSize);
  }
  return (TAny *)mem;

}

void FreeMem(TAny *memory) {
  free(memory);
}

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

// these can be optimized by storing TUint64 until we need to store the remaining values
void SetMemory8(TAny *aDestination, TUint8 aValue, TInt64 aCount) {
  TUint8 *dst = (TUint8 *)aDestination;

  for (TInt64 i=0; i<aCount; i++) {
    *dst++ = aValue;
  }
}

void SetMemory16(TAny *aDestination, TUint16 aValue, TInt64 aCount) {
  TUint16 *dst = (TUint16 *)aDestination;

  for (TInt64 i=0; i<aCount; i++) {
    *dst++ = aValue;
  }
}

void SetMemory32(TAny *aDestination, TUint32 aValue, TInt64 aCount) {
  TUint32 *dst = (TUint32 *)aDestination;

  for (TInt64 i=0; i<aCount; i++) {
    *dst++ = aValue;
  }
}

void SetMemory64(TAny *aDestination, TUint64 aValue, TInt64 aCount) {
  TUint64 *dst = (TUint64 *)aDestination;

  for (TInt64 i=0; i<aCount; i++) {
    *dst++ = aValue;
  }
}

