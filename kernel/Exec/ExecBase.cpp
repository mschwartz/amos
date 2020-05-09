#include <Exec/ExecBase.h>
#include <Inspiration/Inspiration.h>
#include <stdint.h>

#include <Exec/x86/mmu.h>
#include <Exec/x86/idt.h>
#include <Exec/x86/gdt.h>
#include <Exec/x86/cpu.h>
#include <Exec/x86/pic.h>
#include <Exec/x86/ps2.h>

#include <Devices/SerialDevice.h>
#include <Devices/KeyboardDevice.h>
#include <Devices/TimerDevice.h>
#include <Devices/RtcDevice.h>
#include <Devices/MouseDevice.h>

#include <posix/sprintf.h>
#include <Exec/Random.h>

#include <Graphics/BViewPort.h>
#include <Graphics/font/BConsoleFont.h>

ExecBase gExecBase;

extern "C" void schedule_trap();
extern "C" void eputs(const char *s);

extern "C" void enter_next_task();

class IdleTask : public BTask {
public:
  IdleTask() : BTask("Idle Task", LIST_PRI_MIN) {}

public:
  void Run() {
    //    sti();
    dlog("IdleTask Running\n");
    while (1) {
      halt();
    }
  }
};

class TestTask : public BTask {
public:
  TestTask() : BTask("Test Task") {
    dprint("Construct TestTask\n");
  }

public:
  void Run() {
    dlog("***************************** TEST TASK RUNNING\n");
    Sleep(1);
#ifdef KGFX
    ScreenVesa &screen = *gExecBase.GetScreen();
    BBitmap32 &bm = *screen.GetBitmap();
    //    bm.Dump();
    BViewPort32 *vp = new BViewPort32("test vp", &bm);
    TRect rect, screenRect;
    bm.GetRect(screenRect);

    TRGB fg(255, 255, 255), bg(0, 0, 0);
    BConsoleFont32 font(&bm);

    bm.SetFont(&font);
    font.SetColors(fg, bg);
    vp->SetFont(&font);
    vp->SetColors(fg, bg);
    TRect vrect(50, 200, 500, 300);
    vp->SetRect(vrect);

    RtcDevice *rd = (RtcDevice *)gExecBase.FindDevice("rtc.device");
    //    if (!rd) {
    //      dprint("Can't find rct.device\n");
    //      halt();
    //    }
    bm.Clear(0x0000ff);
#if 0
      TRGB color;

      while (ETrue) {
        dlog("START!\n");
        for (TInt n=0; n<100000; n++) {
          if ((n % 100) == 0) {
            dlog("n = %d\n", n);
          }
          rect.x1 = Random(screenRect.x1, screenRect.x2);
          rect.x2 = Random(rect.x1, screenRect.x2);
          rect.y1 = Random(screenRect.y1, screenRect.y2);
          rect.y2 = Random(rect.y1, screenRect.y2);
          TRGB color(Random(0, 255), Random(0, 255), Random(0, 255));
//          dlog("Fill %d,%d,%d,%d %x\n", rect.x1, rect.y1, rect.x2, rect.y2, color.rgb888());
          bm.FillRect(color, rect);
//          bm.DrawRect(color, rect);
        }
        dlog("END!\n");
        Sleep(1);
      }
#else
      bm.FillRect(0xffffff, 300, 300, 500, 500);
//    TInt count = 0;
    while (true) {
//      dlog("test task loop %d\n", ++count);;
      char buf[128];
      sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d.%d", rd->mMonth, rd->mDay, rd->mYear, rd->mHours, rd->mMinutes, rd->mSeconds, rd->mFract);
      //      dlog("buf: %s\n", buf);
      screen.HideCursor();
      vp->DrawText(0, 0, buf);
      screen.ShowCursor();
      //      font.Write(vp, 100, 100, buf);
      Sleep(1);
    }
#endif
#else
    RtcDevice *rd = ENull;
    while (rd == ENull) {
      rd = (RtcDevice *)gExecBase.FindDevice("rtc.device");
      if (rd) {
        break;
      }
      Sleep(1);
    }
    ScreenVGA &screen = *gExecBase.GetScreen();
    screen.MoveTo(20, 20);
    dprint("Test Task\n");
    while (true) {
      char buf[128];
      sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d.%d", rd->mMonth, rd->mDay, rd->mYear, rd->mHours, rd->mMinutes, rd->mSeconds, rd->mFract);
      screen.MoveTo(10, 10);
      dprint(buf);
      Sleep(1);
    }

#endif

    //      TInt64 time = 0;
    //      while (1) {
    //        Sleep(1);
    //        dlog("TestTask: Time %d\n", ++time);
    //      }
  }
};

typedef struct {
  //  TUint16 mPad0;
  TUint32 mMode;
  TUint32 mFrameBuffer;
  TUint32 mWidth;
  TUint32 mHeight;
  TUint32 mPitch;
  TUint32 mDepth;
  TUint32 mPlanes;
  TUint32 mBanks;
  TUint32 mBankSize;
  TUint32 mMemoryModel;
  TUint32 mFrameBufferOffset;
  TUint32 mFrameBufferSize;
  TUint32 mPad2;
  void Dump() {
    dprint("Mode(%x) mode(%x) dimensions(%dx%d) depth(%d)  pitch(%d) lfb(0x%x)\n",
      this, mMode, mWidth, mHeight, mDepth, mPitch, mFrameBuffer);
    //    dlog("Mode(%x) mode(%x) dimensions(%xx%x) bpp(%x)  pitch(%x) lfb(0x%x)\n",
    //        this, mMode, mWidth, mHeight, mDepth, mPitch, mFrameBuffer);
  }
} PACKED TModeInfo;

typedef struct {
  TInt32 mCount;          // number of modes found
  TModeInfo mDisplayMode; // chosen display mode
  TModeInfo mModes[];
  void Dump() {
    dlog("Found %d %x modes\n", mCount, mCount);
    for (TInt16 i = 0; i < mCount; i++) {
      mModes[i].Dump();
    }
  }
} PACKED TModes;

// ExecBase constructor
ExecBase::ExecBase() {
  dlog("ExecBase constructor called\n");
  //  cli(); halt();
  //  Screen s;
  TModes *modes = (TModes *)0xa000;
  TModeInfo &i = modes->mDisplayMode;

#if 0
  dprintf("video_mode_count = %x %d\n", &modes->mCount, modes->mCount);
  dprintf("display_mode = %x\n", &modes->mDisplayMode);
  dprintf("fb = %x %x\n", &i.mFrameBuffer, i.mFrameBuffer);
  dprintf("width = %x %d\n", &i.mWidth, i.mWidth);
  dprintf("height = %x %d\n", &i.mHeight, i.mHeight);
  dprintf("pitch = %x %d\n", &i.mPitch, i.mPitch);
  dprintf("depth = %x %d\n", &i.mDepth, i.mDepth);
#endif

  mSystemInfo.mScreenWidth = i.mWidth;
  mSystemInfo.mScreenHeight = i.mHeight;
  mSystemInfo.mScreenDepth = i.mDepth;
  mSystemInfo.mScreenPitch = i.mPitch;
  TUint64 fb = (TUint64)i.mFrameBuffer;
  mSystemInfo.mScreenFrameBuffer = (TAny *)fb;
  mSystemInfo.mMillis = 0;

#ifdef KGFX
  mScreen = new ScreenVesa();
  dprintf("Using VESA screen\n");
#else
  mScreen = new ScreenVGA();
#endif
  dlog("  initialized screen\n");

  SeedRandom(SystemTicks());
  dlog("\n\nDisplay Mode:\n");
  modes->mDisplayMode.Dump();

  // set up paging
  mMMU = new MMU;
  dlog("  initialized MMU\n");

  mMessagePortList = new MessagePortList("ExecBase MessagePort List");

  mGDT = new GDT;
  dlog("  initialized GDT\n");

  mIDT = new IDT;
  dlog("  initialized IDT\n");

  InitInterrupts();

  // Before enabling interrupts, we need to have the idle task set up
  IdleTask *task = new IdleTask();
  mActiveTasks.Add(*task);
  mCurrentTask = mActiveTasks.First();
  current_task = ENull;
  next_task = &mCurrentTask->mRegisters;

  // set up 8259 PIC
  mPIC = new PIC;
  mDisableNestCount = 0;
  //  sti();
  Disable();
  dlog("  initialized 8259 PIC\n");

#ifdef ENABLE_PS2
  mPS2 = new PS2();
#else
  mPS2 = ENull;
#endif

  // initialize devices
  dlog("  initialize timer\n");
  AddDevice(new TimerDevice());

  dlog("  initialize serial\n");
  AddDevice(new SerialDevice());

  dlog("  initialize rtc \n");
  AddDevice(new RtcDevice());

  dlog("  initialize keyboard \n");
  AddDevice(new KeyboardDevice);

  dlog("  initialize mouse \n");
  AddDevice(new MouseDevice());

  dlog("  initialize Inspiration\n");
  gInspirationBase = *new InspirationBase();

  dlog("  initialize Test Task \n");
  TestTask *test_task = new TestTask();
  gExecBase.AddTask(test_task);
  dlog("  initialized Test Task \n");
  Enable();
}

ExecBase::~ExecBase() {
  dlog("ExecBase destructor called\n");
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

//void ExecBase::AddInterruptHandler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription) {
//  mIDT->InstallHandler(aIndex, aHandler, aData, aDescription);
//}

void ExecBase::putc(char c) {
  mScreen->WriteChar(c);
}

void ExecBase::puts(const char *s) {
  mScreen->WriteString(s);
}

void ExecBase::newline() {
  mScreen->NewLine();
}

void ExecBase::AddTask(BTask *aTask) {
  TUint64 flags = GetFlags();
  cli();

  dlog("    Add Task %016x --- %s --- rip=%016x rsp=%016x\n", aTask, aTask->mNodeName, aTask->mRegisters.rip, aTask->mRegisters.rsp);
  //  aTask->Dump();
  mActiveTasks.Add(*aTask);
  //  mActiveTasks.Dump();
  //  dlog("x\n");

  SetFlags(flags);
}

void ExecBase::DumpTasks() {
  mActiveTasks.Dump();
}

void ExecBase::WaitSignal(BTask *aTask) {
  cli();
  aTask->Remove();
  // If task has received a signal it's waiting for, we don't want to move it to the WAIT list,
  // but it may be lower priority than another task so we need to sort it in to ACTIVE list.
  if (aTask->mSigWait & aTask->mSigReceived) {
    dputs("\n");
    mActiveTasks.Add(*aTask);
    aTask->mTaskState = ETaskRunning;
  }
  else {
    mWaitingTasks.Add(*aTask);
    aTask->mTaskState = ETaskWaiting;
  }
  Schedule();
}

/**
  * Wake() - if task is on Wait list, move it to active list.  If already on active list, re-add it.
  * Note that Add() will sort the task into the list, effecting round-robin order.
  */
void ExecBase::Wake(BTask *aTask) {
  // note that removing and adding the task will sort the task at the end of all tasks with the same priority.
  // this effects round-robin.
  aTask->Remove();
  mActiveTasks.Add(*aTask);
  aTask->mTaskState = ETaskRunning;
}

void ExecBase::Schedule() {
  schedule_trap();
}

void ExecBase::Kickstart() {
  enter_next_task(); // just enter next task
}

/**
 * Determine next task to run.  This should only be called from IRQ/Interrupt context with interrupts disabled.
 */
void ExecBase::RescheduleIRQ() {
  cli();
  if (mCurrentTask) {
    if (mCurrentTask->mForbidNestCount == 0) {
      mCurrentTask->Remove();
      if (mCurrentTask->mTaskState == ETaskWaiting) {
        mWaitingTasks.Add(*mCurrentTask);
      }
      else {
        mActiveTasks.Add(*mCurrentTask);
      }
    }
  }
  mCurrentTask = mActiveTasks.First();
  current_task = &mCurrentTask->mRegisters;
}

void ExecBase::AddMessagePort(MessagePort &aMessagePort) {
  mMessagePortList->Add(aMessagePort);
}

TBool ExecBase::RemoveMessagePort(MessagePort &aMessagePort) {
  if (mMessagePortList->Find(aMessagePort)) {
    aMessagePort.Remove();
    return ETrue;
  }
  return EFalse;
}

MessagePort *ExecBase::FindMessagePort(const char *aName) {
  return (MessagePort *)mMessagePortList->Find(aName);
}

void ExecBase::GuruMeditation(const char *aFormat, ...) {
  cli();
  char buf[512];
  dprint("\n\n***********************\n");
  dprint("GURU MEDITATION at %dms\n", SystemTicks());
  va_list args;
  va_start(args, aFormat);
  vsprintf(buf, aFormat, args);
  dprint(buf);
  dprint("\n");

  mCurrentTask->Dump();
  va_end(args);
  dprint("***********************\n\n\nHalted.\n");
  while (1) {
    halt();
  }
}

void ExecBase::AddDevice(BDevice *aDevice) {
  mDeviceList.Add(*aDevice);
}

BDevice *ExecBase::FindDevice(const char *aName) {
  return mDeviceList.FindDevice(aName);
}

class DefaultException : public BInterrupt {
public:
  DefaultException(const char *aKind) : BInterrupt(aKind, LIST_PRI_MIN) {}
  ~DefaultException();

public:
  TBool Run(TAny *aData) {
    cli();
    gExecBase.GuruMeditation("%s Exception", mNodeName);
    // TODO: kill/remove current task
    halt();
    return ETrue;
  }
};

class DefaultIRQ : public BInterrupt {
public:
  DefaultIRQ(const char *aKind) : BInterrupt(aKind, LIST_PRI_MIN) {}
  ~DefaultIRQ();

public:
  TBool Run(TAny *aData) {
    dlog("%s IRQ\n", mNodeName);
    return ETrue;
  }
};

class NextTaskTrap : public BInterrupt {
public:
  NextTaskTrap(const char *aKind) : BInterrupt(aKind, LIST_PRI_MIN) {}
  ~NextTaskTrap();

public:
  TBool Run(TAny *aData) {
    // at this point current_task is saved
    cli();
    gExecBase.RescheduleIRQ();
    return ETrue;
  }
};

void ExecBase::SetIntVector(EInterruptNumber aInterruptNumber, BInterrupt *aInterrupt) {
  mInterrupts[aInterruptNumber].Add(*aInterrupt);
}

void ExecBase::EnableIRQ(TUint16 aIRQ) {
  mPIC->EnableIRQ(aIRQ);
}

void ExecBase::DisableIRQ(TUint16 aIRQ) {
  mPIC->DisableIRQ(aIRQ);
}

void ExecBase::AckIRQ(TUint16 aIRQ) {
  mPIC->AckIRQ(aIRQ);
}

extern "C" TUint64 GetRFLAGS();

TBool ExecBase::RootHandler(TInt64 aInterruptNumber, TAny *aData) {
  cli();
  BInterruptList *list = &gExecBase.mInterrupts[aInterruptNumber];
  for (BInterrupt *i = (BInterrupt *)list->First(); !list->End(i); i = (BInterrupt *)i->mNext) {
    if (i->Run(aData)) {
      return ETrue;
    }
  }
  // TODO: no handler!
  dlog("No handler!\n");
  return EFalse;
}

void ExecBase::SetException(EInterruptNumber aIndex, const char *aName) {
  TUint64 flags = GetFlags();
  cli();
  IDT::InstallHandler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new DefaultException(aName));
  SetFlags(flags);
}

void ExecBase::SetInterrupt(EInterruptNumber aIndex, const char *aName) {
  TUint64 flags = GetFlags();
  cli();
  IDT::InstallHandler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new DefaultIRQ(aName));
  SetFlags(flags);
}

void ExecBase::SetTrap(EInterruptNumber aIndex, const char *aName) {
  TUint64 flags = GetFlags();
  cli();
  //  dlog("Add Trap %d %s\n", aIndex, aName);
  IDT::InstallHandler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new NextTaskTrap(aName));
  SetFlags(flags);
}

void ExecBase::InitInterrupts() {
  TUint64 flags = GetFlags();
  cli();
  // install default exception handlers
  SetException(EDivideError, "Divide Error");
  SetException(EDebug, "Debug");
  SetException(ENmi, "NMI");
  SetException(EInt3, "Int3");
  SetException(EOverflow, "Overflow");
  SetException(EOutOfBounds, "OutOfBounds");
  SetException(EInvalidOpCode, "InvalidOpCode");
  SetException(EDeviceNotAvailable, "DeviceNotAvailable");
  SetException(EDoubleFault, "DoubleFault");
  SetException(ECoprocessorSegmentOverrun, "CoprocessorSegmentOverrun");
  SetException(EInvalidTSS, "InvalidTSS");
  SetException(ESegmentNotPresent, "SegmentNotPresent");
  SetException(EStackFault, "StackFault");
  SetException(EGeneralProtection, "GeneralProtection");
  SetException(EPageFault, "PageFault");
  SetException(EUnknown, "Unknown");
  SetException(ECoprocessorFault, "CoprocessorFault");
  SetException(EAlignmentCheck, "AlignmentCheck");
  SetException(EMachineCheck, "MachineCheck");
  SetException(ESimdFault, "SimdFault");
  SetException(EVirtualization, "Virtualization");
  SetException(ESecurity, "Security");
  //
  SetInterrupt(ETimerIRQ, "Timer");
  SetInterrupt(EKeyboardIRQ, "Keyboard");
  SetInterrupt(ESlavePicIRQ, "SlavePic");
  SetInterrupt(ECom2IRQ, "Com2");
  SetInterrupt(ECom1IRQ, "Com1");
  SetInterrupt(ELpt2IRQ, "Lpt2");
  SetInterrupt(EFloppyIRQ, "Floppy");
  SetInterrupt(ELpt1IRQ, "Lpt1");
  SetInterrupt(ERtcClockIRQ, "RtClock");
  SetInterrupt(EMasterPicIRQ, "MasterPic");
  SetInterrupt(EReserved1IRQ, "Reserved1");
  SetInterrupt(EReserved2IRQ, "Reserved2");
  SetInterrupt(EMouseIRQ, "Mouse");
  SetInterrupt(ECoprocessorIRQ, "Coprocessor");
  SetInterrupt(EHardDiskIRQ, "HardDisk");
  //  SetInterrupt(EReserved4IRQ, "Reserved4");
  SetTrap(ETrap0, "Trap0");

  SetFlags(flags);
}
