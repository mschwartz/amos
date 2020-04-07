#ifndef DEVICES_BITMAPSCREEN_H
#define DEVICES_BITMAPSCREEN_H

#include <BTypes.h>
#include <Devices/Screen.h>

class BitmapScreen : public Screen {
public:
  BitmapScreen();

public:
  void moveto(int x, int y);
  void getxy(int &x, int &y);
  void down();

  // clear to end of line.  cursor is not moved.
  void cleareol(TUint8 ch = ' ');

  // scroll screen up one line starting at row y.  Fill in bottom row with bTInt
  void scrollup();

  void newline();
  void putc(char c);
  void cls(TUint8 ch = ' ');
  void puts(const char *s);

protected:
  BBitmap *mScreen;
  TRGB mColor;
  TInt16 mX, mY;
};

#endif
