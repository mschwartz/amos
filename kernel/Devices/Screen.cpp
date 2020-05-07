#include <Devices/Screen.h>
#include <Devices/screen/ScreenVGA.h>
#include <Devices/screen/ScreenVesa.h>
#include <Exec/x86/kernel_memory.h>

BScreen *BScreen::CreateScreen() {
#ifdef KGFX
  return new ScreenVesa;
#else
  return new ScreenVGA;
#endif
}

void BScreen::HexNybble(const TUint8 aNybble) {
  const char *nybbles = "0123456789ABCDEF";
  WriteChar(nybbles[aNybble & 0x0f]);
}

void BScreen::HexByte(const TUint8 aByte) {
  HexNybble(aByte >> 4);
  HexNybble(aByte);
}

void BScreen::HexWord(const TUint16 aWord) {
  HexByte(aWord >> 8);
  HexByte(aWord);
}

void BScreen::HexLong(const TUint32 aLong) {
  HexWord((aLong >> 16) & 0xffff);
  HexWord(aLong & 0xffff);
}

void BScreen::HexQuad(const TUint64 aQuad) {
  HexLong((aQuad >> 32) & 0xffffffff);
  HexLong(aQuad & 0xffffffff);
}

void BScreen::HexDump(const TAny *aAddress, TInt aCount) {
  TUint8 *ptr = (TUint8 *)aAddress;
  TUint64 address = (TUint64)aAddress;
  HexLong(address);
  WriteChar(' ');
  for (TInt i = 0; i < aCount; i++) {
    HexByte(*ptr++);
    WriteChar(' ');
  }
  NewLine();
}

//Screen *gScreen;
