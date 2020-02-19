#!/bin/bash

export KERNEL='kernel_start.o main.o'

echo "BUILDING BOOT SECTOR"
nasm -f bin -l boot.lst -o boot.img boot.asm
ls -l boot.img

echo ""
echo "BUILDING KERNEL"
nasm -f elf -o kernel_start.o kernel_start.asm

export CFLAGS='-nostartfiles -ffreestanding -nostdlib -m32'

echo "  COMPILING"
echo "    gcc -O2 -c $CFLAGS -o main.o main.cpp"
gcc -O2 -c $CFLAGS -o main.o main.cpp
echo "  LINKING"
echo "    ld -melf_i386 -Tconfig.ld -o kernel.elf $KERNEL"
ld -melf_i386 -Tconfig.ld -o kernel.elf $KERNEL
echo "    objcopy -O binary kernel.elf kernel.img"
objcopy -O binary kernel.elf kernel.img
echo "    cat boot.img kernel.img > drive.img"
cat boot.img kernel.img > drive.img

echo ""
echo "COMPLETE!"
