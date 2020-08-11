#ifndef KERNEL_GDT_H
#define KERNEL_GDT_H

#include <Types.hpp>

class TSS;

struct gdtp {
  TUint16 len;
  TUint64 *gdt;
} PACKED;

class GDT {
public:
  GDT(TSS *aTSS);
  // GDT();
  ~GDT();

  void Install();

public:
  void set_gate(TInt id, TAny *start_addr, TUint32 size, TUint32 priv_level);
  void set_gate(TInt num, TUint64 base, TUint64 limit, TUint8 access, TUint8 granularity);
  void tss_install();
protected:
  TUint64 mGdt[8] ALIGN16;
  struct gdtp mGdtp ALIGN16;
  TSS *mTss;
};

#endif

