OBJ= 		itoa.o sprintf.o string.o

LIB=		libcclib.a

.cpp.o:
	gcc -g -O2 -c $(CFLAGS) -o $*.o $*.cpp

$(LIB):	$(OBJ) Makefile
	ar rcs $(LIB) $(OBJ)
	ranlib $(LIB)
	@echo ""

clean:
	rm -rf *.o *.a
