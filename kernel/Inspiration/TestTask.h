#ifndef TEST_TASK
#define TEST_TASK

#include <Exec/ExecBase.h>
//#include <Exec/BTask.h>
#include <Inspiration/Inspiration.h>

class TestTask : public BTask {
public:
  TestTask();
  ~TestTask();

public:
  void Run();
};

#endif

