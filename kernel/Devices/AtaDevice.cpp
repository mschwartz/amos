#define DEBUGME
// #undef DEBUGME

#include <Exec/ExecBase.h>
#include <Devices/AtaDevice.h>
#include <Exec/BTask.h>
#include <Exec/x86/cpu_utils.hh>
#include <Exec/x86/pci.h>
#include "ata/ata.h"
#include "ata/task.h"
#include "ata/interrupt.h"

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

AtaDevice::AtaDevice(TPciDevice *aPciDevice) : BDevice("ata.device") {
  mIsPresent = ETrue;
  mPciDevice = aPciDevice;
  mBusMasterPort = mPciDevice->mBar4 & 0xfffffffc;

  DSPACE();
  DLOG("Construct AtaDevice BusMasterPort(0x%04x)\n", mBusMasterPort);
  mPciDevice->Dump();

  gExecBase.AddTask(new AtaTask(this));
  DLOG("  Added AtaTask\n");
  DSPACE();
}

AtaDevice::~AtaDevice() {
}

TUint16 AtaDevice::BusMasterPort() { return mBusMasterPort; }
