/**
 * BTheme
 *
 * Theme settings for rendering Inspiration.
 */

#ifndef AMOS_INSPIRATION_BTHEME_H
#define AMOS_INSPIRATION_BTHEME_H

#include <Types/BList.hpp>
#include <Exec/SpinLock.hpp>

class BConsoleFont32;

class BTheme : public BNode {
public:
  BTheme(const char *aName);
  BTheme(BTheme *aOther);
  ~BTheme();

public:
  void DefaultValues();

public:
  // window themes
  TInt16 mActiveBorderWidth = 2,                  // width of border of active window
    mInactiveBorderWidth = 1;                     // width of border of inactive window
  TUint32 mActiveBorderColor = 0xffffff,          // color of active window border
    mInactiveBorderColor = 0x7f7f7f;              // color of inactive window border
  TUint32 mActiveTitleBackgroundColor = 0xffffff, // color of window titlebar when active
    mInactiveTitleBackroundColor = 0x7f7f7f;      // color of window titlebar when inactive
  TUint32 mActiveTitleColor = 0x000000,           // color of titlebar text when active
    mInactiveTitleColor = 0xffffff;               // color of titlebar text when inactive
  BConsoleFont32 *mTitleFont;                     // window title font
  // screen themes
  TUint32 mScreenBackgroundColor = 0x4f4fff;      // screen/desktop background color
  TUint32 mScreenTitleBackgroundColor = 0x000000, // screen titlebar background color
    mScreenTitleColor = 0xffffff;                 // screen titlebar font color
  BConsoleFont32 *mScreenFont;                    // screen title font

public:
  void Dump() {
    dprint("\n");
    dlog("BTheme(%s) %x\n", mNodeName, this);
    dlog("             mActiveBorderWidth: %d\n", mActiveBorderWidth);
    dlog("           mInactiveBorderWidth: %d\n", mInactiveBorderWidth);

    dlog("             mActiveBorderColor: %06x\n", mActiveBorderColor);
    dlog("           mInactiveBorderColor: %06x\n", mInactiveBorderColor);

    dlog("    mActiveTitleBackgroundColor: %06x\n", mActiveTitleBackgroundColor);
    dlog("  mInactiveTitleBackgroundColor: %06x\n", mInactiveTitleBackroundColor);

    dlog("              mActiveTitleColor: %06x\n", mActiveTitleColor);
    dlog("            mInactiveTitleColor: %06x\n", mInactiveTitleColor);

    dlog("                     mTitleFont: %x\n", mTitleFont);
  }
} PACKED;

class BThemeList : public BList {
public:
  BThemeList() : BList("ThemeList") {
  }

public:
  BTheme *First() {
    return (BTheme *)First();
  }
  BTheme *Next(BTheme *aTheme) { return (BTheme *)BList::Next(aTheme); }
  TBool End(BTheme *aTheme) { return BList::End(aTheme); };

public:
  void Lock() { mSpinLock.Acquire(); }
  void Unlock() { mSpinLock.Release(); }

protected:
  SpinLock mSpinLock;

public:
  void Dump() {
    Lock();
    dprint("\n\n");
    for (BTheme *t = First(); !End(t); t = Next(t)) {
      t->Dump();
    }
    Unlock();
  }
};

#endif
