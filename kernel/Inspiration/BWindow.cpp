#include <Exec/ExecBase.h>
#include <Inspiration/Inspiration.h>
#include <Inspiration/BWindow.h>

const TInt FONT_HEIGHT = 16;
const TInt BORDER_WIDTH = 2;
const TRGB BORDER_COLOR(255, 255, 255);

BWindow::BWindow(const char *aTitle, TInt32 aX, TInt32 aY, TInt32 aW, TInt32 aH)
    : BNode(aTitle), mInspirationBase(*gExecBase.GetInspirationBase()) {

  // mRect x1,y1 is the upper left position of the window in screen coordinates
  //  mRect.Set(aX, aY, aX + aW - 1, aY + aH - 1);

  // bitmap encompasses window decorationsand client area
  mBitmap = new BBitmap32(aW, aH);
  mBitmap->GetRect(mWindowRect);
  mWindowRect.Offset(aX, aY);

  // ViewPort Rect, 0,0 is upper elft of the bitmap/window
  // Window ViewPort to access entire window bitmap (for decorations)
  mWindowViewPort = new BViewPort32(aTitle, mBitmap);

  // ViewPort for client to render to the client area only
  mViewPort = new BViewPort32(aTitle, mBitmap);
  // set Client ViewPort Rect
  TRect cRect;
  mWindowViewPort->GetRect(cRect);
  cRect.x1 += BORDER_WIDTH;
  cRect.y1 += FONT_HEIGHT + 2;
  cRect.x2 -= BORDER_WIDTH;
  cRect.y2 -= BORDER_WIDTH;
  mViewPort->SetRect(cRect);

  mClientRect.Set(cRect);

//  dlog("BWindow mRect\n");
//  mWindowRect.Dump();
//  dlog("Window ViewPort\n");
//  mWindowViewPort->Dump();
//  dlog("Client Viewport\n");
//  mViewPort->Dump();

  mPainting = EFalse;
}

BWindow::~BWindow() {
  //
}

void BWindow::PaintDecorations() {
  dlog("PaintDecorations\n");

//  mWindowViewPort->Dump();
//  mViewPort->Dump();

  BViewPort32 *vp = mWindowViewPort;
//  vp->Clear(0xffffff);
#if 1
  TInt x1 = 0,
       y1 = 0,
       x2 = vp->Width() - 1,
       y2 = vp->Height() - 1;

  TUint32 color = 0xffffff;

  for (TInt w = 0; w < BORDER_WIDTH; w++) {
//    dlog("FLV %x %d,%d, %d\n", color, x1 + w, y1, vp->Height());
    vp->FastLineVertical(color, x1 + w, y1, vp->Height());
//    dlog("FLV %x %d,%d, %d\n", color, x2 - w, y1, vp->Height());
    vp->FastLineVertical(color, x2 - w, y1, vp->Height());
    vp->FastLineHorizontal(color, x1, y2 - w, vp->Width());
  }

//  mViewPort->Dump();
  vp->FillRect(0xffffff, x1, y1, x2, y1 + FONT_HEIGHT + 2);
//  mViewPort->Dump();
  //  mWindowViewPort->DrawRect(BORDER_COLOR, mRect);
#endif
}

void BWindow::Repaint() {
  mInspirationBase.UpdateWindow(this);
}
