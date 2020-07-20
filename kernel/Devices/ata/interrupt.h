#ifndef AMOS_DEVICES_ATA_ATA_INTERRUPT_H
#define AMOS_DEVICES_ATA_ATA_INTERRUPT_H

#include <Types.h>
#include <Exec/BInterrupt.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class AtaTask;

class AtaInterrupt : public BInterrupt {
public:
  AtaInterrupt(AtaTask *aTask, TUint8 aSignalBit, TInt64 aDevice)
      : BInterrupt("ata.device", LIST_PRI_MAX, (TAny *)aDevice) {
    mTask = aTask;
    mSignalBit = aSignalBit;
    mDevice = aDevice;
  }

public:
  TBool Run(TAny *aData);

protected:
  TInt64 mDevice;
  AtaTask *mTask;
  TUint8 mSignalBit;
  TUint64 mSigMask;
};

#endif
