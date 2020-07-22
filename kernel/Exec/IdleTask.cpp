#include "IdleTask.hh"
#include <Exec/ExecBase.h>
#include <Exec/x86/pci.h>

#include <Devices/AtaDevice.h>
#include <Devices/SerialDevice.h>
#include <Devices/KeyboardDevice.h>
#include <Devices/TimerDevice.h>
#include <Devices/RtcDevice.h>
#include <Devices/MouseDevice.h>

#include <Inspiration/InspirationBase.h>
#include <FileSystems/SimpleFileSystem/SimpleFileSystem.h>

TInt64 IdleTask::Run() {
  dprint("\n");
  dlog("IdleTask Run\n");

  // initialize devices
  dlog("  initialize timer\n");
  gExecBase.AddDevice(new TimerDevice());

  // dlog("  initialize serial\n");
  // AddDevice(new SerialDevice());

  dlog("  initialize rtc \n");
  gExecBase.AddDevice(new RtcDevice());

  dlog("  initialize keyboard \n");
  gExecBase.AddDevice(new KeyboardDevice);

  dlog("  initialize mouse \n");
  gExecBase.AddDevice(new MouseDevice());

  // Initialize PCI Devices
  dprint("\n\n");
  dlog("  initialze PCI Devices\n");
  PCI *pci = gExecBase.GetPci();
  for (TPciDevice *d = pci->First(); !pci->End(d); d = pci->Next(d)) {
    d->Dump(EFalse);
    switch (d->mClassId) {
      case EPciUnclassified:
        dlog("    Skipping Unclassified PCI Device\n");
        break;

      case EpciMassStorageController: //   [0x01] = "Mass Storage Controller":
        dlog("    initialize ata disk \n");
        gExecBase.AddDevice(new AtaDevice(d));
        continue;

      case EPciNetworkController: //   [0x02] = "Network Controller":
        dlog("    Skipping NetworkController\n");
        break;

      case EPciDisplayController: //   [0x03] = "Display Controller":
        dlog("    Skipping Display Controller\n");
        break;

      case EPciMultimediaController: //   [0x04] = "Multimedia Controller":
        dlog("    Skipping Multimedia Controller\n");
        break;

      case EPciMemoryController: // [0x05] = "Memory Controller":
        dlog("    Skipping Memory Controller\n");
        break;

      case EPciBridgeDevice: // [0x06] = "Bridge Device":
        dlog("    Skipping Bridge Device\n");
        break;

      case EPciSimpleCommunicationController: // [0x07] = "Simple Communication Controller":
        dlog("    Skipping Simple Communication Controller\n");
        break;

      case EPciBaseSystemPeripheral: // [0x08] = "Base System Peripheral":
        dlog("    Skipping Base System Peripheral\n");
        break;

      case EPciInputDeviceContoller: //   [0x09] = "Input Device Controller":
        dlog("    Skipping Input Device Controller\n");
        break;

      case EPciDockingStation: //   [0x0a] = "Docking Station":
        dlog("    Skipping Docking Station\n");
        break;

      case EPciProcessor: //   [0x0b] = "Processor":
        dlog("    Skipping Processor\n");
        break;

      case EPciSerialBusController: //   [0x0c] = "Serial Bus Controller":
        dlog("    Skipping Serial Bus Controller\n");
        break;

      case EPciWirelessController: //   [0x0d] = "Wireless Controller":
        dlog("    Skipping Wireless Controller\n");
        break;

      case EPciIntellientController: //   [0x0e] = "Intelligent Controller":
        dlog("    Skipping Intelligent Controller\n");
        break;

      case EPciSatelliteCommunicationController: //   [0x0f] = "Satellite Communication Controller":
        dlog("    Skipping Satellite Communication Controller\n");
        break;

      case EPciEncryptionController: //   [0x10] = "Encryption Controller":
        dlog("    Skipping Encryption Controller\n");
        break;

      case EPciSignalProcessingController: //   [0x11] = "Signal Processing Controller":
        dlog("    Skipping Signal Processing Controller\n");
        break;

      case EPciProcessingAccelerator: //   [0x12] = "Processing Accelerator":
        dlog("    Skipping Processing Accelerator\n");
        break;

      case EPciNonEssentialInstrumentation: //   [0x13] = "Non-Essential Instrumentation":
        dlog("    Skipping Non-Essential Instrumentation\n");
        break;

      case EPciReserved: //   [0x14] = "(Reserved)":
        dlog("    Skipping Reserved class PCI Device\n");
        break;

      default:
        dlog("    *** Skipping invlalid PCI class (%x)\n", d->mClassId);
        break;
    }
  }

  dlog("  initialize file system\n");
  gExecBase.AddFileSystem(new SimpleFileSystem("ata.device", 0, gSystemInfo.mRootSector));

  dlog("  initialize Inspiration\n");
  gExecBase.SetInspirationBase(new InspirationBase());
  // gExecBase.mInspirationBase->Init();

  for (;;) {
    dlog("IdleTask Looping\n");
    halt();
  }
}
