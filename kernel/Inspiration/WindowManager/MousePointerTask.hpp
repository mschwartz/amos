#ifndef INSPIRATION_MOUSEPOINTERTASK_H
#define INSPIRATION_MOUSEPOINTERTASK_H

#include <Exec/BTask.hpp>

class Display;
class MouseMessage;

class MousePointerTask : public BTask {
public:
  MousePointerTask();

protected:
  Display *mDisplay;

protected:
  void SendIdcmpMessage(TUint64 aClass, TUint64 aButtons, MouseMessage *aMessage);
  void HandleButtons(MouseMessage *aMessage);

protected:
  TUint64 mLastButtons, mCurrentButtons;

protected:
  TInt32 mMouseX, mMouseY;
  TInt32 mLastMouseX, mLastMouseY;
  
public:
  TInt64 Run();
};

#endif
