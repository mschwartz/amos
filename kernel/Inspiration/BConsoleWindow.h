#ifndef INSPIRATION_BCONSOLEWINDOW_H
#define INSPIRATION_BCONSOLEWINDOW_H

#include <Inspiration/BWindow.h>

/**
 * BConsoleWindow
 * 
 * Window with typewriter/terminal output and ANSI escape sequences.
 * 
 * Note: BeginPaint() -> render tezt as required -> EndPaint().
 *  if EndPaint() is not called, the changes will not end up on the screen.
 */
const TInt CONSOLE_BUFFER_SIZE = 4096;

class ConsoleWindowTask;

class BConsoleWindow : public BWindow {
  friend ConsoleWindowTask;

public:
  BConsoleWindow(const char *aTitle, TInt32 aX, TInt32 aY, TInt32 aW, TInt32 aH, BScreen *aScreen = ENull);
  virtual ~BConsoleWindow();

protected:
  BTask *mConsoleTask;

public:
  void Paint();
  void Dump(); // debugging

protected:
  void Repaint();

  //
  // Dimensions and character map
public:
  //
  // character map
  //
public:
  void Resize(TInt32 aW, TInt32 aH);
  //
  void ClearScreen();
  void ClearEol();
  //
  void ScrollUp(TInt aRow = 0);
  void ScrollDown(TInt aRow = -1);
  //
  void Flush();

protected:
  TUint16 *mCharacterMap, *mCharacterMapEnd;
  TUint16 *mShadowMap;
  TInt64 mCharacterMapSize;
  TInt32 mRows, mCols;

  //
  // cursor
  //
public:
  void RenderCursor(TBool aErase = EFalse);
  void ShowCursor(TBool aEnable = ETrue) { mCursorEnabled = aEnable; }
  void ToggleCursor() { mCursorEnabled = !mCursorEnabled; }
  void MoveTo(TInt32 aRow, TInt32 aCol);
  void Up();
  void Down();
  void Left();
  void Right();

protected:
  TInt32 mRow, mCol;
  TBool mCursorEnabled;

public:
  //
  // strings and characters
  //
  void Write(TInt32 aRow, TInt32 aCol, const char aChar); // putchar
  void Write(const char aChar);                           // putchar

  void Write(TInt32 aRow, TInt32 aCol, const char *aString); // puts
  void Write(const char *aString);                           // puts

  void WriteFormatted(TInt32 aRow, TInt32 aCol, const char *aString, ...); // printf
  void WriteFormatted(const char *aString, ...);                           // printf

  TUint16 GetAt(TInt32 aRow, TInt32 aCol);

  //
  // Keyboard
  //
public:
  TBool KeyReady();                                                             // is a key ready to read from keyboard?
  TInt ReadKey();                                                               // read a key from keyboard
  TInt ReadString(char *aString, TInt aMaxLength = -1, char mDelimiter = '\n'); // read a string from keyboard

protected:
  void FillBuffer();
  char mBuffer[CONSOLE_BUFFER_SIZE];
  TInt mBufferHead, mBufferTail;

  //
  // colors
  //
public:
  void SetForegroundPalette(TUint8 aIndex, TUint32 aColor) {
    mForegroundPalette[aIndex].Set(aColor);
  }
  void SetForegroundPalette(TUint8 aIndex, TRGB &aColor);
  void SetBackgroundPalette(TUint8 aIndex, TUint32 aColor) {
    mBackgroundPalette[aIndex].Set(aColor);
  }
  void SetBackgroundPalette(TUint8 aIndex, TRGB &aColor);

protected:
  TRGB mForegroundPalette[16], mBackgroundPalette[16];

  //
  // font
  //
public:
  void SetFont(BConsoleFont32 *aFont) {
    if (mFont) {
      delete mFont;
    }
    mFont = aFont;
  }

protected:
  BConsoleFont32 *mFont;
};

#endif
