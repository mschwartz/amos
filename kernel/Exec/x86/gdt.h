#ifndef KERNEL_GDT_H
#define KERNEL_GDT_H

#include <Exec/Types.h>

enum GdtEntries {
  GdtNull,
  GdtKCode,
  GdtKData,
  GdtUCode,
  GdtUData,
  GdtTss,
  GdtSize
};

const TInt GDT_NUM_ENTRIES = 6;

class GDT {
public:
  GDT();
  ~GDT();

//public:
//  void set_gate(TInt aIndex, TAny *aBase, TUint32 aLimit, TUint8 aAccess, TUint8 aGranularity);

protected:
  TUint64 mGdt[GdtSize] ALIGN16;
};

#endif
