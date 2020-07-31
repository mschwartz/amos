TOPDIR=		$(shell git rev-parse --show-toplevel)

include $(TOPDIR)/mk/tools.mk

OBJ=	main.o RawDisk.o
PROGRAM=	build-img

.cpp.o:
	$(GCC) -c $(INCLUDE_PATH) -O2 -g $(INCLUDE_PATH) -o $*.o $*.cpp

$(PROGRAM): $(OBJ)
	gcc -o $(PROGRAM) $(OBJ) -lstdc++

all: 	$(PROGRAM)

clean:
	rm -f *.o $(PROGRAM)
