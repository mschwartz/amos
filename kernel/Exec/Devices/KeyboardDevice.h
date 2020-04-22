#ifndef DEVICE_KEYBOARD_H
#define DEVICE_KEYBOARD_H

/*!
 * 8042 keyboard controller
 */

#include <Exec/BDevice.h>
#include <Exec/BMessagePort.h>

const int KEYBOARD_BUFFER_SIZE = 256; /* keyboard buffer size (software buffer) */

class KeyboardDevice : public BDevice {
public:
  KeyboardDevice();
  ~KeyboardDevice();

protected:
  // circular buffer
  char buffer[KEYBOARD_BUFFER_SIZE];
  int ptr1, ptr2;
};

enum EKeyboardDeviceCommand {
  EQueueChar,
  EReadChar,
};

class KeyboardMessage : public BMessage {
public:
  KeyboardMessage(BMessagePort *aReplyPort, EKeyboardDeviceCommand aCommand) : BMessage(mReplyPort) {
    mCommand = aCommand;
  }
  ~KeyboardMessage() {}

public:
  EKeyboardDeviceCommand mCommand;
  TInt64 mResult;
};

#endif
