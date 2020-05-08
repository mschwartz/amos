#include <Inspiration/Inspiration.h>
#include <Exec/ExecBase.h>
#include <Exec/BTask.h>

class MousePointerTask : public BTask {
public:
  MousePointerTask() : BTask("MousePointer", LIST_PRI_MIN) {
    dlog("** Construct MousePointerTask\n");
  }
  void Run() {
    while (1) {
      dlog("MousePointerTask loop\n");
      Sleep(1);
    }
  }
};

InspirationBase::InspirationBase() {
  dlog("** Construct InspirationBase\n");
  gExecBase.AddTask(new MousePointerTask());
}

InspirationBase::~InspirationBase() {
}
