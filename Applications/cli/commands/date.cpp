#define DEBUGME
#undef DEBUGME

#include "commands.hpp"
#include <Exec/ExecBase.hpp>
#include <Devices/RtcDevice.hpp>

const TBool utc = EFalse;
const TBool dst = ETrue;
const TInt tz = utc ? 0 : -(8 + (dst ? -1 : 0));

TInt64 CliTask::command_date(TInt ac, char **av) {
  const char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  const char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

  for (TInt i = 0; i < 10; i++) { // try for 10 seconds
    RtcDevice *rtc = (RtcDevice *)gExecBase.FindDevice("rtc.device");
    if (rtc) {
      TInt hour = (rtc->mHours + tz) % 24;
      const char *ampm = "AM";
      if (utc) {
	ampm = "UTC";
      }
      else if (hour > 12) {
        hour -= 12;
        ampm = "PM";
      }
      DLOG("%3s %2d %3s %4d %02d:%02d:%02d %s\n",
        days[rtc->mWeekday - 1],
        rtc->mDay,
        months[rtc->mMonth - 1],
        rtc->mYear,
        (rtc->mHours & 0x7f) - 8,
        rtc->mMinutes,
        rtc->mSeconds,
        ampm);
      mWindow->WriteFormatted("%3s %2d %3s %4d %02d:%02d:%02d %s\n",
        days[rtc->mWeekday - 1],
        rtc->mDay,
        months[rtc->mMonth - 1],
        rtc->mYear,
        hour,
        rtc->mMinutes,
        rtc->mSeconds,
        ampm);
      return 0;
    }
    Sleep(1);
  }

  return Error("Can't find rtc.device\n");
}
