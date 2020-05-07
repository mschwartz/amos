#include <Exec/ExecBase.h>
#include <Exec/x86/cpu.h>
#include <Exec/x86/ps2.h>
#include <Devices/MouseDevice.h>

#define inportb inb
#define outportb(a, b) outb(b, a)

const TUint32 MOUSE_DR = 0x60;
const TUint32 MOUSE_SR = 0x64;
const TUint32 MOUSE_PORT = 0x60;
const TUint32 MOUSE_STATUS = 0x64;

#define MOUSE_ABIT 0x02
#define MOUSE_BBIT 0x01
#define MOUSE_WRITE 0xd4
#define MOUSE_F_BIT 0x20
#define MOUSE_V_BIT 0x08

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void mouse_wait(TUint8 a_type) {
  TInt32 _time_out = 100000; //unsigned int
  if (a_type == 0) {
    while (_time_out--) {
      if ((inb(MOUSE_STATUS) & 1) == 1) {
        return;
      }
    }
  }
  else {
    while (_time_out--) {
      if ((inb(MOUSE_STATUS) & 2) == 0) {
        return;
      }
    }
  }
  dlog("mouse_wait(%d) timeout\n", a_type);
}

void mouse_write(TUint8 a_write) {
  //Tell the mouse we are sending a command
  mouse_wait(1);
  outportb(MOUSE_STATUS, MOUSE_WRITE);
  mouse_wait(1);
  //Finally write
  outportb(MOUSE_PORT, a_write);
}

TUint8 mouse_read() {
  mouse_wait(0);
  return inportb(MOUSE_PORT);
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class MouseTask;

class MouseInterrupt : public BInterrupt {
protected:
public:
  MouseInterrupt(MouseTask *aTask) : BInterrupt("Mouse Interrupt Handler", LIST_PRI_MAX) {
    mTask = aTask;
  }

public:
  TBool Run(TAny *aData);

protected:
  MouseTask *mTask;
};

class MouseTask : public BTask {
  friend MouseInterrupt;

protected:
public:
  MouseTask(MouseDevice *aDevice) : BTask("mouse.device", LIST_PRI_MAX) {
    mDevice = aDevice;
  }
  ~MouseTask() {}

public:
  void Run();

protected:
  MouseDevice *mDevice;
  MessagePort *mMessagePort;
};

TBool MouseInterrupt::Run(TAny *aData) {
  dlog("Mouse Interrupt: %02x\n", inb(0x60));

//  TUint8 status = inb(MOUSE_STATUS);

//  while (status & MOUSE_BBIT) {
//    TUint8 mouse_in = inb(MOUSE_PORT);
//    status = inb(MOUSE_STATUS);
//  }
  gExecBase.AckIRQ(IRQ_MOUSE);
  return ETrue;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

extern "C" void mouse_trap();

void MouseTask::Run() {
  while (1) { Sleep(5); }
  TUint64 flags = GetFlags();
  cli();

  dlog("MouseTask::Run()\n");

  dlog("Initialize Mouse Interrupt... ");

#if 1
  // enable the auxilliary mouse device
  mouse_wait(1);
  outb(0xa8, 0x64);

  // enable the interrupts
  mouse_wait(1);
  outb(0x20, 0x64);
  mouse_wait(0);
  TUint8 status = (inb(0x60) & ~0x20) | 2;
  mouse_wait(1);
  outb(0x60, 0x64);
  mouse_wait(1);
  outb(status, 0x60);

    // tell the mouse to use the default settings
  mouse_write(0xf6);
  mouse_read();

    // enable the mouse
  mouse_write(0xf4);
  mouse_read();

//  mouse_read(); //Acknowledge
#endif

  dprint("   Enable PIC (%d) and Vector(%d)\n", IRQ_MOUSE, EMouseIRQ);
  gExecBase.SetIntVector(EMouseIRQ, new MouseInterrupt(this));
  gExecBase.EnableIRQ(IRQ_MOUSE);
  gExecBase.AckIRQ(IRQ_MOUSE);

//  dprint("about to trap mouse\n");
//  mouse_trap();
//  SetFlags(flags);

  SetFlags(flags);

  mMessagePort = CreateMessagePort("mouse.device");
  gExecBase.AddMessagePort(*mMessagePort);
  dlog("mouse waitport\n");
  while (WaitPort(mMessagePort)) {
    //
    dlog("mouse gotport\n");
  }
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

MouseDevice::MouseDevice() : BDevice("mouse.device") {
  dlog("MouseDevice Construcotr\n");
  TSystemInfo i;
  gExecBase.GetSystemInfo(&i);
  mScreenWidth = i.mScreenWidth;
  mScreenHeight = i.mScreenWidth;
  // initial position
  mX = mY = 20;
  gExecBase.AddTask(new MouseTask(this));
  //
}

MouseDevice::~MouseDevice() {
  //
}
