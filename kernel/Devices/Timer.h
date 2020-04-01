/*!
 * 8253 Programmable Interupt Timer (PIT)
 */

#ifndef KERNEL_TIMER_H
#define KERNEL_TIMER_H

#include <types.h>

class Timer {
public:
  Timer();
  ~Timer();

public:
  void set_frequency(int hz);
  void increment_ticks() { ticks++; }

protected:
  uint64_t ticks; // ticks/interrupts since start
};

extern Timer *timer;

#endif
