#ifndef EXEC_BBITMAP_H
#define EXEC_BBITMAP_H

#include <Exec/BBase.h>
#include <Graphics/TRGB.h>
#include <Graphics/TRect.h>

// base class for different depth bitmaps
class BBitmap : public BBase {
public:
  TInt32 Width() { return mWidth; }
  TInt32 Height() { return mHeight; }
  TInt32 Depth() { return mDepth; }
  TInt32 Pitch() { return mPitch; }
  void GetRect(TRect &aRect) { aRect = mRect; }
  TBool PointInRect(TInt aX, TInt aY) { return mRect.PointInRect(aX, aY); }

protected:
  BBitmap(TInt32 aWidth, TInt32 aHeight, TInt32 aDepth, TInt32 aPitch, TAny *aMemory);
  virtual ~BBitmap() = 0;

protected:
  TInt32 mWidth, mHeight, mDepth, mPitch;
  TBool mFreePixels; // if set, mPixels will be freed in destructor, default = true
  TRect mRect;
} PACKED;

#include <Graphics/bitmap/BBitmap32.h>

#endif
