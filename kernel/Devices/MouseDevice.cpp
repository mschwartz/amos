#include <Exec/ExecBase.hpp>
#include <Exec/x86/cpu_utils.hpp>
#include <Exec/x86/ps2.hpp>
#include <Devices/MouseDevice.hpp>

const TUint32 MOUSE_DR = 0x60;
const TUint32 MOUSE_SR = 0x64;
const TUint32 MOUSE_PORT = 0x60;
const TUint32 MOUSE_STATUS = 0x64;

#define MOUSE_ABIT 0x02
#define MOUSE_BBIT 0x01
#define MOUSE_WRITE 0xd4
#define MOUSE_F_BIT 0x20
#define MOUSE_V_BIT 0x08

#define MOUSE_ALWAYS_SET (1<<3)

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
  TInt64 mX, mY;
  TUint64 mState;
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
  TInt64 Run();

protected:
  MouseDevice *mDevice;
  MessagePort *mMessagePort;
};

TBool MouseInterrupt::Run(TAny *aData) {
  DISABLE;
  TInt8 in_byte = mouse_read();
  ENABLE;

  switch (mState) {
    case 0:
      mPacket[0] = in_byte;

      if (!(in_byte & MOUSE_ALWAYS_SET)) {
        break;
      }

      mState++;

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
      mState++;
      break;

    case 2:
      mPacket[2] = in_byte;
      mState++;

      if (mPacket[0] & (1 << 7) || mPacket[0] & (1 << 6)) {
	// ignore overflow
	break;
      }
      TInt32 x = mPacket[1];
      if (mPacket[0] & (1 << 4)) {
	x |= 0xffffff00;
      }
      mX += x;
      CLAMP(mX, 0, mMaxX);

      TInt32 y = mPacket[2];
      if (mPacket[0] & (1 << 5)) {
	y |= 0xffffff00;
      }
      mY -= y;
      CLAMP(mY, 0, mMaxY);

      // dlog("Int dx(%02x) dy(%02x)\n", mPacket[1], mPacket[2]);

      // send message to Mouse Task
      {
        MouseMessage *m = new MouseMessage(ENull, EMouseUpdate);
        m->mMouseX = mX;
        m->mMouseY = mY;
        m->mButtons = mButtons;
        m->Send(mTask->mMessagePort);
      }
      // send buttons message to Mouse Task
      {
        MouseMessage *m = new MouseMessage(ENull, EMouseButtonsUpdate);
        m->mMouseX = mX;
        m->mMouseY = mY;
        m->mButtons = mButtons;
        m->Send(mTask->mMessagePort);
      }
      break;
  }

  mState %= 3;

  gExecBase.AckIRQ(IRQ_MOUSE);
  return ETrue;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

extern "C" void mouse_trap();

TInt64 MouseTask::Run() {
  dprint("\n");
  dlog("MouseTask Run\n");
  Sleep(3);

  DISABLE;
  mMessagePort = CreatePort("mouse.device");
  gExecBase.AddMessagePort(*mMessagePort);

  //  dlog("Initialize Mouse Interrupt... ");

  // enable the auxilliary mouse device
  mouse_wait(1);
  outb(MOUSE_STATUS, 0xa8);
  mouse_wait(1);

  // enable the interrupts
  outb(MOUSE_STATUS, 0x20);
  mouse_wait(0);

  // TUint8 status = (inb(0x60) & ~0x20) | 2;
  TUint8 status = inb(0x60) | 2;
  mouse_wait(1);
  outb(MOUSE_STATUS, 0x60);
  mouse_wait(1);
  outb(MOUSE_PORT, status);

  // tell the mouse to use the default settings
  mouse_write(0xf6);
  mouse_read();

  // enable the mouse
  mouse_write(0xf4);
  mouse_read();

  // scroll wheel
  mouse_write(0xf2);
  mouse_read();
  TUint8 result = mouse_read();

  mouse_write(0xf3);
  mouse_read();
  mouse_write(200);
  mouse_read();

  mouse_write(0xf3);
  mouse_read();
  mouse_write(100);
  mouse_read();

  mouse_write(0xf3);
  mouse_read();
  mouse_write(80);
  mouse_read();

  mouse_write(0xf2);
  mouse_read();

  uint8_t tmp = inb(0x61);
  outb(0x61, tmp | 0x80);
  outb(0x61, tmp & 0x7F);
  inb(MOUSE_PORT);

  while ((inb(0x64) & 1)) {
    inb(0x60);
  }

  //  dprint("   Enable PIC (%d) and Vector(%d)\n", IRQ_MOUSE, EMouseIRQ);
  gExecBase.SetIntVector(EMouseIRQ, new MouseInterrupt(this));
  gExecBase.EnableIRQ(IRQ_MOUSE);
  gExecBase.AckIRQ(IRQ_MOUSE);

  ENABLE;

  BMessageList move_messages("move_mouse_list");
  BMessageList buttons_messages("buttons_mouse_list");

  for (;;) {
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
            while ((m = (MouseMessage *)move_messages.RemHead())) {
              // dlog("mouse move %x %d,%d %x\n", m, mDevice->mX, mDevice->mY, mDevice->mButtons);
              m->mMouseX = x;
              m->mMouseY = y;
              m->mButtons = buttons;
              //            dlog(" Reply message %x to port %x\n", m, m->mReplyPort);
              m->Reply();
            }
          } break;

          case EMouseButtonsUpdate: {
            TInt32 x = m->mMouseX,
                   y = m->mMouseY;
            TUint8 buttons = m->mButtons;

            mDevice->mX = x;
            mDevice->mY = y;
            mDevice->mButtons = buttons;

            delete m;

            while ((m = (MouseMessage *)buttons_messages.RemHead())) {
              //            dlog("mouse move %x %d,%d %x\n", m, mDevice->mX, mDevice->mY, mDevice->mButtons);
              m->mMouseX = x;
              m->mMouseY = y;
              m->mButtons = buttons;
              m->Reply();
            }
          } break;

          case EMouseMove:
            move_messages.AddTail(*m);
            break;

          case EMouseButtons:
            buttons_messages.AddTail(*m);
            break;

          default:
            break;
        }
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
