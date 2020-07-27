#ifndef AMOS_INSPIRATION_BWINDOW_H
#define AMOS_INSPIRATION_BWINDOW_H

#include <Types.hpp>
#include <Types/BList.hpp>
#include <Graphics/Graphics.hpp>

#include <Inspiration/NewWindow.hpp>
#include <Exec/MessagePort.hpp>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class InspirationBase;
class MessagePort;
class BScreen;
class BTask;
class IdcmpMessage;

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BWindow : public BNode {
  friend InspirationBase;
  friend BScreen;

public:
  // BWindow(const char *aTitle, TInt32 aX, TInt32 aY, TInt32 aW, TInt32 aH, BScreen *aScreen = ENull);
  BWindow(const TNewWindow &aNewWindow);
  virtual ~BWindow();

public:
  const char *Title() { return mNodeName; }

  TInt32 WindowLeft() { return mWindowRect.x1; }
  TInt32 WindowTop() { return mWindowRect.y1; }

  TInt32 ClientLeft() { return mClientRect.x1; }
  TInt32 ClientTop() { return mClientRect.y1; }

  void MoveTo(TInt32 aX, TInt32 aY);

protected:
  virtual void Repaint();

  /**
   * Paint window title bar, borders, etc.
   */
  void PaintDecorations();

public:
  void BeginPaint() {}
  void EndPaint() {
    Repaint();
  }

  void Clear(TUint32 aColor) {
    // mViewPort->Dump();
    mViewPort->Clear(aColor);
  }

  void Clear(const TRGB &aColor) {
    Clear(aColor.rgb888());
  }

public:
  /**
   * SetIdcmpPort
   * Remove/delete old IdcmpPort and set new one for specified Task
   */
  void SetIdcmpPort(BTask *aTask);

  /**
   * GetMessage
   * Remve and return next IdcmpMessage from IdcmpPort
   */
  IdcmpMessage *GetMessage();

  TBool IsActive();
  void Activate();

public:
  TUint64 mIdcmpFlags;
  MessagePort *mIdcmpPort;

public:
  TUint64 WindowFlags() { return mWindowFlags; }

protected:
  TUint64 mWindowFlags;
  BScreen *mScreen; // BScreen this window is rendered on

  BBitmap32 *mBitmap; // bitmap of window's contents

  TRect mWindowRect, // TRect of the entire window, including titlebar and decorations
    mClientRect;     // TRect of the client area of the window

  BViewPort32 *mWindowViewPort, // entire window
    *mViewPort;                 // client area

  TInt32 mMinWidth, mMinHeight;
  TInt32 mMaxWidth, mMaxHeight;

  TUint32 mForegroundColor,
    mBackgroundColor;

  BConsoleFont32 *mFont;

  BTask *mTask;

  InspirationBase &mInspirationBase;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BWindowList : public BList {
public:
  BWindowList() : BList("Window List") {}

public:
  BWindow *First() { return (BWindow *)mNext; }
  BWindow *Last() { return (BWindow *)mPrev; }
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

struct IdcmpMessage : public BMessage {
  TUint64 mClass;
  TUint64 mCode;
  TUint64 mQualifier;
  TAny *mAddress;
  TInt32 mMouseX, mMouseY;
  TInt32 mLastMouseX, mLastMouseY;
  TUint64 mTime; // milliseconds
  BWindow *mWindow;

public:
  void Dump() {
    dlog("IdcmpMessage(%x)\n", this);
    dlog("       mTime: %d\n", mTime);
    dlog("      mClass: %x\n", mClass);
    dlog("       mCode: %x\n", mCode);
    dlog("  mQualifier: %x\n", mQualifier);
    dlog("    mAddress: %x\n", mAddress);
    dlog("       mouse: %d,%d\n", mMouseX, mMouseY);
    dlog("     mWindow: %s\n", mWindow->Title());
  }
};

enum EIdcmpCommand {
  EIdcmpSubscribe,
  EIdcmpUnsubscribe,
  EIdcmpUpdateFlags,
};

#endif
