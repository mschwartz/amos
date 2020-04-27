#ifndef EXEC_DEVICE_SCREEN_SCREENVGA_H
#define EXEC_DEVICE_SCREEN_SCREENVGA_H

/*!
 * 6845 CRT Controller (Text Mode)
 */

#include <Exec/Types.h>
#include <Devices/Screen.h>

const TInt VGA_WIDTH = 80;
const TInt VGA_HEIGHT = 25;
const TInt VGA_BYTESPERROW = VGA_WIDTH * 2;

const TUint8 BLACK = 0;
const TUint8 BLUE = 1;
const TUint8 GREEN = 2;
const TUint8 CYAN = 3;
const TUint8 RED = 4;
const TUint8 MAGENTA = 5;
const TUint8 BROWN = 6;
const TUint8 LTGREY = 7;

const TUint8 DKGREY = 0x08;
const TUint8 LTBLUE = 0x09;
const TUint8 LTGREEN = 0x0a;
const TUint8 LTCYAN = 0x0b;
const TUint8 LTRED = 0x0c;
const TUint8 PINK = 0x0d;
const TUint8 YELLOW = 0x0e;
const TUint8 WHITE = 0x0f;

class ScreenVGA : public BScreen {
public:
  ScreenVGA();

  TBool IsCharacterDevice() {
    return ETrue;
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

protected:
  TUint8 *screen;
  TUint8 attribute;
  TInt16 row, col;
  char buf[256];
};

#endif
