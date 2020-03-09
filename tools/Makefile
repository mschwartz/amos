BUILDOBJ=	build-img.o

GDTOBJ= 	generate-gdt.o

.cpp.o:
	gcc -c -O2 -g -o $*.o $*.cpp

all: 	build-img generate-gdt

build-img:	$(BUILDOBJ)
	gcc -o build-img $(BUILDOBJ) -lstdc++

generate-gdt:	$(GDTOBJ)
	gcc -o generate-gdt $(GDTOBJ) -lstdc++

clean:
	rm -rf build-img *.o

install: build-img
	cp build-img /usr/local/bin
