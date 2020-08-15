#include "IdleTask.hpp"
#include "InitTask.hpp"
#include <Exec/ExecBase.hpp>

TInt64 IdleTask::Run() {
  CPU *cpu = (CPU *)mCpu;
  // TGS *gs = GetGS();
  // TInt n = mCpu->mProcessor;
  dprint("\n");
  dlog("IdleTask Run CPU(%d)\n", cpu->mProcessorId);

  // if (mCpu->mProcessor != 0) {
  //   bochs;
  // }
  for (;;) {
    CPU *cpu = (CPU *)mCpu;
    dlog("IdleTask Looping CPU(%d)\n", cpu->mProcessorId);
    halt();
  }
}
