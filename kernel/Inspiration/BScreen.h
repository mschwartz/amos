#ifndef INSPIRATION_BSCREEN_H
#define INSPIRATION_BSCREEN_H

/**
 * BScreen
 * 
 * Inspiration renders one or more BScreens to the physical Display.
 * 
 * A BScreen can have one or more BWindows that render on top of the BScreen.
 *
 * BScreen renders a top title bar, similar to the MacOS user interface.  Like the Amiga,
 * the user can depth arrange the screens that are "stacked" and can drag down on the title bar
 * to reveal part of the screen beneath.
 *
 * In theory, an application might create a BScreen and render all its windows on it.
 *
 * There is always one BScreen, the default BScreen or Desktop (Workbench on Amiga).
 * An application might render its windows on the Desktop.  The Desktop also would be where
 * a Dock and desktop icons would be rendered.
 */

#include <Types/BList.h>
#include <Inspiration/Display.h>
#include <Graphics/bitmap/BBitmap32.h>
#include <Inspiration/BWindow.h>
#include <Inspiration/DirtyRect.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BScreen : public BNode {
public:
  BScreen(const char *aTitle);
  ~BScreen();

public:
  void RenderTitlebar();
  void Clear(TUint32 aColor);

public:
  void AddWindow(BWindow *aWindow);
  void UpdateWindow(BWindow *aWindow, TBool mDecorations = EFalse);

public:
  TInt32 Width() { return mBitmap->Width(); }
  TInt32 Height() { return mBitmap->Height(); }

protected:
  InspirationBase &mInspirationBase;
  Display *mDisplay;
  BBitmap32 *mBitmap; // offscreen bitmap, size of screen
  BWindowList mWindowList;
  TInt32 mTopY;
  TBool mDirty;

public:
  // void RenderCursor(Cursor *aCursor, TInt32 aX, TInt32 aY) ;

public:
  void AddDirtyRect(TInt32 aX1, TInt32 aT1, TInt32 aX2, TInt32 aY2);
  void UpdateDirtyRects();

protected:
  DirtyRectList mDirtyRects;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BScreenList : public BList {
public:
  BScreenList() : BList("Screen List") {}

public:
  BScreen *First() { return (BScreen *)BList::First(); }
  BScreen *Next(BScreen *s) { return (BScreen *)BList::Next(s); }
  BScreen *Find(const char *aTitle) { return (BScreen *)BList::Find(aTitle); }
};

#endif
