#ifndef AMOS_INSPIRATION_H
#define AMOS_INSPIRATION_H

#include <Exec/Types.h>
#include <Exec/BBase.h>
#include <Graphics/Graphics.h>

#include <Inspiration/Display.h>
#include <Inspiration/BWindow.h>
#include <Inspiration/BConsoleWindow.h>

class InspirationBase : public BBase {
public:
  InspirationBase();
  ~InspirationBase();

  void Init();

public:
  void AddWindow(BWindow *aWindow);
  void UpdateWindow(BWindow *aWindow, TBool mDecorations = EFalse);

public:
  Display& GetDisplay() { return mDisplay; }

protected:
  Display& mDisplay;
  // TODO: windows belong to screens, not to InspirationBase.
  BWindowList mWindowList;
};

#endif
