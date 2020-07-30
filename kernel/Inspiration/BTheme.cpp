#include <Inspiration/BTheme.hpp>
#include <Graphics/BFont.hpp>
#include <Graphics/font/BConsoleFont.hpp>

BTheme::BTheme(const char *aName) : BNode(aName) {
  mTitleFont = new BConsoleFont32();
  mScreenFont = new BConsoleFont32();
  DefaultValues();
}

BTheme::BTheme(BTheme *aOther) : BNode(aOther->mNodeName) {
  *this = *aOther;
  mTitleFont = new BConsoleFont32;
  mScreenFont = new BConsoleFont32();
  *mTitleFont = *aOther->mTitleFont;
}

void BTheme::DefaultValues() {
  mActiveBorderWidth = 2;
  mInactiveBorderWidth = 1;
  mActiveBorderColor = 0xffffff;                       // white
  mInactiveBorderColor = 0x7f7f7f;                     // half white/grey
  mActiveTitleBackgroundColor = mActiveBorderColor;    // same as border color
  mInactiveTitleBackroundColor = mInactiveBorderColor; // same as border color
  mActiveTitleColor = 0x000000;                        // black
  mInactiveTitleColor = 0x000000;                      // black
  mTitleFont = new BConsoleFont32();

}
