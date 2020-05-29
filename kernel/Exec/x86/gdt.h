#ifndef KERNEL_GDT_H
#define KERNEL_GDT_H

#include <Exec/Types.h>

class TSS;

class GDT {
public:
  // GDT(TSS *aTSS);
  GDT();
  ~GDT();

public:
  void set_gate(TInt id, TAny *start_addr, TUint32 size, TUint32 priv_level);
  void set_gate(TInt num, TUint64 base, TUint64 limit, TUint8 access, TUint8 granularity);
  void tss_install();
};

#endif

