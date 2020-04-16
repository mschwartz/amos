#include <Exec/ExecBase.h>
#include <x86/bochs.h>
#include <stdint.h>

#include <Devices/Keyboard.h>
#include <Devices/Timer.h>
ExecBase gExecBase;

class IdleTask : public BTask {
public:
  IdleTask() : BTask("Idle Task", INT64_MAX) {}

public:
  void Run() {
    //    sti();
    dprint("IdleTask Running\n");
    while (1) {
      halt();
    }
  }
};

ExecBase::ExecBase() {
  in_bochs = *((TUint8 *)0x7c10);
  dprint("bochs %x\n", in_bochs);

  dprint("ExecBase constructor called\n");

  //  sti();  // since nest count is 0, we MUST have interrupts enabled

  //  Screen s;
  mScreen = new Screen;
  dprint("  initialized screen\n");

  mGDT = new GDT;
  dprint("  initialized GDT\n");

  // set up paging
  mMMU = new MMU;
  dprint("  initialized MMU\n");

  mIDT = new IDT;
  dprint("  initialized IDT\n");

  mCPU = new CPU;
  dprint("  initialized CPU vectors\n");

  // Before enabling interrupts, we need to have the idle task set up
  IdleTask *task = new IdleTask();
  mActiveTasks.Add(*task);
  mCurrentTask = mActiveTasks.First();
  current_task = &mCurrentTask->mTaskX64;

  // set up 8259 PIC
  gPIC = new PIC;
  dprint("  initialized 8259 PIC\n");
  mDisableNestCount = 0;
  sti();

  Timer t;
  gTimer = &t;
  kprint("initialized timer\n");

  Keyboard k;
  gKeyboard = &k;
  kprint("initialized keyboard\n");
}

ExecBase::~ExecBase() {
  dprint("ExecBase destructor called\n");
}

void ExecBase::Disable() {
  if (mDisableNestCount++ == 0) {
    cli();
  }
}

void ExecBase::Enable() {
  if (--mDisableNestCount <= 0) {
    mDisableNestCount = 0;
    sti();
  }
}

void ExecBase::AddInterruptHandler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription) {
  mIDT->install_handler(aIndex, aHandler, aData, aDescription);
}

void ExecBase::putc(char c) {
  mScreen->putc(c);
}

void ExecBase::puts(const char *s) {
  mScreen->puts(s);
}

void ExecBase::newline() {
  mScreen->newline();
}

void ExecBase::Hello() {
  dprint("Hello ExecBase\n");
}

void ExecBase::AddTask(BTask *aTask) {
  Disable();
  mActiveTasks.Add(*aTask);
  Enable();
}

void ExecBase::WaitSignal(BTask *aTask) {
  Disable();
  aTask->Remove();
  mWaitingTasks.Add(*aTask);
  Enable();
}

// assumes aTask is in Active list
void ExecBase::Wait(BTask *aTask) {
  Disable();
  // note that removing and adding the task will sort the task at the end of all tasks with the same priority.
  // this effects round-robin.
  aTask->Remove();
  mActiveTasks.Add(*aTask);
  Enable();
}

void ExecBase::Wake(BTask *aTask) {
  Disable();
  // note that removing and adding the task will sort the task at the end of all tasks with the same priority.
  // this effects round-robin.
  aTask->Remove();
  mActiveTasks.Add(*aTask);
  Enable();
}

extern "C" void resume_task();

void ExecBase::Reschedule() {
  if (mCurrentTask->mForbidNestCount == 0) {
    Wake(mCurrentTask);
    mCurrentTask = mActiveTasks.First();
    current_task = &mCurrentTask->mTaskX64;
  }
  resume_task();
}

void ExecBase::GuruMeditation() {
  dprint("GURU MEDIDTATION\n");
  mCurrentTask->Dump();
  while (1)
    ;
}

void ExecBase::AddDevice(BDevice *aDevice) {
  mDeviceList.Add(*aDevice);
}

BDevice *ExecBase::FindDevice(const char *aName) {
  return mDeviceList.FindDevice(aName);
}
