#ifndef AMOS_DEVICES_ATA_PIO_H
#define AMOS_DEVICES_ATA_PIO_H

#include <Exec/BBase.h>
#include "ata.h"

const TInt PIO_IO_SIZE = 512;

class AtaTask;

class PIO : public BBase {
public:
  PIO(AtaTask *aTask);

public:
  TBool ReadSector(TUint64 aLba, TAny *aBuffer);
  TBool ReadSectors(TUint64 aLba, TAny *aBuffer, TInt32 aNumSectors);
  //
  TBool WriteSector(TUint64 aLba, TAny *aBuffer);
  TBool WriteSectors(TUint64 aLba, TAny *aBuffer, TInt32 aNumSectors);

protected:
  AtaTask *mTask;
  TIdeDevice *mIdeDevice;
};

#endif
