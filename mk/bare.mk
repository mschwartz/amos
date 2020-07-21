#
### bare.img
#
$(BAREFILE):	$(ISODIR)/boot.img $(ISODIR)/kernel.img
	@echo ""
	@echo ""
	@cd tools && $(MAKE)
	@echo ""
	@echo ""
	@cd iso && ./make-iso.sh
	@echo ""
	@echo ""
	@./tools/build-img $(ISODIR)/boot.img $(ISODIR)/kernel.img $(ISODIR)/fs.img
