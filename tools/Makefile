include ../mk/build-tools.mk

BUILDOBJ=	build-img.o

GDTOBJ= 	generate-gdt.o

.cpp.o:
	$(GCC) -c -O2 -g -o $*.o $*.cpp

all: 	build-img generate-gdt

build-img:	$(BUILDOBJ)
	@echo Build build-img
	@$(GCC) -o build-img $(BUILDOBJ) -lstdc++

generate-gdt:	$(GDTOBJ)
	@echo Build generate-gdt
	$(GCC) -o generate-gdt $(GDTOBJ) -lstdc++

clean:
	rm -rf build-img *.o

install: build-img
	cp build-img /usr/local/bin
