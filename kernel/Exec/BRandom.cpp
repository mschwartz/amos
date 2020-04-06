#include <Exec/BRandom.h>

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
