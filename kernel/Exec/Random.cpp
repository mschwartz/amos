#include <Exec/ExecBase.h>
#include <Exec/Random.h>

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

extern "C" TUint64 rdrand();

TUint32 Random() {
  return rdrand();
#if 0
  static const TUint32 a = 16807,
          m = 2147483647;
  DISABLE;
  sRandomSeed = (a * sRandomSeed) % m;
  ENABLE;
  return sRandomSeed % m;
#endif
}

TInt32 Random(TInt32 aMin, TInt32 aMax) {
  return TInt32(Random()) % (aMax - aMin) + aMin;
}

TFloat RandomFloat() {
  TInt32 r = TInt32(Random());
  TFloat ret = TFloat(r) / TFloat(UINT32_MAX);
  return ret;
}
