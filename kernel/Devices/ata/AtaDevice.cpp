#include <Exec/ExecBase.h>
#include <Devices/ata/AtaDevice.h>
#include <Devices/ata/AtaTask.h>
#include <Exec/x86/cpu.h>

#define DEBUGME
// #undef DEBUGME

#include "ata.h"

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

AtaDevice::AtaDevice(TPciDevice *aPciDevice) : BDevice("ata.device") {
  DLOG("new AtaDevice(%x)\n", aPciDevice);
  mIsPresent = ETrue;
  mPciDevice = aPciDevice;
  mPciDevice->Dump();
  mBusMasterPort = mPciDevice->mBar4 & 0xfffc;
  DLOG("Bus Master Port (%x)\n", mBusMasterPort);
  gExecBase.AddTask(new AtaTask(this));
  DLOG("  Added AtaTask\n");
}

AtaDevice::~AtaDevice() {
}

