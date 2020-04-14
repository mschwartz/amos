#include "BitmapScreen.h"
#include <x86/bochs.h>

BitmapScreen::BitmapScreen() {
  dprint("BBitmapScreen constructor\n");
  Screen::DumpModes();
//  gDisplayModes->mDisplayMode.Dump();

  TDisplayMode& mode = Screen::GetDisplayMode();
//  TModeInfo *mode = &gDisplayModes->mDisplayMode;
  mScreen = BBitmap::CreateBitmap(mode.mWidth, mode.mHeight, mode.mDepth);
  mX = mY = 0;
  mColor = TRGB(0, 0, 0);
}

void BitmapScreen::moveto(int x, int y) {
  mX = x;
  mY = y;
}

void BitmapScreen::getxy(int &aX, int &aY) {
  aX = mX;
  aY = mY;
}

void BitmapScreen::cleareol(TUint8 ch) {
  mScreen->Clear(mColor);
  //
}

void BitmapScreen::scrollup() {
  //
}

void BitmapScreen::down() {
  //
}

void BitmapScreen::newline() {
  //
}

void BitmapScreen::putc(char c) {
  dprint("bitmap screen putc(%c}\n", c);
}

void BitmapScreen::cls(TUint8 ch) {
  //
}

void BitmapScreen::puts(const char *s) {
  while (*s) {
    putc(*s++);
  }
}
