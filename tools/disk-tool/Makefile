BUILDOBJ=	main.o

.cpp.o:
	$(GCC) -c -O2 -g $(INCLUDE_PATH) -o $*.o $*.cpp

disk-tool: $(BUILDOBJ)
	gcc -o disk-tool $(BUILDOBJ) -lstdc++

all: 	disk-tool

