G=		../Graphics
D=		../Devices
SF=		../FileSystems/SimpleFileSystem
T=		../Types

GFX=		$(G)/BBitmap.o \
		$(G)/bitmap/BBitmap32.o \
		$(G)/BViewPort.o 

FONTS=  	$(G)/BFont.o \
		$(G)/font/BConsoleFont.o \
		$(G)/kfont.o

DEVICES= 	$(D)/KeyboardDevice.o \
		$(D)/TimerDevice.o \
		$(D)/RtcDevice.o \
		$(D)/MouseDevice.o \
		$(D)/SerialDevice.o

ATA=		$(D)/AtaDevice.o \
		$(D)/ata/task.o \
		$(D)/ata/interrupt.o \
		$(D)/ata/dma.o

X86=		x86/bochs.o \
		x86/tss.o \
		x86/gdt.o \
		x86/idt.o \
		x86/kernel_isr.o \
		x86/kernel_tasking.o \
		x86/kernel_utils.o \
		x86/mmu.o \
		x86/pic.o \
		x86/acpi.o \
		x86/pci.o

TYPES= 		$(T)/Types.o \
		$(T)/TBCD.o \
		$(T)/BList.o \
		$(T)/TRect.o \
		$(T)/BAvlTree.o \
		$(T)/BHashTable.o \
		$(T)/BSparseArray.o \
		$(T)/BSymbolTable.h

FS=		$(SF)/SimpleFileSystem.o  \
		$(SF)/SimpleFileSystemTask.o

EXEC=		Memory.o \
		BBase.o \
		Random.o \
		MessagePort.o \
		BInterrupt.o \
		BTask.o \
		BProcess.o \
		BDevice.o \
		BFileSystem.o \
		ExecBase.o 

OBJ= 	 	$(FS) $(TYPES) $(X86) $(DEVICES) $(ATA) $(GFX) $(FONTS) $(EXEC)

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
