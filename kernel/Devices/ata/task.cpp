#include <Exec/ExecBase.h>
#include "task.h"

AtaTask::AtaTask(AtaDevice *aDevice) : BTask("ata.device") {
  mDevice = aDevice;
  mBusMasterPort = mDevice->BusMasterPort();

  mDma =  (mBusMasterPort != 0) ? new DMA(this, mBusMasterPort) : ENull;
}

AtaTask::~AtaTask() {
  DSPACE();
  DLOG("  AtaTask Desctuctor!\n");
  bochs;
}

void AtaTask::WaitIrq() {
  Wait(mSigMask);
}

void AtaTask::Run() {
    // TODO: queue reads in increasing LBA order to optimize (reduce) head seek
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
    ide_initialize(0x1f0, 0x3f6, 0x170, 0x376, 0x000);

    MessagePort *port = CreateMessagePort("ata.device");
    gExecBase.AddMessagePort(*port);

    ENABLE;

    TIdeDevice *drive0 = &mDevices[0];
    while (1) {
      TUint64 sigs = WaitPort(port, mSigMask);
      if (sigs & mSigMask) {
        DLOG("  IRQ SIGNAL\n");
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
