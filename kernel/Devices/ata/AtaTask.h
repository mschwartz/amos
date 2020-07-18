#ifndef DEVICES_ATA_ATATASK_H
#define DEVICES_ATA_ATATASK_H

#include <Devices/ata/AtaDevice.h>
#include <Types.h>
#include <Exec/BTask.h>
#include "ata.h"

const TInt ATA_IO_SIZE = 512;

class AtaTask : public BTask {
public:
  AtaTask(AtaDevice *aDevice);
  ~AtaTask();

protected:
  void io_wait(TUint8 aChannel); // wait 400ms

  // write a value to register in a channel
  void write_register(TUint8 aChannel, TUint8 aRegister, TUint8 aData);
  // Read a register in a channel
  TUint8 read_register(TUint8 aChannel, TUint8 aRegister);

  // PIO read aWordCount words into aBuffer
  void read_buffer(TUint8 aChannel, TUint8 aRegister, TUint8 *aBuffer, TUint32 aWordCount);
  TInt wait_ready(TUint8 aChannel, TBool aAdvancedCheck);

protected:
  TUint8 print_error(TUint aDrive, TUint8 aError);

protected:
  void initialize(TUint32 BAR0, TUint32 BAR1, TUint32 BAR2, TUint32 BAR3, TUint32 BAR4);

protected:
  TBool pio(TIdeDevice *aIdeDevice, TUint64 aLba, TBool aWrite, TUint8 *aBuffer, TInt aNumSectors);

  TBool dma_read_sector(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer);
  TBool dma_read(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TInt aNumSectors);

  TBool dma_write_sector(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer);
  TBool dma_write(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TInt aNumSectors);

  /* TBool AtaTask::ide_dma(TIdeDevice *aIdeDevice, TUint64 aLba, TBool aWrite, TUint8 aBuffer, TInt aNumSectors) { */

  TBool do_io(TIdeDevice *aIdeDevice, TUint64 aLba, TBool aWrite, TUint8 *aBuffer, TInt aNumSectors);

  TBool read_blocks(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TUint64 aCount) {
    return do_io(aIdeDevice, aLba, EFalse, aBuffer, aCount);
  }

  TBool write_blocks(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TUint64 aCount) {
    return do_io(aIdeDevice, aLba, ETrue, aBuffer, aCount);
  }

protected:
  TUint64 mSigMask;
  TUint64 mActiveDevice;
 protected:
  // these need to be in chip memory
  TUint8 *mSectorBuffer;
  TUint8 *mPrdt;

public:
  void Run();

protected:
  AtaDevice *mDevice;
  TIdeChannelRegisters mChannels[2];
  TIdeDevice mDevices[4];
};

#endif
