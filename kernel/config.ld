OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x10000;
    .init : {
	    *(.init)
    }
    .text : { 
	    *(start)
	    *(.text) 
    }
    .init_array : {
	    __init_array_start = .;
	    KEEP (*(.init_array*))
	    __init_array_end = .;
    }
    /*
    .ctors : {
	    __global_ctors = .;
	    KEEP(*(.ctor*))
	    __global_ctors_end = .;
    }
	    */
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss  : { *(.bss) *(COMMON) }
    .fini : {
	    *(.fini)
    }
}
