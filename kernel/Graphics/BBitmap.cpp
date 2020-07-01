#include <Types/TRGB.h>
#include <Graphics/BBitmap.h>
#include <Exec/Memory.h>

//#include <Exec/Memory.h>

BBitmap::BBitmap(TInt aWidth, TInt aHeight, TInt aDepth, TInt aPitch, TAny *aMemory)
    : BBase(), mWidth(aWidth), mHeight(aHeight), mDepth(aDepth), mPitch(aPitch) {
  if (aMemory) {
    mFreePixels = false;
  }
  else {
    mFreePixels = true;
  }
  mRect.Set(0, 0, aWidth - 1, aHeight - 1);
}

BBitmap::~BBitmap() {
  //
}

