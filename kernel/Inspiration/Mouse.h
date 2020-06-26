#ifndef INSPIRATION_MOUSE_H
#define INSPIRATION_MOUSE_H

#include <Exec/BBase.h>
#include <Types/TRect.h>

class Display;

class Mouse : public BBase {
public:
  Mouse(Display *aDisplay);
  ~Mouse();

public:
  void MoveTo(TInt32 aX, TInt32 aY);

public:
  TBool Hide();
  TBool Show();
  TBool Set(TBool aShowit);

public:
  void Restore();
  void Render();

protected:
  Display *mDisplay;
  TUint32 *mBackgroundSave;
  TInt32 mX, mY;
  TInt32 mLastX, mLastY;
  TBool mHidden;
};

#endif
