#ifndef EXEC_DEVICES_SCREEN_SCREENVESA_H
#define EXEC_DEVICES_SCREEN_SCREENVESA_H

#include <Types.h>
#include <Types/BList.h>
#include <Graphics/bitmap/BBitmap32.h>

class BScreen;
class BScreenList;
class DisplayTask;
class Mouse;

class Display : public BNode {
  friend DisplayTask;
  friend Mouse;

public:
  Display();
  ~Display();
  void Init();

public:
  void AddScreen(BScreen *aScreen);

public:
  void Dump() {
    dprint("\n\n");
    dlog("Display(%s) at %x\n", mNodeName, this);
    dlog("             mX, mY: %d, %d\n", mX, mY);
    dlog("       mMouseHidden: %d\n", mMouseHidden);
    dlog("   mMouseX, mMouseY: %d, %d\n", mMouseX, mMouseY);
    dlog("            mBitmap: %x\n", mBitmap);
    mBitmap->Dump();
  }

public:
  TUint32 Width() { return mBitmap->Width(); }
  TUint32 Height() { return mBitmap->Height(); }
  TUint32 Depth() { return mBitmap->Depth(); }

public:
  // mouse curor methods
  void MoveCursor(TInt aX, TInt aY);
  TBool ShowCursor(); // returns previous state
  TBool HideCursor(); // returns previous state

  TBool SetCursor(TBool aShowIt) {
    if (aShowIt) {
      return ShowCursor();
    }
    else {
      return HideCursor();
    }
  }

public:
  void Clear(TUint32 aColor);
  BBitmap32 *GetBitmap() { return mBitmap; }

  // copy aOther bitmap to screen at aDestX,aDestY (as in a window)
  void BltBitmap(BBitmap32 *aOther, TInt32 aDestX, TInt32 aDestY) {
    mBitmap->BltBitmap(aOther, aDestX, aDestY);
  }

public:
  BScreen *FindScreen(const char *aTitle);

protected:
  BScreenList *mScreenList;
  BBitmap32 *mBitmap;
  Mouse *mMouse;

  //  TUint8 *screen;
  TInt mX, mY;
  TInt mMouseX, mMouseY;
  char buf[256];
  TBool mMouseHidden;
};

#endif
