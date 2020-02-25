/*
 * main() entrypoint for our example OS-like application
 */

#include "./x86/types.h"
#include "string.h"
#include "./cclib/kprint.h"
#include "./cclib/Screen.h"
#include "./x86/kernel_memory.h"

extern "C" {
int kernel_main(uint32_t ax);
void foo();
}

int kernel_main(uint32_t ax) {
  Screen s;
  screen = &s;

  //  screen->puts("HERE\n");
  char buf[10];
  memset(buf, 0, 8);

  //  screen->cls();
  //  kprint("hello! %d $%x\n", ax, ax);
  //  kprint("a\n");
  //  kprint("b\n");
  //  foo();
  //  screen->hexword(0xea5a);
  //  ax = 0xdeadbeef;
  //  kprint("ax = %d $%x\n", ax, ax);
  //  screen->hexlong(0xbeefdead);
  uint64_t memory_size = init_memory(),
           pages = memory_size / PAGE_SIZE;
  //  screen->hexlong(pages);
  //  screen->newline();
  kprint("%d bytes, %d pages\n", memory_size, pages);

//  screen->hexdump(0x5000, 20);
  //  kprint("Here %d\n", pages);
  //  screen->hexlong(0xdeadbeef);
  //  screen->newline();
  //  screen->hexlong(pages);
  //  screen->hexbyte(ax);
  //  screen->hexbyte(ax>>8);
  //  screen->hexbyte(0xe5);
  //  screen->hexbyte(0xe5);
  screen->newline();
  for (int i = 0; i < 5; i++) {
    screen->puts("Hello from kernel_main\r\n");
  }
  return 0;
}
