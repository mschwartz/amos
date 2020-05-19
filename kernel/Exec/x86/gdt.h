#ifndef KERNEL_GDT_H
#define KERNEL_GDT_H

#include <Exec/Types.h>

enum GdtEntries {
  GdtNull,
  GdtKCode,
  GdtKData,
  GdtUCode,
  GdtUData,
#if 0
  GdtTssLow,
  GdtTssHigh,
#endif
  GdtSize,
};

const TInt GDT_NUM_ENTRIES = 6;

#if 0
typedef struct {
  TUint32 addr;
  TUint32 flags;

  void Dump(TInt index = -1) {
    if (index >= 0) {
      dlog("GDT %2d %08x %08x\n", index, addr, flags);
      dlog("type: %x\n", (flags >> 8) & 0x0f);
    }
    else {
      dlog("GDT %x %08x %08x\n", this, addr, flags);
    }
  }
} PACKED TGdt;
#else
typedef TUint64 TGdt;
#endif

class GDT {
public:
  GDT();
  ~GDT();

public:
  void SetKernelStack(TUint64 aStackPointer);
//public:
//  void set_gate(TInt aIndex, TAny *aBase, TUint32 aLimit, TUint8 aAccess, TUint8 aGranularity);

protected:
  TGdt mGdt[GdtSize] ALIGN16;
};

#endif
