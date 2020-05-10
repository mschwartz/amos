#ifndef EXEC_DEVICES_SCREEN_SCREENVESA_H
#define EXEC_DEVICES_SCREEN_SCREENVESA_H

#include <Exec/Types.h>
#include <Graphics/bitmap/BBitmap32.h>

class ScreenVesa : public BNode {
public:
  ScreenVesa();

  TBool IsCharacterDevice() {
    return EFalse;
  }
  void attr(TUint8 fg, TUint8 bg) {
    attribute = 0x0f;
    //    attribute = ((bg << 4) & 0xf0) | (fg & 0x0f);
  }

public:
  // mouse curor methods
  void MoveCursor(TInt aX, TInt aY);
  TBool ShowCursor(); // returns previous state
  TBool HideCursor(); // returns previous state
  TBool SetCursor(TBool aShowIt) {
    if (aShowIt) {
      return ShowCursor();
    }
    else {
      return HideCursor();
    }
  }

public:
  void MoveTo(int x, int y);
  void GetXY(int &x, int &y);
  void Down();

  // clear to end of line.  cursor is not moved.
  void ClearEOL(TUint8 ch = ' ');

  // scroll screen up one line starting at row y.  Fill in bottom row with bTInt
  void ScrollUp();

  void NewLine();
  void WriteChar(char c);
  void Clear(TUint32 aColor);
  void WriteString(const char *s);
  void WriteString(TInt aX, TInt aY, const char *s);

  BBitmap32 *GetBitmap() { return mBitmap; }

  // copy aOther bitmap to screen at aDestX,aDestY (as in a window)
  void BltBitmap(BBitmap32 *aOther, TInt32 aDestX, TInt32 aDestY) {
    mBitmap->BltBitmap(aOther, aDestX, aDestY);
  }

protected:
  BBitmap32 *mBitmap;
  //  TUint8 *screen;
  TInt mX, mY;
  TInt mMouseX, mMouseY;
  char buf[256];
  TBool mMouseHidden;
  TUint8 attribute;
};

#endif
