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

static Semaphore *sem;

TInt64 ExitTestTask::Run() {
  dlog("ExitTestTask(%s) mType(%d) mDelay(%d)\n", TaskName(), mType, mDelay);
  Sleep(mDelay);
  DISABLE;
  if (!sem) {
    sem = new Semaphore(SEMAPHORE_NAME);
    gExecBase.AddSemaphore(sem);
    DLOG("Created Semaphore (%s)\n", SEMAPHORE_NAME);
  }
  ENABLE;

  Semaphore *s;
  for (;;) {
    s = gExecBase.FindSemaphore(SEMAPHORE_NAME);
    if (s) {
      break;
    }
    Sleep(mDelay);
  }
  DLOG("Found Semaphore(%s)\n", SEMAPHORE_NAME);
  // Sleep(mDelay);
  // obtain exclusive semaphore
  s->Obtain(ETrue);
  DLOG("Obtained semaphore! Holding for %d seconds\n", mDelay * 10);
  Sleep(mDelay * 10);
  DLOG("Releasing semaphore\n");
  s->Release();
  DLOG("Semaphore relased\n");

  for (;;) {
    Sleep(mDelay);
    switch (mType) {
      case EExitTestReturn:
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
