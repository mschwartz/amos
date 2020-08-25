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

  TCoordinate WindowLeft() { return mWindowRect.x1; }
  TCoordinate WindowTop() { return mWindowRect.y1; }
  TCoordinate WindowWidth() { return mWindowRect.Width(); }
  TCoordinate WindowHeight() { return mWindowRect.Height(); }

  TCoordinate ClientLeft() { return mClientRect.x1; }
  TCoordinate ClientTop() { return mClientRect.y1; }
  TCoordinate ClientWidth() { return mClientRect.Width(); }
  TCoordinate ClientHeight() { return mClientRect.Height(); }

  void MoveTo(TCoordinate aX, TCoordinate aY);

  TBool Obscured(BWindow *aOther);
  TBool OverDragBar(TCoordinate aX, TCoordinate aY);

protected:
  virtual void Repaint();

  /**
   * Paint window title bar, borders, etc.
   */
  void PaintDecorations();

public:
  void Clear(TUint32 aColor) {
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

  TCoordinate mMinWidth, mMinHeight;
  TCoordinate mMaxWidth, mMaxHeight;

  TUint32 mForegroundColor,
    mBackgroundColor;

  BConsoleFont32 *mFont;

  BTask *mTask;

  InspirationBase &mInspirationBase;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class WindowList : public BList {
public:
  WindowList() : BList("Window List") {}

public:
  BWindow *First() { return (BWindow *)mNext; }
  BWindow *Last() { return (BWindow *)mPrev; }

public:
  void Lock() { mSpinLock.Acquire(); }
  void Unlock() { mSpinLock.Release(); }

protected:
  SpinLock mSpinLock;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

struct IdcmpMessage : public BMessage {
  TUint64 mClass;
  TUint64 mCode;
  TUint64 mQualifier;
  TAny *mAddress;
  TCoordinate mMouseX, mMouseY;
  TCoordinate mLastMouseX, mLastMouseY;
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

#endif
