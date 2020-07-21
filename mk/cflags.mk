#
### Compiler Flags
#
CFLAGS= 	-DKFONTSTART="$(KFONTSYMSTART)" \
		-MD -MP \
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
		-DKGFX \
		-DKFONTEND="$(KFONTSYMEND)" \
		-DKFONTSIZE="$(KFONTSYMSIZE)" 

INCLUDE_PATH=	-I$(TOPDIR)/kernel -I$(TOPDIR)/kernel/posix -I$(TOPDIR)/kernel/include -I$(TOPDIR)
