#include "commands.hpp"

TInt64 CliTask::command_cpu(TInt ac, char **av) {
  for (TInt i = 0; i < gExecBase.NumCpus(); i++) {
    CPU *cpu = gExecBase.GetCpu(i);
    mWindow->WriteFormatted("CPU %2d %s %s %d cores\n", cpu->mProcessorId, cpu->mManufacturer, cpu->mBrand, cpu->mCores);
  }
  return 0;
}
