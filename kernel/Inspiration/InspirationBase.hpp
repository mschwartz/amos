#ifndef AMOS_INSPIRATION_H
#define AMOS_INSPIRATION_H

#include <Types.hpp>
#include <Exec/BBase.hpp>
#include <Graphics/Graphics.hpp>

#include <Inspiration/BTheme.hpp>
#include <Inspiration/Display.hpp>
#include <Inspiration/BScreen.hpp>
#include <Inspiration/BWindow.hpp>
#include <Inspiration/Desktop.hpp>
#include <Inspiration/BConsoleWindow.hpp>

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
  void UpdateWindow(BWindow *aWindow);

public:
  BTheme *DefaulTTheme() { return (BTheme *)mThemeList.First(); };
  BTheme *FindTheme(const char *aName) { return (BTheme *)mThemeList.Find(aName); }

public:
  Display *GetDisplay() { return mDisplay; }

public:
  BWindow *ActiveWindow() { return mDisplay->ActiveWindow(); }
  /**
   * Activate window that the point aX,aY is within.
   */
  TBool ActivateWindow(TInt32 aX, TInt32 aY);

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
  BThemeList mThemeList;
};

#endif
