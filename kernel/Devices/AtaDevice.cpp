#include <Exec/ExecBase.h>
#include <Devices/AtaDevice.h>
#include <Exec/BTask.h>
#include <Exec/x86/cpu.h>

#define DEBUGME
#undef DEBUGME

// TODO: make these #defines into const
#define IDE_ATA 0x00
#define IDE_ATAPI 0x01
// #define ATA_PRIMARY 0x1F0
// #define ATA_SECONDARY 0x170
#define ATA_MASTER 0x00
#define ATA_SLAVE 0x10

// STATUS
#define ATA_SR_BSY 0x80  // Busy
#define ATA_SR_DRDY 0x40 // Drive ready
#define ATA_SR_DF 0x20   // Drive write fault
#define ATA_SR_DSC 0x10  // Drive seek complete
#define ATA_SR_DRQ 0x08  // Data request ready
#define ATA_SR_CORR 0x04 // Corrected data
#define ATA_SR_IDX 0x02  // Index
#define ATA_SR_ERR 0x01  // Error

// ERRORS
#define ATA_ER_BBK 0x80   // Bad block
#define ATA_ER_UNC 0x40   // Uncorrectable data
#define ATA_ER_MC 0x20    // Media changed
#define ATA_ER_IDNF 0x10  // ID mark not found
#define ATA_ER_MCR 0x08   // Media change request
#define ATA_ER_ABRT 0x04  // Command aborted
#define ATA_ER_TK0NF 0x02 // Track 0 not found
#define ATA_ER_AMNF 0x01  // No address mark

// COMMANDS
#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

#define ATAPI_CMD_READ 0xA8
#define ATAPI_CMD_EJECT 0x1B

// ATA IDENTIFY 512 BYTE BUFFER OFFSETS
#define ATA_IDENT_DEVICETYPE 0
#define ATA_IDENT_CYLINDERS 2
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_LBA48 83
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID 106
#define ATA_IDENT_MAX_LBA 120
#define ATA_IDENT_COMMANDSETS 164
#define ATA_IDENT_MAX_LBA_EXT 200

// CHANNEL ATA REGISTER OFFSETS
#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D

// CHANNELS
#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01
// DIRECTIONS
#define ATA_READ 0x00
#define ATA_WRITE 0x01

// one of these for master and slave
typedef struct {
  TUint16 base;  // I/O Base.
  TUint16 ctrl;  // Control Base
  TUint16 bmide; // Bus Master IDE
  TUint8 nIEN;   // nIEN (No Interrupt);
} TIdeChannelRegisters;

typedef struct _tag_TIdeDevice {
  TUint8 Reserved;      // 0 (Empty) or 1 (This Drive really exists).
  TUint8 Channel;       // 0 (Primary Channel) or 1 (Secondary Channel).
  TUint8 Drive;         // 0 (Master Drive) or 1 (Slave Drive).
  TUint16 Type;         // 0: ATA, 1:ATAPI.
  TUint16 Signature;    // Drive Signature
  TUint16 Capabilities; // Features.
  TUint32 CommandSets;  // Command Sets Supported.
  TUint32 Size;         // Size in Sectors.
  TUint8 Model[41];     // Model in string.
  TBool mLba48;         // ETrue if device supports LBA48 addressing
  void Dump() {
    if (Reserved == 1) {
      dlog("  Channel(%s) Drive(%s) Type(%s) Size(%d) LBA48(%s) Model(%s)\n",
        Channel ? "Secondary" : "Primary",
        Drive ? "Slave" : "Master",
        Type ? "ATAPI" : "ATA",
        Size,
        mLba48 ? "YES" : "NO",
        Model);
    }
    else {
      dlog("  %s %s %s\n",
        Channel ? "Secondary" : "Primary",
        Drive ? "Slave" : "Master",
        "Not Present");
    }
  }
} TIdeDevice;

typedef struct {
  TUint16 mPad0[27];
  TUint16 mModel[20];
  TUint16 mPad1[13];
  TUint32 mLbaSectors;
  TUint16 mPad2[21];
  TUint16 mLba48;
  TUint16 mPad3[16];
  TUint64 mLba48Sectors;
  TUint16 mPad4[152];
} PACKED TAtaIdentity;

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

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class AtaTask : public BTask {
public:
  AtaTask(AtaDevice *aDevice) : BTask("ata.device") {
    mDevice = aDevice;
  }

  ~AtaTask() {
    dprint("\n");
    dlog("  AtaTask Desctuctor!\n");
    bochs;
  }

protected:
  // write a value to register in a channel
  void ide_write(TUint8 aChannel, TUint8 aRegister, TUint8 aData) {
    if (aRegister > 0x07 && aRegister < 0x0C) {
      ide_write(aChannel, ATA_REG_CONTROL, 0x80 | mChannels[aChannel].nIEN);
    }
    if (aRegister < 0x08) {
      outb(mChannels[aChannel].base + aRegister - 0x00, aData);
    }
    else if (aRegister < 0x0C) {
      outb(mChannels[aChannel].base + aRegister - 0x06, aData);
    }
    else if (aRegister < 0x0E) {
      outb(mChannels[aChannel].ctrl + aRegister - 0x0A, aData);
    }
    else if (aRegister < 0x16) {
      outb(mChannels[aChannel].bmide + aRegister - 0x0E, aData);
    }
    if (aRegister > 0x07 && aRegister < 0x0C) {
      ide_write(aChannel, ATA_REG_CONTROL, mChannels[aChannel].nIEN);
    }
  }

  // Read a register in a channel
  TUint8 ide_read(TUint8 aChannel, TUint8 aRegister) {
    TUint8 result;
    if (aRegister > 0x07 && aRegister < 0x0C) {
      ide_write(aChannel, ATA_REG_CONTROL, 0x80 | mChannels[aChannel].nIEN);
    }
    if (aRegister < 0x08) {
      result = inb(mChannels[aChannel].base + aRegister - 0x00);
    }
    else if (aRegister < 0x0C) {
      result = inb(mChannels[aChannel].base + aRegister - 0x06);
    }
    else if (aRegister < 0x0E) {
      result = inb(mChannels[aChannel].ctrl + aRegister - 0x0A);
    }
    else if (aRegister < 0x16) {
      result = inb(mChannels[aChannel].bmide + aRegister - 0x0E);
    }

    if (aRegister > 0x07 && aRegister < 0x0C) {
      ide_write(aChannel, ATA_REG_CONTROL, mChannels[aChannel].nIEN);
    }

    return result;
  }

  void ide_read_buffer(TUint8 aChannel, TUint8 aRegister, TUint8 *aBuffer, TUint32 aCount) {
    if (aRegister > 0x07 && aRegister < 0x0c) {
      ide_write(aChannel, ATA_REG_CONTROL, 0x80 | mChannels[aChannel].nIEN);
    }
    TUint16 port = 0;
    if (aRegister < 0x08) {
      // port = mChannels[aChannel].base + aRegister - 0x00;
      insl(mChannels[aChannel].base + aRegister - 0x00, aBuffer, aCount);
    }
    else if (aRegister < 0x0c) {
      // port = mChannels[aChannel].base + aRegister - 0x06;
      insl(mChannels[aChannel].base + aRegister - 0x06, aBuffer, aCount);
    }
    else if (aRegister < 0x0e) {
      // port = mChannels[aChannel].base + aRegister - 0x0a;
      insl(mChannels[aChannel].base + aRegister - 0x0a, aBuffer, aCount);
    }
    else if (aRegister < 0x16) {
      // port = mChannels[aChannel].base + aRegister - 0x03;
      insl(mChannels[aChannel].base + aRegister - 0x0e, aBuffer, aCount);
    }

    dlog("PORT(%x)  ", port);
    // read aCount bytes
    // for (TInt i = 0; i < aCount; i++) {
    //   aBuffer[i] = inb(port);
    //   dprint("%02x ", aBuffer[i] & 0xff);
    // }
    // dprint("\n");

    if (aRegister > 0x07 && aRegister < 0x0C) {
      ide_write(aChannel, ATA_REG_CONTROL, mChannels[aChannel].nIEN);
    }
  }

  TInt ide_polling(TUint8 aChannel, TBool aAdvancedCheck) {
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

  TUint8 ide_print_error(TUint aDrive, TUint8 aError) {
    TUint8 err = aError,
           st;

    dprint("\n\n");
    dlog("*** IDE ERROR:\n");

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

  void ide_initialize(TUint32 BAR0, TUint32 BAR1, TUint32 BAR2, TUint32 BAR3, TUint32 BAR4) {
    dlog("  ide_initialize(0x%0x, 0x%0x, 0x%0x, 0x%0, 0x%0xx)\n",
      BAR0, BAR1, BAR2, BAR3, BAR4);

    TUint8 buffer[512];
    // set up controller ports
    mChannels[ATA_PRIMARY].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
    mChannels[ATA_PRIMARY].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
    mChannels[ATA_SECONDARY].base = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
    mChannels[ATA_SECONDARY].ctrl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
    mChannels[ATA_PRIMARY].bmide = (BAR4 & 0xFFFFFFFC) + 0;   // Bus Master IDE
    mChannels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE

    // disable IRQs
    ide_write(ATA_PRIMARY, ATA_REG_CONTROL, 2);
    ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

    // scan for drives connected to each channel
    TInt count = 0;
    for (TUint8 i = 0; i < 2; i++)
      for (TUint8 j = 0; j < 2; j++) {

        TUint8 err = 0, type = IDE_ATA, status;
        mDevices[count].Reserved = 0; // Assuming that no drive here.

        // (I) Select Drive:
        ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
        MilliSleep(1);                                   // Wait 1ms for drive select to work.

        // (II) Send ATA Identify Command:
        ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
        MilliSleep(1); // This function should be implemented in your OS. which waits for 1 ms.
                       // it is based on System Timer Device Driver.

        // (III) Polling:
        if (ide_read(i, ATA_REG_STATUS) == 0) {
          // dlog("  %d no device\n", i);
          continue; // If Status = 0, No Device.
        }

        while (ETrue) {
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
          TUint8 cl = ide_read(i, ATA_REG_LBA1);
          TUint8 ch = ide_read(i, ATA_REG_LBA2);

          if (cl == 0x14 && ch == 0xEB)
            type = IDE_ATAPI;
          else if (cl == 0x69 && ch == 0x96)
            type = IDE_ATAPI;
          else
            continue; // Unknown Type (may not be a device).

          ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
          MilliSleep(1);
        }

        // (V) Read Identification Space of the Device:
        ide_read_buffer(i, ATA_REG_DATA, &buffer[0], 128);
        // dhexdump(buffer, 64);

        // (VI) Read Device Parameters:
        TAtaIdentity *p = (TAtaIdentity *)buffer;
        mDevices[count].Reserved = 1;
        mDevices[count].Type = type;
        mDevices[count].Channel = i;
        mDevices[count].Drive = j;
        mDevices[count].Signature = *((TUint16 *)(buffer + ATA_IDENT_DEVICETYPE));
        mDevices[count].Capabilities = *((TUint16 *)(buffer + ATA_IDENT_CAPABILITIES));
        mDevices[count].CommandSets = *((TUint32 *)(buffer + ATA_IDENT_COMMANDSETS));

        // (VII) Get Size:
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
        for (TInt k = 0; k < 40; k += 2) {
          mDevices[count].Model[k] = buffer[ATA_IDENT_MODEL + k + 1];
          mDevices[count].Model[k + 1] = buffer[ATA_IDENT_MODEL + k];
        }
        mDevices[count].Model[40] = 0; // Terminate String.
        // mDevices[count].mLba48 = *(TUint16 *)(buffer + ATA_IDENT_LBA48) & (1 << 10) ? ETrue : EFalse;
        count++;
      }

    // 4- Print Summary:
    dprint("\n\n");
    dlog("==============\n");
    dlog("IDE Devices:\n");
    for (TInt i = 0; i < 4; i++)
      mDevices[i].Dump();
    dlog("==============\n");
    dprint("\n\n");
  }

  TBool ide_io(TIdeDevice *aIdeDevice, TUint64 aLba, TBool aWrite, TUint8 *aBuffer, TInt aNumSectors) {
    TInt lba_mode = 0,
         cmd;

    TUint8 lba_io[6];

    unsigned int channel = aIdeDevice->Channel; // Read the Channel.
    unsigned int slavebit = aIdeDevice->Drive;  // Read the Drive [Master/Slave]
    unsigned int bus = mChannels[channel].base; // Bus Base, like 0x1F0 which is also data port.
    unsigned int words = 256;                   // Almost every ATA drive has a sector-size of 512-byte.
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

    TBool dma = EFalse; // dma not detected (yet)
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
      dlog("*** DMA not implemented!\n");
      bochs;
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
          dlog("*** ide_polling error(%d)\n", err);
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
          dlog("*** ide_polling error(%d)\n", err);
          bochs;
          return EFalse;
        }
        insw(bus, ptr, words);
        ptr = &ptr[words];
      }
    }
    return ETrue;
  }

  TBool ide_read_blocks(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TUint64 aCount) {
    return ide_io(aIdeDevice, aLba, EFalse, aBuffer, aCount);
  }

  TBool ide_write_blocks(TIdeDevice *aIdeDevice, TUint64 aLba, TUint8 *aBuffer, TUint64 aCount) {
    return ide_io(aIdeDevice, aLba, ETrue, aBuffer, aCount);
  }

protected:
  TUint64 mSigMask;
  TUint64 mActiveDevice;

public:
  void Run() {
    DISABLE;

    dprint("\n");
    dlog("AtaTask running\n");

    TUint8 sigbit = AllocSignal(-1); // this is for signal from IRQ handler(s)
    mSigMask = (1 << sigbit);
    dlog("  AtaTask signal bit %d(%x)\n", sigbit, mSigMask);

    // handlers for ATA1 and ATA2 IRQs
    gExecBase.SetIntVector(EAta1IRQ, new AtaInterrupt(this, sigbit, 1));
    gExecBase.SetIntVector(EAta2IRQ, new AtaInterrupt(this, sigbit, 2));
    gExecBase.EnableIRQ(IRQ_ATA1);
    gExecBase.EnableIRQ(IRQ_ATA2);

    // initialize devices
    mActiveDevice = 0;
    ide_initialize(0x1f0, 0x3f6, 0x170, 0x376, 0x000);

    MessagePort *port = CreateMessagePort("ata.device");
    gExecBase.AddMessagePort(*port);

    ENABLE;

    TIdeDevice *drive0 = &mDevices[0];
    while (1) {
      TUint64 sigs = WaitPort(port, mSigMask);
      if (sigs & mSigMask) {
        dlog("  IRQ SIGNAL\n");
      }
      else {
        while (AtaMessage *m = (AtaMessage *)port->GetMessage()) {
          switch (m->mCommand) {
            case EAtaReadBlocks: {
              TUint8 *buf = (TUint8 *)m->mBuffer;
              TUint64 lba = m->mLba;
              ide_read_blocks(&mDevices[m->mUnit], lba, buf, m->mCount);
            } break;
          }
          m->Reply();
        }
      }
    }

    while (1) {
      Sleep(1);
    }
  }

protected:
  AtaDevice *mDevice;
  TIdeChannelRegisters mChannels[2];
  TIdeDevice mDevices[4];
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

#if 0
static TUint8 sector[512];
class AtaTask : public BTask {
public:
  AtaTask(AtaDevice *aDevice) : BTask("ata.device") {
    mDevice = aDevice;
  }

  ~AtaTask() {
    dprint("\n");
    dlog("  AtaTask Desctuctor!\n");
    bochs;
  }

protected:
  TUint64 mSigMask;
  TUint8 mActiveDevice;

public:
  void Run() {
    DISABLE;

    dprint("\n");
    dlog("AtaTask running\n");

    TUint8 sigbit = AllocSignal(-1); // this is for signal from IRQ handler(s)
    mSigMask = (1 << sigbit);
    dlog("  AtaTask signal bit %d(%x)\n", sigbit, mSigMask);

    // handlers for ATA1 and ATA2 IRQs
    gExecBase.SetIntVector(EAta1IRQ, new AtaInterrupt(this, sigbit, 1));
    gExecBase.SetIntVector(EAta2IRQ, new AtaInterrupt(this, sigbit, 2));
    gExecBase.EnableIRQ(IRQ_ATA1);
    gExecBase.EnableIRQ(IRQ_ATA2);

    // initialize devices
    mActiveDevice = 0;
    init_drive(&drives[0], 0);
    mActiveDevice = 1;
    init_drive(&drives[1], 1);
    mActiveDevice = 2;
    init_drive(&drives[2], 2);
    mActiveDevice = 3;
    init_drive(&drives[3], 3);
    mActiveDevice = -1;

    MessagePort *port = CreateMessagePort("ata.device");
    gExecBase.AddMessagePort(*port);

    ENABLE;
    TAtaDrive *drive0 = &drives[0];
    while (1) {
      TUint64 sigs = WaitPort(port, mSigMask);
      if (sigs & mSigMask) {
        dlog("  IRQ SIGNAL\n");
      }
      else {
        while (AtaMessage *m = (AtaMessage *)port->GetMessage()) {
          switch (m->mCommand) {
            case EAtaReadBlocks: {
              TUint8 *buf = (TUint8 *)m->mBuffer;
              TUint64 lba = m->mLba;
              for (TInt i = 0; i < m->mCount; i++) {
                ata_read_block(&drives[m->mUnit], lba, buf);
                buf += 512;
                lba++;
              }
            } break;
          }
          m->Reply();
        }
      }
    }
  }

protected:
  TUint64 mSigMask;
  AtaDevice *mDevice;
  TInt32 mActiveDevice;
};
#endif

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

AtaDevice::AtaDevice() : BDevice("ata.device") {
  mIsPresent = ETrue;
  gExecBase.AddTask(new AtaTask(this));
  dlog("  Added AtaTask\n");
}

AtaDevice::~AtaDevice() {
}

TBool AtaInterrupt::Run(TAny *aData) {
  TUint64 num = (TUint64)aData;
#ifdef DEBUGME
  dlog("  ----> Ata Interrupt %d\n", num);
#endif

  gExecBase.AckIRQ(num == 1 ? IRQ_ATA1 : IRQ_ATA2);
  // mTask->Signal(1 << mSignalBit);
  return ETrue;
}
