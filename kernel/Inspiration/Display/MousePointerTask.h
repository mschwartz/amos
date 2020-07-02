#ifndef INSPIRATION_MOUSEPOINTERTASK_H
#define INSPIRATION_MOUSEPOINTERTASK_H

#include <Exec/BTask.h>

class Display;

class MousePointerTask : public BTask {
public:
  MousePointerTask();

protected:
  Display *mDisplay;

public:
  void Run();
};

#endif
