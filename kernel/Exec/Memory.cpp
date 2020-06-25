#include <Exec/Types.h>
#include <Exec/Memory.h>
#include <posix/malloc.h>

#define LOCKMEM
//#undef LOCKMEM

#ifdef KERNEL
#ifdef LOCKMEM
#include <Exec/ExecBase.h>
#endif
#endif

void *AllocMem(TInt64 aSize, TInt aFlags) {
#ifdef KERNEL
#ifdef LOCKMEM
  TUint64 flags = GetFlags();
  cli();
#endif
#endif
  TUint8 *mem = (TUint8 *)malloc(aSize);
#ifdef KERNEL
#ifdef LOCKMEM
  SetFlags(flags);
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
