#include "InitTask.hpp"
#include <Exec/ExecBase.hpp>
#include <Exec/x86/pci.hpp>

#include <Devices/AtaDevice.hpp>
#include <Devices/SerialDevice.hpp>
#include <Devices/KeyboardDevice.hpp>
#include <Devices/TimerDevice.hpp>
#include <Devices/RtcDevice.hpp>
#include <Devices/MouseDevice.hpp>

#include <Inspiration/InspirationBase.hpp>
#include <FileSystems/SimpleFileSystem/SimpleFileSystem.hpp>
#include "IdleTask.hpp"
// #include <Examples/Examples.h>

TInt64 InitTask::Run() {
  dprint("\n");
  dlog("InitTask Run\n");

  if (mCpu->mProcessor != 0) {
    return 0;
  }

  dlog("  initialize timer\n");
  gExecBase.AddDevice(new TimerDevice());

  dlog("  initialize rtc \n");
  gExecBase.AddDevice(new RtcDevice());

  gExecBase.AddTask(new IdleTask());
  dlog("  STARTING APs\n");
  for (TInt i = 1; i < gExecBase.NumCpus(); i++) {
    CPU *cpu = gExecBase.GetCpu(i);
    // Apic *apic = cpu->mApic;
    cpu->StartAP(this); // initialize tasking for AP
    // TODO: actually start application processor
    // IPI
    // dlog("CPU %d SendIPI\n", i);
    // apic->SendIPI(cpu->mApicId, 8);
    // delay 10ms
    // MilliSleep(10);
    // SIPI
    // dlog("CPU %d SendSIPI\n", i);
    // apic->SendSIPI(cpu->mApicId, 8);
    // wait for CPU to boot
    // send second SIPI if it didn't boot
    // give up if second SIPI didn't work
    // TODO: this needs to be done from ap_start() in kernel_main.cpp;
    // enter_tasking(); // just enter next task
  }
  // initialize devices

  Sleep(3);

  // dlog("  initialize serial\n");
  // AddDevice(new SerialDevice());

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

  bochs
  dlog("  initialize Inspiration\n");
  gExecBase.SetInspirationBase(new InspirationBase());

  return 0;
}
