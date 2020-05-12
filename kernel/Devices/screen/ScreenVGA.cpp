#include <Devices/screen/ScreenVGA.h>


#include <x86/cpu.h>
#include <x86/kernel_memory.h>

ScreenVGA::ScreenVGA() {
  screen = (TUint8 *)0xb8000;
  row = col = 0;
  attr(BLACK, WHITE);
  ClearScreen();
}

void ScreenVGA::MoveTo(int aX, int aY) {
  TUint16 pos = aY * VGA_WIDTH + aX;
  outb(0x3d4, 0x0f);
  outb(0x3d5, (TUint8)(pos & 0xff));
  outb(0x3d4, 0x0e);
  outb(0x3d5, (TUint8)((pos >> 8) & 0xff));
}

void ScreenVGA::GetXY(int &aX, int &aY) {
  aX = col;
  aY = row;
}

void ScreenVGA::ClearEOL(TUint8 aCharacter) {
  // TODO - VGA_BYTESPERROW should be this->mPitch
  TUint16 *dst = (TUint16 *)&screen[row * VGA_BYTESPERROW],
           blank = (attribute << 8) | aCharacter;

  for (int c = col; c < VGA_WIDTH; col++) {
    *dst++ = blank;
  }
}

void ScreenVGA::ScrollUp() {
  // TODO - VGA_BYTESPERROW should be this->mPitch
  TUint16 *dst = (TUint16 *)&screen[0 * VGA_BYTESPERROW],
           *src = (TUint16 *)&screen[(0 + 1) * VGA_BYTESPERROW],
           blank = (attribute << 8) | ' ';

  for (TInt r = 0; r < 25; r++) {
    for (TInt c = 0; c < VGA_WIDTH; c++) {
      *dst++ = *src++;
    }
  }
  for (TInt c = 0; c < VGA_WIDTH; c++) {
    *dst++ = blank;
  }

  row = 24;
//  cleareol();
  MoveTo(0, 24);
}

void ScreenVGA::Down() {
  row++;
  if (row > 24) {
    ScrollUp();
    row = 24;
  }
  MoveTo(col, row);
}

void ScreenVGA::NewLine() {
  col = 0;
  Down();
}

void ScreenVGA::WriteChar(char aCharacter) {
  if (aCharacter == 10) {
    col = 0;
    Down();
  }
  else if (aCharacter == 13) {
    col = 0;
  }
  else {
    TInt64 offset = row * 160 + col * 2;

    screen[offset++] = aCharacter;
    screen[offset++] = attribute;
    col++;

    if (col > 79) {
      col = 0;
      Down();
    }
  }
  MoveTo(col, row);
}

void ScreenVGA::ClearScreen(TUint8 aCharacter) {
  for (TInt i = 0; i < 25 * 80; i++) {
    screen[2 * i] = aCharacter;
    screen[2 * i + 1] = attribute;
  }
  row = col = 0;
  MoveTo(col, row);
}

void ScreenVGA::WriteString(const char *aString) {
  while (*aString) {
    WriteChar(*aString++);
  }
}

