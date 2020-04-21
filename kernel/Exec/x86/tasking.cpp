#include <Exec/x86/tasking.h>

extern "C" task_t *current_task;
extern "C" task_t *next_task;

//int *__errno_location() {
//  return &current_task->errno;
//}
