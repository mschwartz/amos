#include <Examples/TestTask.hpp>
#include <Examples/ConsoleWindowTest.hpp>
#include <Examples/TestIdcmp.hpp>
#include <Examples/ExitTest.hpp>
#include <Applications/cli/cli.hpp>

extern "C" void StartExamples() {
  dlog("------- Construct TestIdcmp\n");
  gExecBase.AddTask(new TestIdcmpTask());

  // dlog("------- Construct ConsoleWindowTest\n");
  // gExecBase.AddTask(new ConsoleWindowTestTask());

  // dlog("------- Construct TestTask\n");
  // gExecBase.AddTask(new TestTask());

  // dlog("------- Construct CLI\n");
  // gExecBase.AddTask(new CliTask());

  dprint("Construct ExitTests\n");
  // gExecBase.AddTask(new ExitTestTask(EExitTestReturn, "RETURN", 12));
  // gExecBase.AddTask(new ExitTestTask(EExitTestSuicide, "SUICIDE", 8));
  // gExecBase.AddTask(new ExitTestTask(EExitTestExit, "EXIT", 12));
}
