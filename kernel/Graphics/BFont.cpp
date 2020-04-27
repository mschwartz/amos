#include <Graphics/Graphics.h>
#include <Graphics/BFont.h>
#include <Graphics/font/BVectorFont.h>
#include <Graphics/font/BConsoleFont.h>

#include <posix/string.h>
#include <Exec/x86/bochs.h>
#include <stdarg.h>
#include <math.h>

BFont::BFont(const char *aName) : BNodePri(aName) {
  dprint("Construct BFont\n");
//
}
BFont::~BFont() {
//
}
