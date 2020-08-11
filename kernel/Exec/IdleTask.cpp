#include "IdleTask.hpp"
#include "InitTask.hpp"
#include <Exec/ExecBase.hpp>

TInt64 IdleTask::Run() {
  CPU *cpu = (CPU *)mCpu;
  dprint("\n");
  dlog("IdleTask Run CPU(%d)\n", cpu->mProcessor);

  for (;;) {
    CPU *cpu = gExecBase.CurrentCpu();
    dlog("IdleTask Looping CPU(%d)\n", cpu->mProcessor);
    halt();
  }
}
