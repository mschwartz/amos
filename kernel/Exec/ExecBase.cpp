#include <Exec/ExecBase.h>
#include <x86/cpu.h>
#include <x86/bochs.h>

class IdleTask : public BTask {
public:
  IdleTask() : BTask(INT64_MAX, "IDLE TASK"){};

public:
  void Run() {
    while (ETrue)
      ;
  }
};

ExecBase *gExecBase = ENull;

ExecBase &ExecBase::GetExecBase() {
  dprint("GetExecBase %x\n", gExecBase);
  if (!gExecBase) {
    gExecBase = new ExecBase();
  }
  return *gExecBase;
}

void ExecBase::Disable() {
  cli();
  if (mDisableNestCount++ > 0) {
    sti();
  }
}

void ExecBase::Enable() {
  cli();
  if (--mDisableNestCount == 0) {
    // nest count > 0 means we should be disabled;
    sti();
    return;
  }
  else if (mDisableNestCount < 0) {
    mDisableNestCount = 0;
  }
}

ExecBase::ExecBase() : BBase() {
  //  Init();
  //  mExecBase = &gExecBase;
  dprint("Construct ExecBase\n");
  bochs;
  mTasksReady = new BTaskList();
  dprint("Construct TaskReady\n");
  mTasksWait = new BTaskList();
  mDeviceList = new BDeviceList();
  dprint("Constructed Device List\n");
}

ExecBase::~ExecBase() {
  //
}

void ExecBase::AddTask(BTask *aTask) {
  Disable();
  mTasksReady->Add(*aTask);
  Enable();
}

void ExecBase::DumpCurrentTask() {
  gExecBase->mCurrentTask->Dump();
}

void ExecBase::DumpCurrentTaskRegisters() {
  BTask::DumpRegisters(current_task);
}

void ExecBase::Init() {
  dprint("ExecBase::Init\n");
  bochs;
  IdleTask *task = new IdleTask();
  dprint("== IdleTask %x\n", task);
  Disable(); // this operation needs to be atomic
  dprint("== AddTask %x\n", task);
  bochs;
  mTasksReady->Add(*task);
  SetCurrentTask(task);
  Enable();
}

void ExecBase::SetCurrentTask(BTask *aTask) {
  Disable(); // this operation needs to be atomic
  mCurrentTask = aTask;
  current_task = &mCurrentTask->mTaskX64;
  Enable();
}

BDevice *ExecBase::FindDevice(const char *aName) {
  return ENull;
}

void ExecBase::AddDevice(BDevice *aDevice) {
  Disable(); // this operation needs to be atomic
  mDeviceList->Add(*aDevice);
  Enable();
}
