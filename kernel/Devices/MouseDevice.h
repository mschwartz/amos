#ifndef DEVICE_MOUSE_DEVICE_H
#define DEVICE_MOUSE_DEVICE_H

#include <Exec/BDevice.h>
#include <Exec/MessagePort.h>

const TUint8 MOUSE_LEFT_BUTTON = 0x01;
const TUint8 MOUSE_RIGHT_BUTTON = 0x02;
const TUint8 MOUSE_MIDDLE_BUTTON = 0x04;

class MouseTask;
class MouseDevice : public BDevice {
  friend MouseTask;

public:
  MouseDevice();
  ~MouseDevice();

protected:
  TInt mScreenWidth, mScreenHeight;
  TInt32 mX, mY;
  TUint8 mButtons;
};

enum EMouseDeviceCommand {
  EMouseUpdate,
  EMouseState,
};

class MouseMessage : public BMessage {
public:
  MouseMessage(MessagePort *aReplyPort, EMouseDeviceCommand aCommand) : BMessage(mReplyPort) {
    mCommand = aCommand;
  }
  ~MouseMessage() {}

public:
  EMouseDeviceCommand mCommand;
  TInt32 mMouseX, mMouseY;
  TUint8 mButtons;
};

#endif
