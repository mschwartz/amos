#ifndef AMOS_DEVICES_DMA_H
#define AMOS_DEVICES_DMA_H

// DMA

#include <Types.h>

typedef  struct {
  TUint32 mPhysicalAddress;
  TUint16 aSize;
  TUint16 aEot; // 0x8000 at end of table
} PACKED TPrdt;

#endif
