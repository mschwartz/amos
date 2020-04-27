G=	../Graphics
D=	../Devices

#GFX=	$(G)/TRect.o Font.o $(G)/BBitmap.o
GFX=	$(G)/TRect.o $(G)/BBitmap.o $(G)/bitmap/BBitmap32.o $(D)/screen/ScreenVGA.o $(D)/screen/ScreenVesa.o

FONTS=  $(G)/BFont.o $(G)/font/BVectorFont.o $(G)/font/BConsoleFont.o

DEVICES= $(D)/KeyboardDevice.o $(D)/PIC.o $(D)/Screen.o $(D)/TimerDevice.o $(D)/RtcDevice.o

X86=	x86/bochs.o x86/gdt.o x86/idt.o x86/kernel_isr.o x86/kprint.o x86/mmu.o

TYPES=	Types.o Types/TBCD.o 

OBJ= 	 $(TYPES) $(X86) Memory.o BBase.o Random.o BList.o MessagePort.o BInterrupt.o BTask.o BDevice.o ExecBase.o $(DEVICES) $(GFX) $(FONTS)

LIB=	libexec.a

.cpp.o:
	@echo $*.cpp
	$(GCC) -O2 -c $(CFLAGS) $(INCLUDE_PATH) -o $*.o $*.cpp
	@echo ""

.asm.o:
	@echo ""
	nasm -f elf64 -o $*.o $*.asm
	@echo ""

$(LIB):	$(OBJ)
	$(AR) rcs $(LIB) $(OBJ)
	$(RANLIB) $(LIB)

x86/kernel_isr.o: x86/kernel_isr.asm
	@echo ""
	@echo "*** NASM"
	nasm -f elf64 -o $*.o $*.asm
	@echo "***"
	@echo ""

clean:
	rm -rf *.o *.a $(D)/*.o $(D)/*.a $(G)/*.o $(G)/*.a
