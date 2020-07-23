#ifndef AMOS_DEVICES_ATA_ATA_TASK_H
#define AMOS_DEVICES_ATA_ATA_TASK_H

#include <Types.h>
#include <Exec/BTask.h>
#include <Devices/AtaDevice.h>
#include "ata.h"
#include "dma.h"
#include "pio.h"
#include "interrupt.h"

class AtaTask : public BTask {
public:
  AtaTask(AtaDevice *aDevice);
  ~AtaTask();

public:
  void WaitIrq();
  void usleep(TInt aMillis) { MilliSleep(aMillis); }
  TUint16 AtaBasePort(TInt aChannel = 0) {
    return mChannels[aChannel].mBasePort;
  }

protected:
  // write a value to register in a channel
  void ide_write(TUint8 aChannel, TUint8 aRegister, TUint8 aData);
  // Read a register in a channel
  TUint8 ide_read(TUint8 aChannel, TUint8 aRegister);

  void ide_read_buffer(TUint8 aChannel, TUint8 aRegister, TUint8 *aBuffer, TUint32 aCount);

  TInt ide_polling(TUint8 aChannel, TBool aAdvancedCheck);

  TUint8 ide_print_error(TUint aDrive, TUint8 aError);
  void ide_initialize(TUint32 BAR0, TUint32 BAR1, TUint32 BAR2, TUint32 BAR3, TUint32 BAR4);

  TBool ide_io(TIdeDevice *aIdeDevice, TUint64 aLba, TBool aWrite, TUint8 *aBuffer, TInt aNumSectors);

  TBool ide_read_blocks(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TUint64 aCount);

  TBool ide_write_blocks(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TUint64 aCount);

protected:
  TUint64 mSigMask;
  TUint64 mActiveDevice;

public:
  TInt64 Run();

public:
  TIdeDevice *IdeDevice(TInt aChannel = 0) { return &mDevices[aChannel]; }

protected:
  TUint16 mBusMasterPort;
  DMA *mDma;
  PIO *mPio;
  AtaDevice *mDevice;
  TIdeChannelRegisters mChannels[2];
  TIdeDevice mDevices[4];
};

#endif
