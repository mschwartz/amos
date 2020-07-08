#ifndef EXAMPLES_CONSOLEWINDOWTEST_H
#define EXAMPLES_CONSOLEWINDOWTEST_H

#include <Exec/ExecBase.h>
#include <Inspiration/InspirationBase.h>

class ConsoleWindowTestTask : public BProcess {
public:
  ConsoleWindowTestTask() : BProcess("ConsoleWindowTest Process") {}
  ~ConsoleWindowTestTask() {}

protected:
  void HandleKeys(BConsoleWindow *con) {
    while (con->KeyReady()) {
      char c = con->ReadKey();
      dlog("IDCMP key: %x(%c)\n", c, c);
    }
  }
  void wait(TInt n, BConsoleWindow *con) {
    for (TInt i = 0; i < n; i++) {
      HandleKeys(con);
      Sleep(1);
    }
  }

public:
  void Run();
};

#endif
