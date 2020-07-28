/**
 * BTheme
 *
 * Theme settings for rendering Inspiration.
 */

#ifndef AMOS_INSPIRATION_BTHEME_H
#define AMOS_INSPIRATION_BTHEME_H

#include <Types/BList.hpp>

class BConsoleFont32;

class BTheme : public BNode {
public:
  BTheme(const char *aName);
  BTheme(BTheme *aOther);

public:
  void DefaultValues();

public:
  TInt16 mActiveBorderWidth,           // width of border of active window
    mInactiveBorderWidth;              // width of border of inactive window
  TUint32 mActiveBorderColor,          // color of active window border
    mInactiveBorderColor;              // color of inactive window border
  TUint32 mActiveTitleBackgroundColor, // color of window titlebar when active
    mInactiveTitleBackroundColor;      // color of window titlebar when inactive
  TUint32 mActiveTitleColor,           // color of titlebar text when active
    mInactiveTitleColor;               // color of titlebar text when inactive
  BConsoleFont32 *mTitleFont;          // window title font

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
  void Dump() {
    dprint("\n\n");
    for (BTheme *t = First(); !End(t); t = Next(t)) {
      t->Dump();
    }
  }
};

#endif
