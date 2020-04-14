#ifndef EXEC_BBITMAP_H
#define EXEC_BBITMAP_H

#include <Exec/BBase.h>
#include <Graphics/TRect.h>
#include <Graphics/TRGB.h>

// base class for different depth bitmaps
class BBitmap : public BBase {
public:
  // factory method
  // returns appropriate kind of BBitmap with specfied width, height, and depth.
  // if aMemory is provided (not null), that memory is used as the pixels data and that memory
  // will not be freed upon destruction.
  static BBitmap *CreateBitmap(TInt aWidth, TInt aHeight, TInt aDepth, TAny *aMemory = ENull);
protected:
  BBitmap(TInt aWidth, TInt aHeight, TInt aDepth, TAny *aMemory);
  virtual ~BBitmap() = 0;
public:
  virtual void Clear(TRGB& aColor) = 0;
  virtual void Clear(TUint32 aColor) = 0;

public:
  TInt Width() { return mWidth; }
  TInt Height() { return mHeight; }
  TInt Depth() { return mDepth; }
protected:
  TInt mWidth, mHeight, mDepth;
  TAny *mPixels;
  TBool mFreePixels;    // if set, mPixels will be freed in destructor, default = true
  TRect mBounds;
} PACKED;

//
#endif
