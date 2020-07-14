#include <Examples/TestTask.h>
#include <Examples/ConsoleWindowTest.h>
#include <Examples/TestIdcmp.h>

extern "C" void StartExamples() {
  dprint("Construct TestTask\n");
  gExecBase.AddTask(new TestTask());
  dprint("Construct TestTask2\n");
  gExecBase.AddTask(new ConsoleWindowTestTask());
  dprint("Construct TestIdcmp\n");
  gExecBase.AddTask(new TestIdcmpTask());
}
