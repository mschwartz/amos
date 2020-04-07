#ifndef FONT_H
#define FONT_H

#include <BTypes.h>
#include <Exec/Graphics/BFont.h>

extern const TInt16 *gVectorCharset[];

#if 0
class Font {
public:
  static TInt16 scale; // 8.8 fixed point

public:
  // these routine return the width of whatever is printed to the screen
  static TInt8 write(TInt8 x, TInt8 y, char c);
  static TInt8 printf(TInt8 x, TInt8 y);
#ifdef ENABLE_ROTATING_TEXT
  static TInt8 print_string_rotatedx(TInt8 x, TInt8 y, FLOAT angle, const __FlashStringHelper *ifsh);
#endif
  static TInt8 print_string(TInt8 x, TInt8 y, char *s);
  static TInt8 print_long(TInt8 x, TInt8 y, TInt64 n, TInt8 base = 10);
  static TInt8 print_float(TInt8 x, TInt8 y, double number, TInt8 digits = 2);
};
#endif

#endif
