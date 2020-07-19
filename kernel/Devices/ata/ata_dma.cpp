#define DEBUGME
// #undef DEBUGME

#include "ata_io.h"
#include "ata_dma.h"

TBool AtaTask::dma_read_sector(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer) {
  DSPACE();
  DLOG("dma_read_sector(%x) aLba(%x) aBuffer(%x)\n", aIdeDevice, aLba, aBuffer);
  aIdeDevice->Dump();

  TUint32 bm = mDevice->BusMasterPort();

  TUint64 buffer_address = (TUint64)mSectorBuffer,
         prdt_address = (TUint64)mPrdt;

  TUint channel = aIdeDevice->mChannel; // Read the Channel.

  DLOG("  dma_io bm(0x%x) prdt_address(0x%x) buffer_address(0x%x)\n", bm, prdt_address, buffer_address);

  wait_ready(channel, 0);

  // stop
  outb(bm, 0x00);

  // set the prdt
  outl(bm + 0x04, prdt_address);

  // enable error/irq status
  outb(bm + 0x02, inb(bm + 0x02) | 0x04 | 0x02);

  // set read
  outb(bm, 0x08);

  wait_ready(channel, 0);

  // ATA drive controls
  write_register(channel, ATA_REG_CONTROL, 0x00);
  write_register(channel, ATA_REG_HDDEVSEL, 0xe0 | (aIdeDevice->mDrive << 4));

  io_wait(channel);
  write_register(channel, ATA_REG_FEATURES, 0x00);

  write_register(channel, ATA_REG_SECCOUNT0, 0);
  write_register(channel, ATA_REG_LBA0, (aLba & 0xff000000) >> 24);
  write_register(channel, ATA_REG_LBA1, (aLba & 0xff00000000) >> 32);
  write_register(channel, ATA_REG_LBA2, (aLba & 0xff0000000000) >> 40);

  write_register(channel, ATA_REG_SECCOUNT0, 0);
  write_register(channel, ATA_REG_LBA0, (aLba & 0x0000ff) >> 0);
  write_register(channel, ATA_REG_LBA1, (aLba & 0x00ff00) >> 0);
  write_register(channel, ATA_REG_LBA2, (aLba & 0xff0000) >> 0);

  write_register(channel, ATA_REG_COMMAND, DMA_READ_LBA48);
  io_wait(channel);

  // start the DMA!
  DLOG("    START DMA\n");
  outb(bm + 0x00, 0x08 | 0x01);

  // wait for IO to complete
  DLOG("    WAIT DMA\n");
  Wait(mSigMask);
  DLOG("    DMA COMPLETE\n");

#if 1
  while (1) {
    int status = inb(bm + 0x02);
    int dstatus = read_register(channel, ATA_REG_STATUS);
    if (!(status & 0x04)) {
      continue;
    }
    if (!(dstatus & ATA_SR_BSY)) {
      break;
    }
  }
#endif

  // copy memory from our CHIP RAM to the caller's buffer
  CopyMemory(aBuffer, mSectorBuffer, ATA_IO_SIZE);

  // ack DMA
  outb(bm + 2, inb(bm+2) | 0x02 | 0x04);

  return ETrue;
}

TBool AtaTask::dma_read(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TInt aNumSectors) {
  for (TInt n = 0; n < aNumSectors; n++) {
    if (dma_read_sector(aIdeDevice, aLba, &aBuffer[n & 512])) {
      return EFalse;
    }
    aLba++;
  }
  return ETrue;
}

TBool AtaTask::dma_write_sector(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer) {
  // we DMA from mSectorBuffer which is in CHIP RAM (can't access caller's buffer)
  CopyMemory(mSectorBuffer, aBuffer, 512);

  TUint32 bm = mDevice->BusMasterPort();

  TUint64 buffer_address = (TUint64)mSectorBuffer,
          prdt_address = (TUint64)mPrdt;

  TPrd *prdt = (TPrd *)mPrdt;
  TUint channel = aIdeDevice->mChannel; // Read the Channel.
  write_register(channel, ATA_REG_CONTROL, 0x02);

  wait_ready(channel, 0);
  write_register(channel, ATA_REG_HDDEVSEL, 0xe0 | (aIdeDevice->mDrive << 4));
  wait_ready(channel, 0);

  io_wait(channel);
  write_register(channel, ATA_REG_FEATURES, 0x00);

  write_register(channel, ATA_REG_SECCOUNT0, 0);
  write_register(channel, ATA_REG_LBA0, (aLba & 0xff000000) >> 24);
  write_register(channel, ATA_REG_LBA1, (aLba & 0xff00000000) >> 32);
  write_register(channel, ATA_REG_LBA2, (aLba & 0xff0000000000) >> 40);

  write_register(channel, ATA_REG_SECCOUNT0, 0);
  write_register(channel, ATA_REG_LBA0, (aLba & 0x0000ff) >> 0);
  write_register(channel, ATA_REG_LBA1, (aLba & 0x00ff00) >> 0);
  write_register(channel, ATA_REG_LBA2, (aLba & 0xff0000) >> 0);

  write_register(channel, ATA_REG_COMMAND, DMA_WRITE_LBA48);
  io_wait(channel);

  // start the DMA!
  outb(bm + 0x00, 0x08 | 0x01);

  // wait for IO to complete
  Wait(mSigMask);
  return ETrue;
}

TBool AtaTask::dma_write(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TInt aNumSectors) {
  for (TInt n = 0; n < aNumSectors; n++) {
    if (dma_write_sector(aIdeDevice, aLba, &aBuffer[n & 512])) {
      return EFalse;
    }
    aLba++;
  }
  return ETrue;
}
