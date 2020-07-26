#include "commands.hh"
#include <Exec/ExecBase.h>
#include <Devices/RtcDevice.h>

TInt64 CliTask::command_date(TInt ac, char **av) {
  const char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  const char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  for (TInt i = 0; i < 10; i++) {
    RtcDevice *rtc = (RtcDevice *)gExecBase.FindDevice("rtc.device");
    if (rtc) {
      mWindow->WriteFormatted("%3s %2d %3s %4d %02d:%02:%02d %s\n",
			      days[rtc->mDate.mWeekday-1], rtc->mDate.mDay, months[rtc->mDate.mMonth-1], rtc->mDate.mYear + 2000,
			      rtc->mDate.mHours & 0x7f, rtc->mDate.mMinutes, rtc->mDate.mSeconds, rtc->mDate.mPm ? "PM" : "AM");
      return 0;
    }
  }
  return Error("Can't find rtc.device\n");
}
