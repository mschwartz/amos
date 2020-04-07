GFX=	Graphics/TRect.o Graphics/TBCD.o Graphics/BFont.o Graphics/BBitmap.o Graphics/VectorFonts/Font.o 

SCREEN=	Devices/screen/VGAScreen.o Devices/screen/BitmapScreen.o 

DEVICES= Devices/Keyboard.o Devices/PIC.o Devices/Screen.o Devices/Timer.o

OBJ= 	 BTypes.o Memory.o BBase.o BList.o BMessagePort.o BDevice.o $(DEVICES) $(GFX) $(SCREEN)

LIB=	libexec.a

.cpp.o:
	$(GCC) -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp

.asm.o:
	@echo ""
	nasm -f elf64 -o $*.o $*.asm
	@echo ""

$(LIB):	$(OBJ)
	$(AR) rcs $(LIB) $(OBJ)
	$(RANLIB) $(LIB)

kernel_isr.o: kernel_isr.asm
	@echo ""
	@echo "*** NASM"
	nasm -f elf64 -o $*.o $*.asm
	@echo "***"
	@echo ""

clean:
	rm -rf *.o *.a Devies/*.o Devices/*.a Graphics/*.o Graphics/*.a
