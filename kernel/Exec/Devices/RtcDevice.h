#ifndef EXEC_RTC_DEVICE_H
#define EXEC_RTC_DEVICE_H

#include <Exec/BDevice.h>

class RtcDevice : public BDevice {
public:
  RtcDevice();
  ~RtcDevice();
public:
  volatile TUint64 mMillis;
};

enum ERtcDeviceCommand {
  EReadTime,
};

class RtcMessage : public BMessage {
public:
  RtcMessage(BMessagePort *aReplyPort, ERtcDeviceCommand aCommand);
  ~RtcMessage();

public:
  ERtcDeviceCommand mCommand;
  TInt64 mResult;
};

#endif
