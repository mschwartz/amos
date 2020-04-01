/*!
 * 8253 Programmable Interupt Timer (PIT)
 */

#ifndef KERNEL_TIMER_H
#define KERNEL_TIMER_H

#include <Exec/BTypes.h>

class Timer {
public:
  Timer();
  ~Timer();

public:
  void set_frequency(TInt hz);
  void increment_ticks() { ticks++; }
  TUint64 GetTicks() { return ticks; }

protected:
  TUint64 ticks; // ticks/interrupts since start
};

extern Timer *gTimer;

#endif
