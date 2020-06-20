#include <Inspiration/BConsoleWindow.h>
#include <posix/sprintf.h>
#include <stdarg.h>

BConsoleWindow::BConsoleWindow(const char *aTitle, TInt32 aX, TInt32 aY, TInt32 aW, TInt32 aH)
    : BWindow(aTitle, aX, aY, aW, aH) {

  mCharacterMap = ENull;
  Resize(aW, aH);
}

void BConsoleWindow::Resize(TInt32 aW, TInt32 aH) {
  // TODO we're using a 16x8 hard coded font.  This should be dynamic!
  mRows = aH / 16;
  mCols = aW / 8;

  if (mCharacterMap) {
    delete[] mCharacterMap;
  }
  mCharacterMapSize = mRows * mCols * sizeof(TUint16);
  mCharacterMap = (TUint16 *)AllocMem(mCharacterMapSize);
  mCharacterMapEnd = mCharacterMap + mCharacterMapSize;

  mFont = new BConsoleFont32();
  mViewPort->SetFont(mFont);

  // set palettes
  for (TInt i = 0; i < 16; i++) {
    mForegroundPalette[i].Set(255,255,255);
    mBackgroundPalette[i].Set(0,0,0);
  }

  mCursorEnabled = ETrue;

  dlog("About to paint %d x %d @ %d,%d\n", mRows, mCols, mWindowRect.x1, mWindowRect.y1);
  BeginPaint();
  Clear();
  EndPaint();
}

BConsoleWindow::~BConsoleWindow() {
}

void BConsoleWindow::Repaint() {
  Paint();
  BWindow::Repaint();
}

void BConsoleWindow::Paint() {
  dlog("Paint!\n");
  BViewPort32 *vp = mViewPort; // client viewport
  TUint16 *ptr = &mCharacterMap[0];

  for (TInt row = 0; row < mRows; row++) {
    for (TInt col = 0; col < mCols; col++) {
      TUint16 ac = *ptr++;
      TRGB &fg = mForegroundPalette[(ac >> 8) & 0x0f],
           bg = mBackgroundPalette[(ac >> 8) & 0x0f];
      vp->SetColors(fg, bg);

      vp->DrawText(col * 8, row * 16, ac & 0x0ff);
    }
  }
}

void BConsoleWindow::Flush() {
  // flush any buffered characters
}

void BConsoleWindow::Clear() {
  for (TInt i = 0; i < mRows * mCols; i++) {
    mCharacterMap[i] = ' ';
  }
  mRow = mCol = 0;
  Paint();
}

void BConsoleWindow::ClearEol() {
  TUint16 *ptr = &mCharacterMap[mRow * mCols + mCol];
  for (TInt col = mCol; col < mCols; col++) {
    *ptr++ = ' ';
  }
}

void BConsoleWindow::ScrollUp(TInt32 aRow) {
  for (TInt row = aRow; row < mRows - 2; row++) {
    TUint16 *dst = &mCharacterMap[row * mCols + 0],
            *src = &mCharacterMap[(row + 1) * mCols + 0];

    for (TInt col = 0; col < mCols; col++) {
      *dst++ = *src++;
    }
  }

  TUint16 *dst = &mCharacterMap[(mRows - 2) * mCols];
  for (TInt col = 0; col < mCols; col++) {
    *dst++ = ' ';
  }
  // Paint();
}

void BConsoleWindow::ScrollDown(TInt32 aRow) {
  if (aRow == -1) {
    aRow = mRows - 1;
  }

  for (TInt row = aRow; row < mRows; row++) {
    TUint16 *src = &mCharacterMap[row * mCols + 0],
            *dst = &mCharacterMap[(row - 1) * mCols + 0];
    for (TInt col = 0; col < mCols; col++) {
      *dst++ = *src++;
    }
  }

  TUint16 *dst = &mCharacterMap[0];
  for (TInt col = 0; col < mCols; col++) {
    *dst++ = ' ';
  }
}

//
// Cursor
//
void BConsoleWindow::MoveTo(TInt32 aRow, TInt32 aCol) {
  mRow = CLAMP(aRow, 0, mRows);
  mCol = CLAMP(aCol, 0, mCols);
}

void BConsoleWindow::Up() {
  mRow--;
  if (mRow < 0) {
    mRow = 0;
    ScrollDown();
  }
}

void BConsoleWindow::Down() {
  mRow++;
  if (mRow >= mRows) {
    ScrollUp();
    mRow = mRows - 2;
  }
}
void BConsoleWindow::Left() {
  mCol--;
  if (mCol < 0) {
    mCol = 0;
    Up();
  }
}

void BConsoleWindow::Right() {
  mCol++;
  if (mCol >= mCols) {
    mCol = 0;
    Down();
  }
}

//
// strings and characters
//
// These methods advance the cursor, accordingly.
//
void BConsoleWindow::Write(TInt32 aRow, TInt32 aCol, const char aChar) {
  mRow = aRow;
  mCol = aCol;

  if (aChar == '\r') {
    mCol = 0;
  }
  else if (aChar == '\n') {
    ClearEol();
    mCol = 0;
    Down();
  }
  else {
    mCharacterMap[mRow * mCols + mCol] = (TUint16)aChar;

#if 0
    BViewPort32 *vp = mViewPort; // client viewport

    vp->SetColors(mForegroundPalette[0], mBackgroundPalette[0]);
    vp->DrawText(mCol * 8, mRow * 16, aChar);
#endif
    Right();
  }
}

void BConsoleWindow::Write(const char aChar) {
  Write(mRow, mCol, aChar);
}

void BConsoleWindow::Write(TInt32 aRow, TInt32 aCol, const char *aString) {
  MoveTo(aRow, aCol);
  while (*aString != '\0') {
    Write(*aString++);
  }
}
void BConsoleWindow::Write(const char *aString) {
  while (*aString != '\0') {
    Write(*aString++);
  }
}

void BConsoleWindow::WriteFormatted(TInt32 aRow, TInt32 aCol, const char *aFormat, ...) {
  va_list args;
  va_start(args, aFormat);
  char buf[2048], *pc = buf;
  vsprintf(buf, aFormat, args);
  MoveTo(aRow, aCol);
  while (*pc != '\0') {
    Write(*pc++);
  }
  va_end(args);
}

void BConsoleWindow::WriteFormatted(const char *aFormat, ...) {
  va_list args;
  va_start(args, aFormat);
  char buf[2048], *pc = buf;
  vsprintf(buf, aFormat, args);
  while (*pc != '\0') {
    Write(*pc++);
  }
  va_end(args);
}

TUint16 BConsoleWindow::Read(TInt32 aRow, TInt32 aCol) {
  return mCharacterMap[aRow * mCols + aCol];
}

//
// colors
//
void BConsoleWindow::SetForegroundPalette(TUint8 aIndex, TRGB &aColor) {
  mForegroundPalette[aIndex].Set(aColor);
}

void BConsoleWindow::SetBackgroundPalette(TUint8 aIndex, TRGB &aColor) {
  mBackgroundPalette[aIndex].Set(aColor);
}
