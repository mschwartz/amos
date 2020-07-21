TOPDIR=		$(shell git rev-parse --show-toplevel)

FONTDIR=	"$(TOPDIR)/kernel/Graphics/font/console"

BUILDDIR=	$(TOPDIR)/obj
INCLUDE_PATH=	-I$(TOPDIR)/kernel -I$(TOPDIR)/kernel/posix -I$(TOPDIR)/kernel/include -I$(TOPDIR)

SOURCES=	$(shell find kernel -name '*.cpp')
OBJECTS= 	$(addprefix $(BUILDDIR)/, $(SOURCES:.cpp=.o))

EXAMPLESRC=	$(shell find Examples -name '*.cpp')
EXAMPLESOBJ= 	$(addprefix $(BUILDDIR)/, $(EXAMPLESRC:.cpp=.o))

ASM=		$(shell find kernel -name '*.asm')
ASMOBJECTS= 	$(addprefix $(BUILDDIR)/, $(ASM:.asm=.o))

KFONTNAME=	cp866-8x16
KFONT=		$(KFONTNAME).psf
KFONTGZ=	$(KFONT).gz
FONTFILE=	$(FONTDIR)/$(KFONT)
KFONTOBJ=	$(BUILDDIR)/$(KFONTNAME).o

KFONTSYMBASE=	_binary_$(subst -,_,$(subst /,_,$(KFONTOBJ)))
KFONTSYMSTART=	$(subst .o,_psf_start,$(KFONTSYMBASE))
KFONTSYMEND=	$(subst .o,_psf_end,$(KFONTSYMBASE))
KFONTSYMSIZE=	$(subst .o,_psf_size,$(KFONTSYMBASE))

CFLAGS= 	-DKFONTSTART="$(KFONTSYMSTART)" \
		-mno-red-zone \
		-nostartfiles \
		-ffreestanding \
		-nostdlib \
		-m64 \
		-fno-stack-protector \
		-fno-exceptions \
		-fno-use-cxa-atexit \
		-fno-rtti \
		-DKERNEL \
		-DKFONTNAME="$(KFONTNAME)" \
		$(KGFX) \
		-DKFONTEND="$(KFONTSYMEND)" \
		-DKFONTSIZE="$(KFONTSYMSIZE)" 

GCC=		gcc
GPP=		g++
LD=		ld
AR=		ar
RANLIB=		ranlib
OBJCOPY=	objcopy
GZIP=		gzip
NASM=		nasm


bare.img:	kernel.elf
# link

kernel.elf:	$(KFONTOBJ) $(ASMOBJECTS) $(OBJECTS) $(EXAMPLESOBJ)
	@echo "LINKING"
	@echo "     KFONTSYMSTART" $(KFONTSYMSTART)
	@echo "       KFONTSYMEND" $(KFONTSYMEND)
	@echo "      KFONTSYMSIZE" $(KFONTSYMSIZE)
	@nm $(KFONTOBJ)
	@ld  -e _start -Tkernel/config.ld -o kernel.elf $(KFONTOBJ) $(ASMOBJECTS) $(OBJECTS) $(EXAMPLESOBJ)

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "     "CC $*.cpp
	@$(GCC) -O2 -c \
		$(CFLAGS) \
		$(INCLUDE_PATH) \
		-o $@ $<

$(BUILDDIR)/%.o: %.asm
	@mkdir -p $(@D)
	@echo "     "AS $*.asm
	@$(NASM) -I$(TOPDIR)/boot -f elf64 -o $@  $<

$(KFONTOBJ): $(KFONTFILE)
	@echo "CREATING FONT" $(KFONTSYMBASE)
	@mkdir -p $(BUILDDIR)

	cp $(FONTFILE).gz $(BUILDDIR)
	gunzip $(BUILDDIR)/$(KFONT).gz
	$(OBJCOPY) -O elf64-x86-64 -B i386 -I binary $(BUILDDIR)/$(KFONT) $(KFONTOBJ)
	rm -f $(BUILDDIR)/$(KFONT)


# boot/boot.img: boot/*.asm boot/*.inc
# 	+cd boot && make

# kernel/kernel.img:
# 	+cd kernel && make

# tools/build-img:
# 	+cd tools && make

clean:
	find . -name '*.o' -delete
	find . -name '*.a' -delete
	find . -name '*.raw' -delete
	find . -name '*.elf' -delete
	find . -name '*.psf' -delete

run: 	bare.img
	qemu-system-x86_64 bare.img
