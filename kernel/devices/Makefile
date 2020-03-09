OBJ= 	PIC.o Screen.o Keyboard.o Timer.o

LIB=	libdevices.a

ifndef CFLAGS
	CFLAGS=-nostartfiles \
		-ffreestanding \
		-nostdlib \
		-m64 \
		-fno-stack-protector \
		-fno-exceptions $(INCLUDE_PATH)
endif

.cpp.o:
	gcc -O2 -c $(CFLAGS) -o $*.o $*.cpp


$(LIB):	$(OBJ)
	ar rcs $(LIB) $(OBJ)
	ranlib $(LIB)

kernel_isr.o: kernel_isr.asm
	nasm -f elf64 -o $*.o $*.asm

clean:
	rm -rf *.o *.a
