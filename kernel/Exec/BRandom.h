#ifndef EXEC_BRANDOM_H
#define EXEC_BRANDOM_H

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

#endif
