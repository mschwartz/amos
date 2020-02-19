                    bits 32

;                    dd boot

                    global _start
_start:
                    jmp boot

;                    .include "common.asm"

;                    %include "lib/screen.inc"

foo:
                    nop
                    ret

                    extern main
boot:
                    call main

                    ret

