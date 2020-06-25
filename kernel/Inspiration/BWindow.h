#ifndef AMOS_INSPIRATION_BWINDOW_H
#define AMOS_INSPIRATION_BWINDOW_H

#include <Exec/Types.h>
#include <Exec/Types/BList.h>
#include <Graphics/Graphics.h>

class InspirationBase;
class BScreen;

class BWindow : public BNode {
  friend InspirationBase;
  friend BScreen;

public:
  BWindow(const char *aTitle, TInt32 aX, TInt32 aY, TInt32 aW, TInt32 aH);
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
  void BeginPaint() { mPainting = ETrue; }
  void EndPaint() {
    mPainting = EFalse;
    Repaint();
    mDirty = ETrue;
  }

  void Clear(TUint32 aColor) {
    // mViewPort->Dump();
    mViewPort->Clear(aColor);
  }

  void Clear(const TRGB &aColor) {
    Clear(aColor.rgb888());
  }

protected:
  BBitmap32 *mBitmap;
  TRect mWindowRect, mClientRect;
  BViewPort32 *mWindowViewPort, // entire window
    *mViewPort;                 // client area
  TBool mPainting, mDirty;
  InspirationBase &mInspirationBase;
};

class BWindowList : public BList {
public:
  BWindowList() : BList("Window List") {}
};

#endif
