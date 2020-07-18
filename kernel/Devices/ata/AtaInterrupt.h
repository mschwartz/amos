#ifndef AMOS_DEVICES_ATA_ATAINTERRUPT_H
#define AMOS_DEVICES_ATA_ATAINTERRUPT_H

#include "AtaTask.h"

class AtaInterrupt : public BInterrupt {
public:
  AtaInterrupt(AtaTask *aTask, TUint8 aSignalBit, TInt64 aDevice);

public:
  TBool Run(TAny *aData);

protected:
  TInt64 mDevice;
  AtaTask *mTask;
  TUint8 mSignalBit;
  // TUint64 mSigMask;
};

#endif
