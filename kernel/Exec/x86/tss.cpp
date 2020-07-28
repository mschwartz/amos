#include <Exec/x86/tss.hpp>

TSS::TSS() {
  mTss.io_mba = sizeof(struct tss);
  mTss.rsp0 = (TUint64)&rStack0[KSTACK_SIZE];
  mTss.rsp1 = (TUint64)&rStack1[KSTACK_SIZE];
  mTss.rsp2 = (TUint64)&rStack2[KSTACK_SIZE];
  mTss.ist1 = (TUint64)&iStack1[KSTACK_SIZE];
  mTss.ist2 = (TUint64)&iStack2[KSTACK_SIZE];
  mTss.ist3 = (TUint64)&iStack3[KSTACK_SIZE];
  mTss.ist4 = (TUint64)&iStack4[KSTACK_SIZE];
  mTss.ist5 = (TUint64)&iStack5[KSTACK_SIZE];
  mTss.ist6 = (TUint64)&iStack6[KSTACK_SIZE];
  mTss.ist7 = (TUint64)&iStack7[KSTACK_SIZE];
  // Dump();
}

TSS::~TSS() {
  // should never happen!
  bochs;
}

void TSS::Dump() {
  mTss.Dump();
}
