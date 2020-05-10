#ifndef AMOS_INSPIRATION_H
#define AMOS_INSPIRATION_H

#include <Exec/Types.h>
#include <Exec/BBase.h>
#include <Graphics/Graphics.h>

#include <Inspiration/ScreenVesa.h>
#include <Inspiration/BWindow.h>

class InspirationBase : public BBase {
public:
  InspirationBase();
  ~InspirationBase();

  void Init();

public:
  void AddWindow(BWindow *aWindow);
  void UpdateWindow(BWindow *aWindow, TBool mDecorations = EFalse);

public:
  ScreenVesa& GetScreen() { return mScreen; }

protected:
  ScreenVesa& mScreen;
  BWindowList mWindowList;
};

#endif
