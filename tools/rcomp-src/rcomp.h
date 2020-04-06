#ifndef RCOMP_H
#define RCOMP_H

#include "Options.h"

#include <BTypes.h>

#include <cstdio>
//#include <sys/types.h>
#include <cstdarg>
#include <cstdint>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <libgen.h>
//
//#include <BTypes.h>
#include "Memory.h"
#include "Panic.h"

//
const int MAX_LEVELS = 10;
const int MAX_STRING_LENGTH = 4096;

//
#include "RawFile.h"
#include "lib/RawBitmap.h"
#include "BMPFile.h"
#include "ResourceFile.h"
#include "TileMap.h"
#include "TMXFile.h"
#include "SpriteSheet.h"

#ifndef O_BINARY
#define O_BINARY (0)
#endif


#include "lib/utils.h"

#endif //RCOMP_H
