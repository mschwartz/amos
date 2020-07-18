#define DEBUGME
#undef DEBUGME

#include <Examples/TestTask.h>
#include <Examples/ConsoleWindowTest.h>
#include <Examples/TestIdcmp.h>

extern "C" void StartExamples() {
  // DLOG("Construct TestTask\n");
  // gExecBase.AddTask(new TestTask());
  DLOG("Construct TestTask2\n");
  gExecBase.AddTask(new ConsoleWindowTestTask());
  // DLOG("Construct TestIdcmp\n");
  // gExecBase.AddTask(new TestIdcmpTask());
}
