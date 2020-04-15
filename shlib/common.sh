#!/bin/sh

TOP_DIR=`git rev-parse --show-toplevel`
echo $TOP_DIR

#export GCC=/usr/bin/i686-elf-gcc
#export LD=/usr/bin/i686-elf-ld
#export AR=/usr/bin/i686-elf-ar
#export RANLIB=/usr/bin/i686-elf-ranlib
#export OBJCOPY=/usr/bin/i686-elf-objcopy

export GCC=gcc
export LD=ld
export AR=ar
export RANLIB=ranlib
export OBJCOPY=objcopy

export INCLUDE_PATH="\
	-I$TOP_DIR/kernel/include/ \
	-I$TOP_DIR/kernel/ \
	-I$TOP_DIR/kernel/Exec/ \
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
	-DKERNEL \
	$INCLUDE_PATH \
	"

export LIBS="\
	-L$TOP_DIR/kernel/Exec -lexec \
	-L$TOP_DIR/kernel/Exec/x86 -lx86 \
	-L$TOP_DIR/kernel/posix -lposix \
	"

