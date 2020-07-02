#include <Inspiration/Display/Cursor.h>
#include <Inspiration/Display.h>
#include <Inspiration/BScreen.h>
#include <Graphics/bitmap/BBitmap32.h>

static const char *cursor_image[] = {
  "Booooooooooo",
  "BBoooooooooo",
  "BXBooooooooo",
  "BXBBoooooooo",
  "BXXXBooooooo",
  "BXXXXBoooooo",
  "BXXXXXBooooo",
  "BXXXXXXBoooo",
  "BXXXXXXXBooo",
  "BXXXXXXXXBoo",
  "BXXXXXXXXXBo",
  "BXXXXXXBBBBB",
  "BXXXBXXBoooo",
  "BXXBBXXBoooo",
  "BXBooBXXBooo",
  "BBoooBXXBooo",
  "BoooooBXXBoo",
  "ooooooBXXBoo",
  "oooooooBXXBo",
  "oooooooBXXBo",
  "ooooooooBBoo",
};

static const TInt cursor_width = 12, cursor_height = 21;

void Cursor::Render(BBitmap32 *aBitmap, TInt32 aX, TInt32 aY) {

  // dlog("RenderCursor(%d,%d)\n", aX, aY);

  BBitmap32 *bm = aBitmap;
  // TUint32 *sv = mSavedBackground;

  for (TInt y = 0; y < mImage.mHeight; y++) {
    const char *src = mImage.mImage[y];
    for (TInt x = 0; x < mImage.mWidth; x++) {

      TUint32 c = bm->ReadPixel(aX + x, aY + y);
      // bm->PlotPixel(*sv, aLastX + x, aLastY + y);
      // *sv++ = c;

      switch (*src++) {
        case 'B':
          bm->PlotPixel(0xffffff, aX + x, aY + y);
          break;

        case 'X':
          // render black pixel
          bm->PlotPixel(0x000000, aX + x, aY + y);
          break;

        default:
          break;
      }
    }
  }
}

void Cursor::AddDirtyRect(BScreen *aScreen, TInt32 aX, TInt32 aY) {
  aScreen->AddDirtyRect(aX, aY, aX + Width() - 1, aY + Height() - 1);
}

void Cursor::Save(BBitmap32 *aBitmap, TInt32 aX, TInt32 aY) {
  TUint32 *bkg = mSavedBackground;;

  // dlog("SaveCursor(%d,%d)\n", aX, aY);

  for (TInt y = 0; y < cursor_height; y++) {
    for (TInt x = 0; x < cursor_width; x++) {
      *bkg++ = aBitmap->ReadPixel(x + aX, y + aY);
    }
  }
  
}

void Cursor::Restore(BBitmap32 *aBitmap, TInt32 aX, TInt32 aY) {
  TUint32 *bkg = mSavedBackground;;

  // dlog("RestoreCursor(%d,%d)\n", aX, aY);

  for (TInt y = 0; y < cursor_height; y++) {
    for (TInt x = 0; x < cursor_width; x++) {
      aBitmap->PlotPixel(*bkg++, x + aX, y + aY);
    }
  }
  
}

void Cursor::AllocSavedBackground() {
  if (mSavedBackground) {
    delete[] mSavedBackground;
  }
  mSavedBackground = (TUint32 *)AllocMem(mImage.mWidth * mImage.mHeight * sizeof(TUint32));
}

Cursor::Cursor() {
  mSavedBackground = ENull;
  mImage.mWidth = cursor_width;
  mImage.mHeight = cursor_height;
  mImage.mImage = cursor_image;
  AllocSavedBackground();
}

Cursor::Cursor(TInt32 aWidth, TInt32 aHeight, const char **aImage) {
  mSavedBackground = ENull;
  mImage.mWidth = aWidth;
  mImage.mHeight = aHeight;
  mImage.mImage = aImage;
  AllocSavedBackground();
}

Cursor::Cursor(CursorImage *aImage) {
  mSavedBackground = ENull;
  mImage.mWidth = aImage->mWidth;
  mImage.mHeight = aImage->mHeight;
  mImage.mImage = aImage->mImage;
  AllocSavedBackground();
}
