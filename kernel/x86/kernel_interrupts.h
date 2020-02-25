#ifndef KERNEL_INTERRUPTS_H
#define KERNEL_INTERRUPTS_H

#define INTERRUPTS 49

typedef struct _IDT_T_ {
  uint16_t offset_lo;  /* Offset (bits 0-15)	*/
  uint16_t seg_sel;    /* Segment selector	*/
  uint8_t zero;        /* Always zero		*/
  uint8_t type : 5;    /* Type			*/
  uint8_t DPL : 2;     /* Privilege level	*/
  uint8_t present : 1; /* Present		*/
  uint16_t offset_hi;  /* Offset (bits 16-31)	*/

} __attribute__((__packed__)) IDT_t;

#define disable_interrupts()    asm volatile ( "cli\n\t" )
#define enable_interrupts()     asm volatile ( "sti\n\t" )

#define halt()                  asm volatile ( "cli \n\t" "hlt \n\t" );

#define suspend()               asm volatile ( "hlt \n\t" );

#define raise_interrupt(p)      asm volatile ("int %0\n\t" :: "i" (p):"memory")

extern void init_interrupts();

#endif
