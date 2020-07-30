#ifndef INSPIRATION_MOUSEPOINTERTASK_H
#define INSPIRATION_MOUSEPOINTERTASK_H

#include <Inspiration/BWindow.hpp>
#include <Exec/BTask.hpp>
#include <Types/TRect.hpp>

class Display;
class MouseMessage;

typedef struct {
  BWindow *mWindow = ENull;
  TCoordinate mDx, mDy;
} Draggable;

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

protected:
  Draggable mDraggable;
  
public:
  TInt64 Run();
};

#endif
