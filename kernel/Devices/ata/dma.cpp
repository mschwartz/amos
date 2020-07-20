#define DEBUGME
#undef DEBUGME

#include "ata.h"
#include "dma.h"
#include "task.h"

// reference: https://github.com/klange/toaruos/blob/96fe218d2f79998bcfcdc3c6d51444a209b5c279/modules/ata.c

DMA::DMA(AtaTask *aTask, TUint16 aBusMasterPort) {
  DLOG("DMA CONSTRUCTOR\n");
  mTask = aTask;

  // DMA
  mBusMasterPort = aBusMasterPort;

  // we need a buffer in MEMF_CHIP memory for DMA to work
  mDmaBuffer = (TUint8 *)AllocMem(DMA_IO_SIZE, MEMF_CHIP);
  ASSERT(mDmaBuffer != ENull, "Can't AllocMem(%d) mDmaBuffer\n", DMA_IO_SIZE);

  // we're only going to do one PRD at a time
  mPrdt = (TPrd *)AllocMem(sizeof(TPrd), MEMF_CHIP);
  ASSERT(mPrdt != ENull, "Can't AllocMem(%d) TPrd\n", sizeof(TPrd));

  // initialize the PRD table
  mPrdt->mPhysicalAddress = (TUint64)mDmaBuffer;
  mPrdt->mSize = DMA_IO_SIZE;
  mPrdt->mEot = 0x8000;
}

DMA::~DMA() {
  if (mPrdt) {
    FreeMem(mPrdt);
  }
  mPrdt = ENull;

  if (mDmaBuffer) {
    FreeMem(mDmaBuffer);
  }
  mDmaBuffer = ENull;
}

void DMA::CopyToDmaBuffer(TAny *aBuffer, TInt32 aSize) {
  CopyMemory(mDmaBuffer, aBuffer, aSize);
}

void DMA::CopyFromDmaBuffer(TAny *aBuffer, TInt32 aSize) {
  CopyMemory(aBuffer, mDmaBuffer, aSize);
}

TBool DMA::ReadSector(TUint64 aLba, TAny *aBuffer) {
  DLOG("DMA::ReadSector aLba(%d) aBuffer(%x)\n", aLba, aBuffer);
  TIdeDevice *dev = mTask->IdeDevice();
  TUint16 ata = mTask->AtaBasePort(),
          dma = mBusMasterPort;

  // STOP
  outb(dma, 0);
  // SET the PRDT
  outl(dma + 4, (TUint64)mPrdt);

  // enable error, irq status
  outb(dma + 2, inb(dma + 2) | 6);
  // set direction to read
  outb(dma, 0x08);

  // wait for ATA ready
  for (;;) {
    TUint8 status = inb(ata + ATA_REG_STATUS);
    if ((status & ATA_SR_BSY) == 0) {
      break;
    }
  }

  outb(ata + ATA_REG_CONTROL, 0x00);
  outb(ata + ATA_REG_HDDEVSEL, 0xe0 | (dev->Drive << 4));

  // wait 400ms
  inb(ata + ATA_REG_ALTSTATUS); // 100ms
  inb(ata + ATA_REG_ALTSTATUS); // +100ms
  inb(ata + ATA_REG_ALTSTATUS); // +100ms
  inb(ata + ATA_REG_ALTSTATUS); // +100ms

  outb(ata + ATA_REG_FEATURES, 0x00);
  outb(ata + ATA_REG_LBA0, (aLba & 0xff000000) >> 24);
  outb(ata + ATA_REG_LBA1, (aLba & 0xff00000000) >> 32);
  outb(ata + ATA_REG_LBA2, (aLba & 0xff0000000000) >> 40);

  outb(ata + ATA_REG_SECCOUNT0, 1);
  outb(ata + ATA_REG_LBA0, (aLba & 0x000000ff) >> 0);
  outb(ata + ATA_REG_LBA1, (aLba & 0x0000ff00) >> 8);
  outb(ata + ATA_REG_LBA2, (aLba & 0x00ff0000) >> 16);

  // wait for ATA ready
  for (;;) {
    TUint8 status = inb(ata + ATA_REG_STATUS);
    if ((status & ATA_SR_BSY) == 0) {
      break;
    }
  }

  // set DMA read command
  outb(ata + ATA_REG_COMMAND, ATA_CMD_READ_DMA_EXT);

  // wait 400ms
  inb(ata + ATA_REG_ALTSTATUS); // 100ms
  inb(ata + ATA_REG_ALTSTATUS); // +100ms
  inb(ata + ATA_REG_ALTSTATUS); // +100ms
  inb(ata + ATA_REG_ALTSTATUS); // +100ms

  outb(dma, 0x08 | 0x01); // start DMA!

#if 0
  mTask->WaitIrq();
#else
  // wait for DMA?
  for (;;) {
    TUint8 status = inb(dma + 2);
    TUint8 dstatus = inb(ata + ATA_REG_STATUS);
    if ((status & 0x04) == 0) {
      mTask->usleep(1); // so we don't hog the CPU
      continue;
    }
    if ((dstatus & ATA_SR_BSY) == 0) {
      break;
    }
  }
#endif

  CopyFromDmaBuffer(aBuffer, DMA_IO_SIZE);

  return ETrue;
}

TBool DMA::Read(TUint64 aLba, TAny *aBuffer, TInt32 aNumSectors) {
  TUint8 *buf = (TUint8 *)aBuffer;
  for (TInt i = 0; i < aNumSectors; i++) {
    if (!ReadSector(aLba, buf)) {
      return EFalse;
    }
    buf += 512;
  }
  return ETrue;
}

TBool DMA::WriteSector(TUint64 aLba, TAny *aBuffer) {
  return ETrue;
}

TBool DMA::Write(TUint64 aLba, TAny *aBuffer, TInt32 aNumSectors) {
  return ETrue;
}
