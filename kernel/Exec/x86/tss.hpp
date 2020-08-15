#ifndef EXEC_X86_TSS_H
#define EXEC_X86_TSS_H

#include <Types.hpp>
#include <Exec/BBase.hpp>

#define KSTACK_SIZE (2 * 1024 * 1024)

// This is the CPU's required format for TSS
struct tss {
  TUint32 r1;
  TUint64 rsp0;
  TUint64 rsp1;
  TUint64 rsp2;
  TUint64 r2;
  TUint64 ist1;
  TUint64 ist2;
  TUint64 ist3;
  TUint64 ist4;
  TUint64 ist5;
  TUint64 ist6;
  TUint64 ist7;
  TUint64 r3;
  TUint16 r4;
  TUint16 io_mba;
  void Dump() {
    dprint("TSS AT %016x\n", this);
    dprint("  r1: %016x\n", r1);
    dprint("rsp0: %016x\n", rsp0);
    dprint("rsp1: %016x\n", rsp1);
    dprint("rsp2: %016x\n", rsp2);
    dprint("  r2: %016x\n", r2);
    dprint("ist1: %016x\n", ist1);
    dprint("ist2: %016x\n", ist2);
    dprint("ist3: %016x\n", ist3);
    dprint("ist4: %016x\n", ist4);
    dprint("ist5: %016x\n", ist5);
    dprint("ist6: %016x\n", ist6);
    dprint("ist7: %016x\n", ist7);
    dprint("  r3: %016x\n", r3);
    dprint("  r4: %016x\n", r4);
    dprint("  io: %16x %d\n", io_mba, io_mba);
    dprint("\n");
  }
} PACKED;

class TSS : public BBase {
 public:
  TSS();
  ~TSS();
 public:
  void Dump();
 public:
  struct tss mTss ALIGN16;
  TUint8 rStack0[KSTACK_SIZE] ALIGN16;
  TUint8 rStack1[KSTACK_SIZE] ALIGN16;
  TUint8 rStack2[KSTACK_SIZE] ALIGN16;
  TUint8 iStack1[KSTACK_SIZE] ALIGN16;
  TUint8 iStack2[KSTACK_SIZE] ALIGN16;
  TUint8 iStack3[KSTACK_SIZE] ALIGN16;
  TUint8 iStack4[KSTACK_SIZE] ALIGN16;
  TUint8 iStack5[KSTACK_SIZE] ALIGN16;
  TUint8 iStack6[KSTACK_SIZE] ALIGN16;
  TUint8 iStack7[KSTACK_SIZE] ALIGN16;
};

#endif
