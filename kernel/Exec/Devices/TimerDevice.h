/*!
 * 8253 Programmable Interupt Timer (PIT)
 */

#ifndef KERNEL_TIMER_DEVICE_H
#define KERNEL_TIMER_DEVICE_H

#include <Exec/BDevice.h>
#include <Exec/BMessagePort.h>

class TimerDevice : public BDevice {
public:
  TimerDevice();
  ~TimerDevice();

public:
  TUint64 GetTicks() { return mTicks; }
  TUint64 IncrementTicks() { return ++mTicks; }

protected:
  volatile TUint64 mTicks; // ticks/interrupts since start
};


enum ETimerDeviceCommand {
  ETimerReadTicks,
  ETimerSleep,           // mArgs1 = number of seconds
};

class TimerMessage : public BDeviceMessage {
public:
  TimerMessage(BMessagePort *aReplyPort, ETimerDeviceCommand aCommand, TUint64 aArg = 0) : BDeviceMessage(aReplyPort) {
    mCommand = aCommand;
    mArg1 = aArg;
  }
  ~TimerMessage() {}

public:
  ETimerDeviceCommand mCommand;
  TUint64 mArg1;
  TUint64 mResult;
};

#endif
