#ifndef AMOS_INSPIRATION_WINDOWMANAGER_KEYBOARDTASK_H
#define AMOS_INSPIRATION_WINDOWMANAGER_KEYBOARDTASK_H

#include <Exec/BTask.h>

class KeyboardTask : public BTask {
public:
  KeyboardTask();

public:
  void Run();
};

#endif
