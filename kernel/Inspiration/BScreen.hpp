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

#include <Types/BList.hpp>
#include <Inspiration/Display.hpp>
#include <Inspiration/BTheme.hpp>
#include <Graphics/bitmap/BBitmap32.hpp>
#include <Inspiration/BWindow.hpp>
#include <Inspiration/DirtyRect.hpp>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BScreen : public BNode {
public:
  BScreen(const char *aTitle);
  ~BScreen();

public:
  const char *Title() { return mNodeName; }
  void Title(const char *aNewTitle) {
    SetName(aNewTitle);
    RenderTitlebar();
  }
  void RenderTitlebar();
  void Clear(TUint32 aColor);

public:
  void AddWindow(BWindow *aWindow);
  void UpdateWindow(BWindow *aWindow, TBool mDecorations = EFalse);
  void EraseWindow(BWindow *aWindow);
  BWindow *ActiveWindow() { return (BWindow *)mWindowList.First(); }

  void ActivateWindow(BWindow *aWindow);

  // returns ETrue if another window is activated:
  TBool ActivateWindow(TCoordinate aX, TCoordinate aY);
  BWindow *DragWindow(TCoordinate aX, TCoordinate aY);

  void UpdateWindows();

public:
  TCoordinate Width() { return mBitmap->Width(); }
  TCoordinate Height() { return mBitmap->Height(); }

protected:
  InspirationBase &mInspirationBase;
  Display *mDisplay;
  BBitmap32 *mBitmap; // offscreen bitmap, size of screen
  // the background bitmap is used to erase windows and other things to the background image
  BBitmap32 *mBackground; // offscreen background bitmap
  WindowList mWindowList;
  TCoordinate mTopY;

public:
  BTheme *GetTheme() { return mTheme; }
  // void RenderCursor(Cursor *aCursor, TCoordinate aX, TCoordinate aY) ;
protected:
  BTheme *mTheme;

public:
  void AddDirtyRect(TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2);
  void AddDirtyRect(TRect &aRect) {
    AddDirtyRect(aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }
  void UpdateDirtyRects();

protected:
  DirtyRectList mDirtyRects;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class ScreenList : public BList {
public:
  ScreenList() : BList("Screen List") {}

public:
  void Lock() { mSpinLock.Acquire(); }
  void Unlock() { mSpinLock.Release(); }

protected:
  SpinLock mSpinLock;

public:
  BScreen *First() { return (BScreen *)BList::First(); }
  BScreen *Next(BScreen *s) { return (BScreen *)BList::Next(s); }
  BScreen *Find(const char *aTitle) { return (BScreen *)BList::Find(aTitle); }
};

#endif
