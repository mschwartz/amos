SRC=		$(wildcard *.cpp)

OBJ=		$(SRC:.cpp=.o)

LIB=		libexamples.a

.cpp.o:
	@echo $*.cpp
	$(GCC) -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp
	@echo ""

all: $(OBJ)
	# $(AR) rcs $(LIB) $(OBJ)
	# $(RANLIB) $(LIB)

clean:
	rm -f *.a *.o

