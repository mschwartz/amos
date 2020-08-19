#ifndef EXEC_RTC_DEVICE_H
#define EXEC_RTC_DEVICE_H

#include <Exec/BDevice.hpp>

/**
 * RtcDevice
 *
 * Handles 1000Hz RTC timer.
 * Provides 1000Hz (Millisecond resolution) ticks counter.
 * Provides millisecond resolution wait capability, e.g. for usleep().
 * Provides methodology to read RTC date/time.
 */
class RtcDevice : public BDevice {
public:
  RtcDevice();
  ~RtcDevice();

public:
  TUint64 Tick();
  volatile TUint64 GetTicks() { return mMillis; }
  void IncrementTicks() { mMillis++; }

public:
  volatile TUint16 mMonth, mDay, mYear, mWeekday;
  volatile TUint16 mHours, mMinutes, mSeconds, mFract;
  volatile TUint64 mMillis;
  TBool mPm;
};

enum ERtcDeviceCommand {
  ERtcReadTicks,
  ERtcSleep,
  ERtcDate,
};

class RtcMessage : public BMessage {
public:
  RtcMessage(MessagePort *aReplyPort, ERtcDeviceCommand aCommand) {
    mReplyPort = aReplyPort;
    mCommand = aCommand;
  }
  ~RtcMessage() {}

public:
  ERtcDeviceCommand mCommand;
  TInt64 mResult;
  TInt64 mArg1;
};

#endif
