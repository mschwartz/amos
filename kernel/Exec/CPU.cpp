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

CPU::CPU(TUint32 aProcessor, TUint32 aProcessorId, TUint32 aApicId, IoApic *aIoApic) {
  dlog("CONSTRUCT CPU %d\n", aProcessor);
  mGS.mCurrentCpu = this;
  mProcessor = aProcessor;
  mProcessorId = aProcessorId;
  mApicId = aApicId;
  mIoApic = aIoApic;
  mApic = new Apic(aIoApic->Address(), mApicId);

  mTss = new TSS();
  dlog("CPU %d initialized TSS\n", mProcessor);
  mGdt = new GDT(mTss);
  dlog("CPU %d initialized GDT\n", mProcessor);
  mIdt = new IDT();
  dlog("CPU %d initialized IDT\n", mProcessor);

  if (mProcessor == 0) {
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

// This function must run in the CPU!
void CPU::EnterAP() {
  sti();
  mGS.mCurrentCpu = this;
  SetGS(&mGS);
  // write_msr(KERNEL_GS_BASE, (TUint64)(&this->mGS));
  // swapgs();
  // write_msr(KERNEL_GS_BASE, (TUint64)(&this->mGS));
  // swapgs();
  // write_msr(USER_GS_BASE, (TUint64)(&this->mGS));
  // swapgs();
  // write_msr(USER_GS_BASE, (TUint64)(&this->mGS));
  // swapgs();
  SetCPU(this);

  dlog("EnterAP %d gs(%x) mGS(%x) CPU(%x %x)\n", mProcessorId, GetGS(), &mGS, this, GetCPU());

  if (mProcessorId == 0) { // no need to start the boot processsor
    // Before enabling interrupts, we need to have the idle task set up
    // mCurrentTask = mActiveTasks.First();
    // SetCurrentTask(&mCurrentTask->mRegisters);
    InitTask *task = new InitTask();
    task->mCpu = this;
    mActiveTasks.Add(*task);
    mCurrentTask = mActiveTasks.First();
    SetCurrentTask(&mCurrentTask->mRegisters);
  }
  else {
    mGdt->Install();
    mIdt->Install();
    IdleTask *task = new IdleTask();
    task->mCpu = this;
    mActiveTasks.Add(*task);
    mCurrentTask = mActiveTasks.First();
    SetCurrentTask(&mCurrentTask->mRegisters);
  }

  enter_tasking(); // just enter next task
  dlog("BAD\n");
  bochs;
  // IdleTask *task = new IdleTask();
  // task->mCpu = this;
  // mActiveTasks.Add(*task);
  // mCurrentTask = mActiveTasks.First();
  // SetCurrentTask(&mCurrentTask->mRegisters);
  // bochs;
  // enter_tasking(); // just enter next task
}

void CPU::StartAP(BTask *aTask) {
  dlog("CPU %d StartAP\n", mProcessor);

  Apic *apic = mApic;
  aTask->Sleep(2);

  if (!apic->SendIPI(mProcessor, 0x8000)) {
    dlog("*** COULD NOT IPI (%d)\n", mProcessor);
    mCpuState = ECpuUnusable;
    return;
  }
  aTask->MilliSleep(10);

  if (!apic->SendSIPI(mProcessor, 0x8000)) {
    dlog("*** COULD NOT SIPI (%d)\n", mProcessor);
    mCpuState = ECpuUnusable;
    return;
  }

  TInt timeout = 10;
  while (mCpuState != ECpuRunning && timeout--) {
    aTask->MilliSleep(5);
  }

  if (mCpuState != ECpuRunning) {
    // do another SIPI
    if (!apic->SendSIPI(mProcessor, 0x8000)) {
      dlog("*** COULD NOT SIPI (%d)\n", mProcessor);
      mCpuState = ECpuUnusable;
      return;
    }
  }

  timeout = 10;
  while (mCpuState != ECpuRunning && timeout--) {
    aTask->MilliSleep(5);
  }
  if (mCpuState != ECpuRunning) {
    dlog("*** COULD NOT START %d\n", mProcessor);
  }
  else {
    dlog("--> CPU %d running\n", mProcessor);
  }
}

void CPU::AddTask(BTask *aTask) {
  DISABLE;
  aTask->mRegisters.tss = (TUint64)mTss;
  aTask->mCpu = this;
  mActiveTasks.Add(*aTask);
  dlog("    CPU(%d) Add Task %016x --- %s --- rip=%016x rsp=%016x\n",
    mProcessor, aTask, aTask->mNodeName, aTask->mRegisters.rip, aTask->mRegisters.rsp);
  ENABLE;
}

TInt64 CPU::RemoveTask(BTask *aTask, TInt64 aExitCode) {
  DISABLE;
  aTask->Remove();
  TBool isCurrentTask = aTask == mCurrentTask;
  if (isCurrentTask) {
    dlog("CPU %d RemoveTask(%s) code(%d) CURRENT TASK\n", mProcessor, aTask->TaskName(), aExitCode);
  }
  else {
    dlog("CPU %d RemoveTask(%s) code(%d)\n", mProcessor, aTask->TaskName(), aExitCode);
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
  dlog("\n\nActive Tasks\n");
  mActiveTasks.Dump();
  // dlog("Waiting Tasks\n");
  // mWaitingTasks.Dump();
  dlog("\n\n");
}

void CPU::RescheduleIRQ() {
  BTask *t = mCurrentTask;

  if (mCurrentTask && mCurrentTask->mTaskState != ETaskBlocked) {
    if (mCurrentTask->mForbidNestCount == 0) {
      mCurrentTask->Remove();
      if (mCurrentTask->mTaskState == ETaskWaiting) {
        gExecBase.AddWaitingList(*t);
        // mWaitingTasks.Add(*mCurrentTask);
      }
      else {
        mActiveTasks.Add(*mCurrentTask);
      }
    }
    else {
      dlog("FORBID\n");
    }
  }

  mCurrentTask = mActiveTasks.First();
  SetCurrentTask(&mCurrentTask->mRegisters);
  if (t != mCurrentTask && gExecBase.mDebugSwitch) {
    dprint("  CPU %d Reschedule %s\n", mProcessor, mCurrentTask->TaskName());
    dprint("Previous task\n");
    dprint("  Previous Task %s\n", t->TaskName());
    dprint("\n\n\n");
  }
}
