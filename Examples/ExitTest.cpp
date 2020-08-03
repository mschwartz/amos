#define DEBUGME
// #undef DEBUGME

#include "ExitTest.hpp"
#include <Exec/ExecBase.hpp>

const char *SEMAPHORE_NAME = "test";

ExitTestTask::ExitTestTask(EExitTestType aType, const char *aName, TInt64 aDelay) : BTask(aName) {
  mType = aType;
  mDelay = aDelay;
}

ExitTestTask::~ExitTestTask() {
  dlog("ExitTestTask(%s) destructor\n", TaskName());
}

TInt64 ExitTestTask::Run() {
  dlog("ExitTestTask(%s) mType(%d) mDelay(%d)\n", TaskName(), mType, mDelay);
  dlog("ExitTestTask(%s) slept\n", TaskName());

  Semaphore *s;
  if (CompareStrings(TaskName(), "SUICIDE") == 0) {
    s = new Semaphore(SEMAPHORE_NAME);
    DISABLE;
    gExecBase.AddSemaphore(s);
    ENABLE;
    DLOG("Created Semaphore (%s)\n", SEMAPHORE_NAME);
  }
  else {
    Sleep(2);
    for (;;) {
      s = gExecBase.FindSemaphore(SEMAPHORE_NAME);
      if (s) {
        break;
      }
      Sleep(mDelay);
    }
    DLOG("%s Found Semaphore(%s) %x\n", TaskName(), SEMAPHORE_NAME, s);
  }

  // obtain exclusive semaphore
  s->Obtain(ETrue);
  DLOG("Obtained semaphore! Holding for 5 seconds\n");
  Sleep(5);
  DLOG("Releasing semaphore\n");
  s->Release();
  DLOG("Semaphore relased\n");

  for (;;) {
    Sleep(mDelay);
    switch (mType) {
      case EExitTestReturn:
	gExecBase.RemoveSemaphore(s);
	delete s;
        return 0;
      case EExitTestSuicide:
        Suicide(0);
        break;
      case EExitTestExit:
        Exit(0);
        break;
      default:
        dlog("Invalid ExitTestTask(%s) mType(%d)\n", TaskName(), mType);
        break;
    }
  }
}
