#
### KERNEL
#

### KERNEL FONT
FONTDIR=	"$(TOPDIR)/kernel/Graphics/font/console"

#KFONTNAME=	cp866-8x16
KFONT=		$(KFONTNAME).psf
KFONTGZ=	$(KFONT).gz
FONTFILE=	$(FONTDIR)/$(KFONT)
KFONTOBJ=	$(BUILDDIR)/$(KFONTNAME).o

KFONTSYMBASE=	_binary_$(subst -,_,$(subst /,_,$(KFONTOBJ)))
KFONTSYMSTART=	$(subst .o,_psf_start,$(KFONTSYMBASE))
KFONTSYMEND=	$(subst .o,_psf_end,$(KFONTSYMBASE))
KFONTSYMSIZE=	$(subst .o,_psf_size,$(KFONTSYMBASE))

# Kernel sources
SOURCES=	$(shell find kernel -name '*.cpp')
OBJECTS= 	$(addprefix $(BUILDDIR)/, $(SOURCES:.cpp=.o))
DEPENDENCIES= 	$(addprefix $(BUILDDIR)/, $(SOURCES:.cpp=.d))

# Examples sources
EXAMPLESRC=	$(shell find Examples -name '*.cpp')
EXAMPLESOBJ= 	$(addprefix $(BUILDDIR)/, $(EXAMPLESRC:.cpp=.o))

# Kernel assembly sources
ASM=		$(shell find kernel -name '*.asm')
ASMOBJECTS= 	$(addprefix $(BUILDDIR)/, $(ASM:.asm=.o))

CRTBEGIN_OBJ=	$(shell $(GCC) -print-file-name=crtbegin.o)
CRTEND_OBJ=	$(shell $(GCC) -print-file-name=crtend.o)
ALL_OBJ=	$(KFONTOBJ) $(ASMOBJECTS) $(OBJECTS) $(EXAMPLESOBJ)
KERNEL_OBJ=	$(filter-out $(BUILDDIR)/kernel/kernel_main.o, $(filter-out $(BUILDDIR)/kernel/kernel_start.o, $(ALL_OBJ) ))

# the rule
$(ISODIR)/kernel.img: $(ISODIR)/kernel.elf
	$(OBJCOPY) -O binary $(ISODIR)/kernel.elf $(ISODIR)/kernel.img

$(ISODIR)/kernel.elf:	$(ALL_OBJ)
	@echo ""
	@echo "LINKING"
	@echo "    LD "$(ISODIR)/kernel.elf
	@ld  -e _start -Tkernel/config.ld -o $(ISODIR)/kernel.elf $(CRTBEGIN_OBJ) $(BUILDDIR)/kernel/kernel_main.o $(BUILDDIR)/kernel/kernel_start.o $(CRTEND_OBJ) $(KERNEL_OBJ)
	@echo ""
	@echo "     KFONTSYMSTART" $(KFONTSYMSTART)
	@echo "       KFONTSYMEND" $(KFONTSYMEND)
	@echo "      KFONTSYMSIZE" $(KFONTSYMSIZE)
	@nm $(KFONTOBJ)

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
	@echo "     -> CREATING FONT" $(KFONTSYMBASE)
	@mkdir -p $(BUILDDIR)

	@cp $(FONTFILE).gz $(BUILDDIR)
	@gunzip $(BUILDDIR)/$(KFONT).gz
	@$(OBJCOPY) -O elf64-x86-64 -B i386 -I binary $(BUILDDIR)/$(KFONT) $(KFONTOBJ)
	@rm -f $(BUILDDIR)/$(KFONT)
