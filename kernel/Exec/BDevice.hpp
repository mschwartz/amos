#ifndef BDEVICE_H
#define BDEVICE_H

#include <Exec/MessagePort.hpp>

class BDevice : public BNodePri {
public:
  BDevice(const char *aNodeName);
  ~BDevice();
};

class DeviceList : public BListPri {
public:
  DeviceList();
  ~DeviceList();

public:
  void AddDevice(BDevice &aDevice);
  BDevice *FindDevice(const char *aName);

public:
  BDevice *RemHead() { return (BDevice *)BListPri::RemHead(); }

  BDevice *First() { return (BDevice *)mNext; }

  BDevice *Next(BDevice *aCurrent) { return (BDevice *)aCurrent->mNext; }

  BDevice *Last() { return (BDevice *)mPrev; }

  BDevice *Prev(BDevice *aCurrent) { return (BDevice *)aCurrent->mPrev; }

  TBool End(BDevice *aCurrent) { return aCurrent == (BDevice *)this; }

public:
  void Lock() { return mSpinLock.Acquire(); }
  void Unlock() { return mSpinLock.Release(); }

protected:
  SpinLock mSpinLock;
};

class BDeviceMessage : public BMessage {
public:
  BDeviceMessage(MessagePort *aReplyPort) : BMessage(aReplyPort) {
    //
  }
  ~BDeviceMessage() {}
};

#endif
