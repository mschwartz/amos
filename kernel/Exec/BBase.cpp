#include <Exec/BBase.h>
#ifdef KERNEL
#include <x86/bochs.h>
#else
#include <stdlib.h>
#endif

TUint32 Milliseconds() {
  return 0;
}

BBase::BBase() {
#ifdef KERNEL
  dprint("BBase construcotr\n");
#endif
  //
}

BBase::~BBase() {
#ifdef KERNEL
  dprint("BBase destructor\n");
#endif
  //
}

void *BBase::operator new(size_t aSize) { 
#ifdef KERNEL
  dprint("new %d\n", aSize);
  return AllocMem(aSize, MEMF_SLOW); 
#else
  return malloc(aSize);
#endif
}

void *BBase::operator new[](size_t aSize) { 
#ifdef KERNEL
  dprint("new[] %d\n", aSize);
  return AllocMem(aSize, MEMF_SLOW); 
#else
  return malloc(aSize);
#endif
}

void BBase::operator delete(void *ptr) {
#ifdef KERNEL
  FreeMem(ptr);
#else
  free(ptr);
#endif
}

void BBase::operator delete[](void *ptr) {
#ifdef KERNEL
  FreeMem(ptr);
#else
  free(ptr);
#endif
}

TInt StringLength(const char *aString) {
  TInt len;
  for (len = 0; aString[len]; len++)
    ;
  return len;
}

char *DuplicateString(const char *aString, EMemoryFlags aMemoryType) {
  char *s = (char *)AllocMem(StringLength(aString) + 1, aMemoryType);
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

