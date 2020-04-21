#ifndef BDEVICE_H
#define BDEVICE_H

#include <Exec/BMessagePort.h>

class BDevice : public BNodePri {
public:
   BDevice(const char *aNodeName);
  ~BDevice();
};

class BDeviceList : public BListPri {
public:
  BDeviceList();
  ~BDeviceList();

public:
  void AddDevice(BDevice& aDevice);
  BDevice *FindDevice(const char *aName);

public:
  BDevice *RemHead() { return (BDevice *)BListPri::RemHead(); }

  BDevice *First()  { return (BDevice *)mNext; }

  BDevice *Next(BDevice *aCurrent)  { return (BDevice *)aCurrent->mNext; }

  BDevice *Last()  { return (BDevice *)mPrev; }

  BDevice *Prev(BDevice *aCurrent) { return (BDevice *)aCurrent->mPrev; }

  TBool End(BDevice *aCurrent) { return aCurrent == (BDevice *)this; }
};

extern BDeviceList gDeviceList;


#endif
