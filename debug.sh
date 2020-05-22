#!/bin/sh

. ./shlib/platform.sh

./build.sh
if [ "$WSL" != "" ]; then
    if [ -e "/mnt/c/dev/bare.log" ]; then
	mv /mnt/c/dev/bare.log /mnt/c/dev/bare.log.1
    fi
    cp bochsrc /mnt/c/dev/
    cp bare.img /mnt/c/dev
    ping -c 4 192.168.1.1 >/dev/zero
    "$VBOX/VirtualBoxVM.exe" \
	--startvm  \
	bare \
	--debug 
    ping -c 4 192.168.1.1 >/dev/zero
    tail -f /mnt/c/dev/bare.log
else
    if [ $platform = "macos" ]; then
	cp bochsrc.macos bochsrc
    else
	cp bochsrc.linux bochsrc
    fi
    bochs -q
fi

#qemu-system-x86_64 bare.img -boot a -s -S
