#define DEBUGME
// #undef DEBUGME

#include <Inspiration/WindowManager/DisplayTask.hpp>
#include <Inspiration/BScreen.hpp>
#include <Exec/ExecBase.hpp>

DisplayTask::DisplayTask(Display &aDisplay)
    // : BTask("DisplayTask", TASK_PRI_URGENT), mDisplay(aDisplay) {
    : BTask("DisplayTask", TASK_PRI_URGENT), mDisplay(aDisplay) {
  //
}
static inline void start_vbl() {
  // dlog("enter start_vbl\n");
  while ((inb(0x3da) & (1<<3)) == 0) {
    // dlog("loop start_vbl\n");
  }
  // dlog("exit start_vbl\n");
}

static inline void end_vbl() {
  // dlog("enter end_vbl\n");
  while ((inb(0x3da) & (1<<3)) != 0) {
    // dlog("loop end_vbl\n");
  }
  // dlog("exit end_vbl\n");
}

static inline TUint64 time_vbl() {
  // dlog("time_vbl!\n");
  end_vbl();   // wait until we're not in vbl
  start_vbl(); // not in vblank at this point, wait until we are in vblank
  end_vbl();   // we know we're in vblank, wait until we're not in vbl

  // we just ended vblank, time how long it takes
  TUint64 start = gExecBase.SystemTicks();
  start_vbl(); // wait for in vblank
  TUint64 end = gExecBase.SystemTicks();
  DLOG("  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> start(%d) end(%d) vbl_time(%d)\n", start, end, end - start);
  return end - start;
}

TInt64 DisplayTask::Run() {
  DSPACE();
  DPRINT("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
  DLOG("DisplayTask Run %x\n", inb(0x3da));
  Sleep(1);

  TUint64 vbl_time = time_vbl();
  // TUint64 vbl_time = 0;

  if (vbl_time == 0) {
    vbl_time = 16;
  }

  DLOG("  vbl_time %d\n", vbl_time);

  TUint64 elapsed = 0, start = 0, end = 0, now = 0;

  // Wait for vbl in a loop and update screen via DirtyRects
  for (;;) {
    // We need to account for elapsed time for the drawing.
    TUint64 wait_time = vbl_time - elapsed;
    // dlog("wait_time(%d) start(%d) end(%d) vbl_time(%d)\n", wait_time, start, end, vbl_time);
    if (wait_time) {
      MilliSleep(vbl_time - elapsed);
    }
    else {
      // dlog("overrun\n");
    }
    start_vbl(); // wait for in vblank

    // dlog("start vbl (%d,%d)\n", mDisplay.mMouseX, mDisplay.mMouseY);

    // Forbid();
    // update dirty rects and mouse pointer
    start = gExecBase.SystemTicks();
    // DSPACE();
    // DLOG("Start: %d\n", start);

    // Cursor saved pristine screen pixels after all dirty rects were rendered last frame
    // so we need to restore those saved pixels so the screen is pristine again before
    // rendering any new dirty rects.  The mouse may have moved, so its saved pixels
    // need to be revealed.
    mDisplay.RestoreCursor();

    // render dirty rects
    BScreen *screen = mDisplay.TopScreen();
    screen->UpdateDirtyRects();

    // save background under curser and draw mouse pointer
    mDisplay.SaveCursor();
    mDisplay.RenderCursor();

    now = gExecBase.SystemTicks();
    // DLOG("update dirty rects: now: %d elapsed: %d\n", now, now - start);
    screen->UpdateWindows();
    end = gExecBase.SystemTicks();
    // DLOG("Update Windows end: %d elapsed: %d\n", end, end - now);
    elapsed = end - start;
    // DLOG("Elapsed %d\n", elapsed);
    // Permit();
  }
}
