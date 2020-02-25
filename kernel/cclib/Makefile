OBJ= 		string.o Screen.o kprint.o

LIB=		libcclib.a

export CFLAGS=-nostartfiles -ffreestanding -nostdlib -m64 -fno-stack-protector -I../cclib -I../x86

.cpp.o:
	gcc -O2 -c $(CFLAGS) -o $*.o $*.cpp

$(LIB):	$(OBJ)
	ar rcs $(LIB) $(OBJ)
	ranlib $(LIB)

clean:
	rm -rf *.o *.a
