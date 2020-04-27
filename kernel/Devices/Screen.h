/*!
 * 6845 CRT Controller
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <Exec/Types.h>
#include <Exec/BBase.h>
//#include "../x86/types.h"

class BScreen : public BBase {
public:
  static BScreen *CreateScreen();

  virtual TBool IsCharacterDevice() = 0;
  void attr(TUint8 fg, TUint8 bg) {
    attribute = 0x0f;
//    attribute = ((bg << 4) & 0xf0) | (fg & 0x0f);
  }

  virtual void MoveTo(int x, int y) = 0;
  virtual void GetXY(int &x, int &y) = 0;
  virtual void Down() = 0;

  // clear to end of line.  cursor is not moved.
  virtual void ClearEOL(TUint8 ch = ' ') = 0;

  // scroll screen up one line starting at row y.  Fill in bottom row with bTInt
  virtual void ScrollUp() = 0;

  virtual void NewLine() = 0;
  virtual void WriteChar(char c) = 0;
  virtual void ClearScreen(TUint8 ch = ' ') = 0;
  void ClearScreen(TUint32 aColor) {}
  virtual void WriteString(const char *s) = 0;

  void HexNybble(const TUint8 n);
  void HexByte(const TUint8 b);
  void HexWord(const TUint16 w);
  void HexLong(const TUint32 l);
  void HexQuad(const TUint64 l);
  void HexDump(const void *addr, TInt count);

protected:
  TUint8 attribute;
  char buf[256];
};

//extern Screen *gScreen = 0;

#include <Devices/screen/ScreenVGA.h>
#include <Devices/screen/ScreenVesa.h>

#endif
