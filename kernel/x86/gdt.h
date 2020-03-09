#ifndef KERNEL_GDT_H
#define KERNEL_GDT_H

#include <types.h>

class GDT {
public:
  GDT();
  ~GDT();

public:
  void set_gate(int id, void *start_addr, uint32_t size, uint32_t priv_level);
  void set_gate(int num, uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity);
  void tss_install();
};

extern GDT *gdt;

#if 0 
#define GDT 0
#define LDT 1

/* descriptor indexes */
#define SEGM_CODE 1
#define SEGM_DATA 2
#define SEGM_TSS 3

#define PRIV_KERNEL 0
#define PRIV_USER 3


/*! TSS - Task State Segment */
typedef struct _tss_t_
{
	uint32_t	link;	/* not used in this implementation */

	uint32_t	esp0;	/* stack for ring 0 - thread context is here saved */
	uint16_t	ss0;	/* stack segment descriptor for ring 0 */

	uint16_t	ss0__;	/* bits not used */

	/* stack pointers and stack segments for rings 1 and 2 - not used */
	uint32_t	esp1, ss1, esp2, ss2;

	uint32_t	cr3;		/* control register 3 - paging - not used */

	uint32_t	eip, eflags, reg[8], seg[6]; /* hardware context - not used */

	uint32_t	ldt;		/* LDT pointer - not used */

	uint16_t	trap;		/* not used */

	uint16_t	ui_map;		/* not used */
}
__attribute__((__packed__)) tss_t;

/* GDT row format */
typedef struct _GDT_t_
{
	uint16_t	segm_limit0; /* segment limit, bits: 15:00	(00-15) */
	uint16_t	base_addr0;  /* starting address, bits: 15:00	(16-31) */
	uint8_t	base_addr1;  /* starting address, bits: 23:16	(32-38) */
	uint8_t	type	: 4; /* segment type			(39-42) */
	uint8_t	S	: 1; /* type: 0-system, 1-code or data	(43-43) */
	uint8_t	DPL	: 2; /* descriptor privilege level	(44-45) */
	uint8_t	P	: 1; /* present (in memory)		(46-46) */
	uint8_t	segm_limit1: 4; /*segment limit, bits: 19:16	(47-50) */
	uint8_t	AVL	: 1; /* "Available for use"		(51-51) */
	uint8_t	L	: 1; /* 64-bit code?			(52-52) */
	uint8_t	DB	: 1; /* 1 - 32 bit system, 0 - 16 bit	(53-53) */
	uint8_t	G	: 1; /* granularity 0-1B, 1-4kB 	(54-54) */
	uint8_t	base_addr2;  /* starting address, bits: 23:16	(55-63) */
}
__attribute__((__packed__)) GDT_t;

/* GDT register - GDTR */
typedef struct _GDTR_t_
{
	uint16_t limit;	/* GDT size */
	GDT_t *gdt;	/* GDT address (location) */
}
__attribute__((__packed__)) GDTR_t;

#define GDT_DESCRIPTOR(ID, TABLE, PRIV_LEVEL) \
  (((ID) << 3) | ((TABLE) << 2) | (PRIV_LEVEL))

/* GDT elements defaults values */

/* First element (with index 0) must be zero */
#define GDT_0 \
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

/* Second element (with index 1) describes code segment
 * Since no protection is used so far, we turn this off by allowing code from
 * any memory location, within available 4 GB (r-x)
 */
#define GDT_CODE               \
  {                            \
    0xffff, /* segm_limit0	*/  \
      0,    /* base_addr0	*/   \
      0,    /* base_addr1	*/   \
      0x0a, /* type	r-x	*/     \
      1,    /* S		*/           \
      0,    /* DPL - ring 0 */ \
      1,    /* P		*/           \
      0x0f, /* segm_limit1	*/  \
      0,    /* AVL		*/         \
      0,    /* L		*/           \
      1,    /* DB		*/          \
      1,    /* G		*/           \
      0     /* base_addr2	*/   \
  }

/* Third element (with index 2) describes data segment
 * Since no protection is used so far, we turn this off by allowing data from
 * any memory location, within available 4 GB (rw-)
 */
#define GDT_DATA               \
  {                            \
    0xffff, /* segm_limit0	*/  \
      0,    /* base_addr0	*/   \
      0,    /* base_addr1	*/   \
      0x02, /* type	rw-	*/     \
      1,    /* S		*/           \
      0,    /* DPL - ring 0 */ \
      1,    /* P		*/           \
      0x0f, /* segm_limit1	*/  \
      0,    /* AVL		*/         \
      0,    /* L		*/           \
      1,    /* DB		*/          \
      1,    /* G		*/           \
      0     /* base_addr2	*/   \
  }

/* TSS - Task State Segment descriptor */
#define GDT_TSS                       \
  {                                   \
    sizeof(tss_t), /* segm_limit0	*/  \
      0,           /* base_addr0	*/   \
      0,           /* base_addr1	*/   \
      0x09,        /* 10B1 = 1001	*/  \
      0,           /* -		*/           \
      0,           /* DPL - ring 0 */ \
      1,           /* P		*/           \
      0x00,        /* segm_limit1	*/  \
      0,           /* AVL		*/         \
      0,           /* L		*/           \
      0,           /* DB		*/          \
      0,           /* G		*/           \
      0            /* base_addr2	*/   \
  }
#endif

#endif
