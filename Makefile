KERNEL=	kernel_start.o main.o
#KERNEL=	main.o

#CFLAGS=-nostdlib -m32
CFLAGS=-nostartfiles -ffreestanding -nostdlib -m32

.c.o:
	gcc -O2 -c $(CFLAGS) -o $*.o $*.c

.S.o:
	gcc -c $(CFLAGS) -o $*.o $*.S

.asm.o:
	nasm -f elf -o $*.o $*.asm


all:	clean drive.img

kernel.elf: $(KERNEL)
	@echo ">>> Linking kernel.elf"
	ld -melf_i386 -Ttext 0x7e00 -o kernel.elf $(KERNEL)
	ld -melf_i386 -Tconfig.ld -o kernel.elf $(KERNEL)
#        gcc $(CFLAGS) -Ttext 0x8000 -o kernel.elf $(KERNEL)

kernel.img: kernel.elf
	@echo ">>> Building kernel.img"
	objcopy -O binary kernel.elf kernel.img

boot.img: boot.asm
	@echo ">>> Building boot.img"
#        as -o boot.o boot.S
	nasm -fbin -l boot.lst -o boot.img boot.asm


drive.img:	boot.img kernel.img
	cat boot.img kernel.img > drive.img

run:	all
	qemu-system-x86_64 drive.img

clean:
	@rm -f *.o  *.img


