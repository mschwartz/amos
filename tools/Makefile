TOPDIR=		$(shell git rev-parse --show-toplevel)
include $(TOPDIR)/mk/tools.mk

BUILDOBJ=	build-img.o

GDTOBJ= 	generate-gdt.o

all: 
	cd disk-tool && $(MAKE)
	cd build-img && $(MAKE)

# generate-gdt:	$(GDTOBJ)
# 	@echo Build generate-gdt
# 	$(GCC) -o generate-gdt $(GDTOBJ) -lstdc++

# disk-tool:
# 	@echo Build disk-tool
# 	cd disk-tool && make

clean:
	cd disk-tool && $(MAKE) clean
	cd build-img && && $(MAKE) clean

install: build-img
	cp build-img /usr/local/bin
