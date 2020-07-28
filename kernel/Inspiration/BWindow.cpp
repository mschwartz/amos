#include <Exec/ExecBase.hpp>
#include <Inspiration/InspirationBase.hpp>
#include <Inspiration/BWindow.hpp>

const TInt FONT_HEIGHT = 16;

BWindow::BWindow(const TNewWindow &aNewWindow)
    : BNode(aNewWindow.mTitle), mInspirationBase(*gExecBase.GetInspirationBase()) {

  mScreen = aNewWindow.mScreen;
  if (mScreen == ENull) {
    mScreen = mInspirationBase.FindScreen();
  }

  BTheme *theme = mScreen->GetTheme();

  mMinWidth = aNewWindow.mMinWidth ? aNewWindow.mMinWidth : aNewWindow.mWidth;
  mMinHeight = aNewWindow.mMinHeight ? aNewWindow.mMinHeight : aNewWindow.mHeight;

  mMaxWidth = aNewWindow.mMaxWidth ? aNewWindow.mMaxWidth : aNewWindow.mWidth;
  mMaxHeight = aNewWindow.mMaxHeight ? aNewWindow.mMaxHeight : aNewWindow.mHeight;

  mBitmap = new BBitmap32(aNewWindow.mWidth, aNewWindow.mHeight);

  //
  mBitmap->GetRect(mWindowRect);
  mWindowRect.Offset(aNewWindow.mLeft, aNewWindow.mTop);

  mWindowViewPort = new BViewPort32(mNodeName, mBitmap);
  mWindowViewPort->SetFont(new BConsoleFont32());

  // mWindowViewPort->SetColors(TITLE_COLOR, BORDER_COLOR);

  // ViewPort for client to render to the client area only
  mViewPort = new BViewPort32(aNewWindow.mTitle, mBitmap);

  // set Client ViewPort Rect
  TInt border_width = MAX(theme->mInactiveBorderWidth, theme->mActiveBorderWidth);

  TRect cRect;
  mWindowViewPort->GetRect(cRect);
  cRect.x1 += border_width;
  cRect.y1 += FONT_HEIGHT + 2;
  cRect.x2 -= border_width;
  cRect.y2 -= border_width;

  mViewPort->SetRect(cRect);
  mClientRect.Set(cRect);

  mIdcmpFlags = aNewWindow.mIdcmpFlags;
  mWindowFlags = aNewWindow.mWindowFlags;

  mForegroundColor = aNewWindow.mForegroundColor;
  mBackgroundColor = aNewWindow.mBackgroundColor;

  mFont = aNewWindow.mFont;
  if (mFont == ENull) {
    mFont = new BConsoleFont32();
  }

  TRGB fg(mForegroundColor),
    bg(mBackgroundColor);

  mViewPort->SetColors(fg, bg);
  mViewPort->SetFont(mFont);

  mTask = gExecBase.GetCurrentTask();
  mIdcmpPort = mTask->CreatePort();

  mViewPort->Clear(bg);
}

BWindow::~BWindow() {
  //
  delete mIdcmpPort;
  delete mViewPort;
  delete mWindowViewPort;
  delete mBitmap;
}

void BWindow::MoveTo(TInt32 aX, TInt32 aY) {
  //
}

void BWindow::SetIdcmpPort(BTask *aTask) {
  delete mIdcmpPort;
  mIdcmpPort = aTask->CreatePort();
}

IdcmpMessage *BWindow::GetMessage() {
  return (IdcmpMessage *)mIdcmpPort->GetMessage();
}

TBool BWindow::IsActive() {
  return this == mScreen->ActiveWindow();
}

void BWindow::Activate() {
  if (IsActive()) {
    return;
  }
  mScreen->ActivateWindow(this);
}

void BWindow::PaintDecorations() {
  TBool active = IsActive();

  BTheme *theme = mScreen->GetTheme();

  BViewPort32 *vp = mWindowViewPort;
  TInt x1 = 0,
       y1 = 0,
       x2 = vp->Width() - 1,
       y2 = vp->Height() - 1;

  // erase old window border, new one might be different width
  TInt max_bw = MAX(theme->mActiveBorderWidth, theme->mInactiveBorderWidth);
  TUint32 bg_color = mBackgroundColor;
  for (TInt w = 0; w < max_bw; w++) {
    // right side
    vp->FastLineVertical(bg_color, x2 - w, y1, vp->Height());
    // left side
    vp->FastLineVertical(bg_color, x1 + w, y1, vp->Height());
    // bottom
    vp->FastLineHorizontal(bg_color, x1, y2 - w, vp->Width());
  }

  // render new window border
  TInt border_width = active ? theme->mActiveBorderWidth : theme->mInactiveBorderWidth;
  TUint32 border_color = active ? theme->mActiveBorderColor : theme->mInactiveBorderColor;

  for (TInt w = 0; w < border_width; w++) {
    // right side
    vp->FastLineVertical(border_color, x2 - w, y1, vp->Height());
    // left side
    vp->FastLineVertical(border_color, x1 + w, y1, vp->Height());
    // bottom
    vp->FastLineHorizontal(border_color, x1, y2 - w, vp->Width());
  }

  // render title bar
  TUint32 title_bg_color = active ? theme->mActiveTitleBackgroundColor : theme->mInactiveTitleBackroundColor;
  TUint32 title_fg_color = active ? theme->mActiveTitleColor : theme->mInactiveTitleColor;
  vp->FillRect(title_bg_color, x1, y1, x2, y1 + FONT_HEIGHT + 2);
  vp->DrawTextTransparent(1, 1, title_fg_color, Title());
  Repaint();
}

void BWindow::Repaint() {
  mScreen->UpdateWindow(this);
}
