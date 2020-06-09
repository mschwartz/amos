OBJ=	BFileSystem.o

LIB=		libfs.a

.cpp.o:
	@echo $*.cpp
	$(GCC) -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp
	@echo ""

$(LIB):	$(OBJ)
	$(AR) rcs $(LIB) $(OBJ)
	$(RANLIB) $(LIB)
