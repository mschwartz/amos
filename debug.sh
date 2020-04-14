#!/bin/sh

./build.sh
bochs -q -rc bochs_init.txt
#qemu-system-x86_64 bare.img -boot a -s -S

