#
### Boot Program
#

$(ISODIR)/boot.img:
	@echo "     "BUILD boot.img
	@$(NASM) -f bin -I$(TOPDIR)/boot -DKGFX -o $(ISODIR)/boot.img boot/boot.asm
	@ls -l $(ISODIR)/boot.img
