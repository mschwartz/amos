#ifndef DEVICE_MOUSE_DEVICE_H
#define DEVICE_MOUSE_DEVICE_H

#include <Exec/BDevice.hpp>
#include <Exec/MessagePort.hpp>

const TUint8 MOUSE_LEFT_BUTTON = 1 << 0;
const TUint8 MOUSE_MIDDLE_BUTTON = 1 << 1;
const TUint8 MOUSE_RIGHT_BUTTON = 1 << 2;

class MouseTask;

class MouseDevice : public BDevice {
  friend MouseTask;

public:
  MouseDevice();
  ~MouseDevice();

protected:
  TInt mDisplayWidth, mDisplayHeight;
  TInt64 mX, mY;
  TUint8 mButtons;
};

enum EMouseDeviceCommand {
  EMouseUpdate,
  EMouseButtonsUpdate,
  EMouseButtons,
  EMouseMove,
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
