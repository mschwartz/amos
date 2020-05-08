#ifndef AMOS_INSPIRATION_H
#define AMOS_INSPIRATION_H

#include <Exec/Types.h>
#include <Exec/BBase.h>

class InspirationBase : public BBase {
public:
  InspirationBase();
  ~InspirationBase();
};

extern InspirationBase& gInspirationBase;

#endif

