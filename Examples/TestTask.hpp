#ifndef TEST_TASK
#define TEST_TASK

#include <Exec/ExecBase.hpp>
#include <Inspiration/InspirationBase.hpp>

class TestTask : public BProcess {
public:
  TestTask();
  ~TestTask();

public:
  TInt64 Run();
};

#endif

