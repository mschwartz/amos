G=	../Graphics
D=	../Devices

#GFX=	$(G)/TRect.o Font.o $(G)/BBitmap.o
GFX=	$(G)/TRect.o $(G)/BBitmap.o $(G)/bitmap/BBitmap32.o $(G)/BViewPort.o 

FONTS=  $(G)/BFont.o $(G)/font/BVectorFont.o $(G)/font/BConsoleFont.o

DEVICES= $(D)/KeyboardDevice.o $(D)/TimerDevice.o $(D)/RtcDevice.o $(D)/MouseDevice.o $(D)/SerialDevice.o

X86=	x86/bochs.o x86/gdt.o x86/idt.o x86/kernel_isr.o x86/kernel_tasking.o x86/kernel_utils.o x86/mmu.o x86/pic.o 

TYPES=	Types.o Types/TBCD.o 

OBJ= 	 $(TYPES) $(X86) $(G)/kfont.o Memory.o BBase.o Random.o BList.o MessagePort.o BInterrupt.o BTask.o BDevice.o ExecBase.o $(DEVICES) $(GFX) $(FONTS)

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
	nasm -f elf64 -o $*.o $(SERIAL) $*.asm

x86/kernel_tasking.o: x86/kernel_tasking.asm
	nasm -f elf64 -o $*.o $(SERIAL) $*.asm

x86/kernel_utils.o: x86/kernel_utils.asm
	nasm -f elf64 -o $*.o $(SERIAL) $*.asm

clean:
	rm -rf *.o *.a $(D)/*.o $(D)/*.a $(G)/*.o $(G)/*.a
