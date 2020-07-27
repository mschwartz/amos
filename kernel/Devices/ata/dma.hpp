#ifndef AMOS_DEVICES_ATA_DMA_H
#define AMOS_DEVICES_ATA_DMA_H

#include <Types.hpp>
#include <Exec/BBase.hpp>

const TInt DMA_IO_SIZE = 512;

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
} PACKED TPrd; // set MSB for end of table in last table entry

class AtaTask;

class DMA : public BBase {
public:
  DMA(AtaTask *aTask, TUint16 aBusMasterPort);
  ~DMA();

protected:
  AtaTask *mTask;
  // ATA ports
  TUint16 mBusMasterPort,
    mAtaPort;

protected:
  // for DMA write, data has to be copied to MEMF_CHIP mDmaBuffer
  void CopyToDmaBuffer(TAny *aBuffer, TInt32 aSize);
  // for DMA read, data has to be copied from MEMF_CHIP mDmaBuffer
  void CopyFromDmaBuffer(TAny *aBuffer, TInt32 aSize);

public:
  TBool ReadSector(TUint64 aLba, TAny *aBuffer);
  TBool ReadSectors(TUint64 aLba, TAny *aBuffer, TInt32 aNumSectors);
  //
  TBool WriteSector(TUint64 aLba, TAny *aBuffer);
  TBool WriteSectors(TUint64 aLba, TAny *aBuffer, TInt32 aNumSectors);

protected:
  // these must be in CHIP RAM!
  TPrd *mPrdt;
  TUint8 *mDmaBuffer;
};

#endif
