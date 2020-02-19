#!/bin/bash

export KERNEL='kernel_start.o cclib/string.o main.o'

echo "BUILDING BOOT SECTOR"
nasm -f bin -l boot.lst -o boot.img boot.asm
ls -l boot.img

echo ""
echo "BUILDING KERNEL"
export CFLAGS='-nostartfiles -ffreestanding -nostdlib -m64 -fno-stack-protector'

echo "  COMPILING"
echo "    nasm -f elf -o kernel_start.o kernel_start.asm"
nasm -f elf64 -o kernel_start.o kernel_start.asm

echo "    gcc -c $CFLAGS -o cclib/string.o cclib/string.cpp"
gcc -g -c $CFLAGS -o cclib/string.o cclib/string.cpp
echo "    gcc -c $CFLAGS -o main.o main.cpp"
gcc -g -c $CFLAGS -o main.o main.cpp
echo "  LINKING"
echo "    ld -melf_i386 -Tconfig.ld -o kernel.elf $KERNEL"
#ld -melf_i386 -Tconfig.ld -o kernel.elf $KERNEL
ld  -Tconfig.ld -o kernel.elf $KERNEL
echo "    objcopy -O binary kernel.elf kernel.img"
objcopy -O binary kernel.elf kernel.img
cd tools && make && cd ..
./tools/build-img boot.img kernel.img
echo "    cat boot.img kernel.img > drive.img"
#cat boot.img kernel.img > drive.img

echo ""
echo "COMPLETE!"
