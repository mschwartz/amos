#ifndef EXEX_GRAPHICS_GRAPHICS_H
#define EXEX_GRAPHICS_GRAPHICS_H

// Master Graphics include file - baseically defines all graphics methods and types.

#include <Types.hpp>
//#include <Exec/Graphics/TBCD.h>
#include <Types/TRGB.hpp>
#include <Types/TRect.hpp>
#include <Types/TNumber.hpp>

#include <Graphics/BBitmap.hpp>
#include <Graphics/bitmap/BBitmap32.hpp>

#include <Graphics/BFont.hpp>

///////// Assembly helpers

/**
 * Copy aCount RGB pixels from aSource to aDestination
 */
extern "C" void CopyRGB(TAny *aDestination, TAny *aSource, TInt64 aCount);

/**
 * Copy aCount RGB pixels from aSource to aDestination, two pixels at a time
 */
extern "C" void CopyRGB64(TAny *aDestination, TAny *aSource, TInt64 aCount);

#endif
