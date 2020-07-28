#define DEBUGME
// #undef DEBUGME

#include <Exec/ExecBase.hpp>
#include <Devices/AtaDevice.hpp>
#include <Exec/BTask.hpp>
#include <Exec/x86/cpu_utils.hpp>
#include <Exec/x86/pci.hpp>
#include "ata/ata.hpp"
#include "ata/task.hpp"
#include "ata/interrupt.hpp"

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
