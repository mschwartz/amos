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
  dlog("ExitTestTask(%s) slept\n", TaskName());

  if (CompareStrings(TaskName(), "SUICIDE") == 0) {
    sem = new Semaphore(SEMAPHORE_NAME);
    DISABLE;
    gExecBase.AddSemaphore(sem);
    ENABLE;
    DLOG("Created Semaphore (%s)\n", SEMAPHORE_NAME);
  }
  else {
    Sleep(mDelay);
  }

  Semaphore *s;
  DLOG("%s finding Semaphore(%s)\n", TaskName(), SEMAPHORE_NAME);
  for (;;) {
    s = gExecBase.FindSemaphore(SEMAPHORE_NAME);
    if (s) {
      break;
    }
    Sleep(mDelay);
  }
  DLOG("%s Found Semaphore(%s) %x\n", TaskName(), SEMAPHORE_NAME, s);
  // Sleep(mDelay);
  // obtain exclusive semaphore
  s->Obtain(ETrue);
  DLOG("Obtained semaphore! Holding for %d seconds\n", mDelay * 2);
  Sleep(mDelay * 2);
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
