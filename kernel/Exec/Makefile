GFX=	Graphics/TRect.o Graphics/TBCD.o Graphics/BFont.o Graphics/BBitmap.o 

FONTS=	Graphics/Font/BVectorFont.o Graphics/Font/BConsoleFont.o Graphics/Font/VectorFonts/Font.o Graphics/kfont.o

SCREEN=	Devices/screen/VGAScreen.o Devices/screen/BitmapScreen.o 

DEVICES= Devices/Keyboard.o Devices/PIC.o Devices/Screen.o Devices/Timer.o Devices/ScreenDevice.o

OBJ= 	 kprint.o BTypes.o Memory.o BBase.o BList.o BTask.o BMessagePort.o BDevice.o ExecBase.o $(DEVICES) $(GFX) $(SCREEN) $(FONTS)

LIB=	libExec.a

.cpp.o:
	$(GPP) -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp

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
	rm -rf *.o *.a */*.o */*.a */*/*.o */*/*.a
