#include "commands.hpp"

TInt64 CliTask::command_cpu(TInt ac, char **av) {
  for (CPU *cpu = gExecBase.FirstCpu(); !gExecBase.EndCpus(cpu); cpu = gExecBase.NextCpu(cpu)) {
    mWindow->WriteFormatted("CPU %2d %s %s %d cores\n", cpu->mProcessor, cpu->mManufacturer, cpu->mBrand, cpu->mCores);
  }
  return 0;
}
