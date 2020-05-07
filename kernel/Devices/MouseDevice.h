#ifndef DEVICE_MOUSE_DEVICE_H
#define DEVICE_MOUSE_DEVICE_H

#include <Exec/BDevice.h>

class MouseDevice : public BDevice {
public:
  MouseDevice();
  ~MouseDevice();

protected:
  TInt mScreenWidth, mScreenHeight;
  TInt mX, mY;
};

#endif
