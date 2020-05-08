LIB=	libinspiration.a

OBJ=	Inspiration.o

.cpp.o:
	@echo $*.cpp
	$(GCC) -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp
	@echo ""

.asm.o:
	@echo ""
	nasm -f elf64 -o $*.o $*.asm
	@echo ""

$(LIB):	$(OBJ)
	$(AR) rcs $(LIB) $(OBJ)
	$(RANLIB) $(LIB)
