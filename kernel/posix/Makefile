OBJ= 		sbrk.o malloc.o itoa.o sprintf.o string.o 

LIB=		libposix.a

.cpp.o:
	gcc -g -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp

$(LIB):	$(OBJ) Makefile
	ar rcs $(LIB) $(OBJ)
	ranlib $(LIB)
	@echo ""

clean:
	rm -rf *.o *.a
