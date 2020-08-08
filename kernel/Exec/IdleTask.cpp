#include "IdleTask.hpp"
#include "InitTask.hpp"
#include <Exec/ExecBase.hpp>

TInt64 IdleTask::Run() {
  dprint("\n");
  dlog("IdleTask Run\n");

  for (;;) {
    dlog("IdleTask Looping\n");
    halt();
  }
}
