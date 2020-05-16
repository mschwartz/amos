# INSPIRATION

LIB=	libinspiration.a

OBJ=	Inspiration.o ScreenVesa.o BWindow.o TestTask.o

.cpp.o:
	@echo $*.cpp
	$(GCC) -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp
	@echo ""

.asm.o:
	@echo ""
	nasm -f elf64 -o $*.o $(SERIAL) $*.asm
	@echo ""

$(LIB):	$(OBJ)
	$(AR) rcs $(LIB) $(OBJ)
	$(RANLIB) $(LIB)
