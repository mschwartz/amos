#include <Exec/BBase.h>
#include <x86/bochs.h>

#if 0
#ifndef __XTENSA__

#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <cmath>

#else

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#endif
#endif

BBase::BBase() {
  //
}

BBase::~BBase() {
  //
}

/*
 * Random number generator
 * http://www.firstpr.com.au/dsp/rand31/p1192-park.pdf
 *
 * See also: random-number-generator.pdf in references/
 */
static TUint32 sRandomSeed;

TUint32 GetRandomSeed() {
  return sRandomSeed;
}

void SeedRandom(TUint32 aSeed) {
  sRandomSeed = aSeed;
}

TUint32 Random() {
  static const TUint32 a = 16807,
          m = 2147483647;
  sRandomSeed = (a * sRandomSeed) % m;
  return sRandomSeed % m;
}

TInt32 Random(TInt32 aMin, TInt32 aMax) {
  return TInt32(Random()) % (aMax - aMin) + aMin;
}

TFloat RandomFloat() {
  TInt32 r = TInt32(Random());
  TFloat ret = TFloat(r) / TFloat(UINT32_MAX);
  return ret;
}
TUint32 Milliseconds() {
  return 0;
#if 0
#ifdef __XTENSA__
  return (TUint32)(esp_timer_get_time() / 1000);
#else
  TUint32         ms;
  time_t          s;
  struct timespec spec;

  clock_gettime(CLOCK_REALTIME, &spec);
  s  = spec.tv_sec;
  ms = lround(spec.tv_nsec / 1.0e6);
  if (ms > 999) {
    s++;
    ms = 0;
  }
  ms += s * 1000;
  return ms;
#endif
#endif
}

#if 0
#ifndef __XTENSA__

void *operator new(size_t size) { return AllocMem(size, MEMF_SLOW); }

void *operator new[](size_t size) { return AllocMem(size, MEMF_SLOW); }

#ifdef __linux__
void operator delete(void *ptr) _GLIBCXX_USE_NOEXCEPT   { FreeMem(ptr); }
#else
void operator delete(void *ptr)  { FreeMem(ptr); }
#endif


#ifdef __linux__
void operator delete[](void *ptr) _GLIBCXX_USE_NOEXCEPT   { FreeMem(ptr); }
#else
void operator delete[](void *ptr)  { FreeMem(ptr); }
#endif

#endif

#endif
