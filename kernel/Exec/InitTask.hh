#ifndef AMOS_KERNEL_EXEC_INITTASK_HH
#define AMOS_KERNEL_EXEC_INITTASK_HH

#include <Exec/BTask.h>

class ExecBase;

class InitTask : public BTask {
  // friend ExecBase;

public:
  InitTask() : BTask("Init Task", TASK_PRI_DEFAULT) {}
  ~InitTask() {}

public:
  TInt64 Run() ;
};

#endif
