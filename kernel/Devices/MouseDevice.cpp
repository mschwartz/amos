#include <Exec/ExecBase.h>
#include <Exec/x86/cpu.h>
#include <Exec/x86/ps2.h>
#include <Devices/MouseDevice.h>

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

static void mouse_wait(TUint8 a_type) {
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

static void mouse_write(TUint8 a_write) {
  //Tell the mouse we are sending a command
  mouse_wait(1);
  outb(MOUSE_STATUS, MOUSE_WRITE);
  mouse_wait(1);
  //Finally write
  outb(MOUSE_PORT, a_write);
}

static TUint8 mouse_read() {
  mouse_wait(0);
  return inb(MOUSE_PORT);
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class MouseTask;

class MouseInterrupt : public BInterrupt {
public:
  MouseInterrupt(MouseTask *aTask) : BInterrupt("Mouse Interrupt Handler", LIST_PRI_MAX) {
    mTask = aTask;
    mState = 0;
    mX = mY = 20;
    mButtons = 0;
    TSystemInfo info;
    gExecBase.GetSystemInfo(&info);
    mMaxX = info.mDisplayWidth - 1;
    mMaxY = info.mDisplayHeight - 1;
//    dlog("Mouse Max X,Y = %d,%d\n", mMaxX, mMaxY);
  }

public:
  TBool Run(TAny *aData);

protected:
  MouseTask *mTask;
  TUint32 mMaxX, mMaxY;

protected:
  TInt32 mX, mY;
  TUint8 mState;
  TUint8 mButtons;
  TInt8 mPacket[3];
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
  TInt8 in_byte = mouse_read();

  switch (mState) {
    case 0:
      mPacket[0] = in_byte;

      if (in_byte & MOUSE_LEFT_BUTTON) {
        mButtons |= MOUSE_LEFT_BUTTON;
      }
      else {
        mButtons &= ~MOUSE_LEFT_BUTTON;
      }

      if (in_byte & MOUSE_MIDDLE_BUTTON) {
        mButtons |= MOUSE_MIDDLE_BUTTON;
      }
      else {
        mButtons &= ~MOUSE_MIDDLE_BUTTON;
      }

      if (in_byte & MOUSE_RIGHT_BUTTON) {
        mButtons |= MOUSE_RIGHT_BUTTON;
      }
      else {
        mButtons &= ~MOUSE_RIGHT_BUTTON;
      }
      break;

    case 1:
      mPacket[1] = in_byte;
      break;

    case 2:
      mPacket[2] = in_byte;
      mX += mPacket[1];
      if (mX < 0) {
        mX = 0;
      }
      else if (mX > mMaxX) {
        mX = mMaxX;
      }

      mY -= mPacket[2];
      if (mY < 0) {
        mY = 0;
      }
      else if (mY > mMaxY) {
        mY = mMaxY;
      }

      // send message to Mouse Task
      {
        MouseMessage *m = new MouseMessage(ENull, EMouseUpdate);
        m->mMouseX = mX;
        m->mMouseY = mY;
        m->mButtons = mButtons;
        m->Send(mTask->mMessagePort);
      }
      break;
  }
  mState = (mState + 1) % 3;

  gExecBase.AckIRQ(IRQ_MOUSE);
  return ETrue;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

extern "C" void mouse_trap();

void MouseTask::Run() {
  //  while (1) { Sleep(5); }
  TUint64 flags = GetFlags();
  cli();

  dprint("\n");
  dlog("MouseTask Run\n");

  mMessagePort = CreateMessagePort("mouse.device");
  gExecBase.AddMessagePort(*mMessagePort);

//  dlog("Initialize Mouse Interrupt... ");

  // enable the auxilliary mouse device
  mouse_wait(1);
  outb(0x64, 0xa8);

  // enable the interrupts
  mouse_wait(1);
  outb(0x64, 0x20);
  mouse_wait(0);
  TUint8 status = (inb(0x60) & ~0x20) | 2;
  mouse_wait(1);
  outb(0x64, 0x60);
  mouse_wait(1);
  outb(0x60, status);

  // tell the mouse to use the default settings
  mouse_write(0xf6);
  mouse_read();

  // enable the mouse
  mouse_write(0xf4);
  mouse_read();

//  dprint("   Enable PIC (%d) and Vector(%d)\n", IRQ_MOUSE, EMouseIRQ);
  gExecBase.SetIntVector(EMouseIRQ, new MouseInterrupt(this));
  gExecBase.EnableIRQ(IRQ_MOUSE);
  gExecBase.AckIRQ(IRQ_MOUSE);

  //  dprint("about to trap mouse\n");
  //  mouse_trap();
  //  SetFlags(flags);

  SetFlags(flags);

  BMessageList messages("mouselist");

  while (WaitPort(mMessagePort)) {
//    dlog("Wake\n");
    while (MouseMessage *m = (MouseMessage *)mMessagePort->GetMessage()) {
      switch (m->mCommand) {
        case EMouseUpdate: {
          TInt32 x = m->mMouseX,
                 y = m->mMouseY;
          TUint8 buttons = m->mButtons;

          mDevice->mX = x;
          mDevice->mY = y;
          mDevice->mButtons = buttons;
          delete m;
          while ((m = (MouseMessage *)messages.RemHead())) {
//            dlog("mouse move %x %d,%d %x\n", m, mDevice->mX, mDevice->mY, mDevice->mButtons);
            m->mMouseX = x;
            m->mMouseY = y;
            m->mButtons = buttons;
//            dlog(" Reply message %x to port %x\n", m, m->mReplyPort);
            m->Reply();
          }
        } break;
        case EMouseMove:
//          dlog("mouse.device queued %x\n", m);
          messages.AddTail(*m);
          break;
        default:
          break;
      }
    }
  }
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

MouseDevice::MouseDevice() : BDevice("mouse.device") {
  TSystemInfo i;
  gExecBase.GetSystemInfo(&i);
  mDisplayWidth = i.mDisplayWidth;
  mDisplayHeight = i.mDisplayWidth;
  // initial position
  mX = mY = 20;
  gExecBase.AddTask(new MouseTask(this));
  //
}

MouseDevice::~MouseDevice() {
  //
}
