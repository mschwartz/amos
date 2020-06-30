#ifndef INSPIRATION_DESKTOP_H
#define INSPIRATION_DESKTOP_H

#include <Inspiration/BScreen.h>

class Desktop : public BScreen {
public:
  Desktop();

public:
  void RenderTitlebar();
};

#endif
