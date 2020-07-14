#ifndef INSPIRATION_DISPLAYTASK_H
#define INSPIRATION_DISPLAYTASK_H

#include <Inspiration/Display.h>
#include <Exec/BTask.h>

/**
 * DisplayTask
 * 
 * Initialization figures out a rough estimate of how many milliseconds between 
 * vblanks.
 *
 *
 * There is no vbl interrupt, so we are going to try and do our best
 * to use the rtc timer to avoid having to poll for vbl for long period
 * of time, killing CPU (would be usage).
 * 
 * The strategy is to initially wait until we're not in vbl.
 * Then we time how long it takes to get into vbl, in ms.
 * Then we Sleep() for that time, in a loop, and when we wake
 * up, we poll for vbl.  This polling should be < 1ms.
 *
 * Because we don't know the position of the "beam" when not initially
 * in vbl, we need to wait until not in vbl (beam can be anything at this
 * point), then wait until in vblank, then wait until not, then time
 * how long it takes to get into vblank again.
 */

class DisplayTask : public BTask {
public:
  DisplayTask(Display &aDisplay)
      : BTask("DisplayTask", TASK_PRI_URGENT), mDisplay(aDisplay) {
    //
  }

public:
  void Run() ;

protected:
  Display &mDisplay;
};

#endif
