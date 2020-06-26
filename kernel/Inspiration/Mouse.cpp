#include <Inspiration/InspirationBase.h>
#include <Inspiration/Mouse.h>
#include <Inspiration/Display.h>
#include <Exec/ExecBase.h>
#include <Exec/BTask.h>
#include <Devices/MouseDevice.h>

static const char *cursor_image[] = {
  "Booooooooooo",
  "BBoooooooooo",
  "BXBooooooooo",
  "BXBBoooooooo",
  "BXXXBooooooo",
  "BXXXXBoooooo",
  "BXXXXXBooooo",
  "BXXXXXXBoooo",
  "BXXXXXXXBooo",
  "BXXXXXXXXBoo",
  "BXXXXXXXXXBo",
  "BXXXXXXBBBBB",
  "BXXXBXXBoooo",
  "BXXBBXXBoooo",
  "BXBooBXXBooo",
  "BBoooBXXBooo",
  "BoooooBXXBoo",
  "ooooooBXXBoo",
  "oooooooBXXBo",
  "oooooooBXXBo",
  "ooooooooBBoo",
};

static const TInt cursor_width = 12, cursor_height = 21;

class MousePointerTask : public BTask {
public:
  MousePointerTask(Mouse *aMouse)
      : BTask("MousePointer", LIST_PRI_MAX), mDisplay(mInspirationBase.GetDisplay()) {
    mMouse = aMouse;
  }

protected:
  Display *mDisplay;
  Mouse *mMouse;

public:
  void Run() {
    MessagePort *mousePort;

    DISABLE;
    while ((mousePort = gExecBase.FindMessagePort("mouse.device")) == ENull) {
      Sleep(1);
    }
    ENABLE;

    MessagePort *replyPort = CreateMessagePort("replyPort");
    MouseMessage *message = new MouseMessage(replyPort, EMouseMove);

    message->mReplyPort = replyPort;
    message->SendMessage(mousePort);
    while (1) {
      WaitPort(replyPort);
      while (MouseMessage *m = (MouseMessage *)replyPort->GetMessage()) {
        if (m == message) {
          //          dlog("Move Cursor %d,%d\n", m->mMouseX, m->mMouseY);
          mMouse->MoveTo(m->mMouseX, m->mMouseY);
          message->mReplyPort = replyPort;
          message->SendMessage(mousePort);
        }
        else {
          dprint("\n\n");
          dlog("MouseTask: %x != %x\n\n\n", m, message);
          // shouldn't happen!
        }
      }
    }
  }
};

Mouse::Mouse(Display *aDisplay) {
  mDisplay = aDisplay;
  mLastX = mX = 20;
  mLastY = mY = 20;
  mBackgroundSave = (TUint32 *)AllocMem(cursor_width * cursor_height * sizeof(TUint32));
  mHidden = EFalse;
  // Render();
  gExecBase.AddTask(new MousePointerTask(this));
}

Mouse::~Mouse() {
  delete[] mBackgroundSave;
}

void Mouse::MoveTo(TInt32 aX, TInt32 aY) {
  mLastX = mX;
  mX = aX;
  mLastY = mY;
  mY = aY;
  dlog("Mouse::MoveTo(%d,%d)\n", aX, aY);
}

TBool Mouse::Hide() {
  TBool ret = mHidden;
  if (!mHidden) {
    Restore();
  }
  mHidden = ETrue;
  return ret;
}

TBool Mouse::Show() {
  TBool ret = mHidden;
  mHidden = EFalse;
  return ret;
}

TBool Mouse::Set(TBool aShowit) {
  return aShowit ? Show() : Hide();
}

void Mouse::Restore() {
  return;
  if (mHidden) {
    return;
  }

  BBitmap32 *bm = mDisplay->mBitmap;
  TUint32 *bkg = mBackgroundSave;

  for (TInt y = 0; y < cursor_height; y++) {
    for (TInt x = 0; x < cursor_width; x++) {
      bm->PlotPixel(*bkg++, x + mX, y + mY);
    }
  }
}

void Mouse::Render() {
  return;
  if (mHidden) {
    return;
  }
  BBitmap32 *bm = mDisplay->mBitmap;
  TUint32 *bkg = mBackgroundSave;

  dlog("Render(%d, %d) display(%x) bitmap(%x)", mX, mY, mDisplay, bm);

  for (TInt y = 0; y < cursor_height; y++) {
    const char *src = cursor_image[y];
    for (TInt x = 0; x < cursor_width; x++) {
      TRGB c(bm->ReadPixel(mX + x, mY + y));
      *bkg++ = c.rgb888();
      switch (*src++) {
        case 'B':
          // render pixel below half bright
          c.r /= 2;
          c.g /= 2;
          c.b /= 2;
          bm->PlotPixel(c, mX + x, mY + y);
          break;

        case 'X':
          // render black pixel
          bm->PlotPixel(0x000000, mX + x, mY + y);
          break;

        default:
          break;
      }
    }
  }
}
