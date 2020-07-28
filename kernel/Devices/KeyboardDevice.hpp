#ifndef DEVICE_KEYBOARD_H
#define DEVICE_KEYBOARD_H

/*!
 * 8042 keyboard controller
 */

#include <Exec/BDevice.hpp>
#include <Exec/MessagePort.hpp>

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
  EKeyDown, // internal use only!
  EKeyUp,   // internal use only!
  // commands that can be used by applications
  EKeyRead, // read one or more keys from 
};

enum EKeyboardError {
  EKeyboardErrorNone,
  EKeyboardTryAgain, // queue is empty, no keys ready
};

class KeyboardMessage : public BMessage {
public:
  KeyboardMessage(MessagePort *aReplyPort, EKeyboardDeviceCommand aCommand) : BMessage(aReplyPort) {
    mCommand = aCommand;
  }
  ~KeyboardMessage() {}

public:
  EKeyboardDeviceCommand mCommand;
  EKeyboardError mError;
  TInt64 mResult;
};

#endif
