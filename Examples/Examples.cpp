#include <Examples/TestTask.h>
#include <Examples/ConsoleWindowTest.h>
#include <Examples/TestIdcmp.h>
#include <Examples/ExitTest.hh>

extern "C" void StartExamples() {
  dprint("Construct TestTask\n");
  gExecBase.AddTask(new TestTask());
  dprint("Construct TestTask2\n");
  gExecBase.AddTask(new ConsoleWindowTestTask());
  dprint("Construct TestIdcmp\n");
  gExecBase.AddTask(new TestIdcmpTask());
  dprint("Construct ExitTests\n");
  // gExecBase.AddTask(new ExitTestTask(EExitTestReturn, "RETURN", 4));
  // gExecBase.AddTask(new ExitTestTask(EExitTestSuicide, "SUICIDE", 8));
  // gExecBase.AddTask(new ExitTestTask(EExitTestExit, "EXIT", 12));
}
