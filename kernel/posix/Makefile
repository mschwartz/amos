OBJ= 		sbrk.o malloc.o itoa.o sprintf.o string.o 

LIB=		libposix.a

.cpp.o:
	$(GCC) -g -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp

$(LIB):	$(OBJ) Makefile
	$(AR) rcs $(LIB) $(OBJ)
	$(RANLIB) $(LIB)
	@echo ""

clean:
	rm -rf *.o *.a
