#ifndef INSPIRATION_CURSOR_H
#define INSPIRATION_CURSOR_H

#include <Types.hpp>
#include <Exec/BBase.hpp>

class Display;
class BBitmap32;
class BScreen;

typedef struct {
  TInt32 mWidth, mHeight;
  const char **mImage;
} CursorImage;

class Cursor : public BBase {
public:
  Cursor();
  Cursor(TInt32 aWidth, TInt32 aHeight, const char **aImage);
  Cursor(CursorImage *aImage);

protected:
  void AllocSavedBackground();

public:
  void Save(BBitmap32 *aBitmap, TInt32 aX, TInt32 aY);
  void Restore(BBitmap32 *aBitmap, TInt32 aX, TInt32 aY);
  void Render(BBitmap32 *aBitmap, TInt32 aX, TInt32 aY);

public:
  TInt32 Width() { return mImage.mWidth; }
  TInt32 Height() { return mImage.mHeight; }

  void AddDirtyRect(BScreen *aScreen, TInt32 aX, TInt32 aY);

protected:
  CursorImage mImage;
  TUint32 *mSavedBackground;
};

#endif
