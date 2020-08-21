#include <Types.hpp>
#include <Exec/ExecBase.hpp>
#include <Devices/TimerDevice.hpp>

#define PIT_TICKS_PER_DAY 0x1800b0

/* i8253 ports and commands */
#define I8253_CH0 0x40
#define I8253_CH1 0x41 /* channel 1 not used */
#define I8253_CH2 0x42 /* channel 2 not used */
#define I8253_CMD 0x43
#define I8253_CMD_LOAD 0x34
#define I8253_CMD_LATCH 0x04

//#define QUANTUM 100
#define QUANTUM gExecBase.Quantum()

class TimerTask;

extern "C" void pic_100hz();

class TimerInterrupt : public BInterrupt {
public:
  TimerInterrupt(TimerTask *aTask) : BInterrupt("Timer Interrupt Handler", LIST_PRI_MAX) {
    mTask = aTask;
  }

public:
  TBool Run(TAny *g);

protected:
  TimerTask *mTask;
};

class TimerTask : public BTask {
  friend TimerInterrupt;

public:
  TimerTask(TimerDevice *aTimerDevice) : BTask("Timer Task", LIST_PRI_MAX) {
    mTimerDevice = aTimerDevice;

    SetFrequency(QUANTUM);
    gExecBase.SetIntVector(ETimerIRQ, new TimerInterrupt(this));
    gExecBase.EnableIRQ(IRQ_TIMER);
    mSignalBit = AllocSignal(-1);
    mMessagePort = CreatePort("timer.device");
    gExecBase.AddMessagePort(*mMessagePort);
  }

  void SetFrequency(TInt hz) {
    TUint16 divisor = 1193180 / hz;
    outb(I8253_CMD, 0x36);
    outb(I8253_CH0, divisor & 0xff);
    outb(I8253_CH0, (divisor >> 8) & 0xff);
  }

  TInt64 Run();

protected:
  TimerDevice *mTimerDevice;
  MessagePort *mMessagePort;
  TUint16 mSignalBit;
};

TInt64 TimerTask::Run() {
  dprint("\n");
  dlog("TimerTask Run\n");

  BMessageList timerQueue("timer.device queue");

  TUint64 port_mask = 1 << mMessagePort->SignalNumber();
  TUint64 tick_mask = 1 << mSignalBit;

  for (;;) {
    TUint64 sigs = Wait(port_mask | tick_mask);
    if (sigs & port_mask) {
      while (TimerMessage *m = (TimerMessage *)mMessagePort->GetMessage()) {
        switch (m->mCommand) {
          case ETimerReadTicks:
            m->mResult = mTimerDevice->GetTicks();
            m->Reply();
            break;
          case ETimerSleep:
            m->mPri = mTimerDevice->GetTicks() + m->mArg1 * QUANTUM;
            timerQueue.Add(*m);
            break;
          default:
            dlog("timer.device: UnknownmCommand(%d/%x)\n", m->mCommand, m->mCommand);
            break;
        }
      }
    }

    if (sigs & tick_mask) {
      TUint64 current = mTimerDevice->IncrementTicks();
      for (;;) {
        TUint64 flags = GetFlags();
        TimerMessage *m = (TimerMessage *)timerQueue.First();
        if (timerQueue.End(m)) {
          break;
        }
        if (m->mPri > current) {
          SetFlags(flags);
          break;
        }
        m->Remove();
        SetFlags(flags);
        m->Reply();
      }
    }
  }
}

TBool TimerInterrupt::Run(TAny *g) {
  // dlog("TIMER\n");
  CPU *cpu = GetCPU();
  if (!cpu) {
    gExecBase.InterruptOthers(IRQ_TIMER);
    gExecBase.AckIRQ(IRQ_TIMER);
    return ETrue;
  }
  if (cpu->mApicId == 0) {
    mTask->Signal(1 << mTask->mSignalBit);
    gExecBase.InterruptOthers(IRQ_TIMER);
  }

  // dlog("Reschedule cpu(%d)\n", cpu->mProcessorId);
  cpu->RescheduleIRQ(); // maybe wake up new task
  cpu->AckIRQ(IRQ_TIMER);
  return ETrue;
}

TimerDevice::TimerDevice() : BDevice("timer.device") {
  mTicks = 0;
  gExecBase.AddTask(new TimerTask(this));
}

TimerDevice::~TimerDevice() {
  //
}
