#!/bin/sh

./build.sh
bochs -q
#qemu-system-x86_64 bare.img -boot a -s -S

