OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x80000;
    init_start = .;
    .init : {
	    *(.init)
    }
    init_end = .;
    text_start = .;
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
    rodata_start = .;
    .rodata : { *(.rodata) }
    rodata_end = .;
    data_start = .;
    .data : { *(.data) }
    data_end = .;
    bss_start = .;
    .bss  : { *(.bss) *(COMMON) }
    bss_end = .;
    .fini : {
	    *(.fini)
    }
    kernel_end = .;
}
