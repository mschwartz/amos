#ifndef AMOS_KERNEL_EXEC_IDLETASK_HH
#define AMOS_KERNEL_EXEC_IDLETASK_HH

#include <Exec/BTask.hpp>

class ExecBase;

class IdleTask : public BTask {
  // friend ExecBase;

public:
  IdleTask() : BTask("Idle Task", LIST_PRI_MIN) {}
  ~IdleTask() {}

public:
  TInt64 Run() ;
};

#endif

