#!/bin/bash

set -e

PWD=`pwd`
echo $PWD

export INCLUDE_PATH="\
	-I$PWD/kernel/include \
	-I$PWD/kernel \
	-I$PWD/kernel/Exec \
	-I$PWD/kernel/Devices \
	-I$PWD/kernel/cclib \
  	-I. \
	"

export CFLAGS="\
	-mno-red-zone \
	-nostartfiles \
	-ffreestanding \
	-nostdlib \
	-m64 \
	-fno-stack-protector \
	-fno-exceptions \
  -fno-use-cxa-atexit \
  -fno-rtti \
  $INCLUDE_PATH \
	"


export LIBS="\
	-L$PWD/kernel/Exec -lexec \
	-L$PWD/kernel/x86 -lx86 \
	-L$PWD/kernel/Devices -ldevices \
	-L$PWD/kernel/posix -lposix \
	"

CRTBEGIN_OBJ=`gcc -print-file-name=crtbegin.o`
echo ""
echo $CRTBEGIN_OBJ
echo ""
CRTEND_OBJ=`gcc -print-file-name=crtend.o`

echo "CRTBEGIN" $CRTBEGIN_OBJ

#export KERNEL="crti.o $CRTBEGIN_OBJ main.o kernel_start.o $CRTEND_OBJ crtn.o "
export KERNEL="$CRTBEGIN_OBJ main.o kernel_start.o $CRTEND_OBJ  "

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
gcc -c -o crti.o crti.s
gcc -c -o crtn.o crtn.s
echo "  BUILDING EXEC"
cd Exec
make
cd ..
echo "  BUILDING POSIX"
cd posix
make
cd ..
echo "  BUILDING X86"
cd x86 
make
cd ..
echo "  BUILDING DEVICES"
cd Devices
make
cd ..

echo "    gcc -c $CFLAGS -o main.o main.cpp"
gcc -g -c $CFLAGS $INCLUDE_PATH -o main.o main.cpp

#############################

echo "  LINKING"
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
