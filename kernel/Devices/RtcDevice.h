#ifndef EXEC_RTC_DEVICE_H
#define EXEC_RTC_DEVICE_H

#include <Exec/BDevice.h>

class RtcDevice : public BDevice {
public:
  RtcDevice();
  ~RtcDevice();
public:
  void Tick();
public:
  volatile TUint16 mMonth, mDay, mYear;
  volatile TUint16 mHours, mMinutes, mSeconds, mFract;
  volatile TUint64 mMillis;
};

enum ERtcDeviceCommand {
  EReadTime,
};

class RtcMessage : public BMessage {
public:
  RtcMessage(MessagePort *aReplyPort, ERtcDeviceCommand aCommand);
  ~RtcMessage();

public:
  ERtcDeviceCommand mCommand;
  TInt64 mResult;
};

#endif
