#ifndef AMOS_INSPIRATION_WINDOWMANAGER_KEYBOARDTASK_H
#define AMOS_INSPIRATION_WINDOWMANAGER_KEYBOARDTASK_H

#include <Exec/BTask.hpp>

class KeyboardTask : public BTask {
public:
  KeyboardTask();

public:
  TInt64 Run();
};

#endif
