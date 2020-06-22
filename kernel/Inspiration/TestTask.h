#ifndef TEST_TASK
#define TEST_TASK

#include <Exec/ExecBase.h>
#include <Inspiration/Inspiration.h>

class TestTask : public BProcess {
public:
  TestTask();
  ~TestTask();

public:
  void Run();
};

#endif

