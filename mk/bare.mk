#
### bare.img
#
$(BAREFILE):	$(ISODIR)/boot.img $(ISODIR)/kernel.img
	echo `id`
	@echo ""
	@echo ""
	@cd tools && $(MAKE)
	@echo ""
	@echo ""
	@cd iso && ./make-iso.sh
	@echo ""
	@echo ""
	echo `id`
	@./tools/build-img/build-img $(ISODIR)/boot.img $(ISODIR)/kernel.img $(ISODIR)/fs.img
