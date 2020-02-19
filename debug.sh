#!/bin/sh

./build.sh
qemu-system-x86_64 drive.img -boot a -s -S

