#include <Exec/ExecBase.h>
#include <Exec/x86/cpu.h>
#include <Devices/ScreenDevice.h>
#include <Devices/MouseDevice.h>
#include <Devices/Screen.h>
#include <Exec/BTask.h>

#define CURSOR_WIDTH 16
#define CURSOR_HEIGHT 16

static void RenderMouse(TInt aX, TInt aY, BBitmap32 *aBitmap) {
  TInt w = CURSOR_WIDTH / 2,
       maxx = aX + w,
       h = CURSOR_HEIGHT / 2,
       maxy = aY + h;
  TRect rect;
  aBitmap->GetRect(rect);

  for (TInt y = aY - h; y < maxy; y++) {
    for (TInt x = aX - CURSOR_WIDTH; x < maxx; x++) {
      if (rect.PointInRect(x, y)) {
        TUint32 pix = aBitmap->ReadPixel(x, y);
        pix ^= 0xffffffff;
        aBitmap->PlotPixel(pix, x, y);
      }
    }
  }
}

/**
  * MouseTask - receives EMouseMove messgaes from mouse.device and moves the mouse pointer.
  */
class MouseTask : public BTask {
public:
  MouseTask(ScreenDevice *aDevice) : BTask("screen.mouse", 0) {
//    dprint("\nConstruct ScreenDevice/MouseTask\n\n");
//    mDevice = aDevice;
//    mLastX = mLastY = -1;
//    mTaskState = ETaskRunning;
  }

  void Run() {
    while (ETrue) { Sleep(1); }
    dprint("\n============ ScreenDevice MouseTask running\n\n");
    DISABLE;
    bochs
    BBitmap32 *bitmap = mDevice->GetBitmap();
    bochs

    bochs
    MessagePort *replyPort = CreateMessagePort("screen.device.reply");
    bochs

    MessagePort *mousePort;
    dprint("\n\n============= ScreenDevice waiting for  mouse.device\n\n");
    dprint("\n");
    bochs
    ENABLE;

    while (ETrue) {
      mousePort = gExecBase.FindMessagePort("mouse.device");
      if (mousePort) {
        break;
      }
      dprint("\n");
      dprint("======== ScreenDevice find(mouse.device) = %x\n", mousePort);
      dprint("\n");
//      Sleep(1);
    }
    dprint("\n");
    dprint("============= ScreenDevice FOUND mouse.device\n");
    dprint("\n");
    // we can reuse this MouseMessage over and over again.
    MouseMessage *mouseMessage = new MouseMessage(replyPort, EMouseMove);
    while (ETrue) {
      mouseMessage->SendMessage(mousePort);
      dprint("ScreenTask/Mouse sent message %x\n", mouseMessage);
      while (ETrue) {
        WaitPort(replyPort);
        while (MouseMessage *m = (MouseMessage *)replyPort->GetMessage()) {
          if (m == mouseMessage) {
            if (mLastX != -1) {
              RenderMouse(m->mMouseX, m->mMouseY, bitmap);
              mLastX = m->mMouseX;
              mLastY = m->mMouseY;
              RenderMouse(m->mMouseX, m->mMouseY, bitmap);
            }
            mouseMessage->SendMessage(mousePort);
          }
        }
      }
    }
  }

protected:
  TInt mLastX, mLastY;
  ScreenDevice *mDevice;
};

/**
  * ScreenTask creates screen.device MessagePort and handes incoming Screenmessage messages.
  */
class ScreenTask : public BTask {
public:
  ScreenTask(ScreenDevice *aDevice) : BTask("screen.device", 0) {
    dprint("Construct ScreenDevice/ScreenTask\n");
    mDevice = aDevice;
  }

  void Run() {
    dprint("\n============ ScreenDevice ScreenTask running\n\n");
    gExecBase.DumpTasks();
    MessagePort *port = CreateMessagePort("screen.device");
    gExecBase.AddMessagePort(*port);
    while (ETrue) {
      WaitPort(port);
      ScreenMessage *m;
      while ((m = (ScreenMessage *)port->GetMessage())) {
      }
    }
  }

protected:
  ScreenDevice *mDevice;
};

ScreenDevice::ScreenDevice() : BDevice("screen.device") {
  dprint("\n=== Construct ScreenDevice ===\n\n");
  mScreen = new ScreenVesa();
  mScreen->GetBitmap()->GetRect(mRect);
  DISABLE;
  gExecBase.AddTask(new MouseTask(this));
//  gExecBase.AddTask(new ScreenTask(this));
  gExecBase.DumpTasks();
  ENABLE;
}
