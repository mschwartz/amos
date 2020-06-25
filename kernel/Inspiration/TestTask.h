#ifndef TEST_TASK
#define TEST_TASK

#include <Exec/ExecBase.h>
#include <Inspiration/InspirationBase.h>

class TestTask : public BProcess {
public:
  TestTask();
  ~TestTask();

public:
  void Run();
};

#endif

