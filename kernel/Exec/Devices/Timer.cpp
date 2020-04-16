#include <Exec/ExecBase.h>
#include <Devices/PIC.h>
#include <Devices/Timer.h>
#include <x86/bochs.h>

#define PIT_TICKS_PER_DAY 0x1800b0

/* i8253 ports and commands */
#define I8253_CH0 0x40
#define I8253_CH1 0x41 /* channel 1 not used */
#define I8253_CH2 0x42 /* channel 2 not used */
#define I8253_CMD 0x43
#define I8253_CMD_LOAD 0x34
#define I8253_CMD_LATCH 0x04

Timer *gTimer;

bool timer_isr(void *aData) {
  Timer *t = (Timer *)aData;
  t->increment_ticks();
  gPIC->ack(IRQ_TIMER);
  gExecBase.Reschedule();
  return true;
}

void Timer::set_frequency(TInt hz) {
  int divisor = 1193180 / hz;
  outb(I8253_CMD, 0x36);
  outb(I8253_CH0, divisor & 0xff);
  outb(I8253_CH0, divisor >> 8);
}

Timer::Timer() {
  dprint("Construct timer\n");
  ticks = 0;

  set_frequency(100);   // TODO: use ExecBase quantum instead of 100
  gExecBase.AddInterruptHandler(IRQ_TIMER, timer_isr, this, "8253 Timer");
  gPIC->enable_interrupt(IRQ_TIMER);
}

Timer::~Timer() {
}
