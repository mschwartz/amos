OBJ= 	 bochs.o cpu.o kprint.o tasking.o gdt.o idt.o kernel_isr.o mmu.o

LIB=	libx86.a

.cpp.o:
	$(GCC) -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp

.asm.o:
	@echo ""
	nasm -f elf64 -o $*.o $*.asm
	@echo ""

$(LIB):	$(OBJ)
	$(AR) rcs $(LIB) $(OBJ)
	$(RANLIB) $(LIB)

kernel_isr.o: kernel_isr.asm
	@echo ""
	@echo "*** NASM"
	nasm -f elf64 -o $*.o $*.asm
	@echo "***"
	@echo ""

clean:
	rm -rf *.o *.a
