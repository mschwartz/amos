#define DEBUGME
#undef DEBUGME

#include <Types.h>
#include <Exec/ExecBase.h>
#include <Devices/ata/AtaDevice.h>
#include "AtaTask.h"
#include "AtaInterrupt.h"
#include "ata_dma.h"

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

AtaTask::AtaTask(AtaDevice *aDevice) : BTask("ata.device") {
  DLOG("Construct AtaTask\n");
  mDevice = aDevice;
  mSectorBuffer = (TUint8 *)AllocMem(ATA_IO_SIZE, MEMF_CHIP);
  if (mSectorBuffer == ENull) {
    DLOG("can't AllocMem(MEMF_CHIP) for mSectorBuffer\n");
    bochs;
  }
  mPrdt = (TPrd *)AllocMem(sizeof(TPrd), MEMF_CHIP);
  if (mPrdt == ENull) {
    DLOG("can't AllocMem(MEMF_CHIP) for mPrdt\n");
    bochs;
  }
  DLOG("  mSectorBuffer(0x%x) mPrdt(%x)\n", mSectorBuffer, mPrdt);
  TUint64 sb_addr = (TUint64)mSectorBuffer;
  mPrdt->mPhysicalAddress = sb_addr;
  mPrdt->mSize = ATA_IO_SIZE;
  mPrdt->mEot = 0x8000;
}

AtaTask::~AtaTask() {
  DPRINT("\n");
  DLOG("  AtaTask Desctuctor!\n");
  delete mPrdt;
  delete mSectorBuffer;
  bochs;
}

void AtaTask::Run() {
  DISABLE;

  DSPACE();
  DLOG("AtaTask running\n");

  TUint8 sigbit = AllocSignal(-1); // this is for signal from IRQ handler(s)
  mSigMask = (1 << sigbit);
  DLOG("  AtaTask signal bit %d(%x)\n", sigbit, mSigMask);

  // handlers for ATA1 and ATA2 IRQs
  gExecBase.SetIntVector(EAta1IRQ, new AtaInterrupt(this, sigbit, 1));
  gExecBase.SetIntVector(EAta2IRQ, new AtaInterrupt(this, sigbit, 2));
  gExecBase.EnableIRQ(IRQ_ATA1);
  gExecBase.EnableIRQ(IRQ_ATA2);

  // initialize devices
  mActiveDevice = 0;
  initialize(0x1f0, 0x3f6, 0x170, 0x376, 0x000);

  MessagePort *port = CreateMessagePort("ata.device");
  gExecBase.AddMessagePort(*port);

  ENABLE;

  TIdeDevice *drive0 = &mDevices[0];
  while (1) {
    TUint64 sigs = WaitPort(port, mSigMask);
    if (sigs & mSigMask) {
      DLOG("  IRQ SIGNAL\n");
    }
    DLOG("  GET MESSAGES\n");
    while (AtaMessage *m = (AtaMessage *)port->GetMessage()) {
      DLOG("  Message command(%d)\n", m->mCommand);
      switch (m->mCommand) {
        case EAtaReadBlocks: {
          TUint8 *buf = (TUint8 *)m->mBuffer;
          TUint64 lba = m->mLba;
          DLOG("  read_blocks lba(%d) buf(%x) count(%d)\n", lba, buf, m->mCount);
          read_blocks(&mDevices[m->mUnit], lba, buf, m->mCount);
          DLOG("  read successfuly\n");
        } break;
      }
      DLOG("  REPLY\n\n");
      m->Reply();
    }
  }

  while (1) {
    Sleep(1);
  }
}
