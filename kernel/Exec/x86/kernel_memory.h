#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

#include <Types.h>
#include <x86/mmu.h>

const TUint64 KILOBYTE = 1024;
const TUint64 MEGABYTE = 1024 * KILOBYTE;
const TUint64 GIGABYTE = 1024 * MEGABYTE;

//
// memory map
//

// interrupt vector table
const TUint64 IVT              = 0x00000000;   // 1KB
const TUint64 IVT_END          = 0x000003ff;
// BIOS data area
const TUint64 BDA              = 0x00000400;   // 256 bytes
const TUint64 BDA_END          = 0x000004ff;
// low RAM 1
const TUint64 LOW_RAM1         = 0x00000500;   // ~30KB
const TUint64 LOW_RAM1_END     = 0x00007bff;
// boot sector (loaded here);
const TUint64 BOOT_SECTOR      = 0x00007c00;   // 512 bytes
const TUint64 BOOT_SECTOR_END  = 0x00007dff;
// low RAM 2
const TUint64 LOW_RAM2         = 0x00007e00;   // 480.5KB
const TUint64 LOW_RAM2_END     = 0x0007ffff;
// extended BIOS data area
const TUint64 EBDA             = 0x00080000;   // 128KB
const TUint64 EBDA_END         = 0x0009ffff;
// CGA Frame Buffer
const TUint64 FRAME_BUFFER     = 0x000a0000;   // 384KB
const TUint64 FRAME_BUFFER_END = 0x000fffff;
// CGA Text Mode Buffer
const TUint64 TEXT_BUFFER      = 0x000b8000;   // 32KB
const TUint64 TEXT_BUFFER_END  = 0x000bffff;
// ROM BIOS
const TUint64 ROM_BIOS         = 0x000f4000;
const TUint64 ROM_BIOS_END     = 0x0010ffef;
// HIGH RAM1
const TUint64 HIGH_RAM1        = 0x00100000;   // 14MB
const TUint64 HIGH_RAM1_END    = 0x00efffff;
// ISA MEMORY HOLE
const TUint64 ISA_HOLE         = 0x00f00000;   // 1MB
const TUint64 ISA_HOLE_END     = 0x00ffffff;
// HIGH_RAM2
const TUint64 HIGH_RAM2        = 0x01000000;   // whatever exists
const TUint64 HIGH_RAM2_END    = 0xbfffffff;
// HIGH RAM (above 4GB)
const TUint64 HIGH_RAM         = 0x0000000100000000; // unlimited


const TUint64 BIOS_MEMORY = 0x9000;

extern "C" void bzero(TAny *dst, TUint64 size);

#endif
