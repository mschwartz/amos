                    %macro bochs 0
                    xchg bx, bx
                    %endm

                    %macro BOCHS 0
                    xchg bx, bx
                    %endm

                    extern putc64
                    extern puts64
                    extern newline64
                    extern space64
                    extern hexbyte64
                    extern hexword64
                    extern hexbytes64
                    extern hexwords64
                    extern hexlongs64
                    extern hexquads64
                    extern hexbyten64
                    extern hexwordn64
                    extern hexlongn64
                    extern hexquadn64
                    extern hexdump64
                    extern stackdump64

;; CR0
CR0_PE              equ (1<<0)          ; Protected Mode Enable
CR0_MP              equ (1<<1)          ; Monitor Co-Processor
CR0_EM              equ (1<<2)          ; Emulation
CR0_TS              equ (1<<3)          ; Task Switched
CR0_ET              equ (1<<4)          ; Extension Type
CR0_NE              equ (1<<5)          ; Numeric Error
CR0_WP              equ (1<<16)         ; Write protect
CR0_AM              equ (1<<18)         ; Alignment Mask
CR0_NW              equ (1<<29)         ; Not Write Thorugh
CR0_CD              equ (1<<30)         ; Cache Disable
CR0_PG              equ (1<<31)         ; Paging

;; CR4 register (features)
CR4_VME             equ (1<<0)          ; Virtual 808x mode extensions
CR4_PVI             equ (1<<1)          ; Protected Mode Virtual Interrupts
CR4_TSD             equ (1<<2)          ; Time Stamp enabled only in ring 0
CR4_DE              equ (1<<3)          ; debugging extensions
CR4_PSE             equ (1<<4)          ; page size extension
CR4_PAE             equ (1<<5)          ; physical address extension
CR4_MCE             equ (1<<6)          ; machine check exception
CR4_PGE             equ (1<<7)          ; page global enable
CR4_PCE             equ (1<<8)          ; performance monitoring counter enable
CR4_OSFXSR          equ (1<<9)          ; os support for fxsave and fxrstor instrucitons
CR4_OSXMMEXCPT      equ (1<<10)         ; os support for unmasked simd floating point exceptions
CR4_UMIP            equ (1<<11)         ; user mode instruction preventions (disable SGD, SIDT, SLDT, SMSW, and STR)
CR4_RESERVED0       equ (1<<12)         ; reserved
CR4_VMXE            equ (1<<13)         ; virtual machine extensions enable
CR4_SMXE            equ (1<<14)         ; safer mode extensions enable
CR4_RESERVED1       equ (1<<15)         ; reserved
CR4_PCID            equ (1<<17)         ; PCID enable
CR4_OSXSAVE         equ (1<<18)         ; xsave and processor extended states enabble
CR4_RESERVED2       equ (1<<19)         ; reserved
CR4_SMEP            equ (1<<20)         ; supervisor mode executions protection enable
CR4_SMAP            equ (1<<21)         ; supervisor mode access protection enable

;; GDT
NULL_SEG            equ 0
KCODE_SEG           equ 8
KDATA_SEG           equ 10
UCODE_SEG           equ 18
UDATA_SEG           equ 20
TSS_SEG             equ 28

