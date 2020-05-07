#include <stdio.h>
#include <stdint.h>


uint64_t p4_table[512], // PML4
         p3_table[512], // PDT
         p2_table[512], // PD
         p1_table[512]; // PT

void zero_pt(uint64_t *t) {
  for (int i=0; i<512; i++) {
    *t++ = 0;
  }
}

#if 0
uint64_t pml4(uint64_t addr) {
  return (addr > 39); //  & 0x1ff;
}

uint64_t pdt(uint64_t addr) {
  return (addr > 30); //  & 0x1ff;
}

uint64_t pd(uint64_t addr) {
  return (addr > 21); //  & 0x1ff;
}

uint64_t pt(uint64_t addr) {
  return (addr > 12); //  & 0x1ff;
}
#endif

const uint64_t PAGE_SIZE = 2 * 1024 * 1024;
int main(int ac, char *av[]) {
  printf("Paging tool\n");
  zero_pt(p4_table);
  zero_pt(p3_table);
  zero_pt(p2_table);
  zero_pt(p1_table);

  for (uint64_t addr=0; addr < 0x2000000000; addr += PAGE_SIZE) {
    uint64_t pml4 = (addr >> 39) & 0x1ff;
    uint64_t pdt = (addr >> 30) & 0x1ff;
    uint64_t pd = (addr >> 21) & 0x1ff;
    uint64_t pt = (addr >> 12) & 0x1ff;
//    printf("%016lx %016lx\n", addr, (addr>>39) & 0x1ff);
    printf("%016lx    %016lx %016lx %016lx %016lx\n", addr, pml4, pdt, pd, pt);
  }
//  const uint64_t addr = 0xe0000000;
//  printf("pml4 0x%x %d %16x\n", addr, pml4(addr), pml4(addr));
//  printf("pdt  0x%x %d %16x\n", addr, pdt(addr), pdt(addr));
//  printf("pd   0x%x %d %16x\n", addr, pd(addr), pd(addr));
//  printf("pt   0x%x %d %16x\n", addr, pt(addr), pt(addr));
  return 0;
}
