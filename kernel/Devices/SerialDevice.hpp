#ifndef EXEC_DEVICES_SERIAL_DEVICE_H
#define EXEC_DEVICES_SERIAL_DEVICE_H

#include <Exec/BDevice.hpp>
#include <Exec/MessagePort.hpp>

const int SERIAL_BUFFER_SIZE = 256; /* keyboard buffer size (software buffer) */

class SerialTask;

class SerialDevice : public BDevice {
public:
  SerialDevice();
  ~SerialDevice();

public:
  TBool ReadyToSend() {
    return inb(mPort + 5) & 0x20;
  }
  TBool ReadyToReceive() {
    return (inb(mPort + 5) & 0x01) == 1;
  }

  void Send(TUint8 aByte) {
    while (!ReadyToSend()) {
      //      asm("pause");
    }
    outb(mPort, aByte);
  }

  void Send(const char *s) {
    while (*s) {
      Send(*s++);
    }
  }

  TUint8 Receive() {
    while (!ReadyToReceive()) {
      //      asm("pause");
    }
    return inb(mPort);
  }

protected:
  // circular buffer
  SerialTask *mTask;
  TUint16 mPort;
  char buffer[SERIAL_BUFFER_SIZE];
  int ptr1, ptr2;
};

enum ESerialDeviceCommand {
  ESendChar,
};

class SerialMessage : public BMessage {
public:
  SerialMessage(MessagePort *aReplyPort, ESerialDeviceCommand aCommand) : BMessage(mReplyPort) {
    mCommand = aCommand;
  }
  ~SerialMessage() {}

public:
  ESerialDeviceCommand mCommand;
  TUint8 mData;
  TInt64 mResult;
};

#endif
