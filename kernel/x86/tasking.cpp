#include <tasking.h>


//task_t task0;

//extern "C" task_t *current_task;
//Scheduler *scheduler;

//Scheduler::Scheduler() {
//  current_task = &task0;
//}

//Scheduler::~Scheduler() {
//}

int *__errno_location() {
  return &current_task->errno;
}
