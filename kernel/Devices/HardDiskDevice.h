#ifndef AMOS_HARDDISKDEVICE_H
#define AMOS_HARDDISKDEVICE_H

#include <Exec/BDevice.h>

class HardDiskDevice : public BDevice {
public:
  HardDiskDevice();
  ~HardDiskDevice();
};

#endif
