#!/bin/bash

set -e

PWD=`pwd`
echo $PWD

export KERNEL='main.o kernel_start.o'

export INCLUDE_PATH="\
	-I${PWD}/kernel/cclib \
	-I$PWD/kernel/x86 \
	-I$PWD/kernel/devices -I. \
	"

export CFLAGS="\
	-mno-red-zone \
	-nostartfiles \
	-ffreestanding \
	-nostdlib \
	-m64 \
	-fno-stack-protector \
	-fno-exceptions $INCLUDE_PATH \
	"

export LIBS="\
	-L$PWD/kernel/cclib -lcclib \
	-L$PWD/kernel/x86 -lx86 \
	-L$PWD/kernel/devices -ldevices \
	-L$PWD/kernel/cclib -lcclib \
	"

#echo $INCLUDE_PATH
#echo $LIBS
#echo $CFLAGS

#############################

echo "BUILDING BOOT SECTOR"
cd boot
nasm -f bin -l boot.lst -o boot.img boot.asm
ls -l boot.img
cd ..


#############################

echo ""
echo "BUILDING KERNEL"

cd kernel

echo "  COMPILING"
echo "    nasm -f elf -o kernel_start.o kernel_start.asm"
nasm -f elf64 -o kernel_start.o -l kernel_start.lst kernel_start.asm
cd cclib
make
cd ..
cd x86 
make
cd ..
cd devices
make
cd ..

echo "    gcc -c $CFLAGS -o main.o main.cpp"
gcc -g -c $CFLAGS -o main.o main.cpp

#############################

echo "  LINKING"
#export LIBS='-Lcclib -lcclib -Lx86 -lx86 -Ldevices -ldevices'
echo "    ld -melf_i386 -Tconfig.ld -o kernel.elf $KERNEL ${LIBS}"
ld  -e _start -Tconfig.ld -o kernel.elf $KERNEL $LIBS
echo "    objcopy -O binary kernel.elf kernel.img"
objcopy -O binary kernel.elf kernel.img
cd ..
cd tools 
make 
cd ..
./tools/build-img boot/boot.img kernel/kernel.img
rm -f c.img
bximage -q -mode=create -hd=10M -imgmode=flat c.img c.img
echo "cat < bare.img 1<>c.img"
cat < bare.img 1<>c.img

#cat boot.img kernel.img > drive.img

#############################

echo ""
echo "COMPLETE!"
ls -l  */*.img 
echo ""
ls -l *.img
echo ""
nm kernel/kernel.elf | grep " T " | awk '{ print $1" "$3 }' > kernel.sym
echo ""
wc -l kernel.sym
