#!/bin/sh

./build.sh
qemu-system-x86_64 bare.img -boot a -s -S

