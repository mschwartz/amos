#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

#include <types.h>
#include <mmu.h>

const uint64_t KILOBYTE = 1024;
const uint64_t MEGABYTE = 1024 * KILOBYTE;
const uint64_t GIGABYTE = 1024 * MEGABYTE;

//
// memory map
//

// interrupt vector table
const uint64_t IVT              = 0x00000000;   // 1KB
const uint64_t IVT_END          = 0x000003ff;
// BIOS data area
const uint64_t BDA              = 0x00000400;   // 256 bytes
const uint64_t BDA_END          = 0x000004ff;
// low RAM 1
const uint64_t LOW_RAM1         = 0x00000500;   // ~30KB
const uint64_t LOW_RAM1_END     = 0x00007bff;
// boot sector (loaded here);
const uint64_t BOOT_SECTOR      = 0x00007c00;   // 512 bytes
const uint64_t BOOT_SECTOR_END  = 0x00007dff;
// low RAM 2
const uint64_t LOW_RAM2         = 0x00007e00;   // 480.5KB
const uint64_t LOW_RAM2_END     = 0x0007ffff;
// extended BIOS data area
const uint64_t EBDA             = 0x00080000;   // 128KB
const uint64_t EBDA_END         = 0x0009ffff;
// CGA Frame Buffer
const uint64_t FRAME_BUFFER     = 0x000a0000;   // 384KB
const uint64_t FRAME_BUFFER_END = 0x000fffff;
// CGA Text Mode Buffer
const uint64_t TEXT_BUFFER      = 0x000b8000;   // 32KB
const uint64_t TEXT_BUFFER_END  = 0x000bffff;
// ROM BIOS
const uint64_t ROM_BIOS         = 0x000f4000;
const uint64_t ROM_BIOS_END     = 0x0010ffef;
// HIGH RAM1
const uint64_t HIGH_RAM1        = 0x00100000;   // 14MB
const uint64_t HIGH_RAM1_END    = 0x00efffff;
// ISA MEMORY HOLE
const uint64_t ISA_HOLE         = 0x00f00000;   // 1MB
const uint64_t ISA_HOLE_END     = 0x00ffffff;
// HIGH_RAM2
const uint64_t HIGH_RAM2        = 0x01000000;   // whatever exists
const uint64_t HIGH_RAM2_END    = 0xbfffffff;
// HIGH RAM (above 4GB)
const uint64_t HIGH_RAM         = 0x0000000100000000; // unlimited


extern "C" void bzero(void *dst, unsigned long size);

#endif
