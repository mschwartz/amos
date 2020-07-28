#ifndef INSPIRATION_DESKTOP_H
#define INSPIRATION_DESKTOP_H

#include <Inspiration/BScreen.hpp>

class Desktop : public BScreen {
public:
  Desktop();

public:
  void RenderTitlebar();
};

#endif
