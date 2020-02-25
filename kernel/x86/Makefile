OBJ= 	kernel_memory.o

LIB=	libx86.a

.cpp.o:
	gcc -O2 -c $(CFLAGS) -o $*.o $*.cpp

$(LIB):	$(OBJ)
	ar rcs $(LIB) $(OBJ)
	ranlib $(LIB)

clean:
	rm -rf *.o *.a
