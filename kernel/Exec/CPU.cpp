#include <Exec/CPU.hpp>
#include <Exec/ExecBase.hpp>
#include <Exec/IdleTask.hpp>
#include <Exec/InitTask.hpp>

extern "C" TUint64 cpuid(TUint32 *eax, TUint32 *ebx, TUint32 *ecx, TUint32 *edx);
extern "C" void cpu_brand(char *buf);
extern "C" void enter_tasking();

void SetMSR(TUint32 msr, TUint32 lo, TUint32 hi) {
  asm volatile("wrmsr"
               :
               : "a"(lo), "d"(hi), "c"(msr));
}

void CPU::ColdStart() {
  static TGS sGS;
  sGS.mCurrentGs = (TUint64)&sGS;
  sGS.mCurrentTask = ENull;
  sGS.mCurrentCpu = ENull;
  SetGS(&sGS);
  // write_msr(USER_GS_BASE, (TUint64)(&sGS));
  // write_msr(KERNEL_GS_BASE, (TUint64)(&sGS));
  // swapgs();
}

void CPU::InterruptOthers(TUint8 aVector) {
  mApic->InterruptOthers(aVector);
}

void CPU::AckIRQ(TUint16 aIRQ) {
  mApic->EOI(aIRQ);
}

CPU::CPU(TUint32 aProcessorId, TUint32 aApicId, ACPI *aAcpi) {
  mActiveTasks.mMutex.SetName("CPU Active List");
  mBootStack = (TUint8 *)AllocMem(BOOT_STACK_SIZE);
  dlog("CONSTRUCT CPU %d\n", aProcessorId);
  mGS.mCurrentCpu = this;
  mProcessorId = aProcessorId;
  mApicId = aApicId;
  mAcpi = aAcpi;
  // mIoApic = aIoApic;
  mApic = new Apic(mAcpi->ApicAddress(), mApicId);

  mTss = new TSS();
  dlog("CPU %d initialized TSS\n", mProcessorId);
  mGdt = new GDT(mTss);
  dlog("CPU %d initialized GDT\n", mProcessorId);
  mIdt = new IDT();
  dlog("CPU %d initialized IDT\n", mProcessorId);

  if (mProcessorId == 0) {
    TUint64 v = (TUint64)&mGS;
    // SetMSR(0xc0000101, v>>32, v&0xffffffff);
    mGdt->Install();
    mIdt->Install();
    mGS.mCurrentCpu = this;
    // write_msr(KERNEL_GS_BASE, (TUint64)(&this->mGS));
    // swapgs();
    // write_msr(KERNEL_GS_BASE, (TUint64)(&this->mGS));
    // swapgs();
    write_msr(USER_GS_BASE, (TUint64)(&this->mGS));
    // swapgs();
    // write_msr(USER_GS_BASE, (TUint64)(&this->mGS));
    // swapgs();
    SetCPU(this);
    dlog("SetGS(%x)\n", &this->mGS);
  }

  // TODO move this code into a routine that is running in the CPU
  TUint32 eax, ebx, edx, ecx;
  eax = 0;
  cpuid(&eax, &ebx, &ecx, &edx);
  CopyMemory(mManufacturer, (char *)&ebx, 12);
  mManufacturer[12] = '\0';

  mMaxFunction = eax;

  eax = 1;
  cpuid(&eax, &ebx, &ecx, &edx);
  mProcessorVersionInformation = eax;
  mProcessorAdditionalInformation = ebx;
  mProcessorFeatureInformation1 = edx; // bit 0 is FPU
  mProcessorFeatureInformation2 = ecx; // bit 0 is SSE3

  eax = 7;
  cpuid(&eax, &ebx, &ecx, &edx);
  mFeatures1 = ebx;
  mFeatures2 = ecx;
  mFeatures3 = edx;

  eax = 0x80000000;
  cpuid(&eax, &ebx, &ecx, &edx);
  mMaxExtendedFunction = eax;

  eax = 0x80000001;
  cpuid(&eax, &ebx, &ecx, &edx);
  mExtendedFeatures1 = edx;
  mExtendedFeatures2 = ecx;

  cpu_brand(&mBrand[0]);

  if (mMaxExtendedFunction <= 0x80000008) {
    eax = 0x80000008;
    cpuid(&eax, &ebx, &ecx, &edx);
    mPhysicalAddressBits = eax & 0xff;
    mLinearAddressBits = (eax >> 8) & 0xff;
    mCores = (ecx & 0xff) + 1;
    mApicIdBits = (ecx >> 12) & 0x0f;
  }
  else {
    mPhysicalAddressBits = 0;
    mLinearAddressBits = 0;
    mCores = 0;
    mApicIdBits = 0;
  }

  Dump();
}

void CPU::GuruMeditation(const char *aFormat, ...) {
  cli();
  bochs;
  char buf[512];
  dprint("\n\n***********************\n");
  dprint("GURU MEDITATION at %dms in CPU %d\n", gExecBase.SystemTicks(), mProcessorId);

  va_list args;
  va_start(args, aFormat);
  vsprintf(buf, aFormat, args);
  dprint(buf);
  dprint("\n");

  mCurrentTask->Dump();
  va_end(args);
  dprint("***********************\n\n\nHalted.\n");

  for (;;) {
    halt();
  }
}

// This function must run in the CPU!
void CPU::EnterAP() {
  cli();
  mGS.mCurrentCpu = this;
  SetGS(&mGS);
  SetCPU(this);
  mApic->Initialize();
  cli();

  dprint("\n\n");
  dlog("EnterAP %d gs(%x) mGS(%x) CPU(%x %x)\n", mProcessorId, GetGS(), &mGS, this, GetCPU());

  // each CPU has its own IdleTask
  mIdleTask = new IdleTask();
  mIdleTask->mCpu = this;
  LockActiveList();
  mActiveTasks.Add(*mIdleTask);
  UnlockActiveList();

  // BSP needs to run InitTask to initialize things
  if (mProcessorId == 0) { // no need to start the boot processsor
    InitTask *task = new InitTask();
    task->mCpu = this;
    LockActiveList();
    mActiveTasks.Add(*task);
    UnlockActiveList();
  }
  else {
    mGdt->Install();
    mIdt->Install();
  }
  // LockActiveList();
  mCurrentTask = mActiveTasks.First();
  // UnlockActiveList();
  SetCurrentTask(&mCurrentTask->mRegisters);

  // sti();
  enter_tasking(); // just enter next task
  dlog("BAD\n");
  bochs;
}

void CPU::StartAP(BTask *aTask) {
  dlog("CPU %d StartAP\n", mProcessorId);

  Apic *apic = mApic;
  // aTask->Sleep(2);

  dlog("SendIPI\n");
  if (!apic->SendIPI(mProcessorId, 0x8000)) {
    dlog("*** COULD NOT IPI (%d)\n", mProcessorId);
    mCpuState = ECpuUnusable;
    return;
  }
  dlog("sleep 10ms\n");
  // aTask->MilliSleep(10);

  dlog("SendSIPI\n");
  if (!apic->SendSIPI(mProcessorId, 0x8000)) {
    dlog("*** COULD NOT SIPI (%d)\n", mProcessorId);
    mCpuState = ECpuUnusable;
    return;
  }

  dlog("Wait for CPU to be running\n");
  TInt64 timeout = 10; // 100000000000;
  while (mCpuState != ECpuRunning && timeout--) {
    dlog("Wait %d\n", timeout);
    aTask->MilliSleep(1);
    // aTask->MilliSleep(5);
  }
  dlog("Not running yet\n");

  if (mCpuState != ECpuRunning) {
    // do another SIPI
    dlog("Not running, SIPI again\n");
    if (!apic->SendSIPI(mProcessorId, 0x8000)) {
      dlog("*** COULD NOT SIPI (%d)\n", mProcessorId);
      mCpuState = ECpuUnusable;
      return;
    }
  }

  timeout = 100;
  while (mCpuState != ECpuRunning && timeout--) {
    aTask->MilliSleep(10);
  }
  if (mCpuState != ECpuRunning) {
    dlog("*** COULD NOT START %d\n", mProcessorId);
  }
  else {
    dlog("--> CPU %d running\n", mProcessorId);
  }
}

TInt64 CPU::RemoveTask(BTask *aTask, TInt64 aExitCode) {
  DISABLE;

  LockActiveList();
  aTask->Remove();
  UnlockActiveList("remove task");

  TBool isCurrentTask = aTask == mCurrentTask;
  if (isCurrentTask) {
    dlog("CPU %d RemoveTask(%s) code(%d) CURRENT TASK\n", mProcessorId, aTask->TaskName(), aExitCode);
  }
  else {
    dlog("CPU %d RemoveTask(%s) code(%d)\n", mProcessorId, aTask->TaskName(), aExitCode);
  }

  if (isCurrentTask) {
    mCurrentTask = mActiveTasks.First();
    SetCurrentTask(&mCurrentTask->mRegisters);
    enter_tasking(); // just enter next task
  }

  ENABLE;
  return aExitCode;
}

void CPU::DumpTasks() {
  LockActiveList();
  dlog("\n\nActive Tasks\n");
  mActiveTasks.Dump();
  // dlog("Waiting Tasks\n");
  // mWaitingTasks.Dump();
  dlog("\n\n");
  UnlockActiveList();
}

void CPU::WaitSignal(BTask *aTask) {
  DISABLE;

  if (aTask == mIdleTask) {
    dlog("idle task cannot wait()\n");
    bochs;
  }

  // dlog("WaitSignal(%s) %x\n", aTask->TaskName(), this);
  // bochs;
  LockActiveList("CPU add task");
  // dlog("locked\n");
  aTask->Remove(); // remove from this CPU's active list
  // dlog("removed\n");
  UnlockActiveList("CPU add task");
  // dlog("unlocked\n");

  BTask *next_task = gExecBase.RescheduleTask(aTask);

  if (next_task != ENull) {
    next_task->mCpu = this;
    // dlog("%d WaitSignal add task(%s) %d\n", mProcessorId, next_task->TaskName(), mProcessorId);
    LockActiveList();
    mActiveTasks.Add(*next_task);
    UnlockActiveList();
  }

  gExecBase.Schedule();
  dlog("WaitSignal(%s) returns\n", mCurrentTask->TaskName());
  // bochs;
  ENABLE;
}

/**
 * RescheduleIRQ()
 *
 * This is to be called from the Timer Interrupt handler to possibly preempt the current running task
 * and switch to the next task that should run.
 *
 * ExecBase has a master running tasks list that is shared among all the CPUs.  Thus when it is time to 
 * switch to another task, the head of that list is the task that should run.
 *
 * The effect of this scheme is that a task can start in CPU0, get switched from, and resume in any of the
 * CPUs.  This is possibly going to cause CPU cache misses if the task is moved to a different physical CPU 
 * (like on a motherboard with more than one CPU socket).
 */
void CPU::RescheduleIRQ() {
  BTask *t = mCurrentTask,
        *next_task;

  if (t) {
    if (t == mIdleTask) {
      next_task = gExecBase.RescheduleTask(ENull);
    }
    else {
      LockActiveList();
      t->Remove();
      UnlockActiveList();
      next_task = gExecBase.RescheduleTask(t);
    }
    if (next_task == ENull) {
      next_task = mIdleTask;
    }
    next_task->mCpu = this;
    if (next_task != mIdleTask) {
      LockActiveList();
      mActiveTasks.Add(*next_task);
      UnlockActiveList();
    }
  }

  mCurrentTask = mActiveTasks.First();
  SetCurrentTask(&mCurrentTask->mRegisters);

  if (t != mCurrentTask && gExecBase.mDebugSwitch) {
    // if (t != mCurrentTask) {
    dprint("  CPU %d Reschedule %s => %s\n", mProcessorId, mCurrentTask->TaskName(), t->TaskName());
  }
}
