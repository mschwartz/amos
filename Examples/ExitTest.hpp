#ifndef EXAMPLES_EXITTEST_HH
#define EXAMPLES_EXITTEST_HH

#include <Exec/BTask.hpp>

enum EExitTestType {
  EExitTestReturn,
  EExitTestSuicide,
  EExitTestExit
};

class ExitTestTask : public BTask {
public:
  ExitTestTask(EExitTestType aType, const char *aName, TInt64 aDelay);
  ~ExitTestTask();

public:
  TInt64 Run();

protected:
  TInt64 mDelay;
  EExitTestType mType;
};

#endif
