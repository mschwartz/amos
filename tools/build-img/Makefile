TOPDIR=		$(shell git rev-parse --show-toplevel)

include $(TOPDIR)/mk/tools.mk

OBJ=		build-img.o
PROGRAM=	build-img

.cpp.o:
	$(GCC) -c -O2 -g $(INCLUDE_PATH) -o $*.o $*.cpp

disk-tool: $(OBJ)
	gcc -o $(PROGRAM) $(OBJ) -lstdc++

all: 	disk-tool

clean:
	rm -f *.o $(PROGRAM)
