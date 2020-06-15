G=		../Graphics
D=		../Devices
F=		../FileSystems

GFX=		$(G)/TRect.o $(G)/BBitmap.o $(G)/bitmap/BBitmap32.o $(G)/BViewPort.o 

FONTS=  	$(G)/BFont.o $(G)/font/BVectorFont.o $(G)/font/BConsoleFont.o $(G)/kfont.o

DEVICES= 	$(D)/KeyboardDevice.o $(D)/TimerDevice.o $(D)/RtcDevice.o $(D)/MouseDevice.o $(D)/SerialDevice.o $(D)/AtaDevice.o

X86=		x86/bochs.o x86/tss.o x86/gdt.o x86/idt.o x86/kernel_isr.o x86/kernel_tasking.o x86/kernel_utils.o x86/mmu.o x86/pic.o 

TYPES=		Types.o Types/TBCD.o 

CONTAINERS= 	BList.o BAvlTree.o BHashTable.o

FS=		$(F)/BFileSystem.o

EXEC=		Memory.o BBase.o Random.o MessagePort.o BInterrupt.o BTask.o BDevice.o ExecBase.o 

OBJ= 	 	$(FS) $(TYPES) $(X86) $(DEVICES)  $(GFX) $(FONTS) $(CONTAINERS) $(EXEC)

LIB=		libexec.a

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
	nasm -f elf64 -o $*.o $*.asm

x86/kernel_tasking.o: x86/kernel_tasking.asm
	nasm -f elf64 -o $*.o $*.asm

x86/kernel_utils.o: x86/kernel_utils.asm
	nasm -f elf64 -o $*.o $*.asm

clean:
	rm -rf *.o *.a $(D)/*.o $(D)/*.a $(G)/*.o $(G)/*.a
