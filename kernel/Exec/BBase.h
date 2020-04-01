#ifndef BBASE_H
#define BBASE_H

// Uncomment for production build
// #define PRODUCTION

#include <Exec/BTypes.h>
//#include "Memory.h"
//#include <memory>


/**
 * BBase is the base class for EVERYTHING.
 */
class BBase {
public:
  BBase();

  ~BBase();
public:
 void operator delete(void *ptr, unsigned long size) { }
// int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle) { return 0; }

};

//////// RANDOM NUMBER GENERATOR

/**
 * \brief SeedRandom
 *
 * Seed random number generator.
 *
 * @param aSeed
 */
void SeedRandom(TUint32 aSeed);

/**
  * Get random seed
  */
TUint32 GetRandomSeed();

/**
 * Generate a random number
 * @return
 */
TUint32 Random();

/**
 * Generate a random number in given range.
 *
 * Note: this routine produces signed result and takes signed arguments.  This allows a range of something like -10 to 100.
 *
 * @param aMin
 * @param aMax
 * @return
 */
TInt32 Random(TInt32 aMin, TInt32 aMax);

/**
 * Return random number between 0 and 1 (floating point)
 * @return
 */
TFloat RandomFloat();

#if 0
#ifdef __XTENSA__
inline void *operator new(size_t size) { return AllocMem(size, MEMF_SLOW); }

inline void *operator new[](size_t size) { return AllocMem(size, MEMF_SLOW); }

inline void operator delete(void *ptr) {
  //
  FreeMem(ptr);
}

inline void operator delete[](void *ptr) {
  //
  FreeMem(ptr);
}
#else
#if __cplusplus > 199711L
// C++11 deprecates throw
extern void *operator new(size_t size);
extern void *operator new[](size_t size);
#else
// < C++11: throw not deprecated
extern void *operator new(size_t size) throw(std::bad_alloc);
extern void *operator new[](size_t size) throw(std::bad_alloc);
#endif

#ifdef __linux__
extern void operator delete(void *ptr) _GLIBCXX_USE_NOEXCEPT;

extern void operator delete[](void *ptr) _GLIBCXX_USE_NOEXCEPT;
#else
extern void operator delete(void *ptr) throw();

extern void operator delete[](void *ptr) throw();
#endif

#endif

#endif

extern TUint32 Milliseconds();

#endif
