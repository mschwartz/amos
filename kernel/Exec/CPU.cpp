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

void CPU::AckIRQ(TUint16 aIRQ) {
  mApic->EOI(aIRQ);
}

CPU::CPU(TUint32 aProcessorId, TUint32 aApicId, ACPI *aAcpi) {
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
  // bochs;
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

  mIdleTask = new IdleTask();
  mIdleTask->mCpu = this;
  mActiveTasks.Add(*mIdleTask);
  // Before enabling interrupts, we need to have the idle or init task set up
  if (mProcessorId == 0) { // no need to start the boot processsor
    InitTask *task = new InitTask();
    task->mCpu = this;
    mActiveTasks.Add(*task);
  }
  else {
    mGdt->Install();
    mIdt->Install();
  }
  mRunningTaskCount = 1;
  mCurrentTask = mActiveTasks.First();
  SetCurrentTask(&mCurrentTask->mRegisters);

  // sti();
  enter_tasking(); // just enter next task
  dlog("BAD\n");
  bochs;
}

void CPU::StartAP(BTask *aTask) {
  dlog("CPU %d StartAP\n", mProcessorId);

  Apic *apic = mApic;
  aTask->Sleep(2);

  if (!apic->SendIPI(mProcessorId, 0x8000)) {
    dlog("*** COULD NOT IPI (%d)\n", mProcessorId);
    mCpuState = ECpuUnusable;
    return;
  }
  aTask->MilliSleep(10);

  if (!apic->SendSIPI(mProcessorId, 0x8000)) {
    dlog("*** COULD NOT SIPI (%d)\n", mProcessorId);
    mCpuState = ECpuUnusable;
    return;
  }

  TInt timeout = 10;
  while (mCpuState != ECpuRunning && timeout--) {
    aTask->MilliSleep(5);
  }

  if (mCpuState != ECpuRunning) {
    // do another SIPI
    if (!apic->SendSIPI(mProcessorId, 0x8000)) {
      dlog("*** COULD NOT SIPI (%d)\n", mProcessorId);
      mCpuState = ECpuUnusable;
      return;
    }
  }

  timeout = 100;
  while (mCpuState != ECpuRunning && timeout--) {
    aTask->MilliSleep(1);
  }
  if (mCpuState != ECpuRunning) {
    dlog("*** COULD NOT START %d\n", mProcessorId);
  }
  else {
    dlog("--> CPU %d running\n", mProcessorId);
  }
}

void CPU::AddTask(BTask *aTask) {
  DISABLE;
  aTask->mRegisters.tss = (TUint64)mTss;
  aTask->mCpu = this;
  mActiveTasks.Add(*aTask);
  mRunningTaskCount++;
  dlog("    CPU(%d) Add Task %016x --- %s --- rip=%016x rsp=%016x\n",
    mProcessorId, aTask, aTask->mNodeName, aTask->mRegisters.rip, aTask->mRegisters.rsp);
  ENABLE;
}

void CPU::WaitSignal(BTask *aTask) {
  mMutex.Acquire();
  aTask->Remove();
  mRunningTaskCount--;
  // If task has received a signal it's waiting for, we don't want to move it to the WAIT list,
  // but it may be lower priority than another task so we need to sort it in to ACTIVE list.
  if (aTask->mSigWait & aTask->mSigReceived) {
    // TODO: assign task to a CPU
    mActiveTasks.Add(*aTask);
    aTask->mTaskState = ETaskRunning;
  }
  else {
    aTask->mTaskState = ETaskWaiting;
    gExecBase.AddWaitingTask(aTask);
    // mWaitingTasks.Add(*aTask);
  }
  mMutex.Release();
  gExecBase.Schedule();
}

TInt64 CPU::RemoveTask(BTask *aTask, TInt64 aExitCode) {
  mMutex.Acquire();
  aTask->Remove();
  mRunningTaskCount--;
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
    mMutex.Release();
    enter_tasking(); // just enter next task
  }

  mMutex.Release();
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

  if (mCurrentTask) {
    // if task is blocked, it is not on the system waiting list
    // it is potentially on a Sempahore's waiting list or some other waiting list
    if (mCurrentTask->mTaskState != ETaskBlocked) {
      // if Task has called Forbid(), we don't want to switch to another task
      if (mCurrentTask->mForbidNestCount == 0) {
        mCurrentTask->Remove();
        if (mCurrentTask->mTaskState == ETaskWaiting) {
          gExecBase.AddWaitingList(*t);
        }
        else {
          mActiveTasks.Add(*mCurrentTask);
        }
      }
      else {
        dlog("FORBID %d\n", mCurrentTask->mForbidNestCount);
      }
    }
  }

  mCurrentTask = mActiveTasks.First();
  SetCurrentTask(&mCurrentTask->mRegisters);
  if (t != mCurrentTask && gExecBase.mDebugSwitch) {
    dprint("  CPU %d Reschedule %s\n", mProcessorId, mCurrentTask->TaskName());
    dprint("Previous task\n");
    dprint("  Previous Task %s\n", t->TaskName());
    dprint("\n\n\n");
  }
}
