#!/bin/bash

set -e

export KERNEL='main.o kernel_start.o'

echo "BUILDING BOOT SECTOR"
cd boot
nasm -f bin -l boot.lst -o boot.img boot.asm
ls -l boot.img
cd ..


echo ""
echo "BUILDING KERNEL"

cd kernel
export CFLAGS='-nostartfiles -ffreestanding -nostdlib -m64 -fno-stack-protector -I./cclib -I./x86 -I./kernel/cclib -I./kernel/x86'

echo "  COMPILING"
echo "    nasm -f elf -o kernel_start.o kernel_start.asm"
nasm -f elf64 -o kernel_start.o -l kernel_start.lst kernel_start.asm
cd cclib
make
cd ..
cd x86 
make
cd ..

echo "    gcc -c $CFLAGS -o main.o main.cpp"
gcc -g -c $CFLAGS -o main.o main.cpp

echo "  LINKING"
export LIBS='-Lcclib -lcclib -Lx86 -lx86'
echo "    ld -melf_i386 -Tconfig.ld -o kernel.elf $KERNEL ${LIBS}"
ld  -e _start -Tconfig.ld -o kernel.elf $KERNEL $LIBS
echo "    objcopy -O binary kernel.elf kernel.img"
objcopy -O binary kernel.elf kernel.img
cd ..
cd tools 
make 
cd ..
./tools/build-img boot/boot.img kernel/kernel.img
echo "    cat boot.img kernel.img > drive.img"
#cat boot.img kernel.img > drive.img

echo ""
echo "COMPLETE!"
ls -l  */*.img 
echo ""
ls -l *.img
echo ""
