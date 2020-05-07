#include <Exec/Types.h>
#include <Exec/ExecBase.h>
#include <Devices/TimerDevice.h>

#define PIT_TICKS_PER_DAY 0x1800b0

/* i8253 ports and commands */
#define I8253_CH0 0x40
#define I8253_CH1 0x41 /* channel 1 not used */
#define I8253_CH2 0x42 /* channel 2 not used */
#define I8253_CMD 0x43
#define I8253_CMD_LOAD 0x34
#define I8253_CMD_LATCH 0x04

#define QUANTUM 100

class TimerTask;

extern "C" void pic_100hz();

class TimerInterrupt : public BInterrupt {
public:
  TimerInterrupt(TimerTask *aTask) : BInterrupt("Timer Interrupt Handler", LIST_PRI_MAX) {
    mTask = aTask;
  }

public:
  TBool Run(TAny *aData);

protected:
  TimerTask *mTask;
};

class TimerTask : public BTask {
  friend TimerInterrupt;

public:
  TimerTask(TimerDevice *aTimerDevice) : BTask("Timer Task", LIST_PRI_MAX) {
    mTimerDevice = aTimerDevice;

//    pic_100hz();
    SetFrequency(QUANTUM);
//    SetFrequency(gExecBase.Quantum());
    gExecBase.SetIntVector(ETimerIRQ, new TimerInterrupt(this));
    gExecBase.EnableIRQ(IRQ_TIMER);
  }

  void SetFrequency(TInt hz) {
    TUint16 divisor = 1193180 / hz;
    outb(I8253_CMD, 0x36);
    outb(I8253_CH0, divisor & 0xff);
    outb(I8253_CH0, (divisor >> 8) & 0xff);
  }

  void Run();

protected:
  TimerDevice *mTimerDevice;
  MessagePort *mMessagePort;
  TUint16 mSignalBit;
};

void TimerTask::Run() {
  dlog("TimerTask Alive!\n");

  BMessageList timerQueue("timer.device queue");

  mSignalBit = AllocSignal(-1);
  mMessagePort = CreateMessagePort("timer.device");
  gExecBase.AddMessagePort(*mMessagePort);

  TUint64 port_mask = 1<<mMessagePort->SignalNumber();
  TUint64 tick_mask = 1<<mSignalBit;
  
  while (ETrue) {
//    dlog("Timer Device Wait\n");
    TUint64 sigs = Wait(port_mask | tick_mask);
    if (sigs & port_mask) {
      while (TimerMessage *m = (TimerMessage *)mMessagePort->GetMessage()) {
        switch (m->mCommand) {
          case ETimerReadTicks:
            m->mResult = mTimerDevice->GetTicks();
            m->ReplyMessage();
            break;
          case ETimerSleep:
            m->mPri = mTimerDevice->GetTicks() + m->mArg1 * QUANTUM;
//            dlog("Queue %d %d\n", mTimerDevice->GetTicks(), m->mPri);
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
      while (ETrue) {
        TUint64 flags = GetFlags();
        cli();
        TimerMessage *m = (TimerMessage *)timerQueue.First();
        if (timerQueue.End(m)) {
          break;
        }
//        dlog("message %d %d\n", m->mPri, current);
        if (m->mPri > current) {
          SetFlags(flags);
          break;
        }
//        dlog("Wake\n");
        m->Remove();
        SetFlags(flags);
        m->ReplyMessage();
//        dlog("reply %d\n", current);
      }
    }
  }
}

TBool TimerInterrupt::Run(TAny *aData) {
  mTask->Signal(1 << mTask->mSignalBit);
  gExecBase.AckIRQ(IRQ_TIMER);
  // maybe wake up new task
  gExecBase.RescheduleIRQ();
  return ETrue;
}

TimerDevice::TimerDevice() : BDevice("timer.device") {
  mTicks = 0;
  gExecBase.AddTask(new TimerTask(this));
}

TimerDevice::~TimerDevice() {
  //
}
