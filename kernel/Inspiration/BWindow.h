#ifndef AMOS_INSPIRATION_BWINDOW_H
#define AMOS_INSPIRATION_BWINDOW_H

#include <Types.h>
#include <Types/BList.h>
#include <Graphics/Graphics.h>

#include <Inspiration/NewWindow.h>
#include <Exec/MessagePort.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class InspirationBase;
class IdcmpTask;
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
  friend IdcmpTask;

public:
  // BWindow(const char *aTitle, TInt32 aX, TInt32 aY, TInt32 aW, TInt32 aH, BScreen *aScreen = ENull);
  BWindow(const TNewWindow &aNewWindow);
  virtual ~BWindow();

public:
  virtual void Paint() = 0;

public:
  const char *Title() { return mNodeName; }

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
  IdcmpMessage *GetMessage();

protected:
  TUint64 mIdcmpFlags;
  MessagePort *mIdcmpPort;

protected:
  TUint64 mWindowFLags;
  BScreen *mScreen; // BScreen this window is rendered on

  BBitmap32 *mBitmap; // bitmap of window's contents

  TRect mWindowRect, // TRect of the entire window, including titlebar and decorations
    mClientRect;     // TRect of the client area of the window

  BViewPort32 *mWindowViewPort, // entire window
    *mViewPort;                 // client area

  TInt32 mMinWidth, mMinHeight;
  TInt32 mMaxWidth, mMaxHeight;

  BTask *mTask;

  InspirationBase &mInspirationBase;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BWindowList : public BList {
public:
  BWindowList() : BList("Window List") {}
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

struct IdcmpMessage : public BMessage {
  TUint64 mClass;
  TUint64 mCode;
  TUint64 mQualifier;
  TAny *mAddress;
  TInt64 mMouseX, mMouseY;
  TUint64 mTime; // milliseconds
  BWindow *mWindow;
};

enum EIdcmpCommand {
  EIdcmpSubscribe,
  EIdcmpUnsubscribe,
  EIdcmpUpdateFlags,
};

#endif
