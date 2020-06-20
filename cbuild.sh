#!/bin/bash

set -e

. ./shlib/platform.sh
. ./shlib/common.sh

echo ""
echo ""
echo "RCOMP"
rm -f ./kernel/Exec/Memory.o ./kernel/Exec/BBase.o ./kernel/Exec/Types/BList.o ./kernel/Exec/Types/BSymbolTable.o
cd tools/rcomp-src && make

echo ""
echo ""
echo "DISK-TOOL"
rm -f ./kernel/Exec/Memory.o ./kernel/Exec/BBase.o ./kernel/Exec/BList.o ./kernel/Exec/Types/BSymbolTable.o
cd $TOP_DIR/tools/disk-tool && make

cd $TOP_DIR
echo $TOP_DIR
pwd


CRTBEGIN_OBJ=`$GCC -print-file-name=crtbegin.o`
#echo ""
#echo $CRTBEGIN_OBJ
#echo ""
CRTEND_OBJ=`$GCC -print-file-name=crtend.o`

#echo "CRTBEGIN" $CRTBEGIN_OBJ

#export KERNEL="crti.o $CRTBEGIN_OBJ main.o kernel_start.o $CRTEND_OBJ crtn.o "
#export KERNEL="crti.o $CRTBEGIN_OBJ kernel_main.o kernel_start.o $CRTEND_OBJ crtn.o "
export KERNEL="$CRTBEGIN_OBJ kernel_main.o kernel_start.o $CRTEND_OBJ "

#echo $INCLUDE_PATH
#echo $LIBS
#echo $CFLAGS

#############################

echo ""
echo ""
echo "BUILDING BOOT SECTOR"
cd $TOP_DIR/boot
nasm -f bin -l boot.lst $KGFX -o boot.img boot.asm
ls -l boot.img
cd ..


#############################

echo ""
echo ""
echo "Generating console font"

cd kernel/Graphics/font/console
FFILE=$KFONT
if [[ ! -e $FFILE ]]; then
	gzip -d $FFILE.gz
	$OBJCOPY -O elf64-x86-64 -B i386 -I binary $FFILE ../../kfont.o
	gzip  $FFILE
else
	$OBJCOPY -O elf64-x86-64 -B i386 -I binary $FFILE ../../kfont.o
fi

cd ../../../..

echo ""
echo ""
echo "BUILDING KERNEL"

cd kernel

echo ""
echo ""
echo "  COMPILING"
echo "    nasm -f elf -o kernel_start.o kernel_start.asm"
nasm -f elf64 -o kernel_start.o -l kernel_start.lst kernel_start.asm
echo "    $GCC -c -o crti.o crti.s"
$GCC -c -o crti.o crti.s
echo "    $GCC -c -o crtn.o crtn.s"
$GCC -c -o crtn.o crtn.s

echo ""
echo ""
echo "  BUILDING EXEC"
cd Exec
make
cd ..

# echo ""
# echo ""
# echo "  BUILDING FILESYSTEMS"
# cd FileSystems
# make
# cd ..

echo ""
echo ""
echo "  BUILDING INSPIRATION"
cd Inspiration
make
cd ..
#echo ""
#echo ""
#echo "  BUILDING X86"
#cd Exec/x86 
#make
#cd ../..
echo ""
echo ""
echo "  ============== BUILDING POSIX"
cd posix
make
cd ..

echo "    $GCC -c $CFLAGS -o kernel_main.o kernel_main.cpp"
$GCC -g -c $CFLAGS $INCLUDE_PATH -o kernel_main.o kernel_main.cpp

#############################

echo ""
echo ""
echo "  LINKING"
echo "    ld -m64 -Tconfig.ld -o kernel.elf $KERNEL ${LIBS}"
ld  -e _start -Tconfig.ld -o kernel.elf $KERNEL $LIBS  -lexec
echo "    objcopy -O binary kernel.elf kernel.img"
objcopy -O binary kernel.elf kernel.img
cd ..

echo ""
echo ""
echo "  Building build-img tools"
cd tools 
make


echo ""
echo ""
echo "Building disk image"
cd ..
cd iso
./make-iso.sh
cd ..
./tools/build-img boot/boot.img kernel/kernel.img iso/fs.img


echo ""
echo ""
rm -f c.img
bximage -q -mode=create -hd=12M -imgmode=flat c.img c.img
echo "cat < bare.img 1<>c.img"
cat < bare.img 1<>c.img

#cat boot.img kernel.img > drive.img

echo ""
echo ""
#############################

if [ "$WSL" != "" ]; then
  echo "WSL! copy to C:\dev"
  cp bochsrc /mnt/c/dev/
  cp bare.img /mnt/c/dev
fi

echo ""
echo "COMPLETE!"
ls -l  */*.img 
echo ""
ls -l *.img
echo ""
if [ "$WSL" != "" ]; then
  echo "=== WSL c:\dev"
  ls -l /mnt/c/dev
  echo " "
fi
nm kernel/kernel.elf | grep " T " | awk '{ print $1" "$3 }' > kernel.sym
echo ""
wc -l kernel.sym
