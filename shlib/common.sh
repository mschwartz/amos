#!/bin/sh

. ./shlib/platform.sh

TOP_DIR=`git rev-parse --show-toplevel`
#export GCC=/usr/bin/i686-elf-gcc
#export LD=/usr/bin/i686-elf-ld
#export AR=/usr/bin/i686-elf-ar
#export RANLIB=/usr/bin/i686-elf-ranlib
#export OBJCOPY=/usr/bin/i686-elf-objcopy


    export GCC=gcc
    export GPP=g++
    export LD=ld
    export AR=ar
    export RANLIB=ranlib
    export OBJCOPY=objcopy
# if [ $platform == 'macos' ]; then
#     export GCC=gcc-9
#     export GPP=g++-9
#     export LD=ld
#     export AR=gcc-ar-9
#     export RANLIB=gcc-ranlib-9
#     export OBJCOPY=objcopy
# else
#     export GCC=gcc
#     export GPP=g++-9
#     export LD=ld
#     export AR=ar
#     export RANLIB=ranlib
#     export OBJCOPY=objcopy
# fi

# use this one to use graphics mode
export KGFX='-DKGFX'
# use this one to use text mode
#export KGFX=''

export KFONTNAME="cp866-8x16"
export KFONT="$KFONTNAME.psf"

export INCLUDE_PATH="\
	-I$TOP_DIR/kernel/include/ \
	-I$TOP_DIR/kernel/ \
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
  	-DKFONTNAME=$KFONTNAME \
	$KGFX \
	$INCLUDE_PATH \
	"

export LIBS="\
	-L$TOP_DIR/kernel/Exec -lexec \
	-L$TOP_DIR/kernel/Inspiration -linspiration \
	-L$TOP_DIR/kernel/posix -lposix \
	"

