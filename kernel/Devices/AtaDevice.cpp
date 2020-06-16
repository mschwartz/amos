#include <Exec/ExecBase.h>
#include <Devices/AtaDevice.h>
#include <Exec/BTask.h>

#define ATA_PRIMARY 0x1F0
#define ATA_SECONDARY 0x170
#define ATA_MASTER 0x00
#define ATA_SLAVE 0x10

// Input to controller
#define ATA_FEATURE(x) (x + 1)
#define ATA_COUNT(x) (x + 2)
#define ATA_LBAL(x) (x + 3)
#define ATA_LBAM(x) (x + 4)
#define ATA_LBAH(x) (x + 5)
#define ATA_DEVICE(x) (x + 6)
#define ATA_COMMAND(x) (x + 7)
#define ATA_CONTROL(x) (x + 0x206)

// Output from controller
#define ATA_DATA(x) (x + 0)
#define ATA_ERROR(x) (x + 1)
// COUNT
// LBAL
// LBAM
// LBAH
// DEVICE
#define ATA_STATUS(x) (x + 7)

#define ATA_ERR (1 << 0)
#define ATA_DRQ (1 << 3)
#define ATA_SRV (1 << 4)
#define ATA_DF (1 << 5)
#define ATA_RDY (1 << 6)
#define ATA_BSY (1 << 7)

#define ATA_nIEN (1 << 1)
#define ATA_SRST (1 << 2)

#define ATA_CMD_IDENTIFY 0xEC
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_READ_SECTORS 0x20
#define ATA_CMD_WRITE_SECTORS 0x30
#define ATA_CMD_PACKET 0xA0

#define ATAPI_LBA_MAGIC 0xEB1401

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

typedef struct {
  TBool exists;
  TBool atapi;
  TInt16 bus;
  TUint16 mMasterSlave;
  struct {
    TUint16 config;
    TUint16 unused1[9];
    char serial[20];
    TUint16 unused2[3];
    char firmware[8];
    char model[40];
    TUint8 unused3;
    TUint8 sectors_per_interrupt;
    TUint16 unused4;
    TUint16 capabilities[2];
    TUint16 unused5[2];
    TUint16 validity;
    TUint16 unused6[3];
    TUint32 capacity_sectors;
    TUint16 sectors_per_command;
    TUint32 capacity_lba28;
    TUint16 unused7[38];
    TUint64 capcity_lba48;
    TUint16 unused8[152];
  } PACKED id;
} TAtaDrive;

typedef struct {
  TInt16 bus;
  union {
    TUint8 feature;
    TUint8 error;
  };
  TUint8 count;
  union {
    TUint64 lba;
    TUint8 lba_sep[4];
  };
  TUint8 device;
  union {
    TUint8 command;
    TUint8 status;
  };
  TUint8 wait_status;
} TAtaCommand;

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static TAtaDrive drives[4] = {
  { .bus = ATA_PRIMARY, .mMasterSlave = ATA_MASTER },
  { .bus = ATA_PRIMARY, .mMasterSlave = ATA_SLAVE },
  { .bus = ATA_SECONDARY, .mMasterSlave = ATA_MASTER },
  { .bus = ATA_SECONDARY, .mMasterSlave = ATA_SLAVE },
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static TUint8 ata_wait_status(TUint16 bus) {
  inb(ATA_STATUS(bus));
  inb(ATA_STATUS(bus));
  inb(ATA_STATUS(bus));
  inb(ATA_STATUS(bus));
  return inb(ATA_STATUS(bus));
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static TInt ata_send_command(TAtaCommand *cmd) {
  outb(ATA_DEVICE(cmd->bus), cmd->device | (cmd->lba_sep[3] & 0xF));
  ata_wait_status(cmd->bus);

  outb(ATA_COUNT(cmd->bus), cmd->count);
  outb(ATA_LBAL(cmd->bus), cmd->lba_sep[0]);
  outb(ATA_LBAM(cmd->bus), cmd->lba_sep[1]);
  outb(ATA_LBAH(cmd->bus), cmd->lba_sep[2]);
  while (inb(ATA_STATUS(cmd->bus)) & ATA_BSY)
    ;
  outb(ATA_COMMAND(cmd->bus), cmd->command);

  if (!inb(ATA_STATUS(cmd->bus)))
    return 0;

  cmd->status = ata_wait_status(cmd->bus);
  while ((cmd->status = inb(ATA_STATUS(cmd->bus))) & ATA_BSY)
    ;
  if (cmd->wait_status)
    while (!(cmd->status = inb(ATA_STATUS(cmd->bus)) & (cmd->wait_status | ATA_ERR)))
      ;
  cmd->error = inb(ATA_ERROR(cmd->bus));
  cmd->count = inb(ATA_COUNT(cmd->bus));
  cmd->device = inb(ATA_DEVICE(cmd->bus));
  cmd->lba_sep[0] = inb(ATA_LBAL(cmd->bus));
  cmd->lba_sep[1] = inb(ATA_LBAM(cmd->bus));
  cmd->lba_sep[2] = inb(ATA_LBAH(cmd->bus));
  cmd->lba_sep[3] = 0;

  return cmd->status;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static int ata_read_block(TAtaDrive *drive, TUint64 lba, TAny *buffer) {
  int retries = 5;
  while (retries) {
    TUint8 ms = TUint8(drive->mMasterSlave);
    TUint8 lbal = ms | 0xe0 | ((lba >> 24) & 0x0f);
    TAtaCommand command = {
      .bus = drive->bus,
      .count = 1,
      .lba = lba & 0xFFFFFF,
      // .device = E0 | drive->mMasterSlave | ((lba >> 24) & 0xF),
      .device = lbal,
      .command = ATA_CMD_READ_SECTORS,
    };

    dlog("read-block send_command\n");
    int status = ata_send_command(&command);
    dlog("read-block send_command status %x\n", status);
    if (status & (ATA_DF | ATA_ERR) || !(status & ATA_DRQ)) {
      dlog(" read-block send_command status %x fail\n", status);
      retries--;
      continue;
    }

    dlog("read_block read sector data\n");
    TUint16 *buf = (TUint16 *)buffer;
    for (int i = 0; i < 256; i++) {
      buf[i] = inw(ATA_DATA(drive->bus));
    }
    dlog("read_block success!\n");
    return 0;
  }

  dlog("Reading disk timeout\n");
  return -1;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static int ata_write_block(TAtaDrive *drive, TUint64 lba, TAny *buffer) {
  bochs
    bochs
      bochs
        bochs
          TUint8 ms = TUint8(drive->mMasterSlave);
  TUint8 lbal = ms | 0xe0 | ((lba >> 24) & 0x0f);
  TAtaCommand command = {
    .bus = drive->bus,
    .count = 1,
    .lba = lba & 0xFFFFFF,
    // .device = 0xE0 | drive->mMasterSlave | ((lba >> 24) & 0xF),
    .device = lbal,
    .command = ATA_CMD_WRITE_SECTORS,
  };
  int status = ata_send_command(&command);
  if (status & ATA_ERR) {
    dlog("An error occurred\n");
  }
  TUint16 *buf = (TUint16 *)buffer;
  for (int i = 0; i < 256; i++) {
    outw(ATA_DATA(drive->bus), buf[i]);
  }

  return 0;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static TBool init_drive(TAtaDrive *drive, TInt num) {
  dprint("init_drive(%d)\n", num);
  TInt16 bus = drive->bus;
  // Check if the controller exists
  // by writing a value to it and check
  // that the same one is returned
  int v1 = inb(ATA_LBAL(bus));
  outb(ATA_LBAL(bus), (~v1) & 0xFF);
  int v2 = inb(ATA_LBAL(bus));
  if (v2 != ((~v1) & 0xFF)) {
    dprint("Controller does not exist drive %d bus %x\n", num, drive - drive->bus);
    return EFalse;
  }

  // Check if the drive exists
  // by selecting the drive
  outb(ATA_DEVICE(bus), 0xA0 | drive->mMasterSlave);
  if (!(ata_wait_status(bus) & ATA_RDY)) {
    dprint("Drive %d does not exist bus %x\n", num, drive->mMasterSlave);
    return EFalse;
  }

  outb(ATA_CONTROL(bus), ATA_SRST);
  outb(ATA_CONTROL(bus), 0);
  TUint64 lba = inb(ATA_LBAH(bus)) << 16 | inb(ATA_LBAM(bus)) << 8 | inb(ATA_LBAL(bus));

  TAtaCommand command = {
    .bus = bus,
    .device = TUint8(0xA0 | TUint8(drive->mMasterSlave)),
    .command = ATA_CMD_IDENTIFY,
    .wait_status = ATA_DRQ,
  };

  if (lba == ATAPI_LBA_MAGIC) {
    drive->atapi = 1;
    command.command = ATA_CMD_IDENTIFY_PACKET;
  }
  if (!ata_send_command(&command)) {
    dprint("init drive %d ata_send_command failed\n", num);
    return EFalse;
  }

  // Read IDENTIFY information
  TUint16 *buf = (TUint16 *)&drive->id;
  for (int i = 0; i < 256; i++) {
    buf[i] = inw(ATA_DATA(drive->bus));
  }

  drive->exists = 1;
  return ETrue;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class AtaTask;

class AtaInterrupt : public BInterrupt {
public:
  AtaInterrupt(AtaTask *aTask, TUint8 aSignalBit, TInt64 aDevice)
      : BInterrupt("keyboard.device", LIST_PRI_MAX, (TAny *)aDevice) {
    mTask = aTask;
    mSignalBit = aSignalBit;
    mDevice = aDevice;
  }

public:
  TBool Run(TAny *aData);

protected:
  TInt32 mDevice;
  AtaTask *mTask;
  TUint8 mSignalBit;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static TUint8 sector[512];
class AtaTask : public BTask {
public:
  AtaTask(AtaDevice *aDevice) : BTask("ata.device") {
    mDevice = aDevice;
  }

  ~AtaTask() {
    dlog("AtaTask Desctuctor!\n");
    bochs;
  }

public:
  void Run() {
    DISABLE;

    dlog("AtaTask running\n");

    TUint8 sigbit = AllocSignal(-1); // this is for signal from IRQ handler(s)
    mSigMask = (1 << sigbit);
    dlog("AtaTask signal bit %d(%x)\n", sigbit, mSigMask);

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
    dlog("  Initialized drives\n");

    MessagePort *port = CreateMessagePort("ata.device");
    gExecBase.AddMessagePort(*port);

    ENABLE;
    TAtaDrive *drive0 = &drives[0];
    // dlog("Reading sector 0\n");
    // ata_read_block(drive0, 0, sector);
    // dlog("Read sector 0 complete\n");
    // dhexdump(sector, 16);
    while (1) {
      TUint64 sigs = WaitPort(port, mSigMask);
      dlog("Woke %x\n", sigs);
      if (sigs & mSigMask) {
        dlog("  IRQ SIGNAL\n");
      }
      else {
        dlog("Got Message\n");
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
          m->ReplyMessage();
        }
      }
    }
  }

protected:
  TUint64 mSigMask;
  AtaDevice *mDevice;
  TInt32 mActiveDevice;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

AtaDevice::AtaDevice() : BDevice("ata.device") {
  gExecBase.AddTask(new AtaTask(this));
  dlog("Added AtaTask\n");
}

AtaDevice::~AtaDevice() {
}

TBool AtaInterrupt::Run(TAny *aData) {
  TUint64 num = (TUint64)aData;
  dlog("----> Ata Interrupt %d\n", num);

  gExecBase.AckIRQ(num == 1 ? IRQ_ATA1 : IRQ_ATA2);
  // mTask->Signal(1 << mSignalBit);
  return ETrue;
}
