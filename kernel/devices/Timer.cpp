#include <Timer.h>
#include <cpu.h>
#include <bochs.h>
#include <kprint.h>
#include <PIC.h>
#include <idt.h>

#define PIT_TICKS_PER_DAY 0x1800b0

/* i8253 ports and commands */
#define I8253_CH0 0x40
#define I8253_CH1 0x41 /* channel 1 not used */
#define I8253_CH2 0x42 /* channel 2 not used */
#define I8253_CMD 0x43
#define I8253_CMD_LOAD 0x34
#define I8253_CMD_LATCH 0x04

Timer *timer;

bool timer_isr(void *aData) {
//  dprint("TIMER INTERRUPT\n");
  Timer *t = (Timer *)aData;
  t->increment_ticks();
  pic->ack(IRQ_TIMER);
//  dprint(" ack %d\n", IRQ_TIMER);
  return true;
}

void Timer::set_frequency(int hz) {
  int divisor = 1193180 / hz;
  outb(I8253_CMD, 0x36);
  outb(I8253_CH0, divisor & 0xff);
  outb(I8253_CH0, divisor >> 8);
}

Timer::Timer() {
  kprint("construct timer\n");
//  bochs
  ticks = 0;

  set_frequency(100);
  idt->install_handler(IRQ_TIMER, timer_isr, this, "8253 Timer");
  pic->enable_interrupt(IRQ_TIMER);
}

Timer::~Timer() {
}
