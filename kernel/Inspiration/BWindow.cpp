#include <Exec/ExecBase.h>
#include <Inspiration/InspirationBase.h>
#include <Inspiration/BWindow.h>

const TInt FONT_HEIGHT = 16;
const TInt BORDER_WIDTH = 2;
static TRGB BORDER_COLOR(255, 255, 255);
static TRGB TITLE_COLOR(0, 0, 0);

BWindow::BWindow(const TNewWindow &aNewWindow)
    : BNode(aNewWindow.mTitle), mInspirationBase(*gExecBase.GetInspirationBase()) {

  mScreen = aNewWindow.mScreen;

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

  TRGB fg(aNewWindow.mTitleForeground);
  TRGB bg(aNewWindow.mBorderColor);
  mWindowViewPort->SetColors(fg, bg);
  // mWindowViewPort->SetColors(TITLE_COLOR, BORDER_COLOR);

  // ViewPort for client to render to the client area only
  mViewPort = new BViewPort32(aNewWindow.mTitle, mBitmap);

  // set Client ViewPort Rect
  TRect cRect;
  mWindowViewPort->GetRect(cRect);
  cRect.x1 += BORDER_WIDTH;
  cRect.y1 += FONT_HEIGHT + 2;
  cRect.x2 -= BORDER_WIDTH;
  cRect.y2 -= BORDER_WIDTH;
  mViewPort->SetRect(cRect);
  mClientRect.Set(cRect);

  mIdcmpFlags = aNewWindow.mIdcmpFlags;
  mWindowFLags = aNewWindow.mWindowFlags;

  mTask = gExecBase.GetCurrentTask();
  mIdcmpPort = mTask->CreateMessagePort();
}

BWindow::~BWindow() {
  //
  delete mIdcmpPort;
  delete mViewPort;
  delete mWindowViewPort;
  delete mBitmap;
}

IdcmpMessage *BWindow::GetMessage() {
  return (IdcmpMessage *)mIdcmpPort->GetMessage();
}

void BWindow::PaintDecorations() {
  BViewPort32 *vp = mWindowViewPort;
  TInt x1 = 0,
       y1 = 0,
       x2 = vp->Width() - 1,
       y2 = vp->Height() - 1;

  TUint32 color = 0xffffff;

  for (TInt w = 0; w < BORDER_WIDTH; w++) {
    // right side
    vp->FastLineVertical(color, x2 - w, y1, vp->Height());
    // left side
    vp->FastLineVertical(color, x1 + w, y1, vp->Height());
    // bottom
    vp->FastLineHorizontal(color, x1, y2 - w, vp->Width());
  }

  // render title bar
  vp->FillRect(0xffffff, x1, y1, x2, y1 + FONT_HEIGHT + 2);
  vp->DrawTextTransparent(1, 1, Title());
  Repaint();
}

void BWindow::Repaint() {
  mScreen->UpdateWindow(this);
}
