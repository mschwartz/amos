#ifndef EXEC_DEVICES_SCREEN_SCREENVESA_H
#define EXEC_DEVICES_SCREEN_SCREENVESA_H

#include <Exec/Types.h>
#include <Devices/Screen.h>
#include <Graphics/bitmap/BBitmap32.h>

class ScreenVesa : public BScreen {
public:
  ScreenVesa();

  TBool IsCharacterDevice() {
    return EFalse;
  }
  void attr(TUint8 fg, TUint8 bg) {
    attribute = 0x0f;
//    attribute = ((bg << 4) & 0xf0) | (fg & 0x0f);
  }

  void MoveTo(int x, int y);
  void GetXY(int &x, int &y);
  void Down();

  // clear to end of line.  cursor is not moved.
  void ClearEOL(TUint8 ch = ' ');

  // scroll screen up one line starting at row y.  Fill in bottom row with bTInt
  void ScrollUp();

  void NewLine();
  void WriteChar(char c);
  void ClearScreen(TUint8 ch = ' ');
  void WriteString(const char *s);

  BBitmap32 *GetBitmap() { return mBitmap; }

protected:
  BBitmap32 *mBitmap;
//  TUint8 *screen;
  TUint8 attribute;
  TInt mX, mY;
  char buf[256];
};

#endif
