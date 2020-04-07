OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x10000;
    .init : {
	    *(.init)
    }
    init_end = .;
    .text : { 
	    *(start)
	    *(.text) 
    }
    text_end = .;
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
    rodata_end = .;
    .data : { *(.data) }
    data_end = .;
    .bss  : { *(.bss) *(COMMON) }
    bss_end = .;
    .fini : {
	    *(.fini)
    }
    kernel_end = .;
}
