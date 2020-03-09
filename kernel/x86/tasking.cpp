#include <tasking.h>

extern "C" task_t *current_task;

task_t task0;

Scheduler *scheduler;

Scheduler::Scheduler() {
  current_task = &task0;
}

Scheduler::~Scheduler() {
}
