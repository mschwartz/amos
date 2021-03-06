#ifndef BTYPES_H
#define BTYPES_H


//
// Before including Types.h, you can add #define DEBUGME to enable these macros
//
#ifdef DEBUGME
#define DLOG(...) dlog(__VA_ARGS__)
#define DPRINT(...) dprint(__VA_ARGS__)
#define DSPACE(...) dprint("\n\n")
#define DBANNER(...) dprint("\n\n================================================\n")
// ASSERT(value, fmt, ...)
#define ASSERT(...) dassert(__VA_ARGS__);
#else
#define DLOG(...) do {} while (0);
#define DPRINT(...) do {} while (0);
#define DSPACE(...) do {} while (0);
#define DBANNER(...) do {} while (0);
// ASSERT(value, fmt, ...)
#define ASSERT(...) do {} while (0);
#endif


#define PACKED __attribute__ ((packed))
#define ALIGN2 __attribute__ ((aligned (2)))
#define ALIGN4 __attribute__ ((aligned (4)))
#define ALIGN8 __attribute__ ((aligned (8)))
#define ALIGN16 __attribute__ ((aligned (16)))
#define ALIGN32 __attribute__ ((aligned (32)))
#define INLINE __attribute__((always_inline))

typedef unsigned char TUint8;
typedef unsigned short TUint16;
typedef unsigned int TUint32;
typedef unsigned int TUint;
typedef unsigned long TUint64;

typedef  char TInt8;
typedef  short TInt16;
typedef  int TInt32;
typedef  int TInt;
typedef  long TInt64;

typedef int TInt;
typedef void TAny;
typedef bool TBool;

typedef float TFloat;

#define ETrue (true)
#define EFalse (false)

#ifndef NULL
#define NULL nullptr
#endif

#define ENull nullptr

#ifndef UINT32_MAX
#define UINT32_MAX      ((TUint32)(4294967295U))
#endif

#ifndef KILOBYTE
const TUint64 KILOBYTE = 1024;
#endif
#ifndef MEGABYTE
const TUint64 MEGABYTE = KILOBYTE * 1024;
#endif
#ifndef GIGABYTE
const TUint64 GIGABYTE = MEGABYTE * 1024;
#endif
#ifndef TERRABYTE
const TUint64 TERRABYTE = GIGABYTE * 1024;
#endif

#ifndef LOBYTEM
#define LOBYTE(x) TUint8((x)&0xff)
#endif

#ifndef HIBYTE
#define HIBYTE(x) TUint8(((x) >> 8) & TUint8(0xff))
#endif

#ifndef LOWORD
#define LOWORD(x) TUint16((x)&TUint16(0xffff))
#endif

#ifndef HIWORD
#define HIWORD(x) TUint16(((x) >> 16) & TUint16(0xffff))
#endif

#ifndef ABS
#define ABS(x) ((x) < 0 ? -(x) : (x))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? a : b)
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? a : b)
#endif

#ifndef MID
#define MID(a, b) (((a) + (b)) / 2)
#endif

#ifndef SQRT
#define SQRT(f) (sqrt(f))
#endif

#ifndef ACOS
#define ACOS(f) (acos(f))
#endif

#ifndef ASIN
#define ASIN(f) (asin(f))
#endif

#ifndef ATAN
#define ATAN(f) (atan(f))
#endif

#ifndef ATAN2
#define ATAN2(f1, f2) (atan2(f1, f2))
#endif

#ifndef CEIL
#define CEIL(f) (ceil(f))
#endif

#ifndef COS
#define COS(f) (cos(f))
#endif

#ifndef COSH
#define COSH(f) (cosh(f))
#endif

#ifndef SIN
#define SIN(f) (sin(f))
#endif

#ifndef SINH
#define SINH(f) (sinh(f))
#endif

#ifndef TAN
#define TAN(f) (tan(f))
#endif

#ifndef TANH
#define TANH(f) (tanh(f))
#endif

#ifndef EXP
#define EXP(f) (exp(f))
#endif

#ifndef FREXP
#define FREXP(f, result) (frexp(f, result))
#endif

#ifndef FLOOR
#define FLOOR(f) (floor(f))
#endif

#ifndef POW
#define POW(f, exponent) (pow(f, exponent))
#endif

#ifndef LOG
#define LOG(f) (log(f))
#endif

#ifndef LOG10
#define LOG10(f) (log10(f))
#endif

#define MAXINT (0x7fffffff)

#define ROUND_UP_4(x) (((x) + 3) & -4)

// assure min_value <= value <= max_value
#define CLAMP(value, min_value, max_value) (MIN(MAX(value, min_value), max_value))

#ifndef OVERRIDE
#define OVERRIDE override
#endif
#ifndef EXPLICIT
#define EXPLICIT explicit
#endif
#ifndef FINAL
#define FINAL final
#endif

//#ifdef KERNEL
//#include <Exec/x86/bochs.h>
//#endif

//#include "Types/TRGB.h"
//#include "Types/TRect.h"
// #include "Types/TBCD.h"
//#include "Types/TNumber.h"

#ifdef KERNEL
#include <Exec/x86/cpu_utils.hpp>
#define bochs asm volatile("xchg %bx, %bx;");

extern TUint8 in_bochs;

//stops simulation and breaks into the debug console
inline void dbreak() {
    /* outw(0x8A00, 0x8A00); */
    /* outw(0x8A00, 0x08AE0); */
 // /* outw(0x8A00, 0x8a00); */
 // /* outw(0x08AE0, 0x8a00); */
}

extern void dputc(char c);
extern void dlog(const char *fmt, ...);
extern void dputs(const char *s);
extern void dassert(TBool aValue, const char *fmt, ...);
extern void dprintf(const char *fmt, ...);
extern void dprint(const char *fmt, ...);

extern void dhex8(const TUint8 b);
extern void dhex16(const TUint16 w);
extern void dhex32(const TUint32 w);
extern void dhex64(const TUint64 w);

extern void dhexdump(const TAny *aSource, int aLines);
extern void dhexdump16(const TAny *aSource, int aLines);
extern void dhexdump32(const TAny *aSource, int aLines);
extern void dhexdump64(const TAny *aSource, int aLines);
#endif

#endif // BTYPES_H
