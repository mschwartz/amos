;; boot info block

SYS_BOOTDRIVE       equ 0                                   ;; bios boot drive
SYS_BOCHS           equ SYS_BOOTDRIVE + 1                   ;; BOCHS present 
SYS_BOOTSECTOR      equ SYS_BOCHS + 1                       ;; boot sector number
SYS_BOOTSECTORS     equ SYS_BOOTSECTOR + 2                  ;; number of sectors to load
SYS_KERNELSECTOR    equ SYS_BOOTSECTORS + 2                 ;; kernel sector number
SYS_KERNELSECTORS   equ SYS_KERNELSECTOR + 2                ;; number of sectors to load
;; drives?

;; VESA graphics mode
SYS_VIDEOMODE       equ SYS_KERNELSECTORS + 2
SYS_FRAMEBUFFER     equ SYS_VIDEOMODE + 4
SYS_FBWIDTH         equ SYS_FRAMEBUFFER + 4
SYS_FBHEIGHT        equ SYS_FBWIDTH + 4
SYS_FBPITCH         equ SYS_FBHEIGHT + 4
SYS_FBDEPTH         equ SYS_FBPITCH + 4

;; PAGE TABLES
SYS_PAGESIZE        equ SYS_FBDEPTH + 4
SYS_PML4            equ SYS_PAGESIZE + 4
SYS_PDPLOW          equ SYS_PML4 + 8
SYS_PDPHIGH         equ SYS_PDPLOW + 8
SYS_PDLOW           equ SYS_PDPHIGH + 8
SYS_PDHIGH          equ SYS_PDLOW + 8
SYS_PAGETABLE_END   equ SYS_PDHIGH + 8                      ;; page table end address + 1

SYS_ACPI            equ SYS_PAGETABLE_END + 8               ;; address of ACPI tables
SYS_BSPID           equ SYS_ACPI + 4                        ;; APIC ID of the BSP
SYS_CPUSPEED        equ SYS_BSPID  + 4
SYS_CORESACTIVE     equ SYS_CPUSPEED + 2
SYS_CORESDETECTED   equ SYS_CORESACTIVE + 2
SYS_RAMSIZE         equ SYS_CORESDETECTED + 2
SYS_NUMIOAPIC       equ SYS_RAMSIZE + 4
SYS_HPET            equ SYS_NUMIOAPIC + 2
SYS_LAPIC           equ SYS_HPET + 8
SYS_IOAPIC          equ SYS_LAPIC + 8                       ;; array of addresses
SYS_APICIDS         equ SYS_IOAPIC + 8 * 3
SYS_SIZEOF          equ SYS_APICIDS + 3

