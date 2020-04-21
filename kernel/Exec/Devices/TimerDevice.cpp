#include <Devices/TimerDevice.h>
#include <Exec/ExecBase.h>
#include <Devices/PIC.h>
#include <x86/bochs.h>

#define PIT_TICKS_PER_DAY 0x1800b0

/* i8253 ports and commands */
#define I8253_CH0 0x40
#define I8253_CH1 0x41 /* channel 1 not used */
#define I8253_CH2 0x42 /* channel 2 not used */
#define I8253_CMD 0x43
#define I8253_CMD_LOAD 0x34
#define I8253_CMD_LATCH 0x04

class TimerTask;

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
public:
  TimerTask(TimerDevice *aTimerDevice) : BTask("Timer Task", LIST_PRI_MAX) {
    mTimerDevice = aTimerDevice;

    SetFrequency(100);
//    SetFrequency(gExecBase.Quantum());
    gExecBase.SetIntVector(ETimerIRQ, new TimerInterrupt(this));
    gPIC->enable_interrupt(IRQ_TIMER);
  }

  void SetFrequency(TInt hz) {
    int divisor = 1193180 / hz;
    outb(I8253_CMD, 0x36);
    outb(I8253_CH0, divisor & 0xff);
    outb(I8253_CH0, divisor >> 8);
  }

  void Run();

protected:
  TimerDevice *mTimerDevice;
};

void TimerTask::Run() {
  dprint("TimerTask Alive!\n");
  while (ETrue) {
    TUint64 sigs = Wait(1 << 10);
//    dprint("TimerTask sigs %x\n", sigs);
    mTimerDevice->IncrementTicks();
  }
}

TBool TimerInterrupt::Run(TAny *aData) {
  mTask->Signal(1 << 10);
  gPIC->ack(IRQ_TIMER);
  // maybe wake up new task
  gExecBase.RescheduleIRQ();
//  dprint("T");
  return ETrue;
}

TimerDevice::TimerDevice() : BDevice("timer.device") {
  dprint("Timer Device constructor\n");
  mTicks = 0;
  gExecBase.AddTask(new TimerTask(this));
  dprint("Returning from constructor\n");
}

TimerDevice::~TimerDevice() {
}
