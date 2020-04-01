KERNEL=	kernel_start.o main.o

CFLAGS=-nostartfiles -ffreestanding -nostdlib -m64 -fno-stack-protector -I./cclib -I./x86

.cpp.o:
	gcc -O2 -c $(CFLAGS) -o $*.o $*.cpp

all:	kernel.img

kernel_start.o: kernel_start.asm
	@echo "     >>> Assembling kernel_start"
	nasm -f elf64 -o kernel_start.o kernel_start.asm

./posix/libposix:
	+cd cclib && make

kernel.elf: $(KERNEL) ./posix/libposix ./x86/libx86
	@echo "    >>> Linking kernel.elf"
	ld -Tconfig.ld -o kernel.elf $(KERNEL) -L./posix -lposix -L/x86 -lx86
#        gcc $(CFLAGS) -Ttext 0x8000 -o kernel.elf $(KERNEL)

kernel.img: kernel.elf
	@echo "    >>> Building kernel.img"
	objcopy -O binary kernel.elf kernel.img
	@echo "    >>> BUILT!"

run:	all
	qemu-system-x86_64 drive.img

clean:
	@rm -f *.o  *.img
	cd posix && make clean && cd ..
	cd x86 && make clean


