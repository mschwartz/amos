#define DEBUGME
// #undef DEBUGME

#include "AtaTask.h"
#include "ata_pio.h"

TBool AtaTask::pio_read_sector(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer) {
  TUint channel = aIdeDevice->mChannel,
        base = mChannels[channel].mBase; // Read the Channel.
  DSPACE();
  DLOG("pio_read_sector(%x) channel(%x) base(%x) aLba(%x) aBuffer(%x)\n", aIdeDevice, channel, base, aLba, aBuffer);
  aIdeDevice->Dump();

  outb(base | 1, 0x00);
  outb(base | 2, 0x01);
  outb(base | 3, (TUint8)(aLba >> 0));
  outb(base | 4, (TUint8)(aLba >> 8));
  outb(base | 5, (TUint8)(aLba >> 16));
  outb(base | 6, 0xe0 | (aIdeDevice->mDrive << 4) | ((aLba >> 24) & 0x0f));
  outb(base | 7, 0x20); // read sectors

  while (!(inb(base | 7) & 0x08))
    ; // wait for ready

  for (TInt i = 0; i < 512; i++) {
    TUint8 b = inb(base);
    dprint("%02x ", b);
    aBuffer[i] = b;
  }
  dprint("\n");
  return ETrue;
#if 0 
  outb(base + 6, (aIdeDevice->mDrive ? 0xe0 : 0xf0) | ((aLba >> 24) & 0x0f));
  io_wait(channel);
  outb(base + 1, 0x00);
  outb(base + 2, 0x01); // number of sectors
  outb(base + 3, (TUint8)aLba);
  outb(base + 4, (TUint8)(aLba >> 8));
  outb(base + 5, (TUint8)(aLba >> 16));
  outb(base + 7, 0x20);
  TInt i = wait_ready(channel, 0);
  DLOG("wait_ready(%d)\n", i);
  for (TInt i = 0; i < 512; i++) {
    TUint8 b = inb(base);
    dprint("%02x ", b);
    aBuffer[i] = b;
  }
  dprint("\n");
  // insw(reg, aBuffer, 256);
  io_wait(channel);
  return ETrue;
#endif
#if 0
  wait_ready(channel, 0);
  // ATA drive controls
  write_register(channel, ATA_REG_CONTROL, 0x00);
  write_register(channel, ATA_REG_HDDEVSEL, 0xe0 | (aIdeDevice->mDrive << 4));

  io_wait(channel);
  write_register(channel, ATA_REG_FEATURES, 0x00);

  // write_register(channel, ATA_REG_SECCOUNT0, 0);
  // write_register(channel, ATA_REG_LBA0, (aLba & 0xff000000) >> 24);
  // write_register(channel, ATA_REG_LBA1, (aLba & 0xff00000000) >> 32);
  // write_register(channel, ATA_REG_LBA2, (aLba & 0xff0000000000) >> 40);

  write_register(channel, ATA_REG_SECCOUNT0, 0);
  write_register(channel, ATA_REG_LBA0, (aLba & 0x0000ff) >> 0);
  write_register(channel, ATA_REG_LBA1, (aLba & 0x00ff00) >> 0);
  write_register(channel, ATA_REG_LBA2, (aLba & 0xff0000) >> 0);

  write_register(channel, ATA_REG_COMMAND, ATA_CMD_READ_PIO);
  io_wait(channel);

  dlog("Sector: ");
  for (TInt i = 0; i < ATA_IO_SIZE; i++) {
    TUint8 b = inb(mChannels[channel].mBase + ATA_REG_DATA);
    dprint("%02x ", b);
    aBuffer[i] = b;
  }
  dprint("\n");
  // insl(mChannels[channel].mBase + ATA_REG_DATA, aBuffer, ATA_IO_SIZE / 4);

  DLOG("  pio_read_sector complete\n");
  return ETrue;
#endif
}

TBool AtaTask::pio_read(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TInt aNumSectors) {
  for (TInt n = 0; n < aNumSectors; n++) {
    if (pio_read_sector(aIdeDevice, aLba, &aBuffer[n & 512])) {
      return EFalse;
    }
    aLba++;
  }
  return ETrue;
}

TBool AtaTask::pio_write_sector(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer) {
  return ETrue;
}

TBool AtaTask::pio_write(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TInt aNumSectors) {
  for (TInt n = 0; n < aNumSectors; n++) {
    if (pio_read_sector(aIdeDevice, aLba, &aBuffer[n & 512])) {
      return EFalse;
    }
    aLba++;
  }
  return ETrue;
}
