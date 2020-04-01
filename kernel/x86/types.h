#ifndef KERNEL_TYPES_H
#define KERNEL_TYPES_H

#include <BTypes.h>

#define PACKED __attribute__ ((packed))

#if 0
typedef unsigned char uint8_t;
typedef char int8_t;

typedef unsigned short uint16_t;
typedef short int16_t;

typedef unsigned int uint32_t;
typedef int int32_t;

typedef unsigned long  uint64_t;
typedef long  int64_t;


typedef long size_t;

typedef uint8_t UBYTE;
typedef uint8_t UCHAR;
typedef uint16_t USHORT;
typedef uint32_t UWORD;
typedef uint64_t ULONG;

typedef int8_t BYTE;
typedef int16_t SHORT;
typedef int32_t WORD;
typedef int64_t LONG;

#endif


/*!
 * Calculate a*b/c (all operands are 32 bit unsigned integers)
 * \param a
 * \param b
 * \param c
 * \returns (a*b)/c
 */
static inline TUint32 arch_mul_div_32 ( TUint32 a, TUint32 b, TUint32 c )
{
	TUint32 result, m1, m0, mod;

	asm ("mull %2":"=a" (m0), "=d" (m1):"rm" (b), "0" (a) );
	asm ("divl %2":"=a" (result), "=d" (mod):"rm" (c), "0" (m0), "1" (m1) );

	return result; /* could also return remainder in 'mod' if required! */
}
#endif
