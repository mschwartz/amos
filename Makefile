TOPDIR=		$(shell git rev-parse --show-toplevel)

# Where we compile our .o
BUILDDIR=	$(TOPDIR)/obj
ISODIR=		$(TOPDIR)/iso
BAREFILE=	$(ISODIR)/bare.img

include mk/build-tools.mk
include mk/cflags.mk

#
# Defualt rule just prints "use build.sh"
# Using build.sh runs the build using the cross compiler in a docker container.
#
invalid:
	echo "use build.sh"

#
###
#
c.img:	$(BAREFILE)
	@echo ""
	@echo ""
	@echo "    Making c.img"

	@rm -f c.img
	@bximage -q -mode=create -hd=12M -imgmode=flat c.img c.img
	@cat < $(BAREFILE) 1<>c.img
	@chmod 644 c.img
	@echo ""
	@echo ""
	@ls -l  */*.img
	@echo ""
	@ls -l c.img


include mk/bare.mk
include mk/kernel.mk
include mk/boot.mk

clean:
	@find . -name '*.o' -delete
	@find . -name '*.a' -delete
	@find . -name '*.raw' -delete
	@find . -name '*.elf' -delete
	@find . -name '*.psf' -delete
	@find . -name '*.img' -delete
	@echo "Cleaned build files"

-include $(DEPENDENCIES)
