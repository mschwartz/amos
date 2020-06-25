#ifndef EXEC_MEMORY_H
#define EXEC_MEMORY_H

#include <Exec/Types.h>

// these flags may be ORed together
enum EMemoryFlags {
  MEMF_ANY,
  MEMF_PUBLIC,
  MEMF_CHIP,
  MEMF_FAST,
  MEMF_CLEAR
};

extern TAny *AllocMem(TInt64 aSize, int aFlags = MEMF_ANY);
extern void FreeMem(TAny *aMemory);

extern void *operator new(unsigned long aSize);
extern void *operator new[](unsigned long aSize);
extern void operator delete(void *aMemory, unsigned long aSize);
extern void operator delete(void *aMemory);
extern void operator delete[](void *aMemory, unsigned long aSize);
extern void operator delete[](void *aMemory);

TInt StringLength(const char *aString);
void CopyString(char *aDestination, const char *aSource);
char *DuplicateString(const char *aString, EMemoryFlags aMemoryType = MEMF_PUBLIC);

TInt CompareStrings(const char *aString1, const char *aString2);
TInt CompareMemory(const TAny *aMem1, const TAny *aMem2, TUint64 aCount);

void SetMemory8(TAny *aDestination, TUint8 aValue, TInt64 aCount);
void SetMemory16(TAny *aDestination, TUint16 aValue, TInt64 aCount);
void SetMemory32(TAny *aDestination, TUint32 aValue, TInt64 aCount);
void SetMemory64(TAny *aDestination, TUint64 aValue, TInt64 aCount);

void CopyMemory(TAny *aDestination, TAny *aSource, TInt64 aCount);
void CopyMemory64(TAny *aDestination, TAny *aSource, TInt64 aCount);


/** 
 * parse token, separated by aDelim,  from aString into aToken.
 *
 * Returns pointer to aString, just past aDelim.
 */
char *GetToken(char *aString, char *aToken, const char aDelim);

#endif
