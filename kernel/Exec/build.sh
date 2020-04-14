#!/bin/sh

# bulid script for Exec library only

. ../../shlib/common.sh

echo "    creating console font"
FFILE=$TOP_DIR/resources/fonts/console-fonts/$KFONT
if [[ ! -e $FFILE ]]; then
	gzip -d $FFILE.gz
	$OBJCOPY -O elf64-x86-64 -B i386 -I binary $FFILE Graphics/kfont.o
	gzip  $FFILE
else
	$OBJCOPY -O elf64-x86-64 -B i386 -I binary $FFILE Graphics/kfont.o
fi
make

