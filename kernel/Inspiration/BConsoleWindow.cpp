#include <Inspiration/BConsoleWindow.h>
#include <posix/sprintf.h>
#include <stdarg.h>

BConsoleWindow::BConsoleWindow(const char *aTitle,
			       TInt32 aX, TInt32 aY,
			       TInt32 aW, TInt32 aH,
			       BScreen *aScreen)
  : BWindow(TNewWindow({
			.mLeft = aX,
			.mTop = aY,
			.mWidth = aW,
			.mHeight = aH,
			.mTitle = aTitle,
			.mIdcmpFlags = IDCMP_VANILLAKEY,
			.mScreen = aScreen,
      })) {
  mCharacterMap = ENull;

  // set palettes
  for (TInt i = 0; i < 16; i++) {
    mForegroundPalette[i].Set(255, 255, 255);
    mBackgroundPalette[i].Set(0, 0, 0);
  }

  mCursorEnabled = ETrue;

  mFont = new BConsoleFont32();
  mViewPort->SetFont(mFont);

  // keyboard buffer
  mBufferHead = mBufferTail = 0;

  Resize(mClientRect.Width(), mClientRect.Height());
}

void BConsoleWindow::Resize(TInt32 aW, TInt32 aH) {
  // TODO we're using a 16x8 hard coded font.  This should be dynamic!
  TInt newRows = aH / 16,
    newCols = aW / 8;

  // set up new map
  TUint16 *newMap = (TUint16 *)AllocMem(newRows * newCols * sizeof(TUint16));

  // clear new map to blanks
  for (TInt x = 0; x < newRows * newCols; x++) {
    newMap[x] = ' ';
  }

  // if we have an old map, we want to copy existing values into the new map
  if (mCharacterMap) {
    TInt w = newCols >= mCols ? mCols : newCols,
      h = newRows >= mRows ? mRows : newRows;
    for (TInt row = 0; row < h; row++) {
      for (TInt col = 0; col < w; col++) {
        newMap[row * newCols + col] = mCharacterMap[row * mCols + col];
      }
    }
  }

  mRows = newRows;
  mCols = newCols;

  if (mCharacterMap) {
    delete[] mCharacterMap;
  }
  mCharacterMap = newMap;

  mCharacterMapSize = mRows * mCols * sizeof(TUint16);
  mCharacterMapEnd = mCharacterMap + mCharacterMapSize;
}

BConsoleWindow::~BConsoleWindow() {
}

void BConsoleWindow::Dump() {
  dlog("BConsoleWindow(%)\n", mNodeName);
  mWindowRect.Dump();
  mClientRect.Dump();
  dlog("mRows(%d) mCols(%d) w(%d) h(%d)\n", mRows, mCols, mRows * 16, mCols * 8);
}

void BConsoleWindow::Repaint() {
  Paint();
  BWindow::Repaint();
}

void BConsoleWindow::Paint() {
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

void BConsoleWindow::ClearScreen() {
  for (TInt i = 0; i < mRows * mCols; i++) {
    mCharacterMap[i] = ' ';
  }
  mRow = mCol = 0;
  // Paint();
}

void BConsoleWindow::ClearEol() {
  TUint16 *ptr = &mCharacterMap[mRow * mCols + mCol];
  for (TInt col = mCol; col < mCols; col++) {
    *ptr++ = ' ';
  }
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void BConsoleWindow::ScrollUp(TInt32 aRow) {
  for (TInt row = aRow; row < mRows - 1; row++) {
    TUint16 *dst = &mCharacterMap[row * mCols + 0],
      *src = &mCharacterMap[(row + 1) * mCols + 0];

    // dlog("Scrollup row(%d) mRows(%d)\n", row, mRows);
    for (TInt col = 0; col < mCols; col++) {
      *dst++ = *src++;
    }
  }

  TUint16 *dst = &mCharacterMap[(mRows - 1) * mCols];
  for (TInt col = 0; col < mCols; col++) {
    *dst++ = ' ';
  }
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

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

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
    mRow = mRows - 1;
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

TUint16 BConsoleWindow::GetAt(TInt32 aRow, TInt32 aCol) {
  return mCharacterMap[aRow * mCols + aCol];
}

//
// keyboard
//
void BConsoleWindow::FillBuffer() {
  while (IdcmpMessage *m = GetMessage()) {
    if (m->mClass & IDCMP_VANILLAKEY) {
      if (((mBufferTail + 1) % CONSOLE_BUFFER_SIZE) != mBufferHead) {
	mBuffer[mBufferTail++] = m->mCode;
	mBufferTail %= CONSOLE_BUFFER_SIZE;
      }
      else {
	dlog("*** ConsoleWindow keyboard queue full %d/%d\n", mBufferHead, mBufferTail);
	bochs;
      }
    }
    m->Reply();
  }
}

TBool BConsoleWindow::KeyReady() {
  FillBuffer();
  return mBufferHead != mBufferTail;
}

TInt BConsoleWindow::ReadKey() {
  if (!KeyReady()) {
    return -1;
  }
  TInt key = mBuffer[mBufferHead++];
  mBufferHead %= CONSOLE_BUFFER_SIZE;
  return key;
}

TInt BConsoleWindow::ReadString(char *aString, TInt aMaxLength, char mDelimeter) {
  TInt count = 0;
  while (ETrue) {
    TInt key = ReadKey();
    if (key == mDelimeter) {
      break;
    }
    if (key != -1) {
      *aString++ = key;
      count++;
    }
  }
  return 0;
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
