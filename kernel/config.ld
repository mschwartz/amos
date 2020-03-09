OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x9000;
    .text : { 
	    *(start)
	    *(.text) 
    }
    .data : { *(.data) }
    .bss  : { *(.bss) *(COMMON) }
}
