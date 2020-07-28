#define DEBUGME
#undef DEBUGME

#include "ata.hpp"
#include "pio.hpp"
#include "task.hpp"

static void delay400ms(TUint16 aBase) {
  inb(aBase + 7); // 1000ms
  inb(aBase + 7); // +1000ms
  inb(aBase + 7); // +1000ms
  inb(aBase + 7); // +1000ms
}

static TBool poll(TUint16 aBase) {
  delay400ms(aBase);
  for (;;) {
    TUint8 status = inb(aBase + 7);
    if (!(status & ATA_SR_BSY) || (status & ATA_SR_DRQ)) {
      DLOG("poll succeed status: %02x ATA_SR_BSY(%02x) ATA_SR_DRQ(%02x)\n", status, ATA_SR_BSY, ATA_SR_DRQ);
      return ETrue;
    }
    if ((status & ATA_SR_ERR) || (status & ATA_SR_DF)) {
      DLOG("poll fail status: %02x ATA_SR_ERR(%02x) ATA_SR_DF(%02x)\n", status, ATA_SR_ERR, ATA_SR_DF);
      return EFalse;
    }
  }
}

PIO::PIO(AtaTask *aTask) : BBase() {
  mTask = aTask;
  mIdeDevice = mTask->IdeDevice();
}

TBool PIO::ReadSector(TUint64 aLba, TAny *aBuffer) {
  TUint16 base = mTask->AtaBasePort();
  if (mIdeDevice->mLba48) {
    // dlog("lba48\n");
    outb(base + ATA_REG_HDDEVSEL, (mIdeDevice->mSlave ? 0xf0 : 0xe0) | ((aLba >> 24) & 0x0f));
    delay400ms(base);
    outb(base + ATA_REG_FEATURES, 0x00);
    outb(base + ATA_REG_SECCOUNT1, 0);
    outb(base + ATA_REG_LBA0, (TUint8)(aLba >> 24));
    outb(base + ATA_REG_LBA1, (TUint8)(aLba >> 32));
    outb(base + ATA_REG_LBA2, (TUint8)(aLba >> 40));
    outb(base + ATA_REG_SECCOUNT0, 1);
    outb(base + ATA_REG_LBA0, (TUint8)aLba);
    outb(base + ATA_REG_LBA1, (TUint8)(aLba >> 8));
    outb(base + ATA_REG_LBA2, (TUint8)(aLba >> 16));
    outb(base + ATA_REG_COMMAND, 0x24);
  }
  else {
    outb(base + ATA_REG_HDDEVSEL, (mIdeDevice->mSlave ? 0xf0 : 0xe0) | ((aLba >> 24) & 0x0f));
    delay400ms(base);
    outb(base + ATA_REG_FEATURES, 0x00);
    outb(base + ATA_REG_SECCOUNT0, 1);
    outb(base + ATA_REG_LBA0, (TUint8)aLba);
    outb(base + ATA_REG_LBA1, (TUint8)(aLba >> 8));
    outb(base + ATA_REG_LBA2, (TUint8)(aLba >> 16));
    outb(base + ATA_REG_COMMAND, 0x20);
  }
  DLOG("about to poll\n");
  if (!poll(base)) {
    dlog("  *** PIO polling failed\n");
    // return EFalse;
  }
  TUint16 *p = (TUint16 *)aBuffer;
  for (TInt i = 0; i < PIO_IO_SIZE / 2; i++) {
    *p++ = inw(base);
  }
  // insw(base, aBuffer, PIO_IO_SIZE);
  // dhexdump(aBuffer, 32);
  return ETrue;
}

TBool PIO::ReadSectors(TUint64 aLba, TAny *aBuffer, TInt32 aNumSectors) {
  TUint8 *buf = (TUint8 *)aBuffer;
  for (TInt32 i = 0; i < aNumSectors; i++) {
    if (!ReadSector(aLba + i, &buf[i * PIO_IO_SIZE])) {
      return EFalse;
    }
  }
  return ETrue;
}

TBool PIO::WriteSector(TUint64 aLba, TAny *aBuffer) {
  return ETrue;
}

TBool PIO::WriteSectors(TUint64 aLba, TAny *aBuffer, TInt32 aNumSectors) {
  TUint8 *buf = (TUint8 *)aBuffer;
  for (TInt32 i = 0; i < aNumSectors; i++) {
    if (!WriteSector(aLba + i, &buf[i * PIO_IO_SIZE])) {
      return EFalse;
    }
  }
  return ETrue;
}

#if 0
  TInt lba_mode = 0,
       cmd;

  TUint8 lba_io[6];

  unsigned int channel = aIdeDevice->mChannel;      // Read the Channel.
  unsigned int slavebit = aIdeDevice->mSlave;       // Read the Drive [Master/Slave]
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
  else if (aIdeDevice->mCapabilities & 0x200) {
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
#endif
