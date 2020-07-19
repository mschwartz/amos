#ifndef AMOS_DEVICES_ATA_DMA_H
#define AMOS_DEVICES_ATA_DMA_H

// DMA

#include <Types.h>

// PRD table entry (PRD Table is an array of these and must be in MEMF_CHIP)
typedef struct {
  TUint32 mPhysicalAddress; // MEMF_CHIP!
  TUint16 mSize;            // byte count
  TUint16 mEot;             // 0x8000 at end of table
public:
  void Dump() {
    dlog("PRD(0x%x)\n", this);
    dlog(" mPhysicalAddress(0x%x)\n", mPhysicalAddress);
    dlog(" mSize(%d)\n", mSize);
    dlog(" mEot(0x%x)\n", mEot);
    
  }
} PACKED TPrd;              // set MSB for end of table in last table entry

typedef struct {
  TUint8 mCommand;
  TUint8 mPad0;
  TUint8 mStatus;
  TUint8 mPad;
  // PRD Table address low byte to high byte
  TUint8 mPrdt0; // low byte
  TUint8 mPrdt1;
  TUint8 mPrdt2;
  TUint8 mPrdt3; // high byte
} PACKED TBusMaster;

// commands
const TUint8 DMA_READ_LBA28 = 0xc8;
const TUint8 DMA_READ_LBA48 = 0x25;

const TUint8 DMA_WRITE_LBA28 = 0xca;
const TUint8 DMA_WRITE_LBA48 = 0x35;

// status bits
const TUint8 DMA_STATUS_BUSY = 1 << 0;
const TUint8 DMA_STATUS_FAIL = 1 << 1;
const TUint8 DMA_STATUS_IRQ = 1 << 2; // if zero, then IRQ is for some other device

// command bits
const TUint8 DMA_START = 1 << 0; // set this bit to start the DMA transfer
const TUint8 DMA_READ = 0;       // 1<<3 = 0 means read
const TUint8 DMA_WRITE = 1 << 3; // 1 = write

#endif
