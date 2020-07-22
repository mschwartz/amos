#define DEBUGME
#undef DEBUGME

#include <Exec/ExecBase.h>
#include "task.h"

extern "C" TUint8 bochs_present;
inline static void wait_io() {
  outb(0x80, 0x70);
}
static void delay() {
  for (TInt i = 0; i < 100; i++) {
    outb(0x80, 0x00); // 1 microsecond
    // wait_io();
  };
}

AtaTask::AtaTask(AtaDevice *aDevice) : BTask("ata.device") {
  mDevice = aDevice;
  mBusMasterPort = mDevice->BusMasterPort();

  mDma = (mBusMasterPort != 0) ? new DMA(this, mBusMasterPort) : ENull;
}

AtaTask::~AtaTask() {
  DSPACE();
  dlog("  AtaTask Desctuctor!\n");
  bochs;
}

void AtaTask::WaitIrq() {
  Wait(mSigMask);
}

TInt64 AtaTask::Run() {
  // TODO: queue reads in increasing LBA order to optimize (reduce) head seek

  DSPACE();
  dlog("AtaTask running\n");

  DISABLE;
  TUint8 sigbit = AllocSignal(-1); // this is for signal from IRQ handler(s)
  mSigMask = (1 << sigbit);
  dlog("  AtaTask signal bit %d(%x)\n", sigbit, mSigMask);

  // handlers for ATA1 and ATA2 IRQs
  gExecBase.SetIntVector(EAta1IRQ, new AtaInterrupt(this, sigbit, 1));
  gExecBase.SetIntVector(EAta2IRQ, new AtaInterrupt(this, sigbit, 2));
  gExecBase.EnableIRQ(IRQ_ATA1);
  gExecBase.EnableIRQ(IRQ_ATA2);
  // gExecBase.DisableIRQ(IRQ_ATA1);
  // gExecBase.DisableIRQ(IRQ_ATA2);

  // initialize devices
  mActiveDevice = 0;
  ide_initialize(0x1f0, 0x3f6, 0x170, 0x376, 0x000);

  MessagePort *port = CreateMessagePort("ata.device");
  gExecBase.AddMessagePort(*port);

  ENABLE;

  TIdeDevice *drive0 = &mDevices[0];
  for (;;) {
    DLOG("WaitPort\n");
    TUint64 sigs = WaitPort(port, mSigMask);
    if (sigs & mSigMask) {
      DLOG("  IRQ SIGNAL\n");
    }
    else {
      DLOG("ATA MESSAGE?\n");
      while (AtaMessage *m = (AtaMessage *)port->GetMessage()) {
        DLOG("ATA MESSAGE\n");
        switch (m->mCommand) {
          case EAtaReadBlocks: {
            TUint8 *buf = (TUint8 *)m->mBuffer;
            TUint64 lba = m->mLba;
            dlog("read blocks lba(%d) count(%d)\n", lba, m->mCount);
            ide_read_blocks(&mDevices[m->mUnit], lba, buf, m->mCount);
            dlog("  /read blocks\n");
          } break;
        }
        m->Reply();
      }
    }
  }
}

void AtaTask::ide_write(TUint8 aChannel, TUint8 aRegister, TUint8 aData) {
  if (aRegister > 0x07 && aRegister < 0x0C) {
    ide_write(aChannel, ATA_REG_CONTROL, 0x80 | mChannels[aChannel].nIEN);
  }
  if (aRegister < 0x08) {
    outb(mChannels[aChannel].mBasePort + aRegister - 0x00, aData);
  }
  else if (aRegister < 0x0C) {
    outb(mChannels[aChannel].mBasePort + aRegister - 0x06, aData);
  }
  else if (aRegister < 0x0E) {
    outb(mChannels[aChannel].mControlBase + aRegister - 0x0A, aData);
  }
  else if (aRegister < 0x16) {
    outb(mChannels[aChannel].mDmaPort + aRegister - 0x0E, aData);
  }
  if (aRegister > 0x07 && aRegister < 0x0C) {
    ide_write(aChannel, ATA_REG_CONTROL, mChannels[aChannel].nIEN);
  }
}

// Read a register in a channel
TUint8 AtaTask::ide_read(TUint8 aChannel, TUint8 aRegister) {
  TUint8 result;
  if (aRegister > 0x07 && aRegister < 0x0C) {
    ide_write(aChannel, ATA_REG_CONTROL, 0x80 | mChannels[aChannel].nIEN);
  }
  if (aRegister < 0x08) {
    result = inb(mChannels[aChannel].mBasePort + aRegister - 0x00);
  }
  else if (aRegister < 0x0C) {
    result = inb(mChannels[aChannel].mBasePort + aRegister - 0x06);
  }
  else if (aRegister < 0x0E) {
    result = inb(mChannels[aChannel].mControlBase + aRegister - 0x0A);
  }
  else if (aRegister < 0x16) {
    result = inb(mChannels[aChannel].mDmaPort + aRegister - 0x0E);
  }

  if (aRegister > 0x07 && aRegister < 0x0C) {
    ide_write(aChannel, ATA_REG_CONTROL, mChannels[aChannel].nIEN);
  }

  return result;
}

void AtaTask::ide_read_buffer(TUint8 aChannel, TUint8 aRegister, TUint8 *aBuffer, TUint32 aCount) {
  if (aRegister > 0x07 && aRegister < 0x0c) {
    ide_write(aChannel, ATA_REG_CONTROL, 0x80 | mChannels[aChannel].nIEN);
  }
  if (aRegister < 0x08) {
    insl(mChannels[aChannel].mBasePort + aRegister - 0x00, aBuffer, aCount);
  }
  else if (aRegister < 0x0c) {
    insl(mChannels[aChannel].mBasePort + aRegister - 0x06, aBuffer, aCount);
  }
  else if (aRegister < 0x0e) {
    insl(mChannels[aChannel].mBasePort + aRegister - 0x0a, aBuffer, aCount);
  }
  else if (aRegister < 0x16) {
    insl(mChannels[aChannel].mBasePort + aRegister - 0x0e, aBuffer, aCount);
  }

  if (aRegister > 0x07 && aRegister < 0x0C) {
    ide_write(aChannel, ATA_REG_CONTROL, mChannels[aChannel].nIEN);
  }
}

TInt AtaTask::ide_polling(TUint8 aChannel, TBool aAdvancedCheck) {
  // delay 400ms
  ide_read(aChannel, ATA_REG_ALTSTATUS); // 100ms
  ide_read(aChannel, ATA_REG_ALTSTATUS); // +100ms
  ide_read(aChannel, ATA_REG_ALTSTATUS); // +100ms
  ide_read(aChannel, ATA_REG_ALTSTATUS); // +100ms

  while (ide_read(aChannel, ATA_REG_STATUS) & ATA_SR_BSY) {
    // wait for not busy
  }

  // if aAdvancedCheci is set, we check for various failures
  if (aAdvancedCheck) {
    TUint8 state = ide_read(aChannel, ATA_REG_STATUS);
    if (state & ATA_SR_ERR) {
      return 2; // error
    }
    if (state & ATA_SR_DF) {
      return 1; // device fault
    }
    if ((state & ATA_SR_DRQ) == 0) {
      return 3; // DRQ should be set!
    }
  }
  return 0; // no error
}

TUint8 AtaTask::ide_print_error(TUint aDrive, TUint8 aError) {
  TUint8 err = aError,
         st;

  DSPACE();
  DLOG("*** IDE ERROR:\n");

  switch (err) {
    case 1:
      dlog("  Device Fault\n");
      err = 19;
      break;
    case 2:
      st = ide_read(mDevices[aDrive].Channel, ATA_REG_ERROR);
      if (st & ATA_ER_AMNF) {
        dlog("  No address mark found\n");
        err = 7;
      }
      if (st & ATA_ER_TK0NF) {
        dlog("  No media or media error\n");
        err = 3;
      }
      if (st & ATA_ER_ABRT) {
        dlog("  Command aborted\n");
        err = 20;
      }
      if (st & ATA_ER_MCR) {
        dlog("  No media or media error\n");
        err = 3;
      }
      if (st & ATA_ER_IDNF) {
        dlog("  ID mark not found\n");
        err = 21;
      }
      if (st & ATA_ER_MC) {
        dlog("  No media or media error\n");
        err = 3;
      }
      if (st & ATA_ER_UNC) {
        dlog("  Uncorrectable data error\n");
        err = 22;
      }
      if (st & ATA_ER_BBK) {
        dlog("  Bad sectors\n");
        err = 13;
      }
      break;
    case 3:
      dlog("  Reads nothing\n");
      err = 23;
      break;
    case 4:
      dlog("  Model(%s) Write Protected (%s)/(%s)\n",
        mDevices[aDrive].Model,
        mDevices[aDrive].Channel ? "Secondary" : "Primary",
        mDevices[aDrive].Drive ? "Slave" : "Master");
      break;
  }

  return err;
}

void AtaTask::ide_initialize(TUint32 BAR0, TUint32 BAR1, TUint32 BAR2, TUint32 BAR3, TUint32 BAR4) {
  DLOG("  ide_initialize(0x%0x, 0x%0x, 0x%0x, 0x%0, 0x%0xx)\n",
    BAR0, BAR1, BAR2, BAR3, BAR4);

  // Sleep(3);

  TUint8 buffer[512];
  // set up controller ports
  mChannels[ATA_PRIMARY].mBasePort = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
  mChannels[ATA_PRIMARY].mControlBase = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
  mChannels[ATA_SECONDARY].mBasePort = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
  mChannels[ATA_SECONDARY].mControlBase = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
  mChannels[ATA_PRIMARY].mDmaPort = (BAR4 & 0xFFFFFFFC) + 0;   // Bus Master IDE
  mChannels[ATA_SECONDARY].mDmaPort = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE

  // disable IRQs
  ide_write(ATA_PRIMARY, ATA_REG_CONTROL, 2);
  ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

  // scan for drives connected to each channel
  TInt count = 0;
  for (TUint8 i = 0; i < 2; i++) {
    for (TUint8 j = 0; j < 2; j++) { // master, slave

      TUint8 err = 0, type = IDE_ATA, status;
      mDevices[count].Reserved = 0; // Assuming that no drive here.
      DSPACE();
      // (I) Select Drive:
      DLOG("  1) SELECT DRIVE %02x\n", 0xa0 | (j << 4));
      ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
      // ide_read(i, ATA_REG_ALTSTATUS);                  // 100ms
      // Wait();
      // MilliSleep(1); // Wait 1ms for drive select to work.
      // wait_io();
      delay();

      // (II) Send ATA Identify Command:
      DLOG("  2) SEND ATA_CMD_IDENTIFY\n");
      ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
      // ide_read(i, ATA_REG_ALTSTATUS);                  // 100ms
      // Wait();
      // MilliSleep(1); // This function should be implemented in your OS. which waits for 1 ms.
      // wait_io();
      delay();
      // it is based on System Timer Device Driver.

      // (III) Polling:
      DLOG("  3) POLLING\n");
      if (ide_read(i, ATA_REG_STATUS) == 0) {
        DLOG("   --> bus(%d) slave(%d) no device\n", i, j);
        continue; // If Status = 0, No Device.
      }

      for (;;) {
        status = ide_read(i, ATA_REG_STATUS);
        if ((status & ATA_SR_ERR)) {
          err = 1;
          break;
        } // If Err, Device is not ATA.
        if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) {
          break; // Everything is right.
        }
      }

      // (IV) Probe for ATAPI Devices:
      if (err != 0) {
        DLOG("  4) Probe for ATAPI\n");
        TUint8 cl = ide_read(i, ATA_REG_LBA1);
        TUint8 ch = ide_read(i, ATA_REG_LBA2);
        DLOG("cl/LBA1(%02x) ch/LBA2(%02x)\n", cl, ch);

        if (cl == 0x14 && ch == 0xEB) {
          DLOG("  ATAPI\n");
          type = IDE_ATAPI;
        }
        else if (cl == 0x69 && ch == 0x96) {
          DLOG("  ATAPI\n");
          type = IDE_ATAPI;
        }
        else {
          DLOG("    *** Unknown device!\n");
          continue; // Unknown Type (may not be a device).
        }

        ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
        // MilliSleep(1);
        delay();
        // wait_io();
        // Wait();
      }

      // (V) Read Identification Space of the Device:
      DLOG("  5) READ IDENTIFY PACKET\n");
      ide_read_buffer(i, ATA_REG_DATA, &buffer[0], 128);
      // dhexdump(buffer, 64);

      // (VI) Read Device Parameters:
      DLOG("  6) READ DEVICE PARAMETERS\n");
      TAtaIdentity *p = (TAtaIdentity *)buffer;
      mDevices[count].Reserved = 1;
      mDevices[count].Type = type;
      mDevices[count].Channel = i;
      mDevices[count].Drive = j;
      mDevices[count].Signature = *((TUint16 *)(buffer + ATA_IDENT_DEVICETYPE));
      mDevices[count].Capabilities = *((TUint16 *)(buffer + ATA_IDENT_CAPABILITIES));
      mDevices[count].CommandSets = *((TUint32 *)(buffer + ATA_IDENT_COMMANDSETS));

      // (VII) Get Size:
      DLOG("  7) Get Size\n");
      if (mDevices[count].CommandSets & (1 << 26)) {
        // Device uses 48-Bit Addressing:
        mDevices[count].Size = *((TUint32 *)(buffer + ATA_IDENT_MAX_LBA_EXT));
        mDevices[count].mLba48 = ETrue;
      }
      else {
        // Device uses CHS or 28-bit Addressing:
        mDevices[count].Size = *((TUint32 *)(buffer + ATA_IDENT_MAX_LBA));
        mDevices[count].mLba48 = EFalse;
      }

      if (p->mLba48 & (1 << 10)) {
        mDevices[count].mLba48 = ETrue;
      }

      // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
      DLOG("  7) Get Model\n");
      for (TInt k = 0; k < 40; k += 2) {
        mDevices[count].Model[k] = buffer[ATA_IDENT_MODEL + k + 1];
        mDevices[count].Model[k + 1] = buffer[ATA_IDENT_MODEL + k];
      }
      mDevices[count].Model[40] = 0; // Terminate String.
      // mDevices[count].mLba48 = *(TUint16 *)(buffer + ATA_IDENT_LBA48) & (1 << 10) ? ETrue : EFalse;
      count++;
    }
  }

  // 4- Print Summary:
  DSPACE();
  DLOG("==============\n");
  DLOG("IDE Devices:\n");
  for (TInt i = 0; i < 4; i++)
    mDevices[i].Dump();
  DLOG("==============\n");
  DSPACE();
}

TBool AtaTask::ide_io(TIdeDevice *aIdeDevice, TUint64 aLba, TBool aWrite, TUint8 *aBuffer, TInt aNumSectors) {
  TBool dma = mDevice->BusMasterPort() != 0;
  if (!bochs_present) {
    dma = EFalse;
  }

  if (dma && (aWrite == EFalse)) {
    return mDma->Read(aLba, aBuffer, aNumSectors);
  }
  TInt lba_mode = 0,
       cmd;

  TUint8 lba_io[6];

  unsigned int channel = aIdeDevice->Channel;      // Read the Channel.
  unsigned int slavebit = aIdeDevice->Drive;       // Read the Drive [Master/Slave]
  unsigned int bus = mChannels[channel].mBasePort; // Bus Base, like 0x1F0 which is also data port.
  unsigned int words = 256;                        // Almost every ATA drive has a sector-size of 512-byte.
  unsigned short cyl, i;
  unsigned char head, sect, err;

  if (aIdeDevice->mLba48) {
    lba_mode = 2;
    lba_io[0] = (aLba & 0x000000FF) >> 0;
    lba_io[1] = (aLba & 0x0000FF00) >> 8;
    lba_io[2] = (aLba & 0x00FF0000) >> 16;
    lba_io[3] = (aLba & 0xFF000000) >> 24;
    lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
    lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
    head = 0;      // Lower 4-bits of HDDEVSEL are not used here.
  }
  else if (aIdeDevice->Capabilities & 0x200) {
    // LBA28:
    lba_mode = 1;
    lba_io[0] = (aLba & 0x00000FF) >> 0;
    lba_io[1] = (aLba & 0x000FF00) >> 8;
    lba_io[2] = (aLba & 0x0FF0000) >> 16;
    lba_io[3] = 0; // These Registers are not used here.
    lba_io[4] = 0; // These Registers are not used here.
    lba_io[5] = 0; // These Registers are not used here.
    head = (aLba & 0xF000000) >> 24;
  }
  else {
    // CHS:
    lba_mode = 0;
    sect = (aLba % 63) + 1;
    cyl = (aLba + 1 - sect) / (16 * 63);
    lba_io[0] = sect;
    lba_io[1] = (cyl >> 0) & 0xFF;
    lba_io[2] = (cyl >> 8) & 0xFF;
    lba_io[3] = 0;
    lba_io[4] = 0;
    lba_io[5] = 0;
    head = (aLba + 1 - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
  }

  // TBool dma = EFalse; // dma not detected (yet)
  while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY)
    ; // Wait if busy.

  if (lba_mode == 0) {
    ide_write(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS.
  }
  else {
    ide_write(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA
  }

  if (lba_mode == 2) {
    ide_write(channel, ATA_REG_SECCOUNT1, 0);
    ide_write(channel, ATA_REG_LBA3, lba_io[3]);
    ide_write(channel, ATA_REG_LBA4, lba_io[4]);
    ide_write(channel, ATA_REG_LBA5, lba_io[5]);
  }
  ide_write(channel, ATA_REG_SECCOUNT0, aNumSectors);
  ide_write(channel, ATA_REG_LBA0, lba_io[0]);
  ide_write(channel, ATA_REG_LBA1, lba_io[1]);
  ide_write(channel, ATA_REG_LBA2, lba_io[2]);

  if (lba_mode == 0 && dma == 0 && aWrite == EFalse) {
    cmd = ATA_CMD_READ_PIO;
  }
  if (lba_mode == 1 && dma == 0 && aWrite == EFalse) {
    cmd = ATA_CMD_READ_PIO;
  }
  if (lba_mode == 2 && dma == 0 && aWrite == EFalse) {
    cmd = ATA_CMD_READ_PIO_EXT;
  }
  if (lba_mode == 0 && dma == 1 && aWrite == EFalse) {
    cmd = ATA_CMD_READ_DMA;
  }
  if (lba_mode == 1 && dma == 1 && aWrite == EFalse) {
    cmd = ATA_CMD_READ_DMA;
  }
  if (lba_mode == 2 && dma == 1 && aWrite == EFalse) {
    cmd = ATA_CMD_READ_DMA_EXT;
  }
  if (lba_mode == 0 && dma == 0 && aWrite == ETrue) {
    cmd = ATA_CMD_WRITE_PIO;
  }
  if (lba_mode == 1 && dma == 0 && aWrite == ETrue) {
    cmd = ATA_CMD_WRITE_PIO;
  }
  if (lba_mode == 2 && dma == 0 && aWrite == ETrue) {
    cmd = ATA_CMD_WRITE_PIO_EXT;
  }
  if (lba_mode == 0 && dma == 1 && aWrite == ETrue) {
    cmd = ATA_CMD_WRITE_DMA;
  }
  if (lba_mode == 1 && dma == 1 && aWrite == ETrue) {
    cmd = ATA_CMD_WRITE_DMA;
  }
  if (lba_mode == 2 && dma == 1 && aWrite == ETrue) {
    cmd = ATA_CMD_WRITE_DMA_EXT;
  }

  ide_write(channel, ATA_REG_COMMAND, cmd); // Send the Command.

  // read or write the data
  if (dma) {
    DLOG("*** DMA not implemented!\n");
    if (aWrite) {
      // DMA Write
    }
    else {
      // DMA Read
    }
    return EFalse;
  }

  TUint8 *ptr = aBuffer;
  if (aWrite) {
    // write sectors
    for (TInt i = 0; i < aNumSectors; i++) {
      if ((err = ide_polling(channel, 0))) {
        DLOG("*** ide_polling error(%d)\n", err);
        bochs;
        return EFalse;
      }
      outsw(bus, ptr, words);
      ptr = &ptr[words];
    }
    // TODO this is ugly!
    ide_write(channel, ATA_REG_COMMAND, (TUint8[]){ ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT }[lba_mode]);
    ide_polling(channel, 0); // Polling.
  }
  else {
    // read sectors
    for (TInt i = 0; i < aNumSectors; i++) {
      if ((err = ide_polling(channel, 0))) {
        DLOG("*** ide_polling error(%d)\n", err);
        bochs;
        return EFalse;
      }
      insw(bus, ptr, words);
      ptr = &ptr[words];
    }
  }
  return ETrue;
}

TBool AtaTask::ide_read_blocks(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TUint64 aCount) {
  DLOG("ide_read_blocks(%x) aLba(%d) aBuffer(%x) aCount(%d)\n", aIdeDevice, aLba, aBuffer, aCount);
  return ide_io(aIdeDevice, aLba, EFalse, aBuffer, aCount);
}

TBool AtaTask::ide_write_blocks(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TUint64 aCount) {
  DLOG("ide_write_blocks(%x) aLba(%d) aBuffer(%x) aCount(%d)\n", aIdeDevice, aLba, aBuffer, aCount);
  return ide_io(aIdeDevice, aLba, ETrue, aBuffer, aCount);
}
