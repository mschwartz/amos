#include "IdleTask.hh"
#include "InitTask.hh"
#include <Exec/ExecBase.h>

TInt64 IdleTask::Run() {
  dprint("\n");
  dlog("IdleTask Run\n");

  dlog("Adding InitTask\n");
  gExecBase.AddTask(new InitTask());

  for (;;) {
    dlog("IdleTask Looping\n");
    halt();
  }
}
