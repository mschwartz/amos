#include <Exec/ExecBase.h>
#include <x86/bochs.h>

ExecBase gExecBase;

class IdleTask : public BTask {
  public:
    IdleTask() : BTask("Idle Task") {}
  public:
    void Run() {
      while (1) {
      }
    }
};

ExecBase::ExecBase() {
  dprint("ExecBase constructor called\n");

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

  IdleTask *task = new IdleTask();
  mActiveTasks.Add(*task);
  task->Dump();
  // set up 8259 PIC
//  gPIC = new PIC;
//  dprint("  initialized 8259 PIC\n");
//  sti();

}

ExecBase::~ExecBase() {
  dprint("ExecBase destructor called\n");
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
  mActiveTasks.Add(*aTask);
}
