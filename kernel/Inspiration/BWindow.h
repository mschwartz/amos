#ifndef AMOS_INSPIRATION_BWINDOW_H
#define AMOS_INSPIRATION_BWINDOW_H

#include <Types.h>
#include <Types/BList.h>
#include <Graphics/Graphics.h>

#include <Inspiration/NewWindow.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class InspirationBase;
class MessagePort;
class BScreen;
class BTask;

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

protected:
  TUint64 mIdcmpFlags;
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
  MessagePort *mIdcmpPort;

  InspirationBase &mInspirationBase;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BWindowList : public BList {
public:
  BWindowList() : BList("Window List") {}
};

#endif
