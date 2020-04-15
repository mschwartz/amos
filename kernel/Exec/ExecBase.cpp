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
  static bool init = false;
  if (!init) {
    dprint("new ExecBase\n");
    gExecBase = new ExecBase;
    dprint("new ExecBase succeeded\n");
    init = true;
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
  dprint("ExecBase constructor called\n");
  mScreen = Screen::CreateScreen();
  dprint("Created screen\n");


  mGdt = new GDT;
  dprint("Created GDT\n");

  mMmu = new MMU;
  dprint("Created MMU\n");

  mIdt = new IDT;
  dprint("Created IDT\n");

  mCpu = new CPU;
  dprint("Created CPU\n");

  mPic = new PIC;
  dprint("Created PIC\n");

  mTimer = new Timer;
  dprint("Created Timer\n");


//  Init();
  //  Init();
  //  mExecBase = &gExecBase;
  dprint("Construct ExecBase %d\n", sizeof(BTaskList));
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

  ExecBase::GetExecBase().mCurrentTask->Dump();
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

void ExecBase::InstallIrqHandler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription) {
  IDT::install_handler(aIndex, aHandler, aData, aDescription);
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

ExecBase *gExecBase;
