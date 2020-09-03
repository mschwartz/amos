#include <Inspiration/Desktop.hpp>
#include <Exec/ExecBase.hpp>

Desktop::Desktop() : BScreen("Desktop") {
  dprint("\n\n");
  dlog("Construct Desktop\n");
  mTime = 15;
  sprintf(mTitleBuffer, "%s - %8d total / %8d used / %8d available", Title(), TotalMem(), UsedMem(), AvailMem());
}

void Desktop::RenderTitlebar() {
  BBitmap32 *b = mBackground;
  b->FillRect(mTheme->mScreenTitleBackgroundColor, 0, 0, Width() - 1, 26);
  b->SetFont(mTheme->mScreenFont);
  b->SetColors(
    mTheme->mScreenTitleColor,
    mTheme->mScreenTitleBackgroundColor);

  if (mTime-- < 0) {
    mTime = 15;
    sprintf(mTitleBuffer, "%s - %8d total / %8d used / %8d available", Title(), TotalMem(), UsedMem(), AvailMem());
  }

  b->DrawText(4, 4, mTitleBuffer);
  // dlog("%s\n", mTitleBuffer);
  // CopyMemory32(mBackground->GetPixels(), mBitmap->GetPixels(), Width() * 28);

  AddDirtyRect(0, 0, Width() - 1, 26);
}
