/*
 * main() entrypoint for our example OS-like application
 */

#include <Exec/Types.h>
#include <Exec/x86/bochs.h>
#include <Exec/ExecBase.h>

typedef void (*func_ptr)(void);

// these are for calling global constructors
extern "C" func_ptr __init_array_start[0], __init_array_end[0];

static void call_global_constructors(void) {
  for (func_ptr *func = __init_array_start; func != __init_array_end; func++) {
    (*func)();
  }
}

extern "C" int kernel_main(TUint64 ax) {
  call_global_constructors();

  gExecBase.Kickstart();  // does not return

  // it should NEVER get here!
  return 0;
}
