#ifndef EXEC_DEVICES_SCREEN_DEVICE_H
#define EXEC_DEVICES_SCREEN_DEVICE_H

#include <Exec/BDevice.h>
#include <Exec/MessagePort.h>
#include <Graphics/TRect.h>

class ScreenDevice : public BDevice {
public:
  ScreenDevice();
  ~ScreenDevice();

public:
  BBitmap32 *GetBitmap() { return mScreen ? mScreen->GetBitmap() : ENull; }
  void GetRect(TRect &aRect) { aRect = mRect; }

protected:
  ScreenVesa *mScreen;
  TRect mRect;
};

enum EScreenDeviceCommand {
  EGetMouse,
};

class ScreenMessage : public BMessage {
  public:
    ScreenMessage(MessagePort *aReplyPort, EScreenDeviceCommand aCommand) : BMessage(aReplyPort) {
      mCommand = aCommand;
    }
  public:
    EScreenDeviceCommand mCommand;
};

#endif
