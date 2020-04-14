#include <Exec/ExecBase.h>
#include <x86/bochs.h>

#include <x86/gdt.h>
#include <x86/idt.h>
#include <x86/mmu.h>
#include <x86/cpu.h>
#include <Devices/PIC.h>
#include <Devices/Screen.h>
#include <Devices/Keyboard.h>
#include <Devices/Timer.h>

class IdleTask : public BTask {
public:
  IdleTask() : BTask(INT64_MAX, "IDLE TASK"){};

public:
  void Run() {
    while (ETrue)
      ;
  }
};


ExecBase &ExecBase::GetExecBase() {
  return gExecBase;
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
  mScreen = Screen::CreateScreen();

  static GDT gdt;
  mGdt = &gdt;

  static MMU mmu;
  mMmu = &mmu;

  static IDT _idt;
  mIdt = &_idt;

  static CPU _cpu;
  mCpu = &_cpu;

  static PIC pic;
  mPic = &pic;

  static Timer timer;
  mTimer = &timer;


  //  Init();
  //  mExecBase = &gExecBase;
  dprint("Construct ExecBase %d\n", sizeof(BTaskList));
  bochs;
  mDeviceList = new BDeviceList();
  dprint("Constructed Device List\n");

}

ExecBase::~ExecBase() {
  //
}

void ExecBase::AddTask(BTask *aTask) {
  Disable();
  mTasksReady.Add(*aTask);
  Enable();
}

void ExecBase::DumpCurrentTask() {
  gExecBase.mCurrentTask->Dump();
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
  mTasksReady.Add(*task);
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

ExecBase gExecBase;
