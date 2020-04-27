#ifndef EXEC_BBITMAP_H
#define EXEC_BBITMAP_H

#include <Exec/BBase.h>
#include <Graphics/TRGB.h>
#include <Graphics/TRect.h>

// base class for different depth bitmaps
class BBitmap : public BBase {
public:
  // factory method
  // returns appropriate kind of BBitmap with specfied width, height, and depth.
  // if aMemory is provided (not null), that memory is used as the pixels data and that memory
  // will not be freed upon destruction.
  //  static BBitmap *CreateBitmap(TInt32 aWidth, TInt32 aHeight, TInt32 aDepth, TInt32 aPitch, TAny *aMemory = nullptr);

public:
  virtual void ClearScreen(TUint32 aColor) = 0;
  void ClearScreen(TRGB &aColor) { ClearScreen(aColor.rgb888()); }

public:
  TInt32 Width() { return mWidth; }
  TInt32 Height() { return mHeight; }
  TInt32 Depth() { return mDepth; }
  TInt32 Pitch() { return mPitch; }
  void GetRect(TRect &aRect) { aRect = mRect; }

protected:
  BBitmap(TInt32 aWidth, TInt32 aHeight, TInt32 aDepth, TInt32 aPitch, TAny *aMemory);
  virtual ~BBitmap() = 0;

protected:
  TInt32 mWidth, mHeight, mDepth, mPitch;
  TBool mFreePixels; // if set, mPixels will be freed in destructor, default = true
  TRect mRect;
} PACKED;

#endif
