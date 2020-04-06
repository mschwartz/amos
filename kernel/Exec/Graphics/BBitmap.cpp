#include <Exec/Graphics/BBitmap.h>
#include <Exec/Memory.h>

BBitmap::BBitmap(TInt aWidth, TInt aHeight, TInt aDepth, TAny *aMemory) : mWidth(aWidth), mHeight(aHeight), mDepth(aDepth) {
  if (aMemory) {
    mPixels = aMemory;
    mFreePixels = false;
  }
  else {
    mFreePixels = true;
  }
}

BBitmap32::BBitmap32(TInt aWidth, TInt aHeight, TInt aDepth, TAny *aMemory) : BBitmap(aWidth, aHeight, aDepth, aMemory) {
  mPixels32 = (TUint32 *)AllocMem(aWidth * aHeight);
  mPixels = mPixels32;
}

BBitmap32::~BBitmap32() {
  if (mFreePixels && mFreePixels) {
    delete[] mPixels32;
    mPixels = mPixels32 = nullptr;
  }
  mFreePixels = false;
}
