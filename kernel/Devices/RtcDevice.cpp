#include <Devices/RtcDevice.h>
#include <Exec/ExecBase.h>
#include <Exec/BTask.h>

const TUint8 SELECT = 0x70;
const TUint8 DATA = 0x71;

const TUint8 SECONDS = 0;
const TUint8 MINUTES = 2;
const TUint8 HOURS = 4;
const TUint8 DAY = 7;
const TUint8 MONTH = 8;
const TUint8 YEAR = 9;

static int busy() {
  outb(0x0a, SELECT);
  return inb(DATA) & 0x80;
}
static TUint8 read_cmos(TUint8 reg) {
  outb(reg, SELECT);
  return inb(DATA);
}

extern "C" void enable_irq8();

/********************************************************************************
 ********************************************************************************
 ********************************************************************************/

class RtcTask;

class RtcInterrupt : public BInterrupt {
public:
  RtcInterrupt(RtcTask *aTask) : BInterrupt("Rtc Interrupt Handler", LIST_PRI_MAX) {
    mTask = aTask;
  }

public:
  TBool Run(TAny *aData);

protected:
  RtcTask *mTask;
};

/********************************************************************************
 ********************************************************************************
 ********************************************************************************/

class RtcTask : public BTask {
public:
  RtcTask(RtcDevice *aRtcDevice) : BTask("Rtc Task", LIST_PRI_MAX) {
    mRtcDevice = aRtcDevice;
  }

public:
  void Run() {
    TUint64 flags = GetFlags();
    cli();

    dlog("RTC Task running!\n");
    ReadRtc();
    dlog("  --- Read RTC: %02d/%02d/%02d %02d:%02d:%02d\n", mMonth, mDay, mYear, mHours, mMinutes, mSeconds);

    gExecBase.SetIntVector(ERtcClockIRQ, new RtcInterrupt(this));

    // enable RTC interrupt on the RTC controller
    // default rate is 1/1024 (1024 hz) or 0x06
    enable_irq8();
    
    while (ETrue) {
      TUint64 sigs = Wait(1<<10);
    };
  }

protected:
  volatile TUint16 mMonth, mDay, mYear;
  volatile TUint16 mHours, mMinutes, mSeconds, mMillis;

protected:
  void ReadRtc() {
    mMillis = 0;

    TUint8 sec, mins, hr, mo, day, yr;
    TUint8 xsec, xmins, xhr, xmo, xday, xyr;

    while (busy())
      ;
    sec = read_cmos(SECONDS);
    mins = read_cmos(MINUTES);
    hr = read_cmos(HOURS);

    day = read_cmos(DAY);
    mo = read_cmos(MONTH);
    yr = read_cmos(YEAR);

    do {
      xsec = sec;
      xmins = mins;
      xhr = hr;
      xday = day;
      xmo = mo;
      xyr = yr;

      while (busy())
        ;

      sec = read_cmos(SECONDS);
      mins = read_cmos(MINUTES);
      hr = read_cmos(HOURS);

      day = read_cmos(DAY);
      mo = read_cmos(MONTH);
      yr = read_cmos(YEAR);
    } while ((xsec != sec) || (xmins != mins) || (xhr != hr) || (xday != day) || (xmo != mo) || (xyr != yr));

    // convert BCD
    TUint8 registerB = read_cmos(0x0b);
    if ((registerB & 0x04) == 0) {
      mSeconds = (sec & 0x0f) + ((sec / 16 * 10));
      mMinutes = (mins & 0x0f) + ((mins / 16 * 10));
      mHours = (hr & 0x0f) + ((hr / 16 * 10)) | (hr & 0x80);
      mDay = (day & 0x0f) + ((day / 16 * 10));
      mMonth = (mo & 0x0f) + ((mo / 16 * 10));
      mYear = (yr & 0x0f) + ((yr / 16 * 10));
    }
    mYear += 2000; // we will have a y3k problem!!!

    // convert 12 hour time to 24 hour time
    if ((registerB & 0x02) == 0 && (hr & 0x80)) {
      mHours = ((mHours & 0x7f) + 12) % 24;
    }
  }
public:
  void UpdateMillis() {
//    ReadRtc();
    mRtcDevice->mMillis ++;
    mMillis++;
  }
protected:
  RtcDevice *mRtcDevice;
};

/********************************************************************************
 ********************************************************************************
 ********************************************************************************/

extern "C" void ack_irq8();

TBool RtcInterrupt::Run(TAny *aData) {
//  cli();
  mTask->UpdateMillis();
  gPIC->ack(IRQ_RTC);
  outb(SELECT, 0x0c);
  inb(DATA);
  mTask->Signal(1<<10);
  return ETrue;
}

/********************************************************************************
 ********************************************************************************
 ********************************************************************************/

RtcDevice::RtcDevice() : BDevice("rtc.device") {
  mMillis = 0;
  gExecBase.AddTask(new RtcTask(this));
}
