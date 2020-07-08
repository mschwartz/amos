#include <Examples/TestTask.h>
#include <Examples/ConsoleWindowTest.h>

extern "C" void StartExamples() {
  dprint("Construct TestTask\n");
  gExecBase.AddTask(new TestTask());
  dprint("Construct TestTask2\n");
  gExecBase.AddTask(new ConsoleWindowTestTask());
}
