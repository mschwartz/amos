#ifndef AMOS_INSPIRATION_H
#define AMOS_INSPIRATION_H

#include <Types.h>
#include <Exec/BBase.h>
#include <Graphics/Graphics.h>

#include <Inspiration/Display.h>
#include <Inspiration/BScreen.h>
#include <Inspiration/BWindow.h>
#include <Inspiration/Desktop.h>
#include <Inspiration/BConsoleWindow.h>

class IdcmpMessageReaperTask;

class InspirationBase : public BBase {
public:
  InspirationBase();
  ~InspirationBase();

  void Init();

public:
  void AddScreen(BScreen *aScreen);
  BScreen *FindScreen(const char *aTitle = ENull);

public:
  void UpdateWindow(BWindow *aWindow, TBool mDecorations = EFalse);

public:
  Display *GetDisplay() { return mDisplay; }
  BWindow *ActiveWindow() { return mDisplay->ActiveWindow(); }
  /**
   * Send IdcmpMessage to active window.
   *
   * Returns ETrue if message sent.
   * Message won't be sent if window's IDCMP flags are not set for the type of message to be sent.
   */
  TBool SendIdcmpMessage(IdcmpMessage *aMessage);

protected:
  Display *mDisplay;
  Desktop *mDesktop; // Desktop Screen
  IdcmpMessageReaperTask *mReaperTask;
};

#endif
