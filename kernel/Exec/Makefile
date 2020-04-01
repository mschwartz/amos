OBJ= 	 BTypes.o BBase.o BList.o BMessagePort.o BDevice.o

LIB=	libexec.a

#CFLAGS= -I. -I../cclib -I../devices -I../x86

.cpp.o:
	gcc -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp

.asm.o:
	@echo ""
	nasm -f elf64 -o $*.o $*.asm
	@echo ""

$(LIB):	$(OBJ)
	ar rcs $(LIB) $(OBJ)
	ranlib $(LIB)

kernel_isr.o: kernel_isr.asm
	@echo ""
	@echo "*** NASM"
	nasm -f elf64 -o $*.o $*.asm
	@echo "***"
	@echo ""

clean:
	rm -rf *.o *.a
