#include "ExitTest.hh"

ExitTestTask::ExitTestTask(EExitTestType aType, const char *aName, TInt64 aDelay) : BTask(aName) {
  mType = aType;
  mDelay = aDelay;
}

ExitTestTask::~ExitTestTask() {
  dlog("ExitTestTask(%s) destructor\n", TaskName());
}
TInt64 ExitTestTask::Run() {
  dlog("ExitTestTask(%s) mType(%d) mDelay(%d)\n", TaskName(), mType, mDelay);
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
