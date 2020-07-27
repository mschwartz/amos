#include <Exec/ExecBase.hpp>
#include <Exec/Random.hpp>
#include <stdint.h>

/*
 * Random number generator
 * http://www.firstpr.com.au/dsp/rand31/p1192-park.pdf
 *
 * See also: random-number-generator.pdf in references/
 *
 * 64 bit versions: https://github.com/lemire/testingRNG/blob/master/source/lehmer64.h
 */
extern "C" TUint64 rdrand();

static TUint32 sRandomSeed;

static __uint128_t g_lehmer64_state;

TUint64 split_mix64_x;

static inline uint64_t splitmix64_stateless(uint64_t index) {
  uint64_t z = (index + UINT64_C(0x9E3779B97F4A7C15));
  z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
  z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
  return z ^ (z >> 31);
}

static inline void lehmer64_seed(uint64_t seed) {
  g_lehmer64_state = (((__uint128_t)splitmix64_stateless(seed)) << 64) +
                     splitmix64_stateless(seed + 1);
}

static inline uint64_t lehmer64() {
  g_lehmer64_state *= UINT64_C(0xda942042e4dd58b5);
  return g_lehmer64_state >> 64;
}

TUint32 GetRandomSeed() {
  return sRandomSeed;
}

void SeedRandom(TUint32 aSeed) {
  sRandomSeed = aSeed;
}

void SeedRandom64(TUint64 aSeed) {
  dlog("Set Seed %d\n", aSeed);
  lehmer64_seed(aSeed);
}

TUint32 Random() {
#if 0
  return rdrand();
#else
  static const TUint32 a = 16807,
                       m = 2147483647;
  DISABLE;
  sRandomSeed = (a * sRandomSeed) % m;
  ENABLE;
  return sRandomSeed % m;
#endif
}

TUint64 Random64() {
  return lehmer64();
}

TInt32 Random(TInt32 aMin, TInt32 aMax) {
  return TInt32(Random()) % (aMax - aMin) + aMin;
}

TInt64 Random64(TInt64 aMin, TInt64 aMax) {
  TUint64 r = Random64();
//  dlog("r64 %d %d %d\n", r, TUint64(aMax - aMin), r % TUint64(aMax - aMin));
  return r % TUint64(aMax - aMin) + aMin;
}

TFloat RandomFloat() {
  TInt32 r = TInt32(Random());
  TFloat ret = TFloat(r) / TFloat(UINT32_MAX);
  return ret;
}
