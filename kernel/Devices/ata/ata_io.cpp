#define DEBUGME
// #undef DEBUGME

#include "ata_io.h"
// #include "dma.h"

void AtaTask::write_register(TUint8 aChannel, TUint8 aRegister, TUint8 aData) {
  if (aRegister > 0x07 && aRegister < 0x0C) {
    write_register(aChannel, ATA_REG_CONTROL, 0x80 | mChannels[aChannel].mNoInterrupt);
  }

  if (aRegister < 0x08) {
    outb(mChannels[aChannel].mBase + aRegister - 0x00, aData);
  }
  else if (aRegister < 0x0C) {
    outb(mChannels[aChannel].mBase + aRegister - 0x06, aData);
  }
  else if (aRegister < 0x0E) {
    outb(mChannels[aChannel].mControl + aRegister - 0x0A, aData);
  }
  else if (aRegister < 0x16) {
    outb(mChannels[aChannel].mBusMasterIde + aRegister - 0x0E, aData);
  }
  if (aRegister > 0x07 && aRegister < 0x0C) {
    write_register(aChannel, ATA_REG_CONTROL, mChannels[aChannel].mNoInterrupt);
  }
}

TUint8 AtaTask::read_register(TUint8 aChannel, TUint8 aRegister) {
  TUint8 result;
  if (aRegister > 0x07 && aRegister < 0x0C) {
    write_register(aChannel, ATA_REG_CONTROL, 0x80 | mChannels[aChannel].mNoInterrupt);
  }
  if (aRegister < 0x08) {
    result = inb(mChannels[aChannel].mBase + aRegister - 0x00);
  }
  else if (aRegister < 0x0C) {
    result = inb(mChannels[aChannel].mBase + aRegister - 0x06);
  }
  else if (aRegister < 0x0E) {
    result = inb(mChannels[aChannel].mControl + aRegister - 0x0A);
  }
  else if (aRegister < 0x16) {
    result = inb(mChannels[aChannel].mBusMasterIde + aRegister - 0x0E);
  }

  if (aRegister > 0x07 && aRegister < 0x0C) {
    write_register(aChannel, ATA_REG_CONTROL, mChannels[aChannel].mNoInterrupt);
  }
  return result;
}

void AtaTask::read_buffer(TUint8 aChannel, TUint8 aRegister, TUint8 *aBuffer, TUint32 aWordCount) {
  if (aRegister > 0x07 && aRegister < 0x0c) {
    write_register(aChannel, ATA_REG_CONTROL, 0x80 | mChannels[aChannel].mNoInterrupt);
  }

  if (aRegister < 0x08) {
    insl(mChannels[aChannel].mBase + aRegister - 0x00, aBuffer, aWordCount);
  }
  else if (aRegister < 0x0c) {
    insl(mChannels[aChannel].mBase + aRegister - 0x06, aBuffer, aWordCount);
  }
  else if (aRegister < 0x0e) {
    insl(mChannels[aChannel].mBase + aRegister - 0x0a, aBuffer, aWordCount);
  }
  else if (aRegister < 0x16) {
    insl(mChannels[aChannel].mBase + aRegister - 0x0e, aBuffer, aWordCount);
  }

  if (aRegister > 0x07 && aRegister < 0x0C) {
    write_register(aChannel, ATA_REG_CONTROL, mChannels[aChannel].mNoInterrupt);
  }
}

void AtaTask::io_wait(TUint8 aChannel) {
  // delay 400ms
  read_register(aChannel, ATA_REG_ALTSTATUS); // 100ms
  read_register(aChannel, ATA_REG_ALTSTATUS); // +100ms
  read_register(aChannel, ATA_REG_ALTSTATUS); // +100ms
  read_register(aChannel, ATA_REG_ALTSTATUS); // +100ms
}

TInt AtaTask::wait_ready(TUint8 aChannel, TBool aAdvancedCheck) {
  io_wait(aChannel);

  while (read_register(aChannel, ATA_REG_STATUS) & ATA_SR_BSY) {
    // wait for not busy
  }

  // if aAdvancedCheck is set, we check for various failures
  if (aAdvancedCheck) {
    TUint8 state = read_register(aChannel, ATA_REG_STATUS);
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

TUint8 AtaTask::print_error(TUint aDrive, TUint8 aError) {
  TUint8 err = aError,
         st;

  DPRINT("\n\n");
  DLOG("*** IDE ERROR:\n");

  switch (err) {
    case 1:
      DLOG("  Device Fault\n");
      err = 19;
      break;

    case 2:
      st = read_register(mDevices[aDrive].mChannel, ATA_REG_ERROR);
      if (st & ATA_ER_AMNF) {
        DLOG("  No address mark found\n");
        err = 7;
      }
      if (st & ATA_ER_TK0NF) {
        DLOG("  No media or media error\n");
        err = 3;
      }
      if (st & ATA_ER_ABRT) {
        DLOG("  Command aborted\n");
        err = 20;
      }
      if (st & ATA_ER_MCR) {
        DLOG("  No media or media error\n");
        err = 3;
      }
      if (st & ATA_ER_IDNF) {
        DLOG("  ID mark not found\n");
        err = 21;
      }
      if (st & ATA_ER_MC) {
        DLOG("  No media or media error\n");
        err = 3;
      }
      if (st & ATA_ER_UNC) {
        DLOG("  Uncorrectable data error\n");
        err = 22;
      }
      if (st & ATA_ER_BBK) {
        DLOG("  Bad sectors\n");
        err = 13;
      }
      break;

    case 3:
      DLOG("  Reads nothing\n");
      err = 23;
      break;

    case 4:
      DLOG("  Model(%s) Write Protected (%s)/(%s)\n",
        mDevices[aDrive].mModel,
        mDevices[aDrive].mChannel ? "Secondary" : "Primary",
        mDevices[aDrive].mDrive ? "Slave" : "Master");
      break;
  }

  return err;
}

void AtaTask::initialize(TUint32 BAR0, TUint32 BAR1, TUint32 BAR2, TUint32 BAR3, TUint32 BAR4) {
  DLOG("  initialize(0x%0x, 0x%0x, 0x%0x, 0x%0, 0x%0xx)\n",
    BAR0, BAR1, BAR2, BAR3, BAR4);

  TUint8 buffer[512];
  // set up controller ports
  mChannels[ATA_PRIMARY].mBase = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
  mChannels[ATA_PRIMARY].mControl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
  mChannels[ATA_SECONDARY].mBase = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
  mChannels[ATA_SECONDARY].mControl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
  mChannels[ATA_PRIMARY].mBusMasterIde = (BAR4 & 0xFFFFFFFC) + 0;   // Bus Master IDE
  mChannels[ATA_SECONDARY].mBusMasterIde = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE

  // disable IRQs
  write_register(ATA_PRIMARY, ATA_REG_CONTROL, 2);
  write_register(ATA_SECONDARY, ATA_REG_CONTROL, 2);

  // scan for drives connected to each channel
  TInt count = 0;
  for (TUint8 i = 0; i < 2; i++)
    for (TUint8 j = 0; j < 2; j++) {

      TUint8 err = 0, type = IDE_ATA, status;
      mDevices[count].mReserved = 0; // Assuming that no drive here.

      // (I) Select Drive:
      write_register(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
      MilliSleep(1);                                        // Wait 1ms for drive select to work.

      // (II) Send ATA Identify Command:
      write_register(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
      MilliSleep(1); // This function should be implemented in your OS. which waits for 1 ms.
                     // it is based on System Timer Device Driver.

      // (III) Polling:
      if (read_register(i, ATA_REG_STATUS) == 0) {
        // DLOG("  %d no device\n", i);
        continue; // If Status = 0, No Device.
      }

      while (ETrue) {
        status = read_register(i, ATA_REG_STATUS);
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
        TUint8 cl = read_register(i, ATA_REG_LBA1);
        TUint8 ch = read_register(i, ATA_REG_LBA2);

        if (cl == 0x14 && ch == 0xEB)
          type = IDE_ATAPI;
        else if (cl == 0x69 && ch == 0x96)
          type = IDE_ATAPI;
        else
          continue; // Unknown Type (may not be a device).

        write_register(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
        MilliSleep(1);
      }

      // (V) Read Identification Space of the Device:
      read_buffer(i, ATA_REG_DATA, &buffer[0], 128);
      // dhexdump(buffer, 64);

      // (VI) Read Device Parameters:
      TAtaIdentity *p = (TAtaIdentity *)buffer;
      mDevices[count].mReserved = 1;
      mDevices[count].mType = type;
      mDevices[count].mChannel = i;
      mDevices[count].mDrive = j;
      mDevices[count].mSignature = *((TUint16 *)(buffer + ATA_IDENT_DEVICETYPE));
      mDevices[count].mCapabilities = *((TUint16 *)(buffer + ATA_IDENT_CAPABILITIES));
      mDevices[count].mCommandSets = *((TUint32 *)(buffer + ATA_IDENT_COMMANDSETS));

      // (VII) Get Size:
      if (mDevices[count].mCommandSets & (1 << 26)) {
        // Device uses 48-Bit Addressing:
        mDevices[count].mSize = *((TUint32 *)(buffer + ATA_IDENT_MAX_LBA_EXT));
        mDevices[count].mLba48 = ETrue;
      }
      else {
        // Device uses CHS or 28-bit Addressing:
        mDevices[count].mSize = *((TUint32 *)(buffer + ATA_IDENT_MAX_LBA));
        mDevices[count].mLba48 = EFalse;
      }

      if (p->mLba48 & (1 << 10)) {
        mDevices[count].mLba48 = ETrue;
      }

      // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
      for (TInt k = 0; k < 40; k += 2) {
        mDevices[count].mModel[k] = buffer[ATA_IDENT_MODEL + k + 1];
        mDevices[count].mModel[k + 1] = buffer[ATA_IDENT_MODEL + k];
      }
      mDevices[count].mModel[40] = 0; // Terminate String.
      // mDevices[count].mLba48 = *(TUint16 *)(buffer + ATA_IDENT_LBA48) & (1 << 10) ? ETrue : EFalse;
      count++;
    }

  // 4- Print Summary:
  DPRINT("\n\n");
  DLOG("==============\n");
  DLOG("IDE Devices:\n");
  for (TInt i = 0; i < 4; i++) {
    mDevices[i].Dump();
  }
  DLOG("==============\n");
  DPRINT("\n\n");
}

TBool AtaTask::do_io(TIdeDevice *aIdeDevice, TUint64 aLba, TBool aWrite, TUint8 *aBuffer, TInt aNumSectors) {
  // TBool dma = mDevice->BusMasterPort();
  TBool dma = 0;

  if (false && dma) {
    return aWrite ? dma_write(aIdeDevice, aLba, aBuffer, aNumSectors) : dma_read(aIdeDevice, aLba, aBuffer, aNumSectors);
  }
  else {
    return aWrite ? pio_write(aIdeDevice, aLba, aBuffer, aNumSectors) : pio_read(aIdeDevice, aLba, aBuffer, aNumSectors);
  }

  #if 0
  TInt lba_mode = 0,
       cmd;

  TUint8 lba_io[6];

  TUint channel = aIdeDevice->mChannel; // Read the Channel.
  TUint slavebit = aIdeDevice->mDrive;  // Read the Drive [Master/Slave]
  TUint bus = mChannels[channel].mBase; // Bus Base, like 0x1F0 which is also data port.
  TUint words = 256;                    // Almost every ATA drive has a sector-size of 512-byte.
  TUint16 cyl, i;
  TUint8 head, sect, err;

  DSPACE();

  if (false && aIdeDevice->mLba48) {
    DLOG("  LBA48\n");
    lba_mode = 2;
    lba_io[0] = (aLba & 0x000000FF) >> 0;
    lba_io[1] = (aLba & 0x0000FF00) >> 8;
    lba_io[2] = (aLba & 0x00FF0000) >> 16;
    lba_io[3] = (aLba & 0xFF000000) >> 24;
    lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
    lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
    head = 0;      // Lower 4-bits of HDDEVSEL are not used here.
  }
  else if (aIdeDevice->mCapabilities & 0x200) {
    DLOG("  LBA28\n");
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
    DLOG("  CHS\n");
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

  dma = 0;
  while (read_register(channel, ATA_REG_STATUS) & ATA_SR_BSY) {
    // Wait while busy.
  };

  if (lba_mode == 0) {
    write_register(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS.
  }
  else {
    write_register(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA
  }

  if (lba_mode == 2) {
    DLOG("   Write LBA48 registers\n");
    write_register(channel, ATA_REG_SECCOUNT1, 0);
    write_register(channel, ATA_REG_LBA3, lba_io[3]);
    write_register(channel, ATA_REG_LBA4, lba_io[4]);
    write_register(channel, ATA_REG_LBA5, lba_io[5]);
  }
  DLOG("   Write LBAregisters\n");
  write_register(channel, ATA_REG_SECCOUNT0, 1);
  write_register(channel, ATA_REG_LBA0, lba_io[0]);
  write_register(channel, ATA_REG_LBA1, lba_io[1]);
  write_register(channel, ATA_REG_LBA2, lba_io[2]);

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

  DLOG("  cmd = 0x%02x\n", cmd);
  write_register(channel, ATA_REG_COMMAND, cmd); // Send the Command.

  TUint8 *ptr = aBuffer;
  if (aWrite) {

    // write sectors
    for (TInt i = 0; i < aNumSectors; i++) {
      if ((err = wait_ready(channel, 0))) {
        DLOG("*** wait_ready error(%d)\n", err);
        bochs;
        return EFalse;
      }
      outsw(bus, ptr, words);
      ptr = &ptr[words];
    }

    // TODO this is ugly!
    write_register(channel, ATA_REG_COMMAND,
      (TUint8[]){ ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT }[lba_mode]);
    wait_ready(channel, 0); // Polling.

  }
  else {
    DLOG("  READ SECTORS(%d)\n", aNumSectors);
    // read sectors
    for (TInt i = 0; i < aNumSectors; i++) {
      if ((err = wait_ready(channel, 0))) {
        DLOG("*** wait_ready error(%d)\n", err);
        bochs;
        return EFalse;
      }
      dlog("PIO ");
      for (TInt n = 0; n < 512; n++) {
	TUint8 b = inb(bus);
	dprint("%02x ", b);

	*ptr++ = b;
      }
      break;
      // insw(bus, ptr, words);
      // ptr = &ptr[words];
    }
    DLOG("  READ COMPLETED\n");
  }
  return ETrue;
  #endif
}
