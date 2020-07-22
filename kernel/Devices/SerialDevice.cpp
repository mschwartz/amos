#include <Exec/ExecBase.h>
#include <Exec/x86/cpu.h>
#include <Devices/SerialDevice.h>
#include <Exec/BTask.h>

/**
  * TODO: There really should be two devices, com1 and com2.
  */

class SerialTask;

enum ESerialPort : TUint16 {
  ECOM1 = 0x3f8,
  ECOM2 = 0x2f8,
  ECOM3 = 0x3e8,
  ECOM4 = 0x2e8
};

class SerialInterrupt : public BInterrupt {
public:
  SerialInterrupt(SerialTask *aTask) : BInterrupt("serial.device", LIST_PRI_MAX) {
    mTask = aTask;
  }

public:
  TBool Run(TAny *aData);

protected:
  SerialTask *mTask;
};

class SerialTask : public BTask {
public:
  SerialTask(TUint16 aPort) : BTask("serial.device", LIST_PRI_MAX) {
    mPort = aPort;
  }

public:
  TInt64 Run();

protected:
  MessagePort *mMessagePort;
  TUint16 mPort;
};

TBool SerialInterrupt::Run(TAny *aData) {
  return ETrue;
}

TInt64 SerialTask::Run() {
  for (;;) {
    Sleep(1);
  }
}

SerialDevice::SerialDevice() : BDevice("serial.device") {
#if 0
  mPort = ECOM1;

  outb(mPort + 1, 0x00);
  outb(mPort + 3, 0x80, mPort + 3);
  outb(mPort + 0, 0x03);
  outb(mPort + 1, 0x00);
  outb(mPort + 3, 0x03);
  outb(mPort + 2, 0xc7);
  outb(mPort + 4, 0x0b);

  Send("Serial Port Initialized\n");

  mTask = new SerialTask(mPort);
  gExecBase.AddTask(mTask);
#endif
}

SerialDevice::~SerialDevice() {
}

//void SerialDevice::Send(TUint8 aByte) {
//  mTask->Send(aByte);
//}

//void SerialDevice::Send(const char *aString) {
//  while (*aString) {
//    mTask->Send(*aString++);
//  }
//}

//TUint8 SerialDevice::Receive() {
//  return mTask->Receive();
//}
