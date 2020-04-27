#ifndef GRAPHICS_BVIEWPORT_H
#define GRAPHICS_BVIEWPORT_H

#include <Exec/BList.h>

class BViewPort : public BNodePri {
public:
  BViewPort(const char *aName);
  ~BViewPort();
};

#endif
