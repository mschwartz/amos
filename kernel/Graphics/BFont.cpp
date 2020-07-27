#include <Graphics/Graphics.hpp>
#include <Graphics/BFont.hpp>
// #include <Graphics/font/BVectorFont.h>
#include <Graphics/font/BConsoleFont.hpp>

#include <posix/string.h>
#include <stdarg.h>
#include <math.h>

BFont::BFont(const char *aName) : BNodePri(aName) {
  dprint("Construct BFont\n");
//
}
BFont::~BFont() {
//
}
