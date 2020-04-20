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
  void IncrementTicks() { mTicks++; }

protected:
  volatile TUint64 mTicks; // ticks/interrupts since start
};


enum ETimerDeviceCommand {
  EReadTicks,
};

class TimerMessage : public BMessage {
public:
  TimerMessage(BMessagePort *aReplyPort, ETimerDeviceCommand aCommand);
  ~TimerMessage();
public:
  ETimerDeviceCommand mCommand;
  TInt64 mResult;
};

#endif
