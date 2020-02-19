OBJ= 	string.o

LIB=	libcclib.a

.cpp.o:
	gcc -O2 -c $(CFLAGS) -o $*.o $*.c

$(LIB):	$(OBJ)
	ar rcs $(LIB) $(OBJ)
	ranlib $(LIB)
