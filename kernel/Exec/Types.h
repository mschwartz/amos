#ifndef BTYPES_H
#define BTYPES_H

#define PACKED __attribute__ ((packed))
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

//#include "Types/TRGB.h"
//#include "Types/TRect.h"
#include "Types/TBCD.h"
//#include "Types/TNumber.h"

#include <Exec/x86/bochs.h>

#endif // BTYPES_H
