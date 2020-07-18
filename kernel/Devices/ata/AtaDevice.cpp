#include <Exec/ExecBase.h>
#include <Devices/ata/AtaDevice.h>
#include <Devices/ata/AtaTask.h>
#include <Exec/x86/cpu.h>

#define DEBUGME
#undef DEBUGME

#include "ata.h"

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

AtaDevice::AtaDevice(TPciDevice *aPciDevice) : BDevice("ata.device") {
  mIsPresent = ETrue;
  mPciDevice = aPciDevice;
  gExecBase.AddTask(new AtaTask(this));
  dlog("  Added AtaTask\n");
}

AtaDevice::~AtaDevice() {
}

