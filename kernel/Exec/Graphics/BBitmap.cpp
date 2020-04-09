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
  mBounds.Set(0, 0, aWidth-1, aHeight-1);
}

